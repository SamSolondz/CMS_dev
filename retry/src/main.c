#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "infrastructure.h"

/* Libraries containing default Gecko configuration values */
#include "em_system.h"
#include "hal-config.h"
#include "bsphalconfig.h"

#include "em_emu.h"//two cmu files
#include "em_gpio.h"//
#include "em_common.h"//
#include "em_cmu.h" //
#include "em_timer.h"//
#include "em_letimer.h"//
#include "ecode.h" //

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "infrastructure.h"//
#include "retargetserial.h"//
#include "math.h"//


#include "adc_defines.h" //
#include "defines.h"//
#include "spi_functions.h" //
#include "adc_functions.h" //
#include "flash_functions.h"//
#include "bluetooth_utilities.h"
#include "pin_def.h"

#define	DATABUFFER_SIZE  4
#define ONE_MILLISECOND_BASE_VALUE_COUNT            1000
#define ONE_SECOND_TIMER_COUNT                      13672
#define RECORD_ONE_MINUTE							1966080
#define RECORD_ONE_SECOND							32768

#define MODE_FIELD	0
#define MODE_DEMO	1

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif


uint32_t ms_counter = 0;
int measurementCount = 1;			//Will be used to calculate time for each
int readFlag = 0;
int ble_soft_timer_Flag = 0;
int operation_mode = 0;						//Default to field mode
int record_time = RECORD_ONE_SECOND;


uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];
// Gecko configuration parameters (see gecko_configuration.h)
	static const gecko_configuration_t config = {
	  .config_flags = 0,
	  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
	  .bluetooth.max_connections = MAX_CONNECTIONS,
	  .bluetooth.heap = bluetooth_stack_heap,
	  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
	  .bluetooth.sleep_clock_accuracy = 100, // ppm
	  .gattdb = &bg_gattdb_data,
	  .ota.flags = 0,
	  .ota.device_name_len = 3,
	  .ota.device_name_ptr = "OTA",
	#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
	  .pa.config_enable = 1, // Enable high power PA
	  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
	#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
	};


void usart_setup(){

	//Set up clocks
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	CMU_ClockEnable(cmuClock_HFPER, true);

	//Initialize with default settings and then update fields
	USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
	init.clockMode = usartClockMode3;
	init.msbf = true;		//send most sig bit first
	init.autoCsEnable = false;

	USART_InitSync(USART1, &init);

	//Connect USART signals to GPIO peripheral
	/*NOTE: If the USART CS pin is enabled in the route register,
	  but AUTOCS is not being used, then the hardware
	  will disable the port pin driver for the CS pin,
	 leaving the pin output floating.*/

	USART1->ROUTEPEN = 	USART_ROUTEPEN_RXPEN |
						USART_ROUTEPEN_TXPEN |
						USART_ROUTEPEN_CLKPEN;/*|
						USART_ROUTEPEN_CSPEN;*/


	USART1->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC11 |
						USART_ROUTELOC0_TXLOC_LOC11 |
						USART_ROUTELOC0_CLKLOC_LOC11;/*|
						USART_ROUTELOC0_CSLOC_LOC11;*/

	//Setup SPI lines
	GPIO_PinModeSet(CS_PORT_ADC, CS_PIN_ADC, gpioModePushPull, 1);	//CS
	GPIO_PinModeSet(CS_PORT_FLASH, CS_PIN_FLASH, gpioModePushPull, 1);	//CS
	GPIO_PinModeSet(TX_PORT, TX_PIN, gpioModePushPull, 1);	//Tx
	GPIO_PinModeSet(RX_PORT, RX_PIN, gpioModeInput, 0);	//Rx
	GPIO_PinModeSet(SCLK_PORT, SCLK_PIN, gpioModePushPull, 1);	//SCK

    // Clear RX buffer and shift register
    USART1->CMD |= USART_CMD_CLEARRX;

    // Clear TX buffer and shift register
    USART1->CMD |= USART_CMD_CLEARTX;

	USART_Enable(USART1,usartEnable);
}

void mux_select(int select){
	switch(select){
		case 0: //No selection.
			GPIO_PinOutClear(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutClear(MUX_NEG_PORT, MUX_NEG_PIN);
			break;
		case 1:	//Select the x-axis
			GPIO_PinOutSet(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutClear(MUX_NEG_PORT, MUX_NEG_PIN);
			break;
		case 2: //Select the y-axis
			GPIO_PinOutClear(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutSet(MUX_NEG_PORT, MUX_NEG_PIN);
			break;
		case 3: //Select the z-axis
			GPIO_PinOutSet(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutSet(MUX_NEG_PORT, MUX_NEG_PIN);
			break;

		default:
			break;
	}
}


void LETIMER0_IRQHandler(void) {
	  // Clear the interrupt flag
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP0);
		if(ms_counter < record_time)
			ms_counter++;
		else{
			readFlag = 1;
			ms_counter = 0;
			NVIC_DisableIRQ(LETIMER0_IRQn);
		}

}




int main(void){
	/* Chip errata */
	initMcu(); 		//Initialize device
	initBoard(); 	//Initialize board
	initApp();		//Initialize application



	gecko_init(&config);
	RETARGET_SerialInit();
	usart_setup();	//CMU_clock_enable happens here, must occur before gpio/letimersetup
	printf("\n\rHello World!\r\n");

	//Set GPIO pins//
	GPIO_PinModeSet(MUX_POS_PORT, MUX_POS_PIN, gpioModePushPull, 0);	//Pos diff mux
	GPIO_PinModeSet(MUX_NEG_PORT, MUX_NEG_PIN, gpioModePushPull, 0);	//Neg diff mux
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);	//Neg diff mux

	GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 1);	//Pos diff mux

	adc_verify_communication();

	LETIMER_setup();
	NVIC_EnableIRQ(LETIMER0_IRQn);

  /*Initialize Structure to hold recorded data*/
  recorded_data new_data;
  recorded_data *data_ptr;
  data_ptr = &new_data;
  new_data.xaxis = 0x0;
  new_data.yaxis = 0x0;
  new_data.zaxis = 0x0;
  new_data.temp = 0xDEADBEEF;
  new_data.measureNum = 0;

  //Infinite loop
  while(1){
	 //Read ADC Interrupt
	 if(readFlag == 1){
		 	mux_select(1);
			adc_configure_channels();
			uint32_t xread = adc_read_data();

			mux_select(3);
			adc_configure_channels();
			uint32_t zread = adc_read_data();

			mux_select(2);
			adc_configure_channels();
			uint32_t yread = adc_read_data();

			adc_configure_channels();
			uint32_t tempread = adc_read_temperature();


			//Store measurements
			new_data.xaxis = xread;
			new_data.yaxis = yread;
			new_data.zaxis = zread;
			new_data.temp = tempread;
			new_data.measureNum = measurementCount;
			measurementCount++;

			printf("\n\n\r ---Read #%d---", measurementCount);
			printf("\r\n x = %f V", adc_calculate_float(data_ptr->xaxis));
			printf("\r\n y = %f V", adc_calculate_float(data_ptr->yaxis));
			printf("\r\n z = %f V", adc_calculate_float(data_ptr->zaxis));
			printf("\r\n Temperature = %.3f C    .", adc_calculate_float_temp(data_ptr->temp));

			readFlag = 0;
			NVIC_EnableIRQ(LETIMER0_IRQn);
	 }
	 switch(operation_mode){
	 	 case MODE_FIELD:			//store data
	 		 break;
	 	 case MODE_DEMO:			//send data and forget it
	 		 break;
	 }

//	 packet_handler();
//	 if(ble_soft_timer_Flag){
//		 sendData(data_ptr);
//		 ble_soft_timer_Flag = 0;
//	 }

	 //Go to sleep
	 EMU_EnterEM2(1);

  }
 };



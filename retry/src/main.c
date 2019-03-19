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
//#include "em_device.h"//
//#include "em_chip.h"//
#include "em_emu.h"//two cmu files
#include "em_gpio.h"//
#include "em_common.h"//
#include "em_cmu.h" //
#include "em_timer.h"//
#include "em_letimer.h"//
#include "ecode.h" //
//#include "efr32bg1b232f256gm48.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "infrastructure.h"//
#include "retargetserial.h"//
#include "math.h"//
//#include "InitDevice.h"

#include "adc_defines.h" //
#include "defines.h"//
#include "spi_functions.h" //
#include "adc_functions.h" //
#include "flash_functions.h"//
//moved to h file
#define	DATABUFFER_SIZE  4
#define ONE_MILLISECOND_BASE_VALUE_COUNT             1000
#define ONE_SECOND_TIMER_COUNT                        13672
#define ONE_MINUTE_MS							1000//60000

#define TEST 	1

#define letimerClkFreq  19000000

#define UINT64_TO_BITSTREAM(p, n)   { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
                                      *(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); \
									  *(p)++ = (uint8_t)((n) >> 32); *(p)++ = (uint8_t)((n) >> 40); \
                                      *(p)++ = (uint8_t)((n) >> 48); *(p)++ = (uint8_t)((n) >> 56); }

//#define FLOAT_TO_UINT32(m) (((uint32_t)(m) & 0x00FFFFFFU) | (uint32_t)((int32_t)(e) << 24))


// Desired letimer interrupt frequency (in Hz)
#define letimerDesired  1000

#define letimerCompare  letimerClkFreq / letimerDesired

//Poer
#define CS_PIN_ADC       (9)
#define CS_PORT_ADC      (gpioPortC)

#define CS_PIN_FLASH	(10)
#define CS_PORT_FLASH	(gpioPortC)

#define RX_PIN          (7)
#define RX_PORT         (gpioPortC)

#define SCLK_PIN        (8)
#define SCLK_PORT       (gpioPortC)

#define TX_PIN          (6)
#define TX_PORT         (gpioPortC)

#define MUX_POS_PIN		(10)
#define MUX_POS_PORT	(gpioPortC)

#define MUX_NEG_PIN		(11)
#define MUX_NEG_PORT	(gpioPortC)

#define LED0_PIN		(6)
#define	LED0_PORT		(gpioPortF)


uint32_t ms_counter = 0;
int measurementCount = 1;		//Will be used to calculate time for each
int readFlag = 0;



void usart_setup(){

	//Set up clocks
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	CMU_ClockEnable(cmuClock_HFPER, true);

	//Initialize with default settings and then update fields
	USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
	init.clockMode = usartClockMode3;
	init.msbf = true;		//send most signifiant bit dirst
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


//void TIMER0_IRQHandler(void) {
//	TIMER_IntClear(TIMER0, TIMER_IF_OF);      // Clear overflow flag
//	if(ms_counter < ONE_MINUTE_MS)
//		ms_counter++;                             // Increment counter
//	else{
//		GPIO_PinOutToggle(LED0_PORT, LED0_PIN);
//		//Recalibrate ADC channels (TODO: Change this to just gain and offset)
//		ms_counter = 0;
//		int readFlag = 1;
//		NVIC_DisableIRQ(TIMER0_IRQn);
//
//	}
//}

void LETIMER0_IRQHandler(void) {
	  // Clear the interrupt flag
	  	LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP0);
		if(ms_counter < ONE_MINUTE_MS)
			ms_counter++;
		else{
			readFlag = 1;
			ms_counter = 0;
			NVIC_DisableIRQ(LETIMER0_IRQn);
		}

}

//void timer0_setup(void){
//
//	CMU_ClockEnable(cmuClock_TIMER0, true);
//	CMU_OscillatorEnable(cmuOsc_LFXO,true,true);
//	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_LFXO);
//	CMU_ClockEnable(cmuClock_HF, true);
//
//
//	uint32_t val = CMU_ClockFreqGet(cmuClock_CORE)/1000; //get clock in kHz
//	TIMER_TopSet(TIMER0, val);	//Set timer TOP value
//
//	TIMER_Init_TypeDef timerInit =            // Setup Timer initialization
//			{ .enable = true,                  // Start timer upon configuration
//					.debugRun = true,   // Keep timer running even on debug halt
//					.prescale = timerPrescale1, // Use /1 prescaler...timer clock = HF clock = 1 MHz
//					.clkSel = timerClkSelHFPerClk , // Set HF peripheral clock as clock source
//					.fallAction = timerInputActionNone, // No action on falling edge
//					.riseAction = timerInputActionNone, // No action on rising edge
//					.mode = timerModeUp,              // Use up-count mode
//					.dmaClrAct = false,                    // Not using DMA
//					.quadModeX4 = false,               // Not using quad decoder
//					.oneShot = false,          // Using continuous, not one-shot
//					.sync = false, // Not synchronizing timer operation off of other timers
//			};
//	TIMER_IntEnable(TIMER0, TIMER_IF_OF);    // Enable Timer0 overflow interrupt
//	NVIC_EnableIRQ(TIMER0_IRQn);       		// Enable TIMER0 interrupt vector in NVIC
//	TIMER_Init(TIMER0, &timerInit);           // Configure and start Timer0
//}

void LETIMER_setup(void){
	 // Enable clock to the LE modules interface
	CMU_ClockEnable(cmuClock_HFLE, true);
	// Select LFXO for the LETIMER
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_ClockEnable(cmuClock_LETIMER0, true);


	//uint32_t val = CMU_ClockFreqGet(cmuClock_LFA)/1000; //get clock in kHz
	uint32_t val = CMU_ClockFreqGet(cmuClock_LFA); //get clock in kHz
	LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

	// Reload COMP0 on underflow, idle output, and run in repeat mode
	letimerInit.comp0Top  = true;
	letimerInit.ufoa0     = letimerUFOANone;
	letimerInit.repMode   = letimerRepeatFree;

	// Need REP0 != 0 to pulse on underflow
	LETIMER_RepeatSet(LETIMER0, 0, 1);

	// Compare on wake-up interval count
	LETIMER_CompareSet(LETIMER0, 0, val);

	LETIMER_Init(LETIMER0, &letimerInit);

	// Enable LETIMER0 interrupts for COMP0
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);

	// Enable LETIMER interrupts
	NVIC_ClearPendingIRQ(LETIMER0_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);
}
#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
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

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;
/* Our data*/
uint32_t data_out = 0x12358cdf;
/*flag for comparison*/
uint32_t comp_flag;
/*comparator value*/
uint32_t comparator;
void sendData(recorded_data * data_ptr)
{
//	uint32_t xread_m = ( data_ptr->xaxis & 0xffffff);
//	uint32_t xread_e = ( data_ptr->xaxis & (0xff << 24));

//	uint32_t x_value = xread_m | xread_e;

	uint32_t xread = data_ptr->xaxis;
	uint32_t yread = data_ptr->yaxis;
	uint32_t zread = data_ptr->zaxis;
	uint32_t temp  = data_ptr->temp;

	uint8_t flag = 0xff;

	uint8_t tempBuffer[20];
	uint8_t *p = tempBuffer;

	//uint32_t x_value = FLT_TO_UINT32(xread, -3);
	UINT32_TO_BITSTREAM(p, xread);
	UINT8_TO_BITSTREAM(p, flag);

	//uint32_t y_value = FLT_TO_UINT32(yread, -3);
	UINT32_TO_BITSTREAM(p,  yread);
	UINT8_TO_BITSTREAM(p, flag);

	//uint32_t z_value = FLT_TO_UINT32(zread, -3);
	UINT32_TO_BITSTREAM(p, zread);
	UINT8_TO_BITSTREAM(p, flag);

	//uint32_t temp_value = FLT_TO_UINT32(temp, -3);
	UINT32_TO_BITSTREAM(p, temp);
	UINT8_TO_BITSTREAM(p, flag);

	 gecko_cmd_gatt_server_send_characteristic_notification(
	      0xFF, gattdb_Data, 20, tempBuffer);
}

int main(void){
  /* Chip errata */
	  // Initialize device
	  initMcu();
	  // Initialize board
	  initBoard();
	  // Initialize application
	  initApp();
	  gecko_init(&config);
	  RETARGET_SerialInit();
  usart_setup();	//CMU_clock_enable happens here


  printf("\nHello World!\r\n");
  //timer0_setup();
  //LETIMER_setup();
  readFlag = 0;

  GPIO_PinModeSet(MUX_POS_PORT, MUX_POS_PIN, gpioModePushPull, 0);	//Pos diff mux
  GPIO_PinModeSet(MUX_NEG_PORT, MUX_NEG_PIN, gpioModePushPull, 0);	//Neg diff mux
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);	//Neg diff mux


  bool verified = adc_verify_communication();
  if(verified == true){
	 GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 1);	//Pos diff mux
	 printf("\r\n--->Connected to ADC via SPI<---");
	 adc_configure_channels();
	// int test = 0;
  }
  else{
	  printf("\r\n!!!No connection to ADC!!!");
	  //int test = 1; //DEBUG_BREAK
  }


  //Infinite loop
  while(1){

	 //Read ADC Interrupt
	 if(readFlag == 1){
		 //Take measurements
		 mux_select(0);
		 adc_configure_channels();
		 double xread = adc_read_data();

		 mux_select(1);
		 adc_configure_channels();
		 double yread = adc_read_data();

		 mux_select(2);
		 adc_configure_channels();
		 double zread = adc_read_data();

		 float tempread = adc_read_temperature();

		 //Store measurements
		 recorded_data new_data;
		 new_data.xaxis = xread;
		 new_data.yaxis = yread;
		 new_data.zaxis = zread;
		 new_data.temp = tempread;
		 new_data.measureNum = measurementCount;
		 recorded_data *data_ptr;
		 data_ptr = &new_data;

		 //Display measurements
		 printf("\n\n\r ---Read #%d---", measurementCount);
		 printf("\r\n x = %lu V", data_ptr->xaxis);
		 printf("\r\n y = %lu V", data_ptr->yaxis);
		 printf("\r\n z = %lu V", data_ptr->zaxis);
		 printf("\r\n Temperature = %.2lu C", data_ptr->temp);

		 //Update the number of times
		 measurementCount++;

		 readFlag = 0;
		 NVIC_EnableIRQ(LETIMER0_IRQn);
	 }
		 struct gecko_cmd_packet* evt;

		    /* Check for stack event. */
		    evt = gecko_wait_event();

		    /* Handle events */
		    switch (BGLIB_MSG_ID(evt->header)) {
		      /* This boot event is generated when the system boots up after reset.
		       * Do not call any stack commands before receiving the boot event.
		       * Here the system is set to start advertising immediately after boot procedure. */
		      case gecko_evt_system_boot_id:

		        /* Set advertising parameters. 100ms advertisement interval.
		         * The first parameter is advertising set handle
		         * The next two parameters are minimum and maximum advertising interval, both in
		         * units of (milliseconds * 1.6).
		         * The last two parameters are duration and maxevents left as default. */
		        gecko_cmd_le_gap_set_advertise_timing(0, 160, 160, 0, 0);

		        /* Start general advertising and enable connections. */
		        gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
		        break;

		      case gecko_evt_gatt_server_characteristic_status_id:
		    	 if((evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_Data)
		    		 && (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01))
		    	 {

		    		 if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x02)
		    		 {
		    		             /* Indications have been turned ON - start the repeating timer. The 1st parameter '32768'
		    		              * tells the timer to run for 1 second (32.768 kHz oscillator), the 2nd parameter is
		    		              * the timer handle and the 3rd parameter '0' tells the timer to repeat continuously until
		    		              * stopped manually.*/
		    		             gecko_cmd_hardware_set_soft_timer(3 * 32768, 0, 0);
		    		             printf("\ntimer started\n");
		    		 }

		    	 else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00)
		    	 	 	 {
		    	            /* Indications have been turned OFF - stop the timer. */
		    	            gecko_cmd_hardware_set_soft_timer(0, 0, 0);
		    	          }
		    	 }
		    	 // gecko
		   		 //sendData();

		    	 break;

		      case gecko_evt_hardware_soft_timer_id:
		            // if(readFlag == 1){
		            		 //Take measurements
		            		 mux_select(1);
		    	  	  	  	 adc_configure_channels();
		            		 uint32_t xread = adc_read_data();

		            		 mux_select(2);
		    	  	  	  	 adc_configure_channels();
		            		 uint32_t yread = adc_read_data();

		            		 mux_select(3);
		    	  	  	  	 adc_configure_channels();
		            		 uint32_t zread = adc_read_data();


		            		 uint32_t tempread = adc_read_temperature();

		            		 //Store measurements
		            		 recorded_data new_data;
		            		 new_data.xaxis = xread;
		            		 new_data.yaxis = yread;
		            		 new_data.zaxis = zread;
		            		 new_data.temp = tempread;
		            		 new_data.measureNum = measurementCount;
		            		 recorded_data *data_ptr;
		            		 data_ptr = &new_data;
		            		 printf("\n\n\r ---Read #%d---", measurementCount);
		            	     printf("\r\n x = %lu V", data_ptr->xaxis);
		            		 printf("\r\n y = %lu V", data_ptr->yaxis);
		            		 printf("\r\n z = %lu V", data_ptr->zaxis);
		            		 printf("\r\n Temperature = %.2lu C", data_ptr->temp);


		            		 sendData(data_ptr);
		             break;

		 /*     case gecko_evt_gatt_server_attribute_value_id:
		    	  if(evt->data.evt_gatt_server_attribute_value.attribute == gattdb_Data)
		    	    {
		    		  struct data_in_t* data_in = (struct data_in_t*)(evt->data.evt_gatt_server_attribute_value.value.data);
		    		  comparator = (data_in->dataIn)^data_out;
		    		  if (comparator == 0)
		    		  {
		    			  comp_flag = 0;
		    		  }
		    		  else
		    		  {
		    			  comp_flag = 1;
		    		  }
		       	   printf("Sent Data: %32d\r\n", data_out);
		    	   printf("Recieved Data: %32d\r\n", data_in->dataIn);
		    	   printf("Compared Value: %32d\r\n", comparator);
		    	   printf("comp flag: %32d\r\n", comp_flag);
		    	   //sendData();
		    	  }
		    	  break;
			*/


		      case gecko_evt_le_connection_closed_id:

		        /* Check if need to boot to dfu mode */
		        if (boot_to_dfu) {
		          /* Enter to DFU OTA mode */
		          gecko_cmd_system_reset(2);
		        } else {
		          /* Restart advertising after client has disconnected */
		          gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
		        }
		        break;

		      /* Events related to OTA upgrading
		         ----------------------------------------------------------------------------- */

		      /* Check if the user-type OTA Control Characteristic was written.
		       * If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
		      case gecko_evt_gatt_server_user_write_request_id:

		        if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
		          /* Set flag to enter to OTA mode */
		          boot_to_dfu = 1;
		          /* Send response to Write Request */
		          gecko_cmd_gatt_server_send_user_write_response(
		            evt->data.evt_gatt_server_user_write_request.connection,
		            gattdb_ota_control,
		            bg_err_success);

		          /* Close connection to enter to DFU OTA mode */
		          gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
		        }
		        break;

		      default:
		        break;
		    }



	 }

	 //Go to sleep
	 //EMU_EnterEM2(1);


  	};



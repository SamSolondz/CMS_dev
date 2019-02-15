#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_common.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_letimer.h"
#include "ecode.h"
#include "efr32bg1b232f256gm48.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "infrastructure.h"
#include "retargetserial.h"


#include "InitDevice.h"
#include "adc_defines.h"
#include "defines.h"
#include "spi_functions.h"
#include "adc_functions.h"

#define	DATABUFFER_SIZE  4
#define ONE_MILLISECOND_BASE_VALUE_COUNT             1000
#define ONE_SECOND_TIMER_COUNT                        13672
#define MILLISECOND_DIVISOR                           13.672

#define TEST 	1

uint16_t ms_counter = 0;
uint32_t x_values[4] = {0x0000,0x0000,0x0000,0x0000};
uint32_t y_values[4] = {0x0000,0x0000,0x0000,0x0000};
uint32_t z_values[4] = {0x0000,0x0000,0x0000,0x0000};
int i = 0;

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

	GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);	//CS
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
	}
}

void TIMER0_IRQHandler(void) {
	TIMER_IntClear(TIMER0, TIMER_IF_OF);      // Clear overflow flag
	if(ms_counter < 5000)
		ms_counter++;                             // Increment counter
	else{
		GPIO_PinOutToggle(LED0_PORT, LED0_PIN);
		adc_configure_channels();
		ms_counter = 0;
			float xread = adc_read_data();
//			uint32_t y = adc_read_data();
//			uint32_t z = adc_read_data();
			printf(", ");
			printf("x = %.8f V", xread);
//			printf("yaxis : %lu V/n", y);
//			printf("zaxis : %lu V/n", z);

	}
}

void timer0_setup(void){
	CMU_ClockEnable(cmuClock_TIMER0, true);
	TIMER_TopSet(TIMER0, 10000);	//Set timer TOP value

	TIMER_Init_TypeDef timerInit =            // Setup Timer initialization
			{ .enable = true,                  // Start timer upon configuration
					.debugRun = true,   // Keep timer running even on debug halt
					.prescale = timerPrescale1, // Use /1 prescaler...timer clock = HF clock = 1 MHz
					.clkSel = timerClkSelHFPerClk, // Set HF peripheral clock as clock source
					.fallAction = timerInputActionNone, // No action on falling edge
					.riseAction = timerInputActionNone, // No action on rising edge
					.mode = timerModeUp,              // Use up-count mode
					.dmaClrAct = false,                    // Not using DMA
					.quadModeX4 = false,               // Not using quad decoder
					.oneShot = false,          // Using continuous, not one-shot
					.sync = false, // Not synchronizing timer operation off of other timers
			};
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);    // Enable Timer0 overflow interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);       		// Enable TIMER0 interrupt vector in NVIC
	TIMER_Init(TIMER0, &timerInit);           // Configure and start Timer0
}

int main(void){
  /* Chip errata */
  CHIP_Init();
  usart_setup();	//CMU_clock_enable happens here
  RETARGET_SerialInit();
  printf("Hello World!\n");
  timer0_setup();

  GPIO_PinModeSet(MUX_POS_PORT, MUX_POS_PIN, gpioModePushPull, 0);	//Pos diff mux
  GPIO_PinModeSet(MUX_NEG_PORT, MUX_NEG_PIN, gpioModePushPull, 0);	//Neg diff mux
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);	//Neg diff mux


  bool verified = adc_verify_communication();
  if(verified == true){
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 1);	//Pos diff mux
	  printf("Connected to ADC via SPI");
	  adc_configure_channels();

	  mux_select(0);
	  float x_axis = adc_read_data();
	  printf("x: %.8f V\n", x_axis);

	  mux_select(1);
	  float y_axis = adc_read_data();
	  printf("y: %.8f V\n", y_axis);

	  mux_select(2);
	  float z_axis = adc_read_data();
	  printf("z: %.8f V\n", z_axis);

	  float temp = adc_read_temperature();

	  int test = 0;
  }
  else{
	  int test = 1; //DEBUG_BREAK
  }


  //Infinite loop
  while(1){
	  //float x_axis = adc_read_data();
	  //printf("%f\n", x_axis);
  };

}

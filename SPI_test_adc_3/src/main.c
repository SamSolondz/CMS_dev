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
#include "math.h"
#include "InitDevice.h"

#include "adc_defines.h"
#include "defines.h"
#include "spi_functions.h"
#include "adc_functions.h"
#include "flash_functions.h"

#define	DATABUFFER_SIZE  4
#define ONE_MILLISECOND_BASE_VALUE_COUNT             1000
#define ONE_SECOND_TIMER_COUNT                        13672
#define ONE_MINUTE_MS							15//60000

#define TEST 	1

#define letimerClkFreq  19000000

// Desired letimer interrupt frequency (in Hz)
#define letimerDesired  1000

#define letimerCompare  letimerClkFreq / letimerDesired

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

int main(void){
  /* Chip errata */
  CHIP_Init();
  usart_setup();	//CMU_clock_enable happens here
  RETARGET_SerialInit();
  printf("\r\n\nHello World!\n");
  //timer0_setup();
  LETIMER_setup();
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

		 //Display measurements
		 printf("\n\n\r ---Read #%d---", measurementCount);
		 printf("\r\n x = %.17lf V", xread);
		 printf("\r\n y = %.17lf V", yread);
		 printf("\r\n z = %.17lf V", zread);
		 printf("\r\n Temperature = %.2f C V", tempread);

		 //Update the number of times
		 measurementCount++;

		 readFlag = 0;
		 NVIC_EnableIRQ(LETIMER0_IRQn);
	 }

	 //Go to sleep
	 EMU_EnterEM2(1);


  	};

}

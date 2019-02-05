#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_common.h"
#include "em_cmu.h"
#include "em_usart.h"
#include "em_timer.h"
#include "ecode.h"
//#include "efr32bg1p333f256gm48.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "InitDevice.h"
#include "adc_defines.h"
#include "spidrv.h"
#include "defines.h"

#define	DATABUFFER_SIZE  4
#define ONE_MILLISECOND_BASE_VALUE_COUNT             1000
#define ONE_SECOND_TIMER_COUNT                        13672
#define MILLISECOND_DIVISOR                           13.672

#define TEST 	1


void TransferComplete(	SPIDRV_Handle_t handle,
						Ecode_t transferStatus,
						int itemsTransferred){};

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

void spi_write_uint8(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr){
	//Set CS low
	GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);

	int i = 0;
	while(i < number_of_bytes){
		  RXptr[i] = USART_SpiTransfer(USART1, TXptr[i]);
		  uint8_t testRX = RXptr[i];
		  uint8_t testTX = TXptr[i];
		  i++;
	  }

	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

	  return;

}

void spi_write_int32(int number_of_bytes, uint8_t * TXptr, int32_t * RXptr){
	//Set CS low
	GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);

	int i = 0;
	while(i < number_of_bytes){
		  RXptr[i] = USART_SpiTransfer(USART1, TXptr[i]);
		  uint8_t test = RXptr[i];
		  i++;
	  }

	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

	  return;

}

/*void clock_setup(){
	CMU_ClockEnable(cmuClock_TIMER1, true);

    // Create a timerInit object, based on the API default
      TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
      timerInit.prescale = timerPrescale1024;

      //Enable interrupt
      TIMER_IntEnable(TIMER1, TIMER_IF_OF);

      //Enable TIMER0 Interrupt vector in NVIC
      NVIC_EnableIRQ(TIMER1_IRQn);

      //Set TIMER top vlaue
      TIMER_TopSet(TIMER1, ONE_SECOND_TIMER_COUNT);

      TIMER_Init(TIMER1, &timerInit);

}*/

bool adc_verify_communication(){
	 //Set CS low

	  uint8_t TxBuffer[3] = {COMMS_READ_ID, 0x00, 0x00};
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};

	  spi_write_uint8(3, TxBuffer, RxBuffer);

	  if(RxBuffer[1] == ID_VAL1 && (RxBuffer[2] >> 4) == ID_VAL0)
		  return true;
	  else
		  return false;

}

void adc_configure_channels(){	//Write MSB first
	  //Channel 0 Configuration
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};

	  uint8_t Config0Buffer[3] = { CONFIGURE_CH0_WRITE,
			  	  	  	  	  CONFIGURE_CH0_BYTE1,
							  CONFIGURE_CH0_BYTE0};

	  uint8_t Config1Buffer[3] = { CONFIGURE_CH1_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  uint8_t Config2Buffer[3] = { CONFIGURE_CH2_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  uint8_t Config3Buffer[3] = { CONFIGURE_CH3_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  spi_write_uint8(3, Config0Buffer, RxBuffer);
	  spi_write_uint8(3, Config1Buffer, RxBuffer);
	  spi_write_uint8(3, Config2Buffer, RxBuffer);
	  spi_write_uint8(3, Config3Buffer, RxBuffer);


	  //Channel 0 Setup configuration
	  uint8_t SetupBuffer[3] = { SETUP_CONFIG_WRITE,
			  	  	  	  	  SETUP_CONFIG_0_BYTE1,
							  SETUP_CONFIG_0_BYTE0};

	  spi_write_uint8(3, SetupBuffer, RxBuffer);

	  //Channel 0 Filter configuration
	  uint8_t FilterBuffer0[3] = { FILTER_CONFIG_WRITE,
			  	  	  	  	  	  FILTER_CONFIG_BYTE1,
								  FILTER_CONFIG_BYTE0};

	  spi_write_uint8(3, FilterBuffer0, RxBuffer);

	  //Set ADC mode to continuous
	  /*uint8_t AdcBuffer[3] = { ADC_MODE_WRITE,
			  	  	  	  	  	  ADC_MODE_CONT_BYTE1,
								  ADC_MODE_CONT_BYTE0};

	  for(int i = 0; i < TxBuffer_size; i++){
		  USART_SpiTransfer(USART1, AdcBuffer[i]);
	  }*/

	  //Channel 0 Interface
	  uint8_t InterfaceBuffer[3] = { INTERFACE_WRITE,
			  	  	  	  	  	  	 INTERFACE_BYTE1,
									 INTERFACE_BYTE0};

	  spi_write_uint8(3, InterfaceBuffer, RxBuffer);


};

int32_t * adc_read_data(){
	  uint8_t TxBuffer[4] = {STATUS_READ, DATA_READ, 0x00, 0x00};
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};
	  int32_t channelRead = 0x0000;
	  int32_t dataBuffer[4] = {0x00, 0x00, 0x00, 0x00};

	  //Set ADC to single conversion mode
	  uint8_t ADCBuffer[3] = { ADC_MODE_WRITE,
			  	  	  	  	  	  ADC_MODE_SINGLE_BYTE1,
								  ADC_MODE_SINGLE_BYTE0};
	  spi_write_uint8(3, ADCBuffer, RxBuffer);

	  int notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);

	  //Tell ADC we are going to read the data
	  spi_write_uint8(1, TxBuffer + 1, RxBuffer);

	  //Check if !RDY has data
	  int notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
	  while(notRDY == 1){
		  notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
	  };

	  //Check the status register for which channel the data in the data register belongs to
	  //uint8_t channelNum = rxBuffer[j] & 0x2;



	  //Fill the data buffer with ADC value;
	  for(int i = 0; i < DATABUFFER_SIZE; i++){
			  uint8_t readBuffer[1] = {DATA_READ, DATA_READ, DATA_READ, DATA_READ};
			  int32_t dataBuffer[4] = {0x00, 0x00, 0x00, 0x00};
			  spi_write_int32(1, readBuffer, dataBuffer);
	  }

	  channelRead = (dataBuffer[0] << 24) + (dataBuffer[1] << 16)
	  							+ (dataBuffer[2] << 8) + (dataBuffer[3]);
	  return channelRead;
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

int main(void){
  /* Chip errata */
  CHIP_Init();

  usart_setup();	//CMU_clock_enable happens here
  GPIO_PinModeSet(MUX_POS_PORT, MUX_POS_PIN, gpioModePushPull, 0);	//Pos diff mux
  GPIO_PinModeSet(MUX_NEG_PORT, MUX_NEG_PIN, gpioModePushPull, 0);	//Neg diff mux

  bool verified = adc_verify_communication();
  if(verified == true){
	  adc_configure_channels();

	  mux_select(0);
	  uint32_t * x_axis = adc_read_data();

	  mux_select(1);
	  uint32_t * y_axis = adc_read_data();

	  mux_select(2);
	  uint32_t * z_axis = adc_read_data();

	  uint32_t test = 0;
  }

  //SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE)/1000);

  //Infinite loop
  /*while(1){
  };*/

}

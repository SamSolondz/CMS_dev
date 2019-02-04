#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_common.h"
#include "em_cmu.h"
<<<<<<< HEAD
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

=======
#include "ecode.h"
#include "spidrv.h"
#include "dmadrv.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "adc_defines.h"



#define true 1
#define false 0

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;
>>>>>>> 9b9e5e18b5d9f4bfabd1f3a6433dd433b2b9c1ae

void TransferComplete(	SPIDRV_Handle_t handle,
						Ecode_t transferStatus,
						int itemsTransferred){};
<<<<<<< HEAD

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

void spi_write(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr){
	  int i = 0;
	  while(i < number_of_bytes - 1){
		  *RXptr = USART_SpiTransfer(USART1, *TXptr);
		  TXptr++;
		  RXptr++;
		  i++;
	  }
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
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);

	  uint8_t TxBuffer[3] = {COMMS_READ_ID, 0x00, 0x00};
	  uint8_t RxBuffer[3] = {0x00, 0x01, 0x02};

	  RxBuffer[1] = 0;
	  int a = 1;

	  spi_write(3, TxBuffer, RxBuffer);
	  //RxBuffer[1] = *(v + 1 * sizeof(uint8_t));
	  //RxBuffer[2] = *(v + 2 * sizeof(uint8_t));
	  /*rxBuffer[0] = USART_SpiTransfer(USART1, TxBuffer[0]);
	  rxBuffer[1] = USART_SpiTransfer(USART1, TxBuffer[1]);
	  rxBuffer[2] = USART_SpiTransfer(USART1, TxBuffer[2]);*/


	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

	  if(RxBuffer[1] == ID_VAL1 && (RxBuffer[2] >> 4) == ID_VAL0)
		  return true;
	  else
		  return false;



}

void adc_configure_channels(){	//Write MSB first
	 //Set CS low
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);
	  //Channel 0 Configuration

	  uint8_t Config0Buffer[3] = { CONFIGURE_CH0_WRITE,
			  	  	  	  	  CONFIGURE_CH0_BYTE1,
							  CONFIGURE_CH0_BYTE0};

	  uint8_t RxBuffer[3] = {0xff, 0xff, 0xff};


	  uint8_t Config1Buffer[3] = { CONFIGURE_CH1_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  uint8_t Config2Buffer[3] = { CONFIGURE_CH2_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  uint8_t Config3Buffer[3] = { CONFIGURE_CH3_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  spi_write(3, Config0Buffer, RxBuffer);
	  spi_write(3, Config1Buffer, RxBuffer);
	  spi_write(3, Config2Buffer, RxBuffer);
	  spi_write(3, Config3Buffer, RxBuffer);


	  //Channel 0 Setup configuration
	  uint8_t SetupBuffer[3] = { SETUP_CONFIG_WRITE,
			  	  	  	  	  SETUP_CONFIG_0_BYTE1,
							  SETUP_CONFIG_0_BYTE0};

	  spi_write(3, SetupBuffer, RxBuffer);

	  //Channel 0 Filter configuration
	  uint8_t FilterBuffer0[3] = { FILTER_CONFIG_WRITE,
			  	  	  	  	  	  FILTER_CONFIG_BYTE1,
								  FILTER_CONFIG_BYTE0};

	  spi_write(3, FilterBuffer0, RxBuffer);
//		  USART_SpiTransfer(USART1, FilterBuffer[i]);
//	  }

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

	  spi_write(3, InterfaceBuffer, RxBuffer);



	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

};

uint32_t * adc_read_data(){
	  uint32_t static channelReads[3] = {0x0000, 0x0001, 0x0002};
	  uint8_t TxBuffer[4] = {STATUS_READ, DATA_READ, 0x00, 0x00};
	  uint8_t rxBuffer[3] = {0xff, 0xff, 0xff};
	  uint8_t dataBuffer0[DATABUFFER_SIZE] = {0x00, 0x00, 0x00, 0x00};
	  uint8_t dataBuffer1[DATABUFFER_SIZE] = {0x00, 0x00, 0x00, 0x00};
	  uint8_t dataBuffer2[DATABUFFER_SIZE] = {0x00, 0x00, 0x00, 0x00};
	  uint8_t j = 0;

	  //Set CS low
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);

	  //Set ADC to single conversion mode
	  uint8_t AdcBuffer[3] = { ADC_MODE_WRITE,
			  	  	  	  	  	  ADC_MODE_SINGLE_BYTE1,
								  ADC_MODE_SINGLE_BYTE0};
	  uint8_t AdcBuffer_size = 3;
	  for(int i = 0; i < AdcBuffer_size; i++){
		  USART_SpiTransfer(USART1, AdcBuffer[i]);
	  }

	  while(j < 3){

		  /*while((rxBuffer[j] >> 7) != 0x0){
			  USART_SpiTransfer(USART1, TxBuffer[0]);
			  rxBuffer[j] = USART_SpiTransfer(USART1, 0x80);
		*/
	  	  uint8_t notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
		  while(notRDY == 1){
			  notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
		  };

		  //Check the status register for which channel the data in the data register belongs to
		  //uint8_t channelNum = rxBuffer[j] & 0x2;

		  //Tell ADC we are going to read the data
		  USART_SpiTransfer(USART1, TxBuffer[1]);
		  //Fill the data buffer with ADCn
		  for(int i = 0; i < DATABUFFER_SIZE; i++){
			  switch(j){
			  	  case 0:
			  		  dataBuffer0[i] = USART_SpiTransfer(USART1, 0x80);
			  		  break;
			  	  case 1:
			  		  dataBuffer1[i] = USART_SpiTransfer(USART1, 0x80);
			  		  break;
			  	  case 2:
			  		  dataBuffer2[i] = USART_SpiTransfer(USART1, 0x80);
			  		  break;
			  }
		  }

		  j = j + 1;
	  }

	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

	  channelReads[0] = (dataBuffer0[0] << 24) + (dataBuffer0[1] << 16)
	  							  + (dataBuffer0[2] << 8) + (dataBuffer0[3]);
	  channelReads[1] = (dataBuffer1[0] << 24) + (dataBuffer1[1] << 16)
	  							  + (dataBuffer1[2] << 8) + (dataBuffer1[3]);
	  channelReads[2] = (dataBuffer2[0] << 24) + (dataBuffer2[1] << 16)
	  							  + (dataBuffer2[2] << 8) + (dataBuffer2[3]);

	  return channelReads;
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

  usart_setup();	//CMU_clockenable happens here
  GPIO_PinModeSet(MUX_POS_PORT, MUX_POS_PIN, gpioModePushPull, 0);	//Pos diff mux
  GPIO_PinModeSet(MUX_NEG_PORT, MUX_NEG_PIN, gpioModePushPull, 0);	//Neg diff mux

  bool verified = adc_verify_communication();
  if(verified == true){
	  adc_configure_channels();
	  //if(USART1->ROUTEPEN) if !RDY is low, read the data
	  uint32_t * read = adc_read_data();
	  uint32_t read0 = *(read + 0);
	  uint32_t read1 = *(read + 1);
	  uint32_t read2 = *(read + 2);
	  uint32_t test = 0;
  }

  //SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE)/1000);

  //Infinite loop
  /*while(1){
  };*/

=======
void spidrv_setup(){
	//Set up clocks
	CMU_ClockEnable(cmuClock_GPIO, true);


	//Set PIN to monitor ADC (!RDY)
	GPIO_PinModeSet(gpioPortA, 5, gpioModeInput, 1);

	//Initialize and enable SPIDRV
	SPIDRV_Init_t initData = SPIDRV_MASTER_USART0;
	initData.clockMode = spidrvClockMode3;
	//initData.csControl = spidrvCsControlApplication 	// makes SPIDRV not automatically set CS pin

	//Initialize a SPI driver instance
	SPIDRV_Init(handle, &initData);

};

//Ecode_t spi_write(const void * bufferTransmit, int sizeOfWrite){
//	uint8_t tx_data[4];
//
//	Ecode_t txError = SPIDRV_MTransmitB(handle, &tx_data, sizeOfWrite);
//
//	return txError;
//};
//
//Ecode_t spi_read(const void * bufferReceive, int sizeOfRead){
//	uint8_t rx_data[4];
//
//	Ecode_t rxError = SPIDRV_MReceiveB(handle, &rx_data, sizeOfRead);
//
//	return rxeError;
//};

bool adc_verify_communication(){
	  uint8_t tx_data[1];
	  uint8_t rx_data[2];

	  //Read ADC ID register to verify communication
	  tx_data[0] = COMMS_READ_ID;
	  Ecode_t txError = SPIDRV_MTransmitB(handle, &tx_data, 1);
	  Ecode_t rxError = SPIDRV_MReceiveB(handle, &rx_data, 2);

	  //Check txError
	  if(txError == ECODE_EMDRV_SPIDRV_ABORTED){
		  printf("Transfer Aborted");
		  return false;
	  }
	  else{
		  printf("Transfer Success");
	  }

	  //Check rxError
	  if(rxError == ECODE_EMDRV_SPIDRV_ABORTED){
		  printf("Receive Aborted");
		  return false;
	  }
	  else{
		  printf("Receive Success");
			  //Check that ID register has correct value
			  if(rx_data[3] == ID_VAL3 && rx_data[2] == ID_VAL2 && rx_data[1] == ID_VAL1){
				 printf("ID register verified");
			  }
			  else{
				  printf("ID register not verified");
				  char buffer[50];
				  int rxVal_3 = rx_data[3];
				  int rxVal_2 = rx_data[2];
				  int rxVal_1 = rx_data[1];
				  int rxVal_0 = rx_data[0];
				  int n = sprintf(buffer, "0x%d %d %d %d", rxVal_3, rxVal_2, rxVal_1, rxVal_0);
				  printf("ID register contains: %d", n);
			  }
		  return true;
	  }

};

void adc_configure_channels(){
	  uint8_t tx_data[2];
	  tx_data[0] = COMMS_WRITE_CH0;
	  //tx_data[1] = ; Enable channel 0
	  Ecode_t txError = SPIDRV_MTransmitB(handle, &tx_data, 1);
};

int main(void)
{
  /* Chip errata */
  CHIP_Init();
  spidrv_setup();
  //setup_utilities();
  //delay(100);
  bool connected;

  connected = adc_verify_communication();

  if(connected == true)
	  adc_configure_channels();

  /* Infinite loop */
  while (1) {
  }
>>>>>>> 9b9e5e18b5d9f4bfabd1f3a6433dd433b2b9c1ae
}

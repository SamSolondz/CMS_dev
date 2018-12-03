#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_common.h"
#include "em_cmu.h"
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

void TransferComplete(	SPIDRV_Handle_t handle,
						Ecode_t transferStatus,
						int itemsTransferred){};
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
}

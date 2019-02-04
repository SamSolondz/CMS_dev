#include "em_device.h"
#include "em_chip.h"
#include "em_core.h"
#include "adc_defines.h"
#include "spidrv.h"
#include "spidrv_config.h"
#include "rtcdriver.h"
#include "rtcdrv_config.h"
#include "em_gpio.c"
#include "stdint.h"
#include "stdio.h"

#define true 1
#define false 0

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

void TransferComplete(	SPIDRV_Handle_t handle,
						Ecode_t transferStatus,
						int itemsTransferred){};
void spidrv_setup(){
	SPIDRV_HandleData_t handleData;
	SPIDRV_Handle_t handle = &handleData;

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

void verify_adc_communication(){
	  uint8_t tx_data[4];
	  uint8_t rx_data[4];

	  //Read ADC ID register to verify communication
	  tx_data[0] = 0x0;
	  Ecode_t txError = SPIDRV_MTransmitB(handle, &tx_data, 1);
	  Ecode_t rxError = SPIDRV_MReceiveB(handle, &rx_data, 4);
	  /*if(transferError == ECODE_EMDRV_SPIDRV_ABORTED){
		  printf("Transfer Aborted");
	  }
	  else
		  printf("Transfer Success");*/
	  if(rx_data[3] == ID_VAL3 && rx_data[2] == ID_VAL2 && rx_data[1] == ID_VAL1){
		 printf("ID register verified");
	  }
	  else
		  printf("ID register not verified");

};

int main(void)
{
  /* Chip errata */
  CHIP_Init();
  spidrv_setup();
  setup_utilities();
  delay(100);

  verify_adc_communication();

  /* Infinite loop */
  while (1) {
  }
}

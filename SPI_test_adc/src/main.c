#include "em_device.h"
#include "em_chip.h"

#include "adc_defines.h"
#include "spidrv.h"
#include "spidrv_config.h"
#include "rtcdriver.h"
#include "rtcdrv_config.h"
#include "em_gpio.h"
#include "stdint.h"
#include "stdio.h"

typedef int bool;
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
	GPIO_PinModeSet(gpioPortA, 5, gpioModeInput); //TODO: pullup setting?

	//Initialize and enable SPIDRV
	SPIDRV_Init_t initData = SPIDRV_MASTER_USART0;
	initData.clockMode = spidrvClockMode3;
	//initData.csControl = spidrvCsControlApplication 	// makes SPIDRV not automatically set CS pin

	//Initialize a SPI driver instance
	SPIDRV_Init(handle, &initData);

};


int main(void)
{
  /* Chip errata */
  CHIP_Init();
  spidrv_setup();
  setup_utilities();
  delay(100);

  uint8_t tx_data[4];
  uint8_t rx_data[4];


  //Read ADC ID register to verify communication
  tx_data[0] = 0x0;
  Ecode_t transmitError = SPIDRV_MTransmitB(handle, &tx_data, 4);
  Ecode_t recieveError = SPIDRV_MRecieveB(handle, &rx_data, 4);
  /*if(transferError == ECODE_EMDRV_SPIDRV_ABORTED){
	  printf("Transfer Aborted");
  }
  else
	  printf("Transfer Success");*/




  /* Infinite loop */
  while (1) {
  }
}

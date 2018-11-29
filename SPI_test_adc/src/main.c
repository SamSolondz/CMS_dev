#include "em_device.h"
#include "em_chip.h"
#include "spidrv.h"
#include "spidrv_config.h"
#include "rtcdriver.h"
#include "rtcdrv_config.h"
#include "gpio.h"

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



	SPIDRV_Init_t initData = SPIDRV_MASTER_USART0;
	//Initialize a SPI driver instance
	SPIDRV_Init(handle, &initData);

};

int main(void)
{
  /* Chip errata */
  CHIP_Init();

  enter_DefaultMode_from_RESET();

  uint8_t buffer[10];


  //Transmit data using a Blocking transmit function
  SPIDRV_MTransmitB(handle, buffer, 10);

  //Transmit data using a callback to catch transfer complete
  SPIDRV_MTransmit(handle, buffer, 10, TransferComplete);


  /* Infinite loop */
  while (1) {
  }
}

/*
 * flash_functions.c
 *
 *  Created on: Mar 5, 2019
 *      Author: Sam
 */
#include "flash_functions.h"

enum spi_slave flash = FLASH_SEL;

bool flash_verify_communication(){
	  uint8_t TxBuffer[3] = {READ_JEDEC, 0x00, 0x00, 0x00, 0x00};
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00, 0x00, 0x00}; //MfrID, DeviceID_1, DeviceID_0
	  spi_write_uint8(5, TxBuffer, RxBuffer, flash);

	  if(RxBuffer[2] == JEDEC_MFR_ID && RxBuffer[3] == JEDEC_ID_VAL1 && RxBuffer[4] == JEDEC_ID_VAL0)
		  return true;
	  else
		  return false;
};

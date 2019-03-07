/*
 * flash_functions.c
 *	Functions that interface with W25N01GVZEIG 1G flash chip
 *  Created on: Mar 5, 2019
 *      Author: Sam
 *
 */
#include "flash_functions.h"

enum spi_slave flash = FLASH_SEL;

/*Reset the flash chip*/
void flash_reset(){	//5us~500us reset period
	  uint8_t TxBuffer[1] = {FLASH_RESET};
	  uint8_t RxBuffer[1] = {0x00};
	  spi_write_uint8(1, TxBuffer, RxBuffer, flash);
}

/*Read the JEDEC ID to verify SPI communication*/
bool flash_verify_communication(){
	  uint8_t TxBuffer[5] = {READ_JEDEC, 0x00, 0x00, 0x00, 0x00};
	  uint8_t RxBuffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00}; //Dummy, Dummy, MfrID, DeviceID_1, DeviceID_0
	  spi_write_uint8(5, TxBuffer, RxBuffer, flash);

	  if(RxBuffer[2] == JEDEC_MFR_ID && RxBuffer[3] == JEDEC_ID_VAL1 && RxBuffer[4] == JEDEC_ID_VAL0)
		  return true;
	  else
		  return false;
};

/*Read a Status Register
 *(Protection Register, Configuration Register, or Status Register)*/
uint8_t flash_read_status_register(int status_register){
	//Choose the status register to write too=
	uint8_t sr_address;
	switch(status_register){
	case 1:
		sr_address = FLASH_SR1;	//Protection Register
		break;
	case 2:
		sr_address = FLASH_SR2; //Configuration Register
		break;
	case 3:
		sr_address = FLASH_SR3;	//Status register
		break;
	default:
		printf("\n\rFlash status register read error\n");
		return 0;
		break;
	}
	uint8_t TxBuffer[3] = {FLASH_READ_STATUS, sr_address, 0x00};
	uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};
	spi_write_uint8(3, TxBuffer, RxBuffer, flash);

	return RxBuffer[2];
};

/*Write a Status Register
 * (Protection Register or Configuration Register only)*/
void flash_write_status_register(int status_register, uint8_t sr_value){
	//Choose the status register to write too
	uint8_t sr_address;
	switch(status_register){
	case 1:
		sr_address = FLASH_SR1;	//Protection Register
		break;
	case 2:
		sr_address = FLASH_SR2; //Configuration Register
		break;
	default:
		printf("\n\rFlash status register write error\n");
		return;
		break;
	}
	uint8_t TxBuffer[3] = {FLASH_WRITE_STATUS, sr_address, sr_value};
	uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};
	spi_write_uint8(3, TxBuffer, RxBuffer, flash);
}

/*Write Enable sets the Write Enable Latch bit in the Status Register to 1*/
void flash_write_enable(){
	uint8_t TxBuffer[1] = {FLASH_WRITE_ENABLE};
	uint8_t RxBuffer[1] = {0x00};
	spi_write_uint8(1, TxBuffer, RxBuffer, flash);
}

/*Write Disable sets the Write Enable Latch bit in the Status Register to 0*/
void flash_write_disable(){
	uint8_t TxBuffer[1] = {FLASH_WRITE_DISABLE};
	uint8_t RxBuffer[1] = {0x00};
	spi_write_uint8(1, TxBuffer, RxBuffer, flash);
}

/*Create a link between a logical block address (LBA) and physical block address (PBA)
 * LBA (Bad) is replaced by PBA (Good) */
void flash_bad_block_link(uint16_t logical_addr, uint16_t physical_addr){
	uint8_t LBA1 = (logical_addr >> 8);
	uint8_t LBA0 = (logical_addr & 0x00ff);
	uint8_t PBA1 = (physical_addr >> 8);
	uint8_t PBA0 = (physical_addr & 0x00ff);

	flash_write_enable();
	uint8_t TxBuffer[5] = {FLASH_BAD_BLOCK_MAN, LBA1, LBA0, PBA1, PBA0};
	uint8_t RxBuffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	spi_write_uint8(5, TxBuffer, RxBuffer, flash);
}

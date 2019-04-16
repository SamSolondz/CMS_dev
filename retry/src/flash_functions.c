/*
 * flash_functions.c
 *	Functions that interface with W25N01GVZEIG 1G flash chip
 *  Created on: Mar 5, 2019
 *      Author: Sam
 *
 */
#include "spi_functions.h"
#include "flash_functions.h"
#include "pin_def.h"


enum spi_slave flash = FLASH_SEL;

/*Reset the flash chip*/
void flash_reset(){	//5us~500us reset period
	  uint8_t TxBuffer[1] = {FLASH_RESET};
	  uint8_t RxBuffer[1] = {0x00};
	  spi_write_uint8(1, TxBuffer, RxBuffer, flash);
}

/*Read the JEDEC ID to verify SPI communication*/
void flash_verify_communication(){
	  uint8_t TxBuffer[5] = {READ_JEDEC, 0x00, 0x00, 0x00, 0x00};
	  uint8_t RxBuffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00}; //Dummy, Dummy, MfrID, DeviceID_1, DeviceID_0

	  spi_write_uint8(5, TxBuffer, RxBuffer, flash);

	  if(RxBuffer[2] == JEDEC_MFR_ID && RxBuffer[3] == JEDEC_ID_VAL1 && RxBuffer[4] == JEDEC_ID_VAL0){
		  //GPIO_PinOutSet(LED_BLE_PORT, LED_BLE_PIN);
		  printf("\r\n--->Connected to FLASH via SPI<---");
	  }
	  else
		  printf("\r\n!!!No connection to FLASH!!!");

};

/*Read a Status Register
 *(Protection Register, Configuration Register, or Status Register)*/
uint8_t flash_read_status_register(int status_register){
	//Choose the status register to write too
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

bool flash_busy(){
	uint8_t sr3 = flash_read_status_register(3);
	return ((sr3 & 0x01) == 1) ? true : false;
}

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

//flash_bad_block_read_LUT(){
//	uint8_t TxBuffer[1] = {FLASH_READ_BBM_LUT, 0x00, 0x00};
//	uint8_t RxBuffer[1] = {0x00};
//	spi_write_uint8(1, TxBuffer, RxBuffer, flash);
//
//
//}

/*Erase memory within a specified block (64-Pages, 128kB)*/
void flash_block_erase(uint16_t page_address){
	uint8_t page_addr1 = (page_address >> 8);
	uint8_t page_addr0 = (page_address & 0x00ff);

	flash_write_enable();
	uint8_t TxBuffer[4] = {FLASH_BLOCK_ERASE, 0x00, page_addr1, page_addr0};
	uint8_t RxBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	spi_write_uint8(4, TxBuffer, RxBuffer, flash);
	while(flash_read_status_register(3) & FLASH_BUSY_BIT){}
}

/*Load program data into the Data buffer for writing data*/
//void flash_load_program_data(uint16_t column_address, recorded_data *data_ptr){
void flash_load_program_data_8(uint16_t column_address, uint8_t data){
	uint8_t col_addr1 = (column_address >> 8);
	uint8_t col_addr0 = (column_address & 0x00ff);

	flash_write_enable();
//	uint8_t TxBuffer[4] = {FLASH_LOAD_PROGRAM_DATA, col_addr1, col_addr1,
//							data_ptr->xaxis,
//							data_ptr->yaxis,
//							data_ptr->zaxis,
//							data_ptr->temp};
//
//	uint8_t RxBuffer[4] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	uint8_t TxBuffer[4] = {FLASH_RANDOM_LOAD_PROGRAM_DATA, col_addr1, col_addr0, data};
	uint8_t RxBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	spi_write_uint8(4, TxBuffer, RxBuffer, flash);
}

void flash_load_program_data_32(uint16_t column_address, uint32_t data){
	uint8_t col_addr1 = (column_address >> 8);
	uint8_t col_addr0 = (column_address & 0x00ff);
	uint8_t data0 = (data & 0xff);
	uint8_t data1 = (data & 0xff00) >> 8;
	uint8_t data2 = (data & 0xff0000) >> 16;
	uint8_t data3 = (data & 0xff000000) >> 24;

	flash_write_enable();

	uint8_t TxBuffer[7] = {FLASH_RANDOM_LOAD_PROGRAM_DATA, col_addr1, col_addr0, data3, data2, data1, data0};
	uint8_t RxBuffer[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	spi_write_uint8(7, TxBuffer, RxBuffer, flash);
}


/*Write the data in the Data buffer*/
void flash_program_execute(uint16_t page_address){
	uint8_t page_addr1 = (page_address >> 8);
	uint8_t page_addr0 = (page_address & 0x00ff);

	flash_write_enable();
	uint8_t TxBuffer[4] = {FLASH_PROGRAM_EXECUTE, 0x00, page_addr1, page_addr0};
	uint8_t RxBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	spi_write_uint8(4, TxBuffer, RxBuffer, flash);
}

/*Load a page of data into the Data buffer for reading data*/
void flash_load_read_data(uint16_t page_address){
	uint8_t page_addr1 = (page_address >> 8);
	uint8_t page_addr0 = (page_address & 0x00ff);

	uint8_t TxBuffer[4] = {FLASH_PAGE_DATA_READ, 0x00, page_addr1, page_addr0};
	uint8_t RxBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	spi_write_uint8(4, TxBuffer, RxBuffer, flash);
}

/*Read the data in the Data Buffer, BUF = 1
 *Byte_count can be no larger than */
void flash_read_excute_BUF1(uint16_t column_address, int byte_count, uint8_t *data_read){
	uint8_t col_addr1 = (column_address >> 8);
	uint8_t col_addr0 = (column_address & 0x00ff);

	int original_size = 4;	//Number of commands to start a read
	int new_size = original_size + byte_count;

	//Load the TxBuffer with the appropriate commands
	uint8_t TxBuffer[new_size];
	TxBuffer[0] = FLASH_READ_DATA;
	TxBuffer[1] = col_addr1;
	TxBuffer[2] = col_addr0;
	TxBuffer[3] = DUMMY_BYTE;

	uint8_t RxBuffer[new_size];
	RxBuffer[0] = DUMMY_BYTE;
	RxBuffer[1] = DUMMY_BYTE;
	RxBuffer[2] = DUMMY_BYTE;
	RxBuffer[3] = DUMMY_BYTE;

	//Initialize arrays to 0;
	for(int i = original_size; i < new_size; i++){
		TxBuffer[i] = 0x00;
		RxBuffer[i] = 0x00;
	}

	spi_write_uint8(new_size, TxBuffer, RxBuffer, flash);

	//uint8_t data_read[byte_count];
	for(int i = 0; i < byte_count; i++){
		data_read[i] = RxBuffer[i + original_size];
	}
	return;
}


//Writes 32 bits and updates column address
void flash_write_data32(uint32_t data, uint16_t * column_address, uint16_t * page_address){
	flash_write_enable();
	flash_load_program_data_32(*column_address, data);
	flash_program_execute(*page_address);
	while(flash_read_status_register(3) & FLASH_BUSY_BIT){}
	flash_write_disable();

	//When we hit the last column address of the page, reset column and increment page
	if(*column_address == FLASH_FINAL_COLUMN_ADDR){
		*page_address = *page_address + 1;
		*column_address = 0;

		//Update page and column in FLASH
		flash_write_data32_direct(current_page, FLASH_LAST_PAGE, FLASH_PARAM_PAGE);
		flash_write_data32_direct(current_column, FLASH_LAST_COLUMN, FLASH_PARAM_PAGE);
	}
	else
	{
		*column_address  = *column_address + 4;
		//Update column in FLASH
		flash_write_data32_direct(current_column, FLASH_LAST_COLUMN, FLASH_PARAM_PAGE);
	}
}

//Writes 32 bits no updating
void flash_write_data32_direct(uint32_t data, uint16_t column_address, uint16_t page_address){
	flash_write_enable();
	flash_load_program_data_32(column_address, data);
	flash_program_execute(page_address);
	while(flash_read_status_register(3) & FLASH_BUSY_BIT){}
	flash_write_disable();
}


void flash_write_data_TEST(){

	flash_write_status_register(1, 0x00);	//Disable all write protection
	flash_write_enable();
	flash_block_erase(0x0000);	//Erase the first page
	while(flash_read_status_register(3) & FLASH_BUSY_BIT){}	//Wait until erase operation has finished

	//Load some test values into the data buffer
//	flash_load_program_data_8(0x000, 0xDE);
//	flash_load_program_data_8(0x001, 0xAA);
//	flash_load_program_data_8(0x002, 0xAA);
//	flash_load_program_data_8(0x003, 0xEF);
	flash_load_program_data_32(0x0000, 0xDEADBEEF);

	flash_program_execute(0x0000);
	while(flash_read_status_register(3) & FLASH_BUSY_BIT){}	//Wait until erase operation has finished

	flash_write_disable();
}
void flash_read_data_TEST(uint8_t *data_read){
	flash_write_enable();
	//flash_write_status_register(2, 0x00);	//Set BUF = 1 for buffer read mode
	flash_load_read_data(0x0000); //Load the page into the data buffer
	while(flash_read_status_register(3) & FLASH_BUSY_BIT){}	//Wait until operation has finished
	flash_read_excute_BUF1(0x0000, 4, data_read); 	//Read the 4 bytes of 0xDEADBEEF

//	if(data_read[0] == 0xDE && data_read[1] == 0xAD && data_read[2] == 0xBE && data_read[3] == 0xEF)
//	{
//		//GPIO_PinOutSet(LED_BLE_PORT, LED_BLE_PIN);
//	}

};

void flash_read_32(uint32_t page_address, uint32_t column_address, uint8_t *data_read)
{
	flash_write_enable();
	flash_load_read_data(page_address); //Load the page into the data buffer
	while(flash_read_status_register(3) & FLASH_BUSY_BIT){}	//Wait until operation has finished
	flash_read_excute_BUF1(0x0000, 4, data_read); 	//Read the 4 bytes of 0xDEADBEEF
}


uint32_t flash_read_data(uint32_t page_address, uint32_t column_address)
{
	uint8_t data_read[4];
	flash_read_32(page_address, column_address, data_read);
	uint32_t read = (data_read[0] + (data_read[1]<<8)
						+(data_read[2]<<16) + (data_read[3] << 24));
	return read;
}



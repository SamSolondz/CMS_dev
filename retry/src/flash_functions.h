/*
 * flash_functions.h
 *
 *  Created on: Mar 5, 2019
 *      Author: Sam Solondz
 */
#ifndef INC_FLASH_FUNCTIONS_H_
#define INC_FLASH_FUNCTIONS_H_

#include "flash_defines.h"
#include "spi_functions.h"

#include <stdint.h>
#include <stdbool.h>

extern uint16_t current_page;
extern uint16_t current_column;

void flash_verify_communication();
void flash_reset();
uint8_t flash_read_status_register(int status_register);
bool flash_read_busy_bit();
void flash_write_status_register(int status_register, uint8_t sr_value);
void flash_write_enable();
void flash_write_disable();
void flash_bad_block_link(uint16_t logical_addr, uint16_t physical_addr);
void flash_block_erase(uint16_t page_address);

void flash_load_program_data_32(uint16_t column_address, uint32_t data);
void flash_load_program_data_8(uint16_t column_address, uint8_t data);
void flash_program_execute(uint16_t page_address);
void flash_load_read_data(uint16_t page_address);
void flash_read_excute_BUF1(uint16_t column_address, int byte_count, uint8_t *data_read);

void flash_write_data_TEST(uint32_t data);
void flash_read_data_TEST();

void flash_write_data32(recorded_data * data_struct, uint16_t * column_address, uint16_t * page_address);
void flash_write_data32_direct(uint32_t data, uint16_t column_address, uint16_t page_address);
void flash_read_32(uint16_t page_address, uint16_t column_address, uint8_t *data_read);
uint32_t flash_read_data(uint16_t page_address, uint16_t column_address);
#endif /* INC_FLASH_FUNCTIONS_H_ */

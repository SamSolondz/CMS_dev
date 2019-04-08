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

void flash_verify_communication();
void flash_reset();
uint8_t flash_read_status_register(int status_register);
bool flash_read_busy_bit();
void flash_write_status_register(int status_register, uint8_t sr_value);
void flash_write_enable();
void flash_write_disable();
void flash_bad_block_link(uint16_t logical_addr, uint16_t physical_addr);
void flash_block_erase(uint16_t page_address);

void flash_load_program_data(uint16_t column_address);
void flash_program_execute(uint16_t page_address);
void flash_load_read_data(uint16_t page_address);
uint8_t * flash_read_excute_BUF1(uint16_t column_address, int byte_count);
#endif /* INC_FLASH_FUNCTIONS_H_ */

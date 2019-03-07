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
#include <stdbool.h>

bool flash_verify_communication();
void flash_reset();
uint8_t flash_read_status_register(int status_register);
void flash_write_status_register(int status_register, uint8_t sr_value);
void flash_write_enable();
void flash_write_disable();
void flash_bad_block_link(uint16_t logical_addr, uint16_t physical_addr);

#endif /* INC_FLASH_FUNCTIONS_H_ */

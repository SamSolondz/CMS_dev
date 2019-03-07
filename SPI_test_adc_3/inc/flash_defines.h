/*
 * flash_defines.h
 *
 *  Created on: Mar 5, 2019
 *      Author: Sam
 */

#ifndef INC_FLASH_DEFINES_H_
#define INC_FLASH_DEFINES_H_

#define FLASH_RESET		0xff

#define READ_JEDEC 		0x9f
#define JEDEC_MFR_ID	0xef
#define JEDEC_ID_VAL1  	0xaa
#define JEDEC_ID_VAL0 	0x21

#define FLASH_READ_STATUS	0x0f
#define	FLASH_WRITE_STATUS	0x1f
#define FLASH_SR1			0xA0 //"Axh" is x don't care?
#define FLASH_SR2			0xB0
#define FLASH_SR3			0xC0

#define FLASH_WRITE_ENABLE	0x06
#define FLASH_WRITE_DISABLE	0x04

#define FLASH_BAD_BLOCK_MAN	0xa1
#define FLASH_READ_BBM_LUT	0xa5

#define	FLASH_128KB_BLOCK_ERASE	0xd8

#endif /* INC_FLASH_DEFINES_H_ */

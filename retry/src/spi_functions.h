#ifndef INC_SPI_FUNCTIONS_H_
#define INC_SPI_FUNCTIONS_H_

#include "em_usart.h"
#include "em_gpio.h"
#include "adc_defines.h"
//#include "InitDevice.h"
#include <stdio.h>
enum spi_slave{ADC_SEL, FLASH_SEL};

void spi_write_uint8(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr, int device);
void spi_write_cal(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr, int device);
void spi_read_data_reg(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr, int device);
void read_offset_gain();

typedef struct {
	uint32_t xaxis;
	uint32_t yaxis;
	uint32_t zaxis;
	uint32_t temp;
	uint32_t measureNum; 	//Will be used to timestamp data on Android app.
}recorded_data;


#endif

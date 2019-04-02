#ifndef INC_ADC_FUNCTIONS_H_
#define INC_ADC_FUNCTIONS_H_

#include "adc_defines.h"
#include "spi_functions.h"
#include <stdint.h>
#include <stdbool.h>

/*Read ADC ID register to verify SPI is connected*/
void adc_verify_communication();

/*Function to configure ADC_channels*/
void adc_configure_channels();

/*Command ADC to do a conversion and read the data register*/
uint32_t adc_read_data();

/*Read the temperature from the ADC internal sensor*/
uint32_t adc_read_temperature();

/*Calculate float values for data and temperature*/
float adc_calculate_float(uint32_t channelRead);
float adc_calculate_float_temp(uint32_t temp_read);


void adc_calibrate();

typedef struct {
	uint32_t xaxis;
	uint32_t yaxis;
	uint32_t zaxis;
	uint32_t temp;
	int	measureNum; 	//Will be used to timestamp data on Android app.
}recorded_data;

#endif

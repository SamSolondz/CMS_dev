#include "adc_defines.h"
#include "spi_functions.h"
#include <stdbool.h>


/*Read ADC ID register to verify SPI is connected*/
bool adc_verify_communication();

/*Function to configure ADC_channels*/
void adc_configure_channels();

/*Command ADC to do a conversion and read the data register*/
double adc_read_data();

/*Read the temperature from the ADC internal sensor*/
float adc_read_temperature();

void adc_calibrate();

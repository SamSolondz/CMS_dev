#include "em_usart.h"
#include "em_gpio.h"
#include "adc_defines.h"
#include "InitDevice.h"

void spi_write_uint8(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr);
void spi_write_cal(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr);
void spi_read_data_reg(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr);
void read_offset_gain();

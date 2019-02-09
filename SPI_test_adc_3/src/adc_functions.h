#ifndef ADC_FUNCTIONS
#define ADC_FUNCTIONS

//Function prototypes
void spi_write_uint8(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr);
void spi_write_int32(int number_of_bytes, uint8_t * TXptr, int8_t * RXptr);
//bool adc_verify_communication();
void adc_configure_channels();
int32_t adc_read_data();
#endif

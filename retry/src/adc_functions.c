#include "adc_functions.h"

enum spi_slave adc = ADC_SEL;
bool adc_verify_communication(){
	  uint8_t TxBuffer[3] = {COMMS_READ_ID, 0x00, 0x00};
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};

	  spi_write_uint8(3, TxBuffer, RxBuffer, adc);

	  if(RxBuffer[1] == ID_VAL1 && (RxBuffer[2] >> 4) == ID_VAL0)
		  return true;
	  else
		  return false;
};

void adc_configure_channels(){	//Write MSB first
	  //Channel 0 Configuration
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};

	  uint8_t Config0Buffer[3] = {CONFIGURE_CH0_WRITE,
			  	  	  	  	  	  CONFIGURE_CH0_BYTE1,
								  CONFIGURE_CH0_BYTE0};

	  uint8_t Config1Buffer[3] = {CONFIGURE_CH1_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  uint8_t Config2Buffer[3] = {CONFIGURE_CH2_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  uint8_t Config3Buffer[3] = {CONFIGURE_CH3_WRITE,
			  	  	  	  	  	  CHANNEL_DISABLE,
								  CHANNEL_DISABLE};

	  spi_write_uint8(3, Config0Buffer, RxBuffer, adc);
	  spi_write_uint8(3, Config1Buffer, RxBuffer, adc);
	  spi_write_uint8(3, Config2Buffer, RxBuffer, adc);
	  spi_write_uint8(3, Config3Buffer, RxBuffer, adc);


	  //Channel 0 Setup configuration
	  uint8_t SetupBuffer[3] = {SETUP_CONFIG_WRITE,
			  	  	  	  	  	SETUP_CONFIG_0_BYTE1,
								SETUP_CONFIG_0_BYTE0};

	  spi_write_uint8(3, SetupBuffer, RxBuffer, adc);

	  //Channel 0 Filter configuration
	  uint8_t FilterBuffer0[3] = {FILTER_CONFIG_WRITE,
			  	  	  	  	  	  FILTER_CONFIG_BYTE1,
								  FILTER_CONFIG_BYTE0};

	  spi_write_uint8(3, FilterBuffer0, RxBuffer, adc);


	  uint8_t DummyBuffer[4] = {0x00,0x00,0x00,0x00};

	  //Channel 0 Interface
	  uint8_t InterfaceBuffer[3] = { INTERFACE_WRITE,
			  	  	  	  	  	  	 INTERFACE_BYTE1,
									 INTERFACE_BYTE0};

	  spi_write_uint8(3, InterfaceBuffer, RxBuffer, adc);

//	  //Calibrate ADC Offset
//	  uint8_t offset = (ADC_MODE_SINGLE_BYTE0 & (~ADC_MODE_MASK)) | (ADC_MODE_SYS_OFFSET_CAL<<4);
//	  uint8_t CalibrateOffsetBuffer[3] = {ADC_MODE_WRITE, ADC_MODE_SINGLE_BYTE1, offset};
//	  spi_write_cal(3, CalibrateOffsetBuffer, RxBuffer);
//
//	  //Calibrate ADC Gain
//	  uint8_t gain = (ADC_MODE_SINGLE_BYTE0 & (~ADC_MODE_MASK)) | (ADC_MODE_SYS_GAIN_CAL<<4);
//	  uint8_t CalibrateGainBuffer[3] = {ADC_MODE_WRITE, ADC_MODE_SINGLE_BYTE1, gain};
//	  spi_write_cal(3, CalibrateGainBuffer, RxBuffer);

	  adc_calibrate();
};

double adc_read_data(){
	  uint8_t RxBuffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	  //Set ADC to single conversion mode, wait for !RDY, issue data read commands
	  uint8_t TxBuffer[8] = {ADC_MODE_WRITE, ADC_MODE_SINGLE_BYTE1, ADC_MODE_SINGLE_BYTE0,
			  	  	  	  	  DATA_READ, TX_DUMMY, TX_DUMMY, TX_DUMMY, TX_DUMMY};


	  uint32_t channelRead = 0x0000;

	  //Fill the data buffer with ADC value;
	  spi_read_data_reg(8, TxBuffer, RxBuffer, adc);
	  uint8_t j = RxBuffer[6] & 0xFF;
	  channelRead = (RxBuffer[4] << 24) + (RxBuffer[5] << 16)
	  							+ (RxBuffer[6] << 8) + (RxBuffer[7]);


	 //Should fix these "Magic Numbers" to defines or register reads..
	  double c = 2 * ((double)0x555555 / GAIN_DIVIDER);
	  double a = ((double)channelRead + 1)/ c;
	  double b = 2.5/(0.75 * TWO_TO_THE_31);
	  double calc_channelRead = a * b;

	  return channelRead;
};


float adc_read_temperature(){
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};
	  uint8_t TxBuffer[3] = {CONFIGURE_CH0_READ, TX_DUMMY, TX_DUMMY};

	  //Read the current settings of the ADC
	  spi_write_uint8(3, TxBuffer, RxBuffer, adc);
	  uint8_t byte1 = RxBuffer[1];
	  uint8_t byte0 = RxBuffer[0];


	  uint8_t setTempBuffer[3] = {CONFIGURE_CH0_WRITE,
			  	  	  	  	  	  CONFIGURE_CH0_TEMP_BYTE1,
								  CONFIGURE_CH0_TEMP_BYTE0};

	  //Set Channel 0 to take TEMP+ and TEMP- as input
	  spi_write_uint8(3, setTempBuffer, RxBuffer, adc);
	  uint32_t temp_read = adc_calculate_float(adc_read_data());
	  temp_read = (temp_read/TEMP_DIVIDER) - TEMP_OFFSET;

	  //Set Channel0 back to original values;
	  TxBuffer[0] = CONFIGURE_CH0_WRITE;
	  TxBuffer[1] = byte1;
	  TxBuffer[2] = byte0;
	  spi_write_uint8(3, TxBuffer, RxBuffer, adc);


	  return temp_read;
};

void adc_calibrate(){
	uint8_t DummyBuffer[4] = {0x00,0x00,0x00,0x00};
	//Gain Register
	uint8_t GainBuffer[4] = { GAIN_CONFIG_WRITE,
			  	  	  	  	  	GAIN_DEFAULT_BYTE2,
								GAIN_DEFAULT_BYTE1,
								GAIN_DEFAULT_BYTE0};

	spi_write_uint8(4, GainBuffer, DummyBuffer, adc);

	//Offset Register
	uint8_t OffsetBuffer[4] = { OFFSET_DEFAULT_WRITE,
				  	  	  	  	  	  OFFSET_DEFAULT_BYTE2,
		  	  	  	  	  	  	  	  OFFSET_DEFAULT_BYTE1,
									  OFFSET_DEFAULT_BYTE0};

	 spi_write_uint8(4, OffsetBuffer, DummyBuffer, adc);

	 return;
};

float adc_calculate_float(uint32_t channelRead)
{
	 //Should fix these "Magic Numbers" to defines or register reads..
	  double c = 2 * ((double)0x555555 / GAIN_DIVIDER); 	//~1.33
	  double a = ((double)channelRead + 1)/ c;				//
	  double b = 2.5/(0.75 * TWO_TO_THE_31);
	  double calc_channelRead = a * b;

	  return (float)calc_channelRead;
};

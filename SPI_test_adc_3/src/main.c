#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_common.h"
#include "em_cmu.h"
#include "em_usart.h"
#include "em_timer.h"
#include "ecode.h"
#include "efr32bg1b232f256gm48.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "InitDevice.h"
#include "adc_defines.h"

#include "defines.h"

#define	DATABUFFER_SIZE  4
#define ONE_MILLISECOND_BASE_VALUE_COUNT             1000
#define ONE_SECOND_TIMER_COUNT                        13672
#define MILLISECOND_DIVISOR                           13.672

#define TEST 	1


void usart_setup(){

	//Set up clocks
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	CMU_ClockEnable(cmuClock_HFPER, true);

	//Initialize with default settings and then update fields
	USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
	init.clockMode = usartClockMode3;
	init.msbf = true;		//send most signifiant bit dirst
	init.autoCsEnable = false;

	USART_InitSync(USART1, &init);

	//Connect USART signals to GPIO peripheral
	/*NOTE: If the USART CS pin is enabled in the route register,
	  but AUTOCS is not being used, then the hardware
	  will disable the port pin driver for the CS pin,
	 leaving the pin output floating.*/

	USART1->ROUTEPEN = 	USART_ROUTEPEN_RXPEN |
						USART_ROUTEPEN_TXPEN |
						USART_ROUTEPEN_CLKPEN;/*|
						USART_ROUTEPEN_CSPEN;*/


	USART1->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC11 |
						USART_ROUTELOC0_TXLOC_LOC11 |
						USART_ROUTELOC0_CLKLOC_LOC11;/*|
						USART_ROUTELOC0_CSLOC_LOC11;*/

	GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);	//CS
	GPIO_PinModeSet(TX_PORT, TX_PIN, gpioModePushPull, 1);	//Tx
	GPIO_PinModeSet(RX_PORT, RX_PIN, gpioModeInput, 0);	//Rx
	GPIO_PinModeSet(SCLK_PORT, SCLK_PIN, gpioModePushPull, 1);	//SCK

    // Clear RX buffer and shift register
    USART1->CMD |= USART_CMD_CLEARRX;

    // Clear TX buffer and shift register
    USART1->CMD |= USART_CMD_CLEARTX;

	USART_Enable(USART1,usartEnable);
}

/*Function to write to registers 1 byte at a time. */
void spi_write_uint8(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr){
	//Set CS low
	GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);

	int i = 0;
	while(i < number_of_bytes){
		  RXptr[i] = USART_SpiTransfer(USART1, TXptr[i]);
		  uint8_t testRX = RXptr[i];
		  uint8_t testTX = TXptr[i];
		  i++;
	  }

	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

	  return;
}

/*Function to set ADC to calibration mode and wait*/
void spi_write_cal(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr){
	//Set CS low
	GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);

	int i = 0;
	while(i < number_of_bytes){
		  RXptr[i] = USART_SpiTransfer(USART1, TXptr[i]);
		  uint8_t testRX = RXptr[i];
		  uint8_t testTX = TXptr[i];
		  i++;
	  }

	  volatile int notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
	  while(notRDY == 1){
		  notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
	  };

	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

	  return;

}

/*Function to read from the ADC data register*/
void spi_read_data_reg(int number_of_bytes, uint8_t * TXptr, uint8_t * RXptr){
	//Set CS low
	GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 0);

	int i = 0;
	while(i < number_of_bytes){
		if(TXptr[i] == 0x44){
			//Check if !RDY has data
			int notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
			while(notRDY == 1){
				notRDY = GPIO_PinInGet(RX_PORT, RX_PIN);
			};
		}
		RXptr[i] = USART_SpiTransfer(USART1, TXptr[i]);
		uint8_t testRX = RXptr[i];
		uint8_t testTX = TXptr[i];
		i++;
	}

	  // Clear RX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARRX;
	  // Clear TX buffer and shift register
	  USART1->CMD |= USART_CMD_CLEARTX;

	  //Set CS high
	  GPIO_PinModeSet(CS_PORT, CS_PIN, gpioModePushPull, 1);

	  return;

}


bool adc_verify_communication(){
	 //Set CS low

	  uint8_t TxBuffer[3] = {COMMS_READ_ID, 0x00, 0x00};
	  uint8_t RxBuffer[3] = {0x00, 0x00, 0x00};

	  spi_write_uint8(3, TxBuffer, RxBuffer);

	  if(RxBuffer[1] == ID_VAL1 && (RxBuffer[2] >> 4) == ID_VAL0)
		  return true;
	  else
		  return false;

}

/*Function to configure ADC_channels*/
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

	  spi_write_uint8(3, Config0Buffer, RxBuffer);
	  spi_write_uint8(3, Config1Buffer, RxBuffer);
	  spi_write_uint8(3, Config2Buffer, RxBuffer);
	  spi_write_uint8(3, Config3Buffer, RxBuffer);


	  //Channel 0 Setup configuration
	  uint8_t SetupBuffer[3] = {SETUP_CONFIG_WRITE,
			  	  	  	  	  	SETUP_CONFIG_0_BYTE1,
								SETUP_CONFIG_0_BYTE0};

	  spi_write_uint8(3, SetupBuffer, RxBuffer);

	  //Channel 0 Filter configuration
	  uint8_t FilterBuffer0[3] = {FILTER_CONFIG_WRITE,
			  	  	  	  	  	  FILTER_CONFIG_BYTE1,
								  FILTER_CONFIG_BYTE0};

	  spi_write_uint8(3, FilterBuffer0, RxBuffer);


	  uint8_t DummyBuffer[4] = {0x00,0x00,0x00,0x00};

	  //Channel 0 Interface
	  uint8_t InterfaceBuffer[3] = { INTERFACE_WRITE,
			  	  	  	  	  	  	 INTERFACE_BYTE1,
									 INTERFACE_BYTE0};

	  spi_write_uint8(3, InterfaceBuffer, RxBuffer);

//	  //Calibrate ADC Offset
//	  uint8_t offset = (ADC_MODE_SINGLE_BYTE0 & (~ADC_MODE_MASK)) | (ADC_MODE_SYS_OFFSET_CAL<<4);
//	  uint8_t CalibrateOffsetBuffer[3] = {ADC_MODE_WRITE, ADC_MODE_SINGLE_BYTE1, offset};
//	  spi_write_cal(3, CalibrateOffsetBuffer, RxBuffer);
//
//	  //Calibrate ADC Gain
//	  uint8_t gain = (ADC_MODE_SINGLE_BYTE0 & (~ADC_MODE_MASK)) | (ADC_MODE_SYS_GAIN_CAL<<4);
//	  uint8_t CalibrateGainBuffer[3] = {ADC_MODE_WRITE, ADC_MODE_SINGLE_BYTE1, gain};
//	  spi_write_cal(3, CalibrateGainBuffer, RxBuffer);

	  //Gain Register
	  uint8_t GainBuffer[4] = { GAIN_CONFIG_WRITE,
			  	  	  	  	  	GAIN_DEFAULT_BYTE2,
								GAIN_DEFAULT_BYTE1,
								GAIN_DEFAULT_BYTE0};

	  spi_write_uint8(4, GainBuffer, DummyBuffer);

	  //Offset Register
	  uint8_t OffsetBuffer[4] = { OFFSET_DEFAULT_WRITE,
				  	  	  	  	  	  OFFSET_DEFAULT_BYTE2,
		  	  	  	  	  	  	  	  OFFSET_DEFAULT_BYTE1,
									  OFFSET_DEFAULT_BYTE0};

	  spi_write_uint8(4, OffsetBuffer, DummyBuffer);


};
/*Command ADC to do a conversion and read the data register*/
float adc_read_data(){
	  uint8_t RxBuffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	  //Set ADC to single conversion mode, wait for !RDY, issue data read commands
	  uint8_t TxBuffer[8] = {ADC_MODE_WRITE, ADC_MODE_SINGLE_BYTE1, ADC_MODE_SINGLE_BYTE0,
			  	  	  	  	  DATA_READ, TX_DUMMY, TX_DUMMY, TX_DUMMY, TX_DUMMY};


	  uint32_t channelRead = 0x0000;

	  //Fill the data buffer with ADC value;
	  spi_read_data_reg(8, TxBuffer, RxBuffer);
	  uint8_t j = RxBuffer[6] & 0xFF;
	  channelRead = (RxBuffer[4] << 24) + (RxBuffer[5] << 16)
	  							+ (RxBuffer[6] << 8) + (RxBuffer[7]);


	 //Should fix these "Magic Numbers" to defines or register reads..
	  float c = 2 * ((float)0x555555 / GAIN_DIVIDER);
	  float a = ((float)channelRead + 1)/ c;
	  float b = 2.5/(0.75 * TWO_TO_THE_31);
	  float calc_channelRead = a * b;

	  return calc_channelRead;
}

float adc_read_temperature(){
	  int8_t RxBuffer[3] = {0x00, 0x00, 0x00};
	  uint8_t TxBuffer[3] = {CONFIGURE_CH0_READ, TX_DUMMY, TX_DUMMY};

	  //Read the current settings of the ADC
	  spi_write_uint8(3, TxBuffer, RxBuffer);
	  uint8_t byte1 = RxBuffer[1];
	  uint8_t byte0 = RxBuffer[0];


	  uint8_t setTempBuffer[3] = {CONFIGURE_CH0_WRITE,
			  	  	  	  	  	  CONFIGURE_CH0_TEMP_BYTE1,
								  CONFIGURE_CH0_TEMP_BYTE0};

	  //Set Channel 0 to take TEMP+ and TEMP- as input
	  spi_write_uint8(3, setTempBuffer, RxBuffer);
	  float temp_read = adc_read_data();
	  temp_read = (temp_read/TEMP_DIVIDER) - TEMP_OFFSET;

	  //Set Channel0 back to original values;
	  TxBuffer[0] = CONFIGURE_CH0_WRITE;
	  TxBuffer[1] = byte1;
	  TxBuffer[2] = byte0;
	  spi_write_uint8(3, TxBuffer, RxBuffer);


	  return temp_read;
}

void mux_select(int select){
	switch(select){
		case 0: //No selection.
			GPIO_PinOutClear(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutClear(MUX_NEG_PORT, MUX_NEG_PIN);
			break;
		case 1:	//Select the x-axis
			GPIO_PinOutSet(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutClear(MUX_NEG_PORT, MUX_NEG_PIN);
			break;
		case 2: //Select the y-axis
			GPIO_PinOutClear(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutSet(MUX_NEG_PORT, MUX_NEG_PIN);
			break;
		case 3: //Select the z-axis
			GPIO_PinOutSet(MUX_POS_PORT, MUX_POS_PIN);
			GPIO_PinOutSet(MUX_NEG_PORT, MUX_NEG_PIN);
			break;
	}
}

void read_offset_gain(){
	  //See what is in the gain and offset regs
/*	  uint8_t offsetTXBuff[4] = {0b01110000, TX_DUMMY, TX_DUMMY, TX_DUMMY};
	  uint8_t offsetRXBuff[4] = {0x00, 0x00, 0x00, 0x00};
	  spi_write_uint8(4, offsetTXBuff, offsetRXBuff);

	  uint8_t gainTXBuff[4] = {0b01111000, TX_DUMMY, TX_DUMMY, TX_DUMMY};
	  uint8_t gainRXBuff[4] = {0x00, 0x00, 0x00, 0x00};
	  spi_write_uint8(4, gainTXBuff, gainRXBuff);*/

}

int main(void){
  /* Chip errata */
  CHIP_Init();

  usart_setup();	//CMU_clock_enable happens here
  GPIO_PinModeSet(MUX_POS_PORT, MUX_POS_PIN, gpioModePushPull, 0);	//Pos diff mux
  GPIO_PinModeSet(MUX_NEG_PORT, MUX_NEG_PIN, gpioModePushPull, 0);	//Neg diff mux

  bool verified = adc_verify_communication();
  if(verified == true){
	  adc_configure_channels();

	  mux_select(0);
	  float x_axis = adc_read_data();


	  mux_select(1);
	  float y_axis = adc_read_data();
	  mux_select(2);
	  float z_axis = adc_read_data();
	  float temp = adc_read_temperature();

	  int test = 0;
  }
  else{
	  int test = 1; //DEBUG_BREAK
  }


  //Infinite loop
  /*while(1){
  };*/

}

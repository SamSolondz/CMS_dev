#include "spi_functions.h"

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
};

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

};

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
};

void read_offset_gain(){
	  //See what is in the gain and offset regs
	  uint8_t offsetTXBuff[4] = {0b01110000, TX_DUMMY, TX_DUMMY, TX_DUMMY};
	  uint8_t offsetRXBuff[4] = {0x00, 0x00, 0x00, 0x00};
	  spi_write_uint8(4, offsetTXBuff, offsetRXBuff);

	  uint8_t gainTXBuff[4] = {0b01111000, TX_DUMMY, TX_DUMMY, TX_DUMMY};
	  uint8_t gainRXBuff[4] = {0x00, 0x00, 0x00, 0x00};
	  spi_write_uint8(4, gainTXBuff, gainRXBuff);
};

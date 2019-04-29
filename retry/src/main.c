#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "infrastructure.h"

/* Libraries containing default Gecko configuration values */
#include "em_system.h"
#include "hal-config.h"
#include "bsphalconfig.h"

#include "em_emu.h"//two cmu files
#include "em_gpio.h"//
#include "em_common.h"//
#include "em_cmu.h" //
#include "em_timer.h"//
#include "em_letimer.h"//
#include "ecode.h" //

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "infrastructure.h"//
#include "retargetserial.h"//
#include "math.h"//

#include "adc_defines.h" //
#include "defines.h"//
#include "spi_functions.h" //
#include "adc_functions.h" //
#include "flash_functions.h"//
#include "bluetooth_utilities.h"
#include "pin_def.h"
#include "ustimer.h"

#define	DATABUFFER_SIZE  4
#define ONE_MILLISECOND_BASE_VALUE_COUNT            1000
#define ONE_SECOND_TIMER_COUNT                      13672
#define RECORD_FIELD_TIME							(1 * 32768)//(60 * 32768)//1966080
#define RECORD_DEMO_TIME							(1 * 32768) // 32768 = 1 sec
#define PULSE_HIGH									(3 * 32768)
#define SR_TIME										(60 * 30 * 32768)

#define MODE_FIELD	0
#define MODE_DEMO	1

#define USER_FLAG_NOP		0
#define USER_FLAG_CLEAR 	1
#define USER_FLAG_OFFLOAD	2

#define AVERAGE_NUM			250

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif

#define CMS 1

uint32_t ms_counter = 0;
int readFlag = 0;

int ble_soft_timer_Flag = 0;
int operation_mode = MODE_FIELD;						//Default to field mode
int user_flag = USER_FLAG_NOP;
int record_time = RECORD_FIELD_TIME;
int running_flag = 0;

uint16_t current_page = 0x00;
uint16_t current_column = 0;
uint32_t measurementCount = 1;
uint32_t demoCount = 1;

uint32_t time_start = 0;
uint32_t b = 0x1;

recorded_data holding[500];
int holding_index = 0;

int SR_flag = 0;
uint32_t SR_counter = 0;

uint32_t x_arr[60];
uint32_t y_arr[60];
uint32_t z_arr[60];
int samples_index = 0;


uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];
// Gecko configuration parameters (see gecko_configuration.h)
	static const gecko_configuration_t config = {
	  .config_flags = 0,
	  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
	  .bluetooth.max_connections = MAX_CONNECTIONS,
	  .bluetooth.heap = bluetooth_stack_heap,
	  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
	  .bluetooth.sleep_clock_accuracy = 100, // ppm
	  .gattdb = &bg_gattdb_data,
	  .ota.flags = 0,
	  .ota.device_name_len = 3,
	  .ota.device_name_ptr = "OTA",
	#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
	  .pa.config_enable = 1, // Enable high power PA
	  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
	#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
	};


void usart_setup(){

	//Set up clocks
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	CMU_ClockEnable(cmuClock_HFPER, true);

	//Initialize with default settings and then update fields
	USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
	init.clockMode = usartClockMode3;
	init.msbf = true;		//send most sig bit first
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

	//Setup SPI lines
	GPIO_PinModeSet(CS_PORT_ADC, CS_PIN_ADC, gpioModePushPull, 1);	//CS
	GPIO_PinModeSet(CS_PORT_FLASH, CS_PIN_FLASH, gpioModePushPull, 1);	//CS
	GPIO_PinModeSet(TX_PORT, TX_PIN, gpioModePushPull, 1);	//Tx
	GPIO_PinModeSet(RX_PORT, RX_PIN, gpioModeInput, 0);	//Rx
	GPIO_PinModeSet(SCLK_PORT, SCLK_PIN, gpioModePushPull, 1);	//SCK

    // Clear RX buffer and shift register
    USART1->CMD |= USART_CMD_CLEARRX;

    // Clear TX buffer and shift register
    USART1->CMD |= USART_CMD_CLEARTX;

	USART_Enable(USART1,usartEnable);
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

		default:
			break;
	}
}

void SR_toggle()
{
	  GPIO_PinOutClear(SR_PORT, SR_PIN);
	  USTIMER_Delay(3000000);
	  GPIO_PinOutSet(SR_PORT, SR_PIN);

}


void LETIMER0_IRQHandler(void) {
	  // Clear the interrupt flag
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP0);
		if(ms_counter < record_time)
			ms_counter++;
		else{
			readFlag = 1;
			ms_counter = 0;
			NVIC_DisableIRQ(LETIMER0_IRQn);
		}

		if(SR_counter < SR_TIME)
			SR_counter++;
		else{
			SR_flag = 1;
			SR_counter = 0;
			NVIC_DisableIRQ(LETIMER0_IRQn);
		}
}

void OFFLOAD_DATA(){

	recorded_data offload;
	recorded_data * offload_ptr = &offload;
	//uint16_t read_page = FLASH_DATA_PAGE_START;
	//uint16_t read_column = 0x00;

	int j = 0;
	while(j < holding_index && (holding_index > 0)){

//		offload.xaxis = flash_read_data(read_page, 0x00);
//		offload.yaxis = flash_read_data(read_page, 0x04);
//		offload.zaxis = flash_read_data(read_page, 0x08);
//		offload.temp = flash_read_data(read_page, 0x12);
//		offload.measureNum = flash_read_data(read_page, 0x016);

		offload = holding[j];

		USTIMER_Delay(500000);
		GPIO_PinOutClear(LED_BLE_PORT, LED_BLE_PIN);
		packet_handler();
		sendData(offload_ptr);
		USTIMER_Delay(500000);
		GPIO_PinOutSet(LED_BLE_PORT, LED_BLE_PIN);

		j++;
		//read_page = read_page + (uint16_t)64;
	}
}

void CLEAR_DATA() {


	recorded_data clear;
	clear.measureNum = 0x00;
	packet_handler();
	sendData(&clear);

	holding_index = 0;
	measurementCount = 1;

//	uint16_t erase_page = FLASH_DATA_PAGE_START;

//	while(erase_page <= current_page){
//		flash_block_erase(erase_page);
//		erase_page = erase_page + (uint16_t)64;
//	}
//
//	//Reset current_page to start of data page
//	current_page = FLASH_DATA_PAGE_START;
//	current_column = 0x0;

	//Update page and column in FLASH
	//flash_write_data32_direct(current_page, FLASH_LAST_PAGE, FLASH_PARAM_PAGE);
	//flash_write_data32_direct(current_column, FLASH_LAST_COLUMN, FLASH_PARAM_PAGE);

};

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void x_merge(uint32_t x_arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    uint32_t L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = x_arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = x_arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            x_arr[k] = L[i];
            i++;
        }
        else
        {
            x_arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        x_arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        x_arr[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void x_mergeSort(uint32_t x_arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        x_mergeSort(x_arr, l, m);
        x_mergeSort(x_arr, m+1, r);

        x_merge(x_arr, l, m, r);
    }
}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void y_merge(uint32_t y_arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    uint32_t L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = y_arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = y_arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            y_arr[k] = L[i];
            i++;
        }
        else
        {
            y_arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        y_arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        y_arr[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void y_mergeSort(uint32_t y_arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        y_mergeSort(y_arr, l, m);
        y_mergeSort(y_arr, m+1, r);

        y_merge(y_arr, l, m, r);
    }
}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void z_merge(uint32_t z_arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    uint32_t L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = z_arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = z_arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            z_arr[k] = L[i];
            i++;
        }
        else
        {
            z_arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        z_arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        z_arr[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void z_mergeSort(uint32_t z_arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        z_mergeSort(z_arr, l, m);
        z_mergeSort(z_arr, m+1, r);

        z_merge(z_arr, l, m, r);
    }
}


int main(void){
	/* Chip errata */
	initMcu(); 		//Initialize device
	initBoard(); 	//Initialize board
	initApp();		//Initialize application

	gecko_init(&config);
	RETARGET_SerialInit();
	usart_setup();	//CMU_clock_enable happens here, must occur before gpio/letimersetup
	USTIMER_Init();
	printf("\n\rHello World!\r\n");


	//Set GPIO pins//
	GPIO_PinModeSet(MUX_POS_PORT, MUX_POS_PIN, gpioModePushPull, 0);	//Pos diff mux
	GPIO_PinModeSet(MUX_NEG_PORT, MUX_NEG_PIN, gpioModePushPull, 0);	//Neg diff mux
	GPIO_PinModeSet(SR_PORT, SR_PIN, gpioModePushPull, 1);	//SR pin

	GPIO_PinModeSet(LED_POWER_PORT, LED_POWER_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(LED_BLE_PORT, LED_BLE_PIN, gpioModePushPull, 0);

	GPIO_PinOutSet(LED_POWER_PORT, LED_POWER_PIN);

	//adc_verify_communication();
	//flash_verify_communication();


	operation_mode = MODE_FIELD;//flash_read_data(FLASH_PARAM_PAGE, FLASH_LAST_MODE);
	CLEAR_DATA();

	if(operation_mode == MODE_FIELD)
	{
		record_time = RECORD_FIELD_TIME;
		//Set the column and page to last recorded
		//current_column = (uint16_t) flash_read_data(FLASH_PARAM_PAGE, FLASH_LAST_COLUMN);
		//current_page = (uint16_t) flash_read_data(FLASH_PARAM_PAGE, FLASH_LAST_PAGE);
		current_page = 0;
		current_column = 0;
	}
	else
	{
		//Restart column and page to zero (reset)
		record_time = RECORD_DEMO_TIME;
		current_column = 0x0;
		current_page = FLASH_DATA_PAGE_START;
		//flash_write_data32_direct(current_page, FLASH_LAST_PAGE, FLASH_PARAM_PAGE);
		//flash_write_data32_direct(current_column, FLASH_LAST_COLUMN, FLASH_PARAM_PAGE);
	}

	 //Trigger a SR
	// SR_toggle();

  /*Initialize Structure to hold recorded data*/
  recorded_data new_data;
  recorded_data *data_ptr;
  data_ptr = &new_data;
  new_data.xaxis = 0x0;
  new_data.yaxis = 0x0;
  new_data.zaxis = 0x0;
  new_data.temp = 0xDEADBEEF;
  new_data.measureNum = 0;

  recorded_data offload;
  recorded_data * offload_ptr = &offload;



  LETIMER_setup();
  NVIC_EnableIRQ(LETIMER0_IRQn);

  //Infinite loop
   while(1){
	  packet_handler();
	 if(readFlag && running_flag){

		new_data.xaxis = 0x0;
		new_data.yaxis = 0x0;
		new_data.zaxis = 0x0;
		new_data.measureNum = 0x0;
		new_data.count = 0x0;

		uint32_t xread = 0x0;
		uint32_t yread = 0x0;
		uint32_t zread = 0x0;


		uint32_t read;
		uint32_t count = 1;

		 for(int z = 0; z < AVERAGE_NUM; z++)
		 {

			mux_select(1);
			adc_configure_channels();
			read = adc_read_data();
			if (new_data.xaxis > (new_data.xaxis + read))
				break;
			xread = read + new_data.xaxis;

			mux_select(3);
			adc_configure_channels();
			read = adc_read_data();
			if (new_data.zaxis > (new_data.zaxis + read))
				break;
			zread = read + new_data.zaxis;

			mux_select(2);
			adc_configure_channels();
			read = adc_read_data();
			if (new_data.yaxis > (new_data.yaxis + read))
				break;
			yread = read + new_data.yaxis;

			//Store measurements
			new_data.xaxis = xread;
			new_data.yaxis = yread;
			new_data.zaxis = zread;
			count++;
		 }
		 new_data.count = count;

		 //Read temperature
		adc_configure_channels();
		uint32_t tempread = adc_read_temperature();

			new_data.temp = tempread;
			if(MODE_FIELD){
				new_data.measureNum = measurementCount;
				//measurementCount++;
			}
			else{
				new_data.measureNum = demoCount;
				demoCount++;
			}

			printf("\n\n\r ---Read #%lu---", measurementCount);
			printf("\r\n x = %f V", adc_calculate_float(data_ptr->xaxis));
			printf("\r\n y = %f V", adc_calculate_float(data_ptr->yaxis));
			printf("\r\n z = %f V", adc_calculate_float(data_ptr->zaxis));
			printf("\r\n Temperature = %.3f C    .", adc_calculate_float_temp(data_ptr->temp));


			 switch(operation_mode){
				 case MODE_FIELD:			//store data
					printf("\r\nFIELD");
#ifdef CMS
//					if(current_page < 65536){
//						flash_write_data32(data_ptr, &current_column, &current_page);
//						current_page = current_page + (uint16_t)64;
//					}

					if(samples_index == 60 && holding_index < 500){
						//Average the medians after 60 samples and store to struct
						x_mergeSort(x_arr, 0, (sizeof(x_arr)/sizeof(x_arr[0])) -  1);
						y_mergeSort(y_arr, 0, (sizeof(y_arr)/sizeof(y_arr[0])) -  1);
						z_mergeSort(z_arr, 0, (sizeof(z_arr)/sizeof(z_arr[0])) -  1);

						new_data.xaxis = x_arr[30];
						new_data.yaxis = y_arr[30];
						new_data.zaxis = z_arr[30];
						new_data.measureNum = measurementCount;

						measurementCount++;



						samples_index = 0;
						holding[holding_index] = new_data;
						holding_index++;


						sendData(data_ptr);
					}
					else{
						x_arr[samples_index] = new_data.xaxis;
						y_arr[samples_index] = new_data.yaxis;
						z_arr[samples_index] = new_data.zaxis;
						samples_index++;
					}

#endif
					 break;
				 case MODE_DEMO:			//send data and forget it
					 printf("\r\nDEMO");
					 sendData(data_ptr);
					 break;
			 }
				readFlag = 0;
				NVIC_EnableIRQ(LETIMER0_IRQn);

	 }
	if(SR_flag)
	{
		SR_toggle();
		SR_flag = 0;
		NVIC_EnableIRQ(LETIMER0_IRQn);
	}

	 switch(user_flag){
	 	 case USER_FLAG_CLEAR:
	 		NVIC_DisableIRQ(LETIMER0_IRQn);
	 		CLEAR_DATA();
	 		printf("\n\rClear selected");
	 		user_flag = USER_FLAG_NOP;
	 		NVIC_EnableIRQ(LETIMER0_IRQn);
	 		break;
	 	 case USER_FLAG_OFFLOAD:
	 		NVIC_DisableIRQ(LETIMER0_IRQn);
		 	OFFLOAD_DATA();
		 	printf("\n\rOffload selected");
		 	user_flag = USER_FLAG_NOP;
		 	NVIC_EnableIRQ(LETIMER0_IRQn);
		 	break;
	 	 default:
	 		 break;
	 }

	 //Go to sleep
	 //EMU_EnterEM2(1);

  }
 };



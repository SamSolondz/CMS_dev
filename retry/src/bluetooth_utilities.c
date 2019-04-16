/*
 * bluetooth_utilities.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Touzong
*/
#include "bluetooth_utilities.h"


void LETIMER_setup(void){
	 // Enable clock to the LE modules interface
	CMU_ClockEnable(cmuClock_HFLE, true);
	// Select LFXO for the LETIMER
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_ClockEnable(cmuClock_LETIMER0, true);


	//uint32_t top_val = CMU_ClockFreqGet(cmuClock_LETIMER0);
	uint16_t top_val = 0xffff;


	LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

	// Reload COMP0 on underflow, idle output, and run in repeat mode
	letimerInit.comp0Top  = true;
	letimerInit.ufoa0     = letimerUFOANone;
	letimerInit.repMode   = letimerRepeatFree;

	// Need REP0 != 0 to pulse on underflow
	LETIMER_RepeatSet(LETIMER0, 0, 1);

	// Compare on wake-up interval count
	LETIMER_CompareSet(LETIMER0, 0, top_val);

	LETIMER_Init(LETIMER0, &letimerInit);

	// Enable LETIMER0 interrupts for COMP0
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);

	// Enable LETIMER interrupts
	NVIC_SetPriority(LETIMER0_IRQn, 0);
	NVIC_ClearPendingIRQ(LETIMER0_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);
}

void get_time() {

	uint32_t days, hours, mins, sec, ms;

	struct gecko_msg_hardware_get_time_rsp_t *time;

	time = gecko_cmd_hardware_get_time();

	days = time->seconds/60/60/25;
	hours = time-> seconds/60/60 %24;
	mins = time-> seconds/60 %60;
	sec = time-> seconds % 60;
	ms = time->ticks * 1000/32768;

//	printf("%03d-%02d:%02d:%02d.%03d \r",days,hours,mins,sec,ms);

}


// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

/*following 2 val currently unused*/
uint32_t comp_flag;
uint32_t comparator;

/* Test data*/
uint32_t data_out = 0x12358cdf;
PACKSTRUCT(struct mode_t{
	int data_in;
});

PACKSTRUCT(struct offload_t{
	int offload;
});

PACKSTRUCT(struct clear_t{
	int clear;
});



void sendData(recorded_data * data_ptr)
{

	uint32_t xread = data_ptr->xaxis;
	uint32_t yread = data_ptr->yaxis;
	uint32_t zread = data_ptr->zaxis;
	uint32_t temp  = data_ptr->temp;
	uint32_t measureNum = data_ptr->measureNum;

	uint8_t flag = 0xff;
	uint8_t tempBuffer[40];//should be 40
	uint8_t *p = tempBuffer;

	UINT32_TO_BITSTREAM(p, measureNum);
	UINT8_TO_BITSTREAM(p, flag);

	UINT32_TO_BITSTREAM(p, xread);
	UINT8_TO_BITSTREAM(p, flag);

	UINT32_TO_BITSTREAM(p,  yread);
	UINT8_TO_BITSTREAM(p, flag);

	UINT32_TO_BITSTREAM(p, zread);
	UINT8_TO_BITSTREAM(p, flag);

	UINT32_TO_BITSTREAM(p, temp);
	UINT8_TO_BITSTREAM(p, flag);



	 gecko_cmd_gatt_server_send_characteristic_notification(
	      0xFF, gattdb_Data, 40 , tempBuffer);
}

void packet_handler(){
	 struct gecko_cmd_packet* evt;

	 /* Check for stack event. */
	 //Changed to gecko_peek_event
	 evt = gecko_peek_event();

	 /* Handle events */
	 switch (BGLIB_MSG_ID(evt->header)) {
	 /* This boot event is generated when the system boots up after reset.
	  * Do not call any stack commands before receiving the boot event.
	  * Here the system is set to start advertising immediately after boot procedure. */
	 case gecko_evt_system_boot_id:
		 /* Set advertising parameters. 100ms advertisement interval.
		  * The first parameter is advertising set handle
		  * The next two parameters are minimum and maximum advertising interval, both in
		  * units of (milliseconds * 1.6).
		  * The last two parameters are duration and maxevents left as default. */
		 gecko_cmd_le_gap_set_advertise_timing(0, 160, 160, 0, 0);


		 /* Start general advertising and enable connections. */
		 gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
		 break;

	//Turn on the BLE light on a connection
	 case gecko_evt_le_connection_opened_id:
		GPIO_PinOutSet(LED_BLE_PORT, LED_BLE_PIN);
		break;

	 case gecko_evt_gatt_server_characteristic_status_id:
		 if((evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_Data)
				 && (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01))
		 {

			 if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x02)
			 {

				 /* Indications have been turned ON - start the repeating timer. The 1st parameter '32768'
				  * tells the timer to run for 1 second (32.768 kHz oscillator), the 2nd parameter is
				  * the timer handle and the 3rd parameter '0' tells the timer to repeat continuously until
				  * stopped manually.*/
				 	// gecko_cmd_hardware_set_soft_timer(32768, 0, 0);
				 	// printf("\n soft timer STARTED\n");
			 }

			 else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00)
			 {
				 /* Indications have been turned OFF - stop the timer. */

				 //gecko_cmd_hardware_set_soft_timer(0, 0, 0);
				// printf("\n soft timer STOPED\n");
			 }
		   }

		   break;
//
//		case gecko_evt_hardware_soft_timer_id:
//			ble_soft_timer_Flag = 1;
//			break;
			//Telling MCU what mode we want

		case gecko_evt_gatt_server_attribute_value_id:
			 if(evt->data.evt_gatt_server_attribute_value.attribute == gattdb_mode)
			    {
			    struct mode_t* mode_bit = (struct mode_t*)(evt->data.evt_gatt_server_attribute_value.value.data);

			    if(operation_mode == MODE_FIELD){
			    	operation_mode = MODE_DEMO;
			    	record_time = RECORD_FIVE_SECOND;
			    }
			    else{
			    	operation_mode = MODE_FIELD;
			    	record_time = RECORD_ONE_MINUTE;
			    }
			   }
			 if(evt->data.evt_gatt_server_attribute_value.attribute == gattdb_offload)
			    {
				 user_flag = USER_FLAG_OFFLOAD;
			    }
			 if(evt->data.evt_gatt_server_attribute_value.attribute == gattdb_ClearMem)
			 {
				 user_flag = USER_FLAG_CLEAR;
				 //GPIO_PinOutClear(LED_BLE_PORT, LED_BLE_PIN);
			 }
			  break;


		case gecko_evt_le_connection_closed_id:
			GPIO_PinOutClear(LED_BLE_PORT, LED_BLE_PIN);
			/* Check if need to boot to dfu mode */
			if (boot_to_dfu) {
				/* Enter to DFU OTA mode */
				gecko_cmd_system_reset(2);
			}
			else {
				/* Restart advertising after client has disconnected */
					gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
			      }
			break;

		/* Events related to OTA upgrading
			         ----------------------------------------------------------------------------- */

			/* Check if the user-type OTA Control Characteristic was written.
			       * If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
		case gecko_evt_gatt_server_user_write_request_id:
			if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
			          /* Set flag to enter to OTA mode */
			          boot_to_dfu = 1;
			          /* Send response to Write Request */
			          gecko_cmd_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection, gattdb_ota_control, bg_err_success);

			          /* Close connection to enter to DFU OTA mode */
			          gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
			        }
			        break;

		default:
			break;
	}

}

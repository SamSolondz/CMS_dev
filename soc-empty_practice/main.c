/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
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
#include "em_emu.h"
#include "em_cmu.h"

#include "em_letimer.h"
#include "em_gpio.h"

#define letimerClkFreq 19000000
#define letimerDesired 1000
#define LetimerCompare letimerClkFreq / letimerDesired
#define data_size 20

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

#include "stdio.h"
#include "retargetserial.h"

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
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


// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;
/* Our data*/
uint32_t data_out = 0x12358cdf;
/*flag for comparison*/
uint32_t comp_flag;
/*comparator value*/
uint32_t comparator;
void sendData()
{
	uint8_t tempBuffer[data_size];
	uint8_t *p = tempBuffer;

	UINT32_TO_BITSTREAM(p,data_out);
	 gecko_cmd_gatt_server_send_characteristic_notification(
	      0xFF, gattdb_Data, data_size, tempBuffer);
}

PACKSTRUCT(struct data_in_t{
	uint32_t dataIn;
});



/**
 * @brief  Main function
 */
void main(void)
{
//CMU_ClockEnable(cmuClock_HFLE, true);
//CMU_ClockEnable(cmuClock_GPIO, true);
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Initialize stack
  gecko_init(&config);
  //LETIMER_setup();
  RETARGET_SerialInit();
 // printf("hello"\n);

  //GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull, 1);
  //GPIO_PinOutSet(gpioPortF, 4);


  //GPIO_PinModeSet(gpioPortF, 3, gpioModePushPull, 1);
  //GPIO_PinOutSet(gpioPortF, 3);


  while (1) {
    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;

    /* Check for stack event. */
    evt = gecko_wait_event();

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
    		             gecko_cmd_hardware_set_soft_timer(32768, 0, 0);
    		             printf("timer started\n");
    		 }

    	 else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00)
    	 	 	 {
    	            /* Indications have been turned OFF - stop the timer. */
    	            gecko_cmd_hardware_set_soft_timer(0, 0, 0);
    	          }
    	 }
    	 // gecko
   		 //sendData();

    	 break;

      case gecko_evt_hardware_soft_timer_id:

             sendData();
             break;
 //
      case gecko_evt_gatt_server_attribute_value_id:
    	  if(evt->data.evt_gatt_server_attribute_value.attribute == gattdb_PhoneData)
    	    {
    		  struct data_in_t* data_in = (struct data_in_t*)(evt->data.evt_gatt_server_attribute_value.value.data);
    		  comparator = (data_in->dataIn)^data_out;
    		  if (comparator == 0)
    		  {
    			  comp_flag = 0;
    		  }
    		  else
    		  {
    			  comp_flag = 1;
    		  }
       	   printf("Sent Data: %32d\r\n", data_out);
    	   printf("Recieved Data: %32d\r\n", data_in->dataIn);
    	   printf("Compared Value: %32d\r\n", comparator);
    	   printf("comp flag: %32d\r\n", comp_flag);
    	   //sendData();
    	  }
    	  break;



      case gecko_evt_le_connection_closed_id:

        /* Check if need to boot to dfu mode */
        if (boot_to_dfu) {
          /* Enter to DFU OTA mode */
          gecko_cmd_system_reset(2);
        } else {
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
          gecko_cmd_gatt_server_send_user_write_response(
            evt->data.evt_gatt_server_user_write_request.connection,
            gattdb_ota_control,
            bg_err_success);

          /* Close connection to enter to DFU OTA mode */
          gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
        }
        break;

      default:
        break;
    }
  }
}


/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */

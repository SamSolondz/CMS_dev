/*
 * bluetooth_utilities.h
 *
 *  Created on: Mar 31, 2019
 *      Author: Touzong
 */

#ifndef BLUETOOTH_UTILITIES_H_
#define BLUETOOTH_UTILITIES_H_


#include "ble-configuration.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "infrastructure.h"

/* Libraries containing default Gecko configuration values */
#include "em_system.h"
#include "hal-config.h"
#include "bsphalconfig.h"

#include "em_emu.h"
#include "em_gpio.h"

#include "em_cmu.h"
#include "em_letimer.h"
#include "pin_def.h"

#include "adc_functions.h"
#include "flash_defines.h"
#include "flash_functions.h"

#define MODE_FIELD	0
#define MODE_DEMO	1
#define RECORD_FIELD_TIME							(1 * 32768)//(60 * 32768)//1966080
#define RECORD_DEMO_TIME							(1 * 32768)

#define USER_FLAG_NOP		0
#define USER_FLAG_CLEAR 	1
#define USER_FLAG_OFFLOAD	2

extern ble_soft_timer_Flag;
extern record_time;
extern int operation_mode;
extern int user_flag;
extern int running_flag;
extern uint32_t time_start;


//extern operation_mode;

void LETIMER_setup(void);
void sendData(recorded_data * data_ptr);
void packet_handler();
void get_time();




#endif /* BLUETOOTH_UTILITIES_H_ */

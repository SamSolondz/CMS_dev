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

#include "adc_functions.h"

extern ble_soft_timer_Flag;
extern record_time;
extern bool operation_mode;
extern bool offload_flag;
extern bool clear_flag;

//extern operation_mode;

void LETIMER_setup(void);
void sendData(recorded_data * data_ptr);
void packet_handler();
void get_time();




#endif /* BLUETOOTH_UTILITIES_H_ */

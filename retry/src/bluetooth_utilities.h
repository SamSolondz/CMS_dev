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
//#include "em_common.h"

#include "em_cmu.h"
//#include "em_timer.h"
#include "em_letimer.h"
//#include "ecode.h" //

//#include <time.h>
//#include <stdio.h>
//#include <stdbool.h>
//#include <stdint.h>
//#include <inttypes.h>
//#include "infrastructure.h"//
//#include "retargetserial.h"//
//#include "math.h"//

#include "adc_functions.h"

extern ble_soft_timer_Flag;

void LETIMER_setup(void);
void sendData(recorded_data * data_ptr);
void packet_handler();





#endif /* BLUETOOTH_UTILITIES_H_ */

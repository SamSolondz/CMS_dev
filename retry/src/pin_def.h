/*
 * pin_def.h
 *
 *  Created on: Mar 14, 2019
 *      Author: Touzong
 */

#ifndef PIN_DEF_H_
#define PIN_DEF_H_


#define CS_PIN_ADC       (9)
#define CS_PORT_ADC      (gpioPortC)

#define CS_PIN_FLASH	(5)
#define CS_PORT_FLASH	(gpioPortF)

#define RX_PIN          (7)
#define RX_PORT         (gpioPortC)

#define SCLK_PIN        (8)
#define SCLK_PORT       (gpioPortC)

#define TX_PIN          (6)
#define TX_PORT         (gpioPortC)

#define MUX_POS_PIN		(10)
#define MUX_POS_PORT	(gpioPortC)

#define MUX_NEG_PIN		(11)
#define MUX_NEG_PORT	(gpioPortC)

#define LED_POWER_PIN		(4)
#define	LED_POWER_PORT		(gpioPortF)

#define LED_BLE_PIN		(3)
#define	LED_BLE_PORT	(gpioPortF)

#define SR_PIN			(3)
#define SR_PORT			(gpioPortA)


#endif /* PIN_DEF_H_ */

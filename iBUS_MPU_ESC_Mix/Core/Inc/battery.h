/*
 * battery.h
 *
 *  Created on: Jul 14, 2026
 *      Author: ADMIN
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "main.h"

void Battery_Init(ADC_HandleTypeDef *hadc);
float Battery_GetVoltage(void);
uint8_t Battery_CheckLow(void);


#endif /* INC_BATTERY_H_ */

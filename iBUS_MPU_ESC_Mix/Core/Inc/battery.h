/*
 * battery.h
 *
 *  Created on: Jul 14, 2026
 *      Author: ADMIN
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "main.h"
//15.4V lúc đo pin / raw_adc  ≈ 0.01363f
#define BAT_CALIB_FACTOR 	0.01363f

void BAT_INIT(ADC_HandleTypeDef *bat_hadc, uint32_t *pData);
void BAT_GET_VOL(uint32_t raw_adc_vl, float *bal_ptr);
uint8_t is_bat_low(float bal_val);

#endif /* INC_BATTERY_H_ */

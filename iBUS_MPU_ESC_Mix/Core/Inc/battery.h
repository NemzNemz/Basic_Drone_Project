/*
 * battery.h
 *
 *  Created on: Jul 14, 2026
 *      Author: ADMIN
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "main.h"
// 4S: 3.3/4096 * 6 (R4=820k, R5=200k) = 0.004108887f
#define BAT_CALIB_FACTOR 	0.004576857f //0.00483396f

void BAT_INIT(ADC_HandleTypeDef *bat_hadc, uint32_t *pData);
void BAT_GET_VOL(uint32_t raw_adc_vl, float *bal_ptr);
uint8_t is_bat_low(float bal_val);

#endif /* INC_BATTERY_H_ */

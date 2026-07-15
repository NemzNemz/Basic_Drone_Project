/*
 * battery.h
 *
 *  Created on: Jul 14, 2026
 *      Author: ADMIN
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "main.h"
#define BAT_CALIB_FACTOR 	0.00080566f //0.0044675f 0.00080566f

void BAT_INIT(ADC_HandleTypeDef *bat_hadc, uint32_t *pData);
void BAT_GET_VOL(uint32_t raw_adc_vl, float *bal_ptr);
uint8_t is_bat_low(float bal_val);

#endif /* INC_BATTERY_H_ */

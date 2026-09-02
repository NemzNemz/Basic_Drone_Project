/*
 * battery.h
 *
 *  Created on: Jul 14, 2026
 *      Author: ADMIN
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "main.h"
// Cầu phân áp: R1 = 820k, R3 = 200k (Tỷ lệ chia áp = 5.1)
// Điện áp pin = RAW_ADC * (3.3 / 4096) * 5.1
#define BAT_CALIB_FACTOR 	0.01071167f

void BAT_INIT(ADC_HandleTypeDef *bat_hadc, uint32_t *pData);
void BAT_GET_VOL(uint32_t raw_adc_vl, float *bal_ptr);
uint8_t is_bat_low(float bal_val);

#endif /* INC_BATTERY_H_ */

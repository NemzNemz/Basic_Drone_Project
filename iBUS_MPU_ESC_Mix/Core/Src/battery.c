/*
 * battery.c
 *
 *  Created on: Jul 14, 2026
 *      Author: ADMIN
 */

#include "assert.h"
#include "battery.h"
static ADC_HandleTypeDef *hw_adc;

void BAT_INIT(ADC_HandleTypeDef *bat_hadc, uint32_t *pData){
	hw_adc = bat_hadc;
	//adc_x khong duoc rong!
	assert(hw_adc != NULL);
	HAL_ADC_Start_DMA(hw_adc, pData, 1);
}

void BAT_GET_VOL(uint32_t raw_adc_vl, float *bal_val_ptr){
	*bal_val_ptr = (float)raw_adc_vl * BAT_CALIB_FACTOR;
}

uint8_t is_bat_low(float bal_val){
	static uint8_t bat_low = 0;
	//Mốt tự thay 12.8f hoặc 13.0f vô
	if(bal_val < 2.8f){
		bat_low = 1;
	}
	else if (bal_val > 3.0f){
		bat_low =  0;
	}
	return bat_low;
}




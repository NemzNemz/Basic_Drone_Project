/*
 * motor.c
 *
 *  Created on: Jul 13, 2026
 *      Author: ADMIN
 */

#include "motor.h"
#include "assert.h"

static TIM_HandleTypeDef *esc_htim;

void Motor_Init(TIM_HandleTypeDef *hw_esc_htim){
	esc_htim = hw_esc_htim;
	//esc_htim ko duoc rong!
	assert(esc_htim != NULL);
}

void Motor_Set_Speed(uint16_t speed){
	esc_htim->Instance->CCR1 = speed;
	esc_htim->Instance->CCR2 = speed;
	esc_htim->Instance->CCR3 = speed;
	esc_htim->Instance->CCR4 = speed;
}

/*
 * motor.h
 *
 *  Created on: Jul 13, 2026
 *      Author: Nemz
 */
#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_
#define MOTOR_MIN_PWM 13000
#define MOTOR_MAX_PWM 25000

#include "main.h"

typedef struct{
	uint16_t m1;
	uint16_t m2;
	uint16_t m3;
	uint16_t m4;
}MOTOR;

void Motor_Update_Values(MOTOR *mt_ptr, uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
void Motor_Set_Speed(MOTOR *mt_ptr);
void Motor_Init(TIM_HandleTypeDef *hw_esc_htim);
void Motor_Lock(TIM_HandleTypeDef *hw_esc_htim);
void Motor_Min_Throttle(TIM_HandleTypeDef *hw_esc_htim);

#endif /* INC_MOTOR_H_ */

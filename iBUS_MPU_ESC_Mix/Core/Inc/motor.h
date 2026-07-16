/*
 * motor.h
 *
 *  Created on: Jul 13, 2026
 *      Author: ADMIN
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"
void Motor_Set_Speed(uint16_t speed);
void Motor_Init(TIM_HandleTypeDef *hw_esc_htim);

#endif /* INC_MOTOR_H_ */

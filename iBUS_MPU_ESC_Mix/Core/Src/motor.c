/*
 * motor.c
 *
 *  Created on: Jul 13, 2026
 *      Author: Nemz
 */

#include "motor.h"
#include "assert.h"

static TIM_HandleTypeDef *esc_htim;

void Motor_Init(TIM_HandleTypeDef *hw_esc_htim){
	esc_htim = hw_esc_htim;
	//esc_htim ko duoc rong!
	assert(esc_htim != NULL);
}

void Motor_Set_Speed(MOTOR *mt_ptr){
	esc_htim->Instance->CCR1 = mt_ptr->m1;
	esc_htim->Instance->CCR2 = mt_ptr->m2;
	esc_htim->Instance->CCR3 = mt_ptr->m3;
	esc_htim->Instance->CCR4 = mt_ptr->m4;
}

void Motor_Lock(TIM_HandleTypeDef *hw_esc_htim){
	esc_htim->Instance->CCR1 = 12500;
	esc_htim->Instance->CCR2 = 12500;
	esc_htim->Instance->CCR3 = 12500;
	esc_htim->Instance->CCR4 = 12500;
}

void Motor_Min_Throttle(TIM_HandleTypeDef *hw_esc_htim){
	esc_htim->Instance->CCR1 = 13000;
	esc_htim->Instance->CCR2 = 13000;
	esc_htim->Instance->CCR3 = 13000;
	esc_htim->Instance->CCR4 = 13000;
}

void Motor_Update_Values(MOTOR *mt_ptr, uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    // Ép biên cho 4 động cơ. Đây là M1
    if(m1 > MOTOR_MAX_PWM){
        mt_ptr->m1 = MOTOR_MAX_PWM;
    }
    else if(m1 < MOTOR_MIN_PWM){
        mt_ptr->m1 = MOTOR_MIN_PWM;
    }
    else mt_ptr->m1 = m1;

    //M2
    if(m2 > MOTOR_MAX_PWM){
        mt_ptr->m2 = MOTOR_MAX_PWM;
    }
    else if(m2 < MOTOR_MIN_PWM){
        mt_ptr->m2 = MOTOR_MIN_PWM;
    }
    else mt_ptr->m2 = m2;

    //M3
    if(m3 > MOTOR_MAX_PWM){
        mt_ptr->m3 = MOTOR_MAX_PWM;
    }
    else if(m3 < MOTOR_MIN_PWM){
        mt_ptr->m3 = MOTOR_MIN_PWM;
    }
    else  mt_ptr->m3 = m3;

    //M4
    if(m4 > MOTOR_MAX_PWM){
        mt_ptr->m4 = MOTOR_MAX_PWM;
    }
    else if(m4 < MOTOR_MIN_PWM){
        mt_ptr->m4 = MOTOR_MIN_PWM;
    }
    else mt_ptr->m4 = m4;
}






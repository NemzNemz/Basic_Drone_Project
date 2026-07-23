/*
 * pid.h
 *
 *  Created on: Jul 22, 2026
 *      Author: ADMIN
 */
#include "main.h"
#ifndef INC_PID_H_
#define INC_PID_H_

#define DELTA_T 		0.002f //Tần số 500Hz

typedef struct{
	//Các K này là CỐ ĐỊNH
	float KP;
	float KI;
	float KD;
	float PID_OUT;
	float error_sum;
	float prev_val;
}PID_t;

void test_pid_pitch(uint8_t mt_flag ,uint16_t rc_raw_pitch, uint16_t rc_raw_throttle, float gyro_x, float euler_pitch, PID_t *outer, PID_t *inner);

#endif /* INC_PID_H_ */

/*
 * pid.h
 *
 *  Created on: Jul 22, 2026
 *      Author: ADMIN
 */
#include "main.h"
#ifndef INC_PID_H_
#define INC_PID_H_
//Doi RAD sang độ
#define RAD_TO_DEG 		57.295f
//Tần số 500Hz
#define DELTA_T 		0.002f

typedef struct{
	//Các K này là CỐ ĐỊNH
	float KP;
	float KI;
	float KD;
	float PID_OUT;
	float error_sum;
	float prev_val;
	float IIR_derivative;
}PID_t;

void pid_pitch_roll(uint16_t rc_raw_axis, float gyro_val, float euler_val, PID_t *outer, PID_t *inner);
void pid_yaw_rate(uint16_t rc_raw_axis, float gyroz_val, PID_t *yaw_rate);
void pid_yaw_angle(uint16_t rc_raw_axis, float euler_yaw, float gyroz_val, PID_t *yaw_angle);

void reset_error(PID_t *err);


#endif /* INC_PID_H_ */

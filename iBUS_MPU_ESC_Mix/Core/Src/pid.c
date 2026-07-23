/*
 * pid.c
 *
 *  Created on: Jul 22, 2026
 *      Author: Nemz
 */

#include "pid.h"

void test_pid_pitch(uint8_t mt_flag ,uint16_t rc_raw_pitch, uint16_t rc_raw_throttle, float gyro_x, float euler_pitch, PID_t *outer, PID_t *inner){
	/*
	  Đây là Outer Loop, tính Kp trước này
	*/
	float setpoint = (rc_raw_pitch - 1500) * 0.1f;
	float error = setpoint - euler_pitch;
	float p_temp = error * outer->KP;

	//Tính Ki này
	outer->error_sum = outer->error_sum + error * DELTA_T;
	if (mt_flag == 1 || rc_raw_throttle < 1050) {
		outer->error_sum = 0.0f;
	}
	float i_temp = outer->error_sum *  outer->KI;

	//Kd này
	float derivative = gyro_x;
	float d_temp = - derivative * outer->KD ;
	outer->PID_OUT = p_temp + i_temp + d_temp;
	/*
	  Hết Outer Loop, tới Inner Loop
	*/

	/*
	  INNER LOOP
	*/
	float rate_setpoint = outer->PID_OUT;
	float rate_error = rate_setpoint - gyro_x;
	float p_temp_rate = rate_error * inner->KP;

	//Tính Ki này
	inner->error_sum = inner->error_sum + rate_error * DELTA_T;
	if(mt_flag == 1 || rc_raw_throttle < 1050){
		inner->error_sum = 0;
	}
	float i_temp_rate = inner->error_sum *  inner->KI;

	//Kd này
	float rate_derivative = (gyro_x - inner->prev_val) / DELTA_T;
	inner->prev_val = gyro_x;
	float d_temp_rate = - rate_derivative * inner->KD ;

	inner->PID_OUT = p_temp_rate + i_temp_rate + d_temp_rate;
	/*
	  HẾT INNER LOOP
	*/
}



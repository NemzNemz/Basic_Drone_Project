/*
 * pid.c
 *
 *  Created on: Jul 22, 2026
 *      Author: Nemz
 */

#include "pid.h"
/*
 * Tham số ánh xạ phần cứng:
 * - rc_raw_axis: Tín hiệu RC thô tương ứng từng trục (rc_raw_pitch cho Pitch, rc_raw_roll cho Roll).
 * - gyro_val: Tốc độ quay thô từ MPU6050 (gyro_y cho Pitch, gyro_x cho Roll).
 * - euler_val:   Góc thái độ đã qua lọc từ MPU6050 (euler_pitch cho Pitch, euler_roll cho Roll).
 */
void pid_pitch_roll(uint8_t mt_flag, uint16_t rc_raw_axis, uint16_t rc_raw_throttle, float gyro_val, float euler_val, PID_t *outer, PID_t *inner){
	/*
	  Đây là Outer Loop, tính Kp trước này 0.1f này nhớ sửa lại theo góc tối đa
	*/
	float setpoint = (rc_raw_axis - 1500) * 0.1f;
	float error = setpoint - euler_val;
	float p_temp = error * outer->KP;

	//Tính Ki này
	outer->error_sum = outer->error_sum + error * DELTA_T;
	if (mt_flag == 1 || rc_raw_throttle < 1050) {
		outer->error_sum = 0.0f;
	}
	//Chống bão hoà tích phân khi hệ I quá cao.
	if(outer->error_sum > 500.0f){
		outer->error_sum = 500.0f;
	}
	else if(outer->error_sum < -500.0f){
		outer->error_sum = -500.0f;
	}
	float i_temp = outer->error_sum *  outer->KI;

	//Kd này
	float derivative = gyro_val;
	//Thêm phần lọc nhiẽu IIR vào để hệ KD nó mượt mà hơn
	outer->IIR_derivative = outer->IIR_derivative * 0.3f + derivative * 0.7f;
	float d_temp = - outer->IIR_derivative * outer->KD ;
	outer->PID_OUT = p_temp + i_temp + d_temp;
	/*
	  Hết Outer Loop, tới Inner Loop
	*/

	/*
	  INNER LOOP
	*/
	float rate_setpoint = outer->PID_OUT;
	float rate_error = rate_setpoint - gyro_val;
	float p_temp_rate = rate_error * inner->KP;

	//Tính Ki này
	inner->error_sum = inner->error_sum + rate_error * DELTA_T;
	if(mt_flag == 1 || rc_raw_throttle < 1050){
		inner->error_sum = 0;
	}
	//Chống bão hoà tích phân khi hệ I quá cao.
	if(inner->error_sum > 500.0f){
		inner->error_sum = 500.0f;
	}
	else if(inner->error_sum < -500.0f){
		inner->error_sum = -500.0f;
	}
	float i_temp_rate = inner->error_sum *  inner->KI;

	//Kd này
	float rate_derivative = (gyro_val - inner->prev_val) / DELTA_T;
	inner->prev_val = gyro_val;
	//Thêm phần lọc nhiẽu IIR vào để hệ KD nó mượt mà hơn
	inner->IIR_derivative = inner->IIR_derivative * 0.5f + rate_derivative * 0.5f;
	float d_temp_rate = - inner->IIR_derivative * inner->KD ;
	//PID tổng quát
	inner->PID_OUT = p_temp_rate + i_temp_rate + d_temp_rate;
	/*
	  HẾT INNER LOOP
	*/
}



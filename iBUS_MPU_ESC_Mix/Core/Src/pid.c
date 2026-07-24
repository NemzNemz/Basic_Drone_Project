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
void pid_pitch_roll(uint16_t rc_raw_axis, float gyro_val, float euler_val, PID_t *outer, PID_t *inner){
	/*
	  Đây là Outer Loop, tính Kp trước này. 0.1f này nhớ sửa lại theo góc tối đa
	*/
	float setpoint = (rc_raw_axis - 1500) * 0.1f;
	float error = setpoint - euler_val;
	float p_temp = error * outer->KP;

	//Tính Ki này
	outer->error_sum = outer->error_sum + error * DELTA_T;
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

void pid_yaw_rate(uint16_t rc_raw_axis, float gyroz_val, PID_t *yaw_rate){
	//Tinh Kp
	//1.0f nhớ sửa lai theo góc tối đa
	float gyroz_deg = gyroz_val * RAD_TO_DEG;
	//Tinhs Kp trước này
	float rate_setpoint = (rc_raw_axis - 1500) * 1.0f;
	float rate_error = rate_setpoint - gyroz_deg;
	float p_temp_rate = rate_error * yaw_rate->KP;
	
	//Tinhs Ki
	yaw_rate->error_sum = yaw_rate->error_sum  + rate_error * DELTA_T;
	//Chống bão hoà tích phân khi hệ I quá cao.
	if(yaw_rate->error_sum > 500.0f){
		yaw_rate->error_sum = 500.0f;
	}
	else if(yaw_rate->error_sum < -500.0f){
		yaw_rate->error_sum = -500.0f;
	}
	float i_temp_rate = yaw_rate->error_sum * yaw_rate->KI;

	//Tinh Kd
	float rate_derivative = (gyroz_deg - yaw_rate->prev_val) / DELTA_T;
	yaw_rate->prev_val = gyroz_deg;
	//Thêm phần lọc nhiẽu IIR vào để hệ KD nó mượt mà hơn
	yaw_rate->IIR_derivative = yaw_rate->IIR_derivative * 0.5f + rate_derivative * 0.5f;
	float d_temp_rate = - yaw_rate->IIR_derivative * yaw_rate->KD;
	//Ra PID
	yaw_rate->PID_OUT = p_temp_rate + i_temp_rate + d_temp_rate;
}

void pid_yaw_angle(uint16_t rc_raw_axis, float euler_yaw, float gyroz_val, PID_t *yaw_angle){
	//Tinh Kp
	//1.0f nhớ sửa lai theo góc tối đa
	float angle_setpoint = (rc_raw_axis - 1500) * 1.0f;
	float angle_error = angle_setpoint - euler_yaw;
	if(angle_error > 180.0f){
		angle_error -= 360.0f;
	}
	else if(angle_error < -180.0f){
		angle_error += 360.0f;
	}
	float p_temp_angle = angle_error * yaw_angle->KP;

	//BỎ KI do MPU6050 xài cái này còn tệ hơn với trục YAW chế độ góc xoay
	
	//Tinh Kd
	float angle_derivative = gyroz_val;
	//Thêm phần lọc nhiẽu IIR vào để hệ KD nó mượt mà hơn
	yaw_angle->IIR_derivative = yaw_angle->IIR_derivative * 0.5f + angle_derivative * 0.5f;
	float d_temp_angle = - yaw_angle->IIR_derivative * yaw_angle->KD;
	//Ra PID
	yaw_angle->PID_OUT = p_temp_angle + d_temp_angle;
}

void reset_error(PID_t *err){
	err->error_sum = 0.0f;
}



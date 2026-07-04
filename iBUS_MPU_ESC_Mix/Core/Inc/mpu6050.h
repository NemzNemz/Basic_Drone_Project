/*
 * mpu6050.h
 *
 *  Created on: Jun 1, 2026
 *      Author: Nemz
 */
#include "main.h"

#ifndef MPU6050_H
#define MPU6050_H

#define MPU6050_I2C_ADDR 			(0x68 << 1)
//Đây là GIÁ TRỊ HEX của thanh ghi WHO_AM_I (6bit địa chỉ từ 1->6, bit 0,7 reversed)
#define MPU6050_WHO_AM_I_VAL 		0x68
//Đây là ĐỊA CHỈ HEX của thanh ghi WHO_AM_I
#define MPU6050_WHO_AM_I_REG 		0x75
#define MPU6050_PWR_MGMT1 			0x6B
//Đây là ĐỊA CHỈ HEX của thanh ghi SIGNAL_PATH_RESET
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG 				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
//14 Byte data bắt đầu đọc từ Accel_Xout_H
#define MPU6050_ACCEL_XOUT_H		0x3B
//14 Byte data kết thúc tại Gyro_ZOUT_L
#define MPU6050_RAW_DATA_SIZE 		14
//Định danh biên địa chỉ phù hợp từ dãy thấp đến cao
#define MPU6050_LREG    			MPU6050_SMPLRT_DIV
#define MPU6050_HREG    			MPU6050_WHO_AM_I_REG
//Doi RAD sang độ
#define RAD_TO_DEG 57.295f
#define DEG_TO_RAD 0.017453f

//Struc chứa 3 vectơ x, y, z để sau này đi cùng struct gia tốc và con quay
typedef struct {
	float x, y, z;
}vec3f;

//Struc chứa 2 dạng data thô của MPU-> 3 vecto cho từng loại data
typedef struct{
	vec3f accel;
	vec3f gyro;
	//14byte data thô của MPU
	uint8_t raw_buffer[MPU6050_RAW_DATA_SIZE];
}MPU_MEASUREMENT;

typedef struct {
    float roll;
    float pitch;
    float yaw;
}EULER_MEASUREMENT;

void MPU_INIT(I2C_HandleTypeDef *hi2c_hw);
//Tham số đầu vào là 1 struct
void MPU_RAW_MEASUREMENT(MPU_MEASUREMENT*);
//Lọc nhiễu tĩnh con quay hồi chuyển
void MPU_SET_GYRO_BIAS(float x, float y, float z);
//Chuyển thành các góc thái độ
void CONVERT_TO_ORIENT(MPU_MEASUREMENT*, EULER_MEASUREMENT*);
/*
Hàm phát động đọc ngắt chân INT
Kiểu HAL_StatusTypeDef này tương tự enum, chỉ trả về 4 trạng thái sau:
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 Nhận vào 1 con trỏ kiểu I2C_HandleTypeDef, liên kết với hi2c1 của main.c
*/
HAL_StatusTypeDef MPU_TRIGGER_READ_IT
(I2C_HandleTypeDef *hi2c_hw, MPU_MEASUREMENT*);

#endif /* MPU6050_H */

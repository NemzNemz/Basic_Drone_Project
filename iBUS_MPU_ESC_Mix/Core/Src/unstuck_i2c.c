/*
 * unstuck_i2c.c
 *
 *  Created on: Jun 3, 2026
 *      Author: ADMIN
 */
#include "unstuck_i2c.h"
#include "main.h"
#include "tim.h"
#include "i2c.h"

static void i2c_absolve(void);
static void unstuck_i2c1_rcc_reset(void);
void unstuck_i2c1(void);
void delay_nanos(uint16_t hns);

//Delay nano giây
void delay_nanos(uint16_t hns)
{
    TIM2->CNT = 0;
    //Đợi tràn thì thôi
    while (TIM2->CNT < hns);
}

//Hàm giải vây bus i2c
static void i2c_absolve(void){
	//Cấu hình chân SCL trở thành GPIO output
	GPIO_InitTypeDef SCL = {0};
	SCL.Pin = GPIO_PIN_6;
	SCL.Mode = GPIO_MODE_OUTPUT_PP;
	SCL.Pull = GPIO_NOPULL;
	SCL.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &SCL);

	//Kích 9 xung lên đường SCL
	for(uint8_t i =0; i <9; i++){
		//Keo xuong thap
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		/*Table 11 UM10204. Characteristics of the SDA and SCL bus lines, tối thiểu 1.3us*/
		delay_nanos(15);
		//Keo len cao
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		/*Table 11 UM10204. Characteristics of the SDA and SCL bus lines, tối thiểu 0.6us*/
		delay_nanos(15);
	}

	//Cấu hình lại chân SCL thành chức năng I2C
	SCL.Pin = GPIO_PIN_6;
	SCL.Mode = GPIO_MODE_AF_OD;
	SCL.Pull = GPIO_NOPULL;
	SCL.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	SCL.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &SCL);
}

//Hàm ép buộc reset, giải phóng và khởi tạo lại I2C1
static void unstuck_i2c1_rcc_reset(void){
	__HAL_RCC_I2C1_FORCE_RESET();
	__HAL_RCC_I2C1_RELEASE_RESET();
	//Gọi lại I2C1
	HAL_I2C_Init(&hi2c1);
}

//Hàm giải vây i2c kết hợp 2 hàm trên kia
void unstuck_i2c1(void){
	/*Errata 2.9.1 ES0287*/
	if((hi2c1.ErrorCode & HAL_I2C_ERROR_BERR) != 0){
		/*18.6.6 RM0383: Gán bit BERR, là bit số 8 trong thanh ghi I2C_SR1, về mức 0 (rc_w0)*/
		I2C1->SR1 = (uint16_t)(~I2C_SR1_BERR);
	}else{
		/*18.6.1 RM0383: Xoá cờ I2C_CR1->PE về 0, toàn bộ từ bit số 8->0 trong thanh I2C_SR1 sẽ được xoá*/
		I2C1->CR1 &= ~I2C_CR1_PE;
		/*Đập 9 xung để cứu đường SDA đang bị treo*/
		i2c_absolve();
		unstuck_i2c1_rcc_reset();
	}
}




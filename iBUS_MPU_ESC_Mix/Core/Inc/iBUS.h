/*
 * iBUS.h
 *
 *  Created on: Jun 22, 2026
 *      Author: ADMIN
 */

#ifndef INC_IBUS_H_
#define INC_IBUS_H_

#include "main.h"
#include <stdint.h>

typedef struct{
	uint16_t L_UD;		// Trái lên xuống
	uint16_t L_RL;		// Trái phải trái
	uint16_t R_UD;		// Phải lên xuống
	uint16_t R_RL;		// Phải phải trái
	uint16_t SwA;
	uint16_t SwB;
	uint16_t SwC;
	uint16_t SwD;
	uint16_t VrA;
	uint16_t VrB;
	uint16_t failsafe;
}FS_I6AB_VAR;
//Định danh struct là fs_i6
extern FS_I6AB_VAR fs_i6;
extern uint8_t usart1_rx_flag;
extern uint8_t usart1_rx_data;
extern uint8_t ibus_cplt_flag;
extern uint8_t ibus_rx_buf[32];

uint8_t ibus_check_sum(uint8_t* data, uint8_t len);
void ia6b_decode_data(uint8_t* data, FS_I6AB_VAR*fs_i6);
void fs_i6ab_init(UART_HandleTypeDef *huart_hw);
uint8_t is_failsafe(FS_I6AB_VAR*fs_i6);
void iBUS_Parse_Byte(uint8_t* data);
uint8_t is_iBUS_Throttle_Min(void);
uint8_t is_iBUS_Received(void);

#endif /* INC_IBUS_H_ */

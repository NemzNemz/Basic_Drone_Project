/*
 * iBUS.c
 *
 *  Created on: Jun 22, 2026
 *      Author: ADMIN
 */
#include "iBUS.h"
#include "assert.h"

static UART_HandleTypeDef *ibus_huart;
uint8_t usart2_rx_data;
uint8_t usart2_rx_flag;
uint8_t ibus_rx_buf[32];
uint8_t ibus_cplt_flag = 0;
FS_I6AB_VAR fs_i6;

uint8_t ibus_check_sum(uint8_t* data, uint8_t len){
	//Biến tạm checksum 0xFFFF để trừ dần
	uint16_t CKC = 0xFFFF;
	uint16_t cal_CKC = 0;
	for(uint8_t i =0; i < (len -2); i++){
		CKC = CKC - data[i];
	}
	//2Byte cuối của data mà bằng CKC-30 byte đầu là ok
	cal_CKC = ((data[31] << 8) | data[30]);
	if(CKC ==  cal_CKC) return 1;
	else return 0;
}

void ia6b_decode_data(uint8_t* data, FS_I6AB_VAR* fs_i6){
	// Kênh CHx, byte thứ 2-3(CH1), data dạng 4+12bit, 4bit cờ H + 12bit data L
	fs_i6 -> R_RL 		= ((data[2] |  data[3] << 8) & 0x0FFF);		//CH1
	fs_i6 -> R_UD		= ((data[4] |  data[5] << 8) & 0x0FFF);		//CH2
	fs_i6 -> L_UD		= ((data[6] |  data[7] << 8) & 0x0FFF);		//CH3
	fs_i6 -> L_RL		= ((data[8] |  data[9] << 8) & 0x0FFF);		//CH4
	fs_i6 -> SwA		= ((data[10] |  data[11] << 8) & 0x0FFF);
	fs_i6 -> SwB		= ((data[12] |  data[13] << 8) & 0x0FFF);
	// Chỉ cần 4 bit cao, chủ yếu là lấy cờ failsafe của RX
	fs_i6 -> failsafe	= (data[13] >> 4);
}

uint8_t is_failsafe(FS_I6AB_VAR*fs_i6){
	//Khac 0 la co failsafe
	return fs_i6 -> failsafe != 0;
}

void iBUS_Parse_Byte(uint8_t* data){
	static uint8_t index = 0;
	switch(index){
	//Byte 0 phải bằng 0x20
	case 0:
		if(*data == 0x20){
			ibus_rx_buf[index] = *data;
			index++;
		}
		break;
		//Byte 1 phải bằng 0x40, không thì bắt đầu lại
	case 1:
		if(*data == 0x40){
			ibus_rx_buf[index] = *data;
			index++;
		}
		else index = 0;
		break;
		//Byte kết thúc frame thì reset index
	case 31:
		ibus_rx_buf[index] = *data;
		index = 0;
		//Xong 1 frame thì bật biến cờ lên
		ibus_cplt_flag = 1;
		break;
		//Từ byte 2-30 thì cứ tăng dần biến đếm
	default:
		ibus_rx_buf[index] = *data;
		index++;
		break;
	}
}

void fs_i6ab_init(UART_HandleTypeDef *huart_hw){
	ibus_huart = huart_hw;
	assert(ibus_huart != NULL);
	// Cho mấy cái cờ về 0 hết đã
    ibus_cplt_flag = 0;
    usart2_rx_flag = 0;
    //Bật ngắt lên
    HAL_UART_Receive_IT(ibus_huart, &usart2_rx_data, 1);
}

uint8_t is_iBUS_Throttle_Min(){
	if(ibus_cplt_flag == 1){
		// Reset cờ truyền frame ibus
		ibus_cplt_flag = 0;
		if(ibus_check_sum(&ibus_rx_buf[0], 32) == 1){
			ia6b_decode_data(&ibus_rx_buf[0], &fs_i6);
			//Nếu như mức ga (dãy iBUS 1000-2000) đang thấp thì trả về 1
			if(fs_i6.L_UD <1010) return 1;
		}
	}
	return 0;
}

uint8_t is_iBUS_Received(){
	if(ibus_cplt_flag == 1){
		// Reset cờ truyền frame ibus
		ibus_cplt_flag = 0;
		if(ibus_check_sum(&ibus_rx_buf[0], 32) == 1){
			ia6b_decode_data(&ibus_rx_buf[0], &fs_i6);
			//Có nhận data thì trả về 1
			return 1;
		}
	}
	return 0;
}

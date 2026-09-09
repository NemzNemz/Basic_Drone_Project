/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "unstuck_i2c.h"
#include "mpu6050.h"
#include "iBUS.h"
#include "motor.h"
#include "battery.h"
#include "pid.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//DEBUG DWT
volatile uint32_t dwt_last_tim10_tick = 0;
volatile uint32_t dwt_tim10_period_ticks = 0; // Chu kỳ giữa 2 lần vào ngắt TIM10
volatile float tim10_period_us = 0.0f;        // Chu kỳ TIM10 tính bằng micro-giây

volatile uint32_t dwt_loop_exec_ticks = 0;    // Thời gian CPU xử lý 1 vòng lặp PID
volatile float loop_exec_us = 0.0f;           // Thời gian thực thi PID (us)


uint32_t tim1_ch1 = 12500;
uint32_t tim1_ch2 = 12500;
uint32_t tim1_ch3 = 12500;
uint32_t tim1_ch4 = 12500;

float Pitch_PID = 0.0f;
float Roll_PID = 0.0f;
//Chỉ để tường minh, ko cần tới nỗi 3 biến
float Yaw_RT_PID = 0.0f;
float Yaw_ANGLE_PID = 0.0f;
float Yaw_FINAL_PID = 0.0f;
//Biến tham chiếu góc mục tiêu 
float Yaw_HEADING_REF = 0.0f;

//PID kép trục Pitch
PID_t pid_pitch_outer = {.KP = 2.0f, .KI = 0.0f, .KD = 0.0f, .error_sum = 0.0f, .prev_val = 0.0f, .IIR_derivative = 0.0f, .PID_OUT = 0.0f};
PID_t pid_pitch_inner = {.KP = 1.0f, .KI = 0.0f, .KD = 0.0f, .error_sum = 0.0f, .prev_val = 0.0f, .IIR_derivative = 0.0f, .PID_OUT = 0.0f};

//PID kép trục Roll
PID_t pid_roll_outer  = {.KP = 2.0f, .KI = 0.0f, .KD = 0.0f, .error_sum = 0.0f, .prev_val = 0.0f, .IIR_derivative = 0.0f, .PID_OUT = 0.0f};
PID_t pid_roll_inner  = {.KP = 1.0f, .KI = 0.0f, .KD = 0.0f, .error_sum = 0.0f, .prev_val = 0.0f, .IIR_derivative = 0.0f, .PID_OUT = 0.0f};

//PID đơn trục Yaw, tốc độ góc
PID_t pid_yaw_rt  = {.KP = 1.0f, .KI = 0.0f, .KD = 0.0f, .error_sum = 0.0f, .prev_val = 0.0f, .IIR_derivative = 0.0f, .PID_OUT = 0.0f};

//PID đơn trục Yaw, chỉ góc
PID_t pid_yaw_ag  = {.KP = 2.0f, .KI = 0.0f, .KD = 0.0f, .error_sum = 0.0f, .prev_val = 0.0f, .IIR_derivative = 0.0f, .PID_OUT = 0.0f};


MPU_MEASUREMENT mpu_mea = {0};
EULER_MEASUREMENT eul_mea = {0};
//Mặc định xung PWM xuất ra là 125us, tức méo quay quạt đâu
MOTOR motor_speed = {12500, 12500, 12500, 12500};

volatile uint8_t mpu_data_ready_flag = 0;
volatile uint8_t ibus_loss_connect_flag = 0;
volatile uint8_t ibus_loss_connect_cnt =0;

//MPU hàng bãi, áp dụng logic check xem nó có bị lỏ như mất RX ko
volatile uint8_t mpu_loss_cnt = 0;
volatile uint8_t mpu_loss_flag = 0;
//Biến lấy đại 1 trục bất kì của MPU, vì mpu ko thể có data cũ và mới giống nhau
int16_t old_raw_z = 0;
int16_t curren_raw_z = 0;

uint8_t failsafe_flag = 0;
//Mặc định là lock ko cho chạy motor
uint8_t motor_lock_flag = 1;
uint16_t ibus_prev_val =0;

//Cờ cho ngắt PID
uint8_t pid_flag = 0;
uint32_t raw_adc_val;
//Bien đọc điện áp của cục pin
float BAT_vol;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Pre_Flight_Check(void);
void Buzzer_Error_Beep(void);
void Buzzer_Status_Beep(void);
void Buzzer_Success(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void ESC_Calib(void);
void Buzzer_Ont(void);
void Motor_Safety(MOTOR *mt_ptr);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // Kích hoạt khối TRACE và bộ đếm CYCCNT của nhân ARM Cortex-M4
  //CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  //DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  //Timer cho ESC
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

  //Timer cho unstuck_i2c hàm nanos
  HAL_TIM_Base_Start(&htim2);

  //Timer dành cho PID
  HAL_TIM_Base_Start_IT(&htim10);
  //Timer dành cho check connect RX vật lý, chạy ngắt 500Hz
  HAL_TIM_Base_Start_IT(&htim11);

  MPU_INIT(&hi2c1);
  fs_i6ab_init(&huart2);
  Motor_Init(&htim1);
  BAT_INIT(&hadc1, &raw_adc_val);
  MPU_CALIB_GYRO(500, &mpu_mea);

  //Hàm tổng hợp các bước check an toàn bay
  Pre_Flight_Check();
  //Timer cho cái còi lởm
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if (mpu_data_ready_flag == 1) {
		mpu_data_ready_flag = 0;
		// Gọi hàm toán tử giải mã đại số bitwise bằng cơ chế truyền con trỏ tham chiếu
		MPU_RAW_MEASUREMENT(&mpu_mea);
		CONVERT_TO_ORIENT(&mpu_mea, &eul_mea);
		curren_raw_z = (int16_t)((mpu_mea.raw_buffer[4] << 8) | mpu_mea.raw_buffer[5]);
		//Nếu daata biến thiên thì ok mpu vẫn chạy
		if(curren_raw_z != old_raw_z){
			mpu_loss_cnt = 0;
			mpu_loss_flag = 0;
		}
		//Data cũ sẽ chạy dưới đây
		old_raw_z = curren_raw_z;
	}

	  if(ibus_cplt_flag == 1){
		  // Reset cờ truyền frame ibus
		  ibus_cplt_flag = 0;
		  if(ibus_check_sum(&ibus_rx_buf[0], 32) == 1){
			  ia6b_decode_data(&ibus_rx_buf[0], &fs_i6);
			  ibus_loss_connect_cnt = 0;
			  ibus_loss_connect_flag = 0;
			  //Nếu
			  if(is_failsafe(&fs_i6)!= 0){
				  failsafe_flag = 1;
			  }
			  else failsafe_flag = 0;
		  }
	  }
	  Motor_Safety(&motor_speed);
	  
	  //Lấy giá trị điện áp cục pin
	  BAT_GET_VOL(raw_adc_val, &BAT_vol);

	  //Nếu pin yếu thì còi cho biết còn bay về
	  if(is_bat_low(BAT_vol) == 1 && motor_lock_flag == 0){
		Buzzer_On();
	  }

	  if(pid_flag == 1){
		//uint32_t start_exec = DWT->CYCCNT;
		pid_flag = 0;
		if(fs_i6.L_UD <= 1015 || motor_lock_flag == 1){
			reset_error(&pid_roll_outer);
			reset_error(&pid_roll_inner);
			reset_error(&pid_pitch_outer);
			reset_error(&pid_pitch_inner);
			reset_error(&pid_yaw_rt);
			reset_error(&pid_yaw_ag);

			Pitch_PID = 0.0f;
			Roll_PID = 0.0f;
			Yaw_FINAL_PID = 0.0f;
		}
		//PID Kép trục Pitch
		pid_pitch_roll(
					   3000 - fs_i6.R_UD,
		               mpu_mea.gyro.y,
		               eul_mea.pitch,
		               &pid_pitch_outer,
		               &pid_pitch_inner);
		Pitch_PID = pid_pitch_inner.PID_OUT;
		
		//PID kép trục Roll
		pid_pitch_roll(
		               fs_i6.R_RL,
		               -mpu_mea.gyro.x,
		               eul_mea.roll,
		               &pid_roll_outer,
		               &pid_roll_inner);
		Roll_PID = pid_roll_inner.PID_OUT;
		
		if(fs_i6.L_RL < 1450 ||  fs_i6.L_RL > 1550){	
			//Tham chiếu để Drone biết nên điều tốc motor xoay ra sao
			Yaw_HEADING_REF = eul_mea.yaw;
			//PID đơn trục Yaw theo tốc độ góc
			pid_yaw_rate(fs_i6.L_RL, mpu_mea.gyro.z, &pid_yaw_rt);
			Yaw_RT_PID = pid_yaw_rt.PID_OUT;
			Yaw_FINAL_PID = Yaw_RT_PID;
		}
		else {
			//PID đơn trục Yaw theo góc thái độ
			pid_yaw_angle(Yaw_HEADING_REF, eul_mea.yaw, mpu_mea.gyro.z, &pid_yaw_ag);
			Yaw_ANGLE_PID = pid_yaw_ag.PID_OUT;
			Yaw_FINAL_PID = Yaw_ANGLE_PID;
		}

		//Chuyển đổi tín hiệu tay cầm thành xung PWM
		uint16_t target_pwm_m1 = (uint16_t)(12500 + (fs_i6.L_UD - 1000) * 12.5f +
	  							Pitch_PID  +
								Roll_PID -
								Yaw_FINAL_PID);
	  							//(fs_i6.L_RL - 1500) * 5.0f);
	  
		uint16_t target_pwm_m2 = (uint16_t)(12500 + (fs_i6.L_UD - 1000) * 12.5f -
	  							Pitch_PID +
								Roll_PID +
								Yaw_FINAL_PID);
								//(fs_i6.L_RL - 1500) * 5.0f);
	  
		uint16_t target_pwm_m3 = (uint16_t)(12500 + (fs_i6.L_UD - 1000) * 12.5f -
	  							Pitch_PID -
								Roll_PID -
								Yaw_FINAL_PID);
								//(fs_i6.L_RL - 1500) * 5.0f);

		uint16_t target_pwm_m4 = (uint16_t)(12500 + (fs_i6.L_UD - 1000) * 12.5f +
	  							Pitch_PID -
								Roll_PID +
								Yaw_FINAL_PID);
								//(fs_i6.L_RL - 1500) * 5.0f);
	  	Motor_Update_Values(&motor_speed, target_pwm_m4, target_pwm_m2, target_pwm_m3, target_pwm_m1);
	  	// Tính thời gian CPU chạy hết khối lệnh trên:
	  	//dwt_loop_exec_ticks = DWT->CYCCNT - start_exec;
	  	//loop_exec_us = (float)dwt_loop_exec_ticks / 100.0f;
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Pre_Flight_Check(void){
	while(is_iBUS_Received() == 0){
		Buzzer_Error_Beep();
	}
	Buzzer_Success();
	
	if(fs_i6.SwB == 2000){
		Buzzer_Status_Beep();
		ESC_Calib ();
		while(fs_i6.SwB != 1000){
			is_iBUS_Received();
			Buzzer_Status_Beep();
		}
	}
	Buzzer_Success();
	
	//Nếu ga chưa về 0 sau quá trình khởi tạo thì ko thể chạy hàm main, đảm bảo an toàn
	//Nếu ga về 0 mà chưa chuyển về Disarm thì cũng khoá ko cho bay
	while(is_iBUS_Throttle_Min()== 0 || fs_i6.SwA == 2000){
		Buzzer_Error_Beep();
	}
	Buzzer_Success();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // Kiểm tra chân phát động ngắt có phải là PB1
  if (GPIO_Pin == GPIO_PIN_1)
  {
    // Phát động lệnh đọc chuỗi 14 bytes không chặn qua ngắt I2C1
    HAL_StatusTypeDef status = MPU_TRIGGER_READ_IT(&hi2c1, &mpu_mea);

    // Bẫy lỗi treo bus nếu xuất hiện bằng cách giải vây
    if (status == HAL_BUSY)
    {
      unstuck_i2c1();
    }
  }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  // Kiểm tra thực thể bus vừa hoàn thành ngắt có phải là I2C1 hay không
  if (hi2c->Instance == I2C1)
  {
    mpu_data_ready_flag = 1;
  }
}

//Callback cho nhận data iBUS
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){


	if(huart->Instance == USART2){
		usart2_rx_flag = 1;
		//Chạy hàm đọc 32byte iBUS
		iBUS_Parse_Byte(&usart2_rx_data);
		HAL_UART_Receive_IT(&huart2, &usart2_rx_data, 1);
	}
}

void Buzzer_On(void){
	TIM3->ARR = 99;
	TIM3->CCR1 = 50;					
	TIM3->PSC = 284;				 
	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void Buzzer_Off(void){
	TIM3->CCR1 = 0;
}

// Buzz cao dành cho tín hiệu iBUS
void Buzzer_Error_Beep(void){
	TIM3->ARR = 99;
	TIM3->CCR1 = 50; // Duy trì chính xác 50% Duty Cycle cho màng loa rung cực đại
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	TIM3->PSC = 284;
	HAL_Delay(100);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	HAL_Delay(100);
}

void Buzzer_Status_Beep(void){
	TIM3->ARR = 99;
	TIM3->CCR1 = 50; // Duy trì chính xác 50% Duty Cycle cho màng loa rung cực đại
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	TIM3->PSC = 999;
	HAL_Delay(100);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	HAL_Delay(100);
}

//Hàm AI viết để nghe tiếng như winXP
void Buzzer_Success(void)
{
	TIM3->ARR = 99;
	TIM3->CCR1 = 35;

	const uint16_t psc[] =
	{
			270,
			315,
			365,
			395
	};

	const uint16_t duration[] =
	{
			120,
			120,
			150,
			260
	};

	for(uint8_t i = 0; i < 4; i++)
	{
		TIM3->PSC = psc[i];

		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		HAL_Delay(duration[i]);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);

		HAL_Delay(35);
	}
	HAL_Delay(1000);
}

void ESC_Calib(){
	TIM1->CCR1 = 25000;
	TIM1->CCR2 = 25000;
	TIM1->CCR3 = 25000;
	TIM1->CCR4 = 25000;
	HAL_Delay(7000);

	TIM1->CCR1 = 12500;
	TIM1->CCR2 = 12500;
	TIM1->CCR3 = 12500;
	TIM1->CCR4 = 12500;
	HAL_Delay(8000);
}

void Motor_Safety(MOTOR *mt_ptr){
	  //Nếu như gạt cần bay và ga thấp nhất thì mới cho bay + gỡ cờ khoá
	  if(fs_i6.SwA == 2000 && is_iBUS_Throttle_Min() == 1) {
	  	if(motor_lock_flag == 1){
			// Chụp lại góc Yaw thực tế lúc drone nằm trên mặt đất
			Yaw_HEADING_REF = eul_mea.yaw; 
            motor_lock_flag = 0;
		}
	  	//Pin ko yếu thì ko có còi đâu
	  	if(is_bat_low(BAT_vol) == 0) {
	  		Buzzer_Off();
	  	}
	    //Động cơ quay chậm để cho biết sẵn sàng
		Motor_Min_Throttle(&htim1);
	  }

	  //Nếu như gạt cần bay XUỐNG (SwA = 2000) nhưng ga VẪN CAO
	  if(fs_i6.SwA == 2000 && motor_lock_flag == 1) {
		Buzzer_On();
	  }

	  //Gạt cần SwA lên là khoá, và tắt kèn
	  if(fs_i6.SwA == 1000) {
		  motor_lock_flag = 1;
		  Buzzer_Off();
	  }

	  //Nếu cờ lock đang bật thì khoá động cơ
	  if(motor_lock_flag == 1){
	  	 Motor_Lock(&htim1);
	  }

	  //Nếu bất kì điều kiện nào trong đây được kích hoạt thì báo còi + giảm tốc lực
	  else if(failsafe_flag == 1 || ibus_loss_connect_flag == 1 || mpu_loss_flag == 1){
		Buzzer_On();
		Motor_Min_Throttle(&htim1);
	  }
	  //Thoãa hết thì bay
	  else 
	  {
	  	//Nếu ga cao cao tí thì nhận điều tốc bằng PWM
	  	if(fs_i6.L_UD > 1050){
			Motor_Set_Speed(mt_ptr);
	  	}
	  	else{
			Motor_Min_Throttle(&htim1);
		}
	  	//Còi tắt nếu pin ko yếu
	  	if(is_bat_low(BAT_vol) == 0) {
	  		Buzzer_Off();
	  	}
	  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//Boi vi ngat nao cung nhay vao ham nay, nen phai check xem co phai dung TIM khong?
	if (htim->Instance == htim11.Instance) {
		ibus_loss_connect_cnt++;
		mpu_loss_cnt++;
		if (ibus_loss_connect_cnt >= 250) {
			//Bật cờ gì đó ở đây
			ibus_loss_connect_flag = 1;
			ibus_loss_connect_cnt = 0;
		}
		if (mpu_loss_cnt >= 50){
			//MPU hẻo rồi đó
			mpu_loss_flag = 1;
			mpu_loss_cnt = 0;
		}
	}
	//Nếu là ngắt TIM10 cho PID thì đảo chân để test cái, mốt xoá sau
	if (htim->Instance == htim10.Instance){
		//Debug lần đầi với DWT
		//uint32_t current_tick = DWT->CYCCNT;
		//dwt_tim10_period_ticks = current_tick - dwt_last_tim10_tick;
		//dwt_last_tim10_tick = current_tick;

		// Ở xung nhịp 100 MHz: 100 ticks = 1 us
		//tim10_period_us = (float)dwt_tim10_period_ticks / 100.0f;

		//Chân này ko còn chức năng debug nữa, chuyển dịch sang INT của MPU
		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
		pid_flag = 1;
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

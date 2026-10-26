#ifndef FIRMWARE_HOST_COMPAT_H
#define FIRMWARE_HOST_COMPAT_H
#ifndef FIRMWARE_HOST_TEST
#error "Header chi danh cho host verification"
#endif
#include <stdint.h>
#include <stddef.h>
#define __MAIN_H
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { HAL_OK=0, HAL_ERROR=1, HAL_BUSY=2, HAL_TIMEOUT=3 } HAL_StatusTypeDef;
typedef struct { volatile uint32_t CCR1,CCR2,CCR3,CCR4,ARR,PSC,CNT; } TIM_TypeDef;
typedef struct { volatile uint32_t CR1,SR1; } I2C_TypeDef;
typedef struct { uint32_t marker; } GPIO_TypeDef;
typedef struct { TIM_TypeDef *Instance; } TIM_HandleTypeDef;
typedef struct { I2C_TypeDef *Instance; uint32_t State,ErrorCode; } I2C_HandleTypeDef;
typedef struct { void *Instance; } UART_HandleTypeDef;
typedef struct { void *Instance; } ADC_HandleTypeDef;
typedef struct { uint32_t Pin,Mode,Pull,Speed,Alternate; } GPIO_InitTypeDef;
typedef struct { uint32_t PLLState,PLLSource,PLLM,PLLN,PLLP,PLLQ; } HostPLL;
typedef struct { uint32_t OscillatorType,HSEState; HostPLL PLL; } RCC_OscInitTypeDef;
typedef struct { uint32_t ClockType,SYSCLKSource,AHBCLKDivider,APB1CLKDivider,APB2CLKDivider; } RCC_ClkInitTypeDef;
extern TIM_TypeDef host_tim[5];
extern I2C_TypeDef host_i2c;
extern GPIO_TypeDef host_gpio;
extern unsigned host_uart_instance;
#define TIM1 (&host_tim[0])
#define TIM2 (&host_tim[1])
#define TIM3 (&host_tim[2])
#define TIM10 (&host_tim[3])
#define TIM11 (&host_tim[4])
#define I2C1 (&host_i2c)
#define GPIOB (&host_gpio)
#define USART2 ((void *)&host_uart_instance)
#define GPIO_PIN_1 2U
#define GPIO_PIN_6 64U
#define GPIO_PIN_RESET 0U
#define GPIO_PIN_SET 1U
#define GPIO_MODE_OUTPUT_PP 1U
#define GPIO_MODE_AF_OD 2U
#define GPIO_NOPULL 0U
#define GPIO_SPEED_FREQ_VERY_HIGH 3U
#define GPIO_AF4_I2C1 4U
#define TIM_CHANNEL_1 0U
#define TIM_CHANNEL_2 4U
#define TIM_CHANNEL_3 8U
#define TIM_CHANNEL_4 12U
#define I2C_MEMADD_SIZE_8BIT 1U
#define HAL_I2C_ERROR_BERR 1U
#define I2C_SR1_BERR (1U<<8)
#define I2C_CR1_PE 1U
#define __HAL_I2C_RESET_HANDLE_STATE(h) ((h)->State=0U)
void host_rcc_reset(unsigned release);
#define __HAL_RCC_I2C1_FORCE_RESET() host_rcc_reset(0U)
#define __HAL_RCC_I2C1_RELEASE_RESET() host_rcc_reset(1U)
#define __HAL_RCC_PWR_CLK_ENABLE() ((void)0)
#define __HAL_PWR_VOLTAGESCALING_CONFIG(v) ((void)(v))
#define PWR_REGULATOR_VOLTAGE_SCALE1 1U
#define RCC_OSCILLATORTYPE_HSE 1U
#define RCC_HSE_ON 1U
#define RCC_PLL_ON 1U
#define RCC_PLLSOURCE_HSE 1U
#define RCC_PLLP_DIV2 2U
#define RCC_CLOCKTYPE_HCLK 1U
#define RCC_CLOCKTYPE_SYSCLK 2U
#define RCC_CLOCKTYPE_PCLK1 4U
#define RCC_CLOCKTYPE_PCLK2 8U
#define RCC_SYSCLKSOURCE_PLLCLK 1U
#define RCC_SYSCLK_DIV1 1U
#define RCC_HCLK_DIV1 1U
#define RCC_HCLK_DIV2 2U
#define FLASH_LATENCY_3 3U
uint32_t __get_PRIMASK(void);
void __disable_irq(void);
void __set_PRIMASK(uint32_t value);
void Error_Handler(void);
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *cfg);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *cfg,uint32_t latency);
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *h);
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *h);
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *h,uint32_t channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *h,uint32_t channel);
void HAL_Delay(uint32_t ms);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *h,uint8_t *data,uint16_t size);
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *h,uint32_t *data,uint32_t size);
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *h);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *h,uint16_t addr,uint16_t reg,uint16_t regsize,uint8_t *data,uint16_t size,uint32_t timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *h,uint16_t addr,uint16_t reg,uint16_t regsize,uint8_t *data,uint16_t size,uint32_t timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *h,uint16_t addr,uint16_t reg,uint16_t regsize,uint8_t *data,uint16_t size);
void HAL_GPIO_Init(GPIO_TypeDef *gpio,GPIO_InitTypeDef *cfg);
void HAL_GPIO_WritePin(GPIO_TypeDef *gpio,uint16_t pin,uint32_t value);
void HAL_GPIO_EXTI_Callback(uint16_t pin);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *h);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *h);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *h);
int host_run_main(void);
void host_stop_main(void);
#ifdef __cplusplus
}
#endif
#endif

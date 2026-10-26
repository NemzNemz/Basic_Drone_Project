#ifndef FIRMWARE_FAKE_HAL_H
#define FIRMWARE_FAKE_HAL_H
#include "host_compat.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "tim.h"
#include "i2c.h"
#include "adc.h"
#include "usart.h"
#include "pid.h"
#include "mpu6050.h"
#include "iBUS.h"
#include "motor.h"
#include "battery.h"
#include "unstuck_i2c.h"
extern MPU_MEASUREMENT mpu_mea;
extern EULER_MEASUREMENT eul_mea;
extern MOTOR motor_speed;
extern PID_t pid_pitch_outer,pid_pitch_inner,pid_roll_outer,pid_roll_inner,pid_yaw_rt,pid_yaw_ag;
extern float Pitch_PID,Roll_PID,Yaw_FINAL_PID,Yaw_HEADING_REF,BAT_vol;
extern volatile uint32_t raw_adc_val;
extern volatile uint8_t mpu_data_ready_flag,mpu_loss_flag,mpu_loss_cnt,ibus_loss_connect_flag,ibus_loss_connect_cnt,pid_flag;
extern uint8_t motor_lock_flag,failsafe_flag,ibus_fresh_data,ibus_snapshot[32];
extern int16_t curren_raw_z,old_raw_z;
void Motor_Safety(MOTOR *motor);
void Pre_Flight_Check(void);
void ESC_Calib(void);
typedef struct { uint32_t ms,ccr[4]; } HostDelay;
typedef struct {
    uint8_t regs[256],calibration_frame[14],sample_frame[14],calibration_sequence[4][14];
    uint32_t calibration_sequence_count,calibration_sequence_index;
    uint8_t boot_frame[32],iteration_frame[4][32],restore_frame[32];
    uint32_t reg_writes[256],read_calls,frame_reads,write_calls,async_calls,i2c_init_calls;
    HAL_StatusTypeDef async_status,blocking_status;
    uint32_t blocking_fail_remaining;
    uint16_t addr,reg,regsize,size;
    uint8_t *buffer,*uart_buffer;
    UART_HandleTypeDef *uart_handle;
    uint32_t uart_calls,adc_calls,adc_size,*adc_buffer;
    uint32_t primask,mask_calls,restore_calls,reset_calls,gpio_calls,pwm_start_calls;
    uint32_t delay_count,delay_total;
    HostDelay delays[1024];
    uint32_t auto_uart,auto_mpu,drive_main,iteration,max_iterations,restore_iteration,skip_frame_mask;
    void (*iteration_hook)(uint32_t iteration);
} FakeHal;
extern FakeHal fake;
void fake_reset(void);
void app_reset(void);
void make_frame(uint8_t *dst,uint16_t roll,uint16_t pitch,uint16_t throttle,uint16_t yaw,uint16_t swa,uint16_t swb,uint8_t flags);
void feed_parser(const uint8_t *frame);
void feed_uart(const uint8_t *frame);
void put_raw(uint8_t *frame,unsigned offset,int16_t value);
#ifdef __cplusplus
}
#endif
#endif

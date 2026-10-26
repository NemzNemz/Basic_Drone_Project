#include "fake_hal.h"
#include <cstring>
#include <cstdlib>
TIM_TypeDef host_tim[5];
I2C_TypeDef host_i2c;
GPIO_TypeDef host_gpio;
unsigned host_uart_instance;
TIM_HandleTypeDef htim1={TIM1},htim2={TIM2},htim3={TIM3},htim10={TIM10},htim11={TIM11};
I2C_HandleTypeDef hi2c1={I2C1,0,0};
ADC_HandleTypeDef hadc1={0};
UART_HandleTypeDef huart2={USART2};
FakeHal fake;
void fake_reset(void)
{
    std::memset(&fake,0,sizeof fake);
    std::memset(host_tim,0,sizeof host_tim);
    std::memset(&host_i2c,0,sizeof host_i2c);
    hi2c1.Instance=I2C1;hi2c1.State=0;hi2c1.ErrorCode=0;
    htim1.Instance=TIM1;htim2.Instance=TIM2;htim3.Instance=TIM3;
    htim10.Instance=TIM10;htim11.Instance=TIM11;huart2.Instance=USART2;
    fake.regs[0x75]=0x68;fake.regs[0x6b]=0x40;
    fake.async_status=HAL_OK;fake.blocking_status=HAL_OK;
    put_raw(fake.sample_frame,4,2048);
}
void app_reset(void)
{
    // Reset qua API hoac bien public; khong bo static cua production.
    uint8_t zero=0;
    for(unsigned i=0;i<32;i++) iBUS_Parse_Byte(&zero);
    std::memset(ibus_rx_buf,0,32);std::memset(ibus_mailbox,0,32);
    std::memset(ibus_snapshot,0,32);std::memset(&fs_i6,0,sizeof fs_i6);
    ibus_cplt_flag=0;usart2_rx_flag=0;
    std::memset(&mpu_mea,0,sizeof mpu_mea);std::memset(&eul_mea,0,sizeof eul_mea);
    motor_speed={12500,12500,12500,12500};
    pid_pitch_outer={};pid_pitch_inner={};pid_roll_outer={};pid_roll_inner={};pid_yaw_rt={};pid_yaw_ag={};
    pid_pitch_outer.KP=pid_roll_outer.KP=pid_yaw_ag.KP=2;
    pid_pitch_inner.KP=pid_roll_inner.KP=pid_yaw_rt.KP=1;
    Pitch_PID=Roll_PID=Yaw_FINAL_PID=Yaw_HEADING_REF=0;
    mpu_data_ready_flag=mpu_loss_flag=mpu_loss_cnt=ibus_loss_connect_flag=ibus_loss_connect_cnt=pid_flag=0;
    motor_lock_flag=1;failsafe_flag=ibus_fresh_data=0;old_raw_z=curren_raw_z=0;
    BAT_vol=14;raw_adc_val=1100;
    is_bat_low(14);MPU_SET_GYRO_BIAS(0,0,0);Motor_Init(&htim1);
}
void put_raw(uint8_t *f,unsigned offset,int16_t value)
{
    const uint16_t bits=(uint16_t)value;
    f[offset]=(uint8_t)(bits>>8);f[offset+1]=(uint8_t)bits;
}
void make_frame(uint8_t *f,uint16_t r,uint16_t p,uint16_t t,uint16_t y,uint16_t a,uint16_t b,uint8_t flags)
{
    std::memset(f,0,32);f[0]=0x20;f[1]=0x40;
    const uint16_t values[6]={r,p,t,y,a,b};
    for(unsigned i=0;i<6;i++){f[2+2*i]=(uint8_t)values[i];f[3+2*i]=(uint8_t)(values[i]>>8);}
    f[13]|=(uint8_t)(flags<<4);
    uint32_t sum=0;for(unsigned i=0;i<30;i++)sum+=f[i];
    const uint16_t check=(uint16_t)(65535U-sum);f[30]=(uint8_t)check;f[31]=(uint8_t)(check>>8);
}
void feed_parser(const uint8_t *f){for(unsigned i=0;i<32;i++){uint8_t b=f[i];iBUS_Parse_Byte(&b);}}
void feed_uart(const uint8_t *f)
{
    for(unsigned i=0;i<32;i++){
        if(!fake.uart_buffer) std::abort();
        *fake.uart_buffer=f[i];HAL_UART_RxCpltCallback(&huart2);
    }
}
uint32_t __get_PRIMASK(void)
{
    if(fake.drive_main){
        fake.iteration++;
        if(fake.iteration>fake.max_iterations)host_stop_main();
        if(fake.iteration<=4 && !(fake.skip_frame_mask&(1U<<(fake.iteration-1))))feed_uart(fake.iteration_frame[fake.iteration-1]);
        if(fake.iteration_hook)fake.iteration_hook(fake.iteration);
        HAL_TIM_PeriodElapsedCallback(&htim10);
    }
    return fake.primask;
}
void __disable_irq(void){fake.primask=1;fake.mask_calls++;}
void __set_PRIMASK(uint32_t v)
{
    fake.primask=v;fake.restore_calls++;
    if(fake.drive_main && fake.restore_iteration==fake.iteration && v==0)feed_uart(fake.restore_frame);
}
void host_rcc_reset(unsigned release){(void)release;fake.reset_calls++;}
HAL_StatusTypeDef HAL_Init(void){return HAL_OK;}
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *c){(void)c;return HAL_OK;}
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *c,uint32_t l){(void)c;(void)l;return HAL_OK;}
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *h){(void)h;return HAL_OK;}
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *h){if(h->Instance==TIM10)HAL_TIM_PeriodElapsedCallback(h);return HAL_OK;}
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *h,uint32_t c){(void)h;(void)c;fake.pwm_start_calls++;return HAL_OK;}
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *h,uint32_t c){(void)h;(void)c;return HAL_OK;}
void HAL_Delay(uint32_t ms)
{
    if(fake.delay_count<1024){HostDelay &d=fake.delays[fake.delay_count];d.ms=ms;d.ccr[0]=TIM1->CCR1;d.ccr[1]=TIM1->CCR2;d.ccr[2]=TIM1->CCR3;d.ccr[3]=TIM1->CCR4;}
    fake.delay_count++;fake.delay_total+=ms;
    if(fake.auto_uart && fake.uart_buffer)feed_uart(fake.boot_frame);
}
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *h,uint8_t *d,uint16_t s)
{fake.uart_handle=h;fake.uart_buffer=d;fake.uart_calls++;fake.size=s;return HAL_OK;}
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *h,uint32_t *d,uint32_t s)
{(void)h;fake.adc_calls++;fake.adc_buffer=d;fake.adc_size=s;return HAL_OK;}
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *h){(void)h;fake.i2c_init_calls++;return HAL_OK;}
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *h,uint16_t a,uint16_t r,uint16_t rs,uint8_t *d,uint16_t s,uint32_t timeout)
{
    (void)h;(void)timeout;fake.read_calls++;fake.addr=a;fake.reg=r;fake.regsize=rs;fake.size=s;fake.buffer=d;
    if(fake.blocking_fail_remaining){fake.blocking_fail_remaining--;return fake.blocking_status;}
    if(r==0x3b && s==14){
        const uint8_t *source=fake.calibration_frame;
        if(fake.calibration_sequence_count){source=fake.calibration_sequence[fake.calibration_sequence_index%fake.calibration_sequence_count];fake.calibration_sequence_index++;}
        std::memcpy(d,source,14);fake.frame_reads++;
    }
    else {for(unsigned i=0;i<s;i++)d[i]=fake.regs[(r+i)&255];}
    return HAL_OK;
}
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *h,uint16_t a,uint16_t r,uint16_t rs,uint8_t *d,uint16_t s,uint32_t timeout)
{
    (void)h;(void)a;(void)rs;(void)timeout;fake.write_calls++;
    for(unsigned i=0;i<s;i++){fake.regs[(r+i)&255]=d[i];fake.reg_writes[(r+i)&255]++;}
    // Chi mo phong thiet bi reset; khong tinh conversion/filter trong fake.
    if(r==0x6b && (d[0]&0x80))fake.regs[0x6b]=0x40;
    if(r==0x38 && (d[0]&1) && fake.auto_mpu)HAL_GPIO_EXTI_Callback(GPIO_PIN_1);
    return HAL_OK;
}
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *h,uint16_t a,uint16_t r,uint16_t rs,uint8_t *d,uint16_t s)
{
    fake.async_calls++;fake.addr=a;fake.reg=r;fake.regsize=rs;fake.size=s;fake.buffer=d;
    if(fake.async_status==HAL_OK && fake.auto_mpu){std::memcpy(d,fake.sample_frame,s);HAL_I2C_MemRxCpltCallback(h);}
    return fake.async_status;
}
void HAL_GPIO_Init(GPIO_TypeDef *g,GPIO_InitTypeDef *c){(void)g;(void)c;fake.gpio_calls++;}
void HAL_GPIO_WritePin(GPIO_TypeDef *g,uint16_t p,uint32_t v){(void)g;(void)p;(void)v;fake.gpio_calls++;}
extern "C" {
void MX_GPIO_Init(void){} void MX_DMA_Init(void){} void MX_I2C1_Init(void){}
void MX_ADC1_Init(void){} void MX_TIM1_Init(void){} void MX_TIM2_Init(void){}
void MX_TIM3_Init(void){} void MX_TIM10_Init(void){} void MX_TIM11_Init(void){}
void MX_USART2_UART_Init(void){}
}

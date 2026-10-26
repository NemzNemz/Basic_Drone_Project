#include "test_support.h"
#include <cstring>
TEST_GROUP(MPU) {
    MPU_MEASUREMENT m; EULER_MEASUREMENT e;
    void setup(){fake_reset();app_reset();MPU_INIT(&hi2c1);MPU_SET_GYRO_BIAS(0,0,0);m={};e={};}
};
TEST(MPU, MPU001_InitConfiguration)
{OBS_EQ("UT-MPU-001","PWR",1,fake.regs[0x6b]);OBS_EQ("UT-MPU-001","divider",1,fake.regs[0x19]);OBS_EQ("UT-MPU-001","DLPF",2,fake.regs[0x1a]);OBS_EQ("UT-MPU-001","FS_GYRO",8,fake.regs[0x1b]);OBS_EQ("UT-MPU-001","FS_ACCEL",24,fake.regs[0x1c]);OBS_EQ("UT-MPU-001","DRDY_not_enabled",0,fake.regs[0x38]);}
TEST(MPU, MPU002_StartDrdyPreservesOtherBits)
{fake.regs[0x37]=0x80;fake.regs[0x38]=0x10;MPU_DATA_READY_START();OBS_EQ("UT-MPU-002","INT_PIN_CFG",0xb0,fake.regs[0x37]);OBS_EQ("UT-MPU-002","INT_ENABLE",0x11,fake.regs[0x38]);}
TEST(MPU, MPU003_ScaleAndBigEndian)
{
    put_raw(m.raw_buffer,0,2048);put_raw(m.raw_buffer,2,-1024);put_raw(m.raw_buffer,4,-2048);
    put_raw(m.raw_buffer,8,6550);put_raw(m.raw_buffer,10,-6550);put_raw(m.raw_buffer,12,131);
    MPU_RAW_MEASUREMENT(&m);
    OBS_NEAR("UT-MPU-003","ax_g",1,m.accel.x,1e-6);OBS_NEAR("UT-MPU-003","ay_g",-.5,m.accel.y,1e-6);OBS_NEAR("UT-MPU-003","az_g",-1,m.accel.z,1e-6);
    OBS_NEAR("UT-MPU-003","gx_rad",1.74532925199433,m.gyro.x,1e-6);OBS_NEAR("UT-MPU-003","gy_rad",-1.74532925199433,m.gyro.y,1e-6);OBS_NEAR("UT-MPU-003","gz_rad",.034906585039887,m.gyro.z,1e-7);
}
TEST(MPU, MPU004_SignedExtremesAndTemperatureIgnored)
{
    put_raw(m.raw_buffer,0,-32768);put_raw(m.raw_buffer,2,32767);put_raw(m.raw_buffer,6,-32768);
    put_raw(m.raw_buffer,8,-32768);put_raw(m.raw_buffer,10,32767);MPU_RAW_MEASUREMENT(&m);
    OBS_NEAR("UT-MPU-004","min_accel",-16,m.accel.x,1e-6);OBS_NEAR("UT-MPU-004","max_accel",15.99951171875,m.accel.y,1e-6);
    OBS_NEAR("UT-MPU-004","min_gyro",-32768.0/65.5*3.141592653589793/180,m.gyro.x,2e-6);
    OBS_NEAR("UT-MPU-004","max_gyro",32767.0/65.5*3.141592653589793/180,m.gyro.y,2e-6);
    const float old=m.gyro.x;put_raw(m.raw_buffer,6,32767);MPU_RAW_MEASUREMENT(&m);OBS_NEAR("UT-MPU-004","temp_ignored",old,m.gyro.x,0);
}
TEST(MPU, MPU005_BiasAppliedInRadians)
{MPU_SET_GYRO_BIAS(.1f,-.2f,.3f);MPU_RAW_MEASUREMENT(&m);OBS_NEAR("UT-MPU-005","gx",-.1,m.gyro.x,1e-7);OBS_NEAR("UT-MPU-005","gy",.2,m.gyro.y,1e-7);OBS_NEAR("UT-MPU-005","gz",-.3,m.gyro.z,1e-7);}
TEST(MPU, MPU006_CalibrationMeanAndZeroSampleRule)
{
    put_raw(fake.calibration_frame,8,131);put_raw(fake.calibration_frame,10,-262);put_raw(fake.calibration_frame,12,393);
    const unsigned reads=fake.frame_reads,delays=fake.delay_count;MPU_CALIB_GYRO(4,&m);MPU_RAW_MEASUREMENT(&m);
    // Calibration de lai raw cu; su dung zero-frame de quan sat bias rieng.
    std::memset(m.raw_buffer,0,14);MPU_RAW_MEASUREMENT(&m);
    OBS_EQ("UT-MPU-006","reads",4,fake.frame_reads-reads);OBS_EQ("UT-MPU-006","delays",4,fake.delay_count-delays);
    OBS_NEAR("UT-MPU-006","bias_x",-2*.017453,m.gyro.x,1e-7);OBS_NEAR("UT-MPU-006","bias_y",4*.017453,m.gyro.y,1e-7);OBS_NEAR("UT-MPU-006","bias_z",-6*.017453,m.gyro.z,1e-7);
    const unsigned before=fake.frame_reads;MPU_CALIB_GYRO(0,&m);OBS_EQ("UT-MPU-006","zero_becomes_one",1,fake.frame_reads-before);
}
TEST(MPU, MPU007_LevelAndGyroSigns)
{m.accel.z=1;m.gyro={1,1,1};CONVERT_TO_ORIENT(&m,&e);OBS_NEAR("UT-MPU-007","roll",-.98*.002*57.295,e.roll,1e-6);OBS_NEAR("UT-MPU-007","pitch",.98*.002*57.295,e.pitch,1e-6);OBS_NEAR("UT-MPU-007","yaw",.002*57.295,e.yaw,1e-6);}
TEST(MPU, MPU008_AccelTiltThirtyDegrees)
{m.accel={-.5f,0,.8660254038f};CONVERT_TO_ORIENT(&m,&e);const double expected=.02*3.141592653589793/6*57.295;OBS_NEAR("UT-MPU-008","pitch",expected,e.pitch,2e-6);OBS_NEAR("UT-MPU-008","roll_zero",0,e.roll,1e-7);e={};m.accel={0,-.5f,.8660254038f};CONVERT_TO_ORIENT(&m,&e);OBS_NEAR("UT-MPU-008","roll",expected,e.roll,2e-6);}
TEST(MPU, MPU009_PreviousStateAndFixedStep)
{
    m.accel.z=1;e={10,-20,30};CONVERT_TO_ORIENT(&m,&e);
    OBS_NEAR("UT-MPU-009","roll_state",.98*10*.017453*57.295,e.roll,2e-5);OBS_NEAR("UT-MPU-009","pitch_state",-.98*20*.017453*57.295,e.pitch,2e-5);OBS_NEAR("UT-MPU-009","yaw_state",30,e.yaw,0);
    e={};m.gyro.z=1;for(unsigned n=0;n<1000;n++)CONVERT_TO_ORIENT(&m,&e);OBS_NEAR("UT-MPU-009","yaw_1000",114.59,e.yaw,.002);
}
TEST(MPU, MPU010_AsyncArgumentsAndAllStatuses)
{
    const HAL_StatusTypeDef statuses[]={HAL_OK,HAL_BUSY,HAL_ERROR,HAL_TIMEOUT};
    for(unsigned n=0;n<4;n++){fake.async_status=statuses[n];OBS_EQ("UT-MPU-010","status_passthrough",statuses[n],MPU_TRIGGER_READ_IT(&hi2c1,&m));}
    OBS_EQ("UT-MPU-010","addr",0xd0,fake.addr);OBS_EQ("UT-MPU-010","start_reg",0x3b,fake.reg);OBS_EQ("UT-MPU-010","size",14,fake.size);POINTERS_EQUAL(m.raw_buffer,fake.buffer);
}
TEST(MPU, MPU011_ExtiAndCompletionFilters)
{
    const unsigned before=fake.async_calls;HAL_GPIO_EXTI_Callback(0x10);OBS_EQ("UT-MPU-011","other_pin",before,fake.async_calls);
    HAL_GPIO_EXTI_Callback(GPIO_PIN_1);OBS_EQ("UT-MPU-011","correct_pin",before+1,fake.async_calls);POINTERS_EQUAL(mpu_mea.raw_buffer,fake.buffer);
    I2C_HandleTypeDef other={0,0,0};HAL_I2C_MemRxCpltCallback(&other);OBS_EQ("UT-MPU-011","other_i2c",0,mpu_data_ready_flag);HAL_I2C_MemRxCpltCallback(&hi2c1);OBS_EQ("UT-MPU-011","completion",1,mpu_data_ready_flag);
    fake.async_status=HAL_BUSY;HAL_GPIO_EXTI_Callback(GPIO_PIN_1);OBS_EQ("UT-MPU-011","busy_no_reset",0,fake.reset_calls);OBS_EQ("UT-MPU-011","busy_no_gpio",0,fake.gpio_calls);
}
TEST(MPU, MPU012_BlockingRegisterRetryBerrBranch)
{fake.blocking_fail_remaining=1;fake.blocking_status=HAL_ERROR;hi2c1.ErrorCode=HAL_I2C_ERROR_BERR;MPU_INIT(&hi2c1);OBS_EQ("UT-MPU-012","reinit",1,fake.i2c_init_calls);OBS_EQ("UT-MPU-012","BERR_clear_write",0xfeff,I2C1->SR1);OBS_EQ("UT-MPU-012","no_gpio_recovery",0,fake.gpio_calls);}
TEST(MPU, MPU013_CalibrationMixedSamples)
{
    const int16_t x[4]={131,262,-131,0},y[4]={0,131,262,393},z[4]={-131,-131,131,131};
    for(unsigned n=0;n<4;n++){put_raw(fake.calibration_sequence[n],8,x[n]);put_raw(fake.calibration_sequence[n],10,y[n]);put_raw(fake.calibration_sequence[n],12,z[n]);}
    fake.calibration_sequence_count=4;const unsigned elapsed=fake.delay_total;MPU_CALIB_GYRO(4,&m);
    std::memset(m.raw_buffer,0,14);MPU_RAW_MEASUREMENT(&m);
    OBS_NEAR("UT-MPU-013","mean_bias_x",-.017453,m.gyro.x,1e-7);OBS_NEAR("UT-MPU-013","mean_bias_y",-.052359,m.gyro.y,1e-7);OBS_NEAR("UT-MPU-013","mean_bias_z",0,m.gyro.z,1e-7);OBS_EQ("UT-MPU-013","requested_delay_ms",12,fake.delay_total-elapsed);
}

#include "test_support.h"
TEST_GROUP(Application) {
    void setup(){fake_reset();app_reset();motor_speed={18000,19000,20000,21000};fs_i6.SwA=2000;fs_i6.L_UD=1500;}
};
TEST(Application, APP001_LockedDefault)
{Motor_Safety(&motor_speed);OBS_EQ("UT-APP-001","lock",1,motor_lock_flag);expect_ccr("UT-APP-001",12500,12500,12500,12500);}
TEST(Application, APP002_FreshArmAndConsume)
{fs_i6.L_UD=1010;ibus_fresh_data=1;eul_mea.yaw=12;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-002","armed",0,motor_lock_flag);OBS_EQ("UT-APP-002","fresh_consumed",0,ibus_fresh_data);OBS_NEAR("UT-APP-002","yaw_ref",12,Yaw_HEADING_REF,0);expect_ccr("UT-APP-002",13000,13000,13000,13000);}
TEST(Application, APP003_RejectHighThrottleAndStaleArm)
{fs_i6.L_UD=1011;ibus_fresh_data=1;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-003","reject_high",1,motor_lock_flag);OBS_EQ("UT-APP-003","fresh_consumed",0,ibus_fresh_data);fs_i6.L_UD=1000;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-003","reject_stale",1,motor_lock_flag);}
TEST(Application, APP004_DisarmHasPriority)
{motor_lock_flag=0;fs_i6.SwA=1000;failsafe_flag=1;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-004","disarmed",1,motor_lock_flag);expect_ccr("UT-APP-004",12500,12500,12500,12500);}
TEST(Application, APP005_ThrottleOutputBoundary)
{motor_lock_flag=0;fs_i6.L_UD=1050;Motor_Safety(&motor_speed);expect_ccr("UT-APP-005",13000,13000,13000,13000);fs_i6.L_UD=1051;Motor_Safety(&motor_speed);expect_ccr("UT-APP-005",18000,19000,20000,21000);}
TEST(Application, APP006_IbusLossMinimumAndRecovery)
{motor_lock_flag=0;ibus_loss_connect_flag=1;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-006","no_lock",0,motor_lock_flag);expect_ccr("UT-APP-006",13000,13000,13000,13000);ibus_loss_connect_flag=0;Motor_Safety(&motor_speed);expect_ccr("UT-APP-006",18000,19000,20000,21000);}
TEST(Application, APP007_FailsafeMinimum)
{motor_lock_flag=0;failsafe_flag=1;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-007","lock_unchanged",0,motor_lock_flag);expect_ccr("UT-APP-007",13000,13000,13000,13000);}
TEST(Application, APP008_MpuLossOverridesIbus)
{motor_lock_flag=0;mpu_loss_flag=1;failsafe_flag=1;ibus_loss_connect_flag=1;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-008","mpu_lock",1,motor_lock_flag);expect_ccr("UT-APP-008",12500,12500,12500,12500);}
TEST(Application, APP009_LevelTriggeredRearmAfterMpuRecovery)
{motor_lock_flag=0;mpu_loss_flag=1;Motor_Safety(&motor_speed);mpu_loss_flag=0;fs_i6.L_UD=1000;ibus_fresh_data=1;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-009","rearm_without_switch_edge",0,motor_lock_flag);OBS_EQ("UT-APP-009","consumed",0,ibus_fresh_data);}
TEST(Application, APP010_TimerInstanceAndPidFlag)
{HAL_TIM_PeriodElapsedCallback(&htim3);OBS_EQ("UT-APP-010","other_timer",0,pid_flag);HAL_TIM_PeriodElapsedCallback(&htim10);OBS_EQ("UT-APP-010","tim10_event",1,pid_flag);HAL_TIM_PeriodElapsedCallback(&htim10);OBS_EQ("UT-APP-010","binary_event",1,pid_flag);}
TEST(Application, APP011_WatchdogExactThresholds)
{
    ibus_loss_connect_cnt=248;mpu_loss_cnt=48;HAL_TIM_PeriodElapsedCallback(&htim11);
    OBS_EQ("UT-APP-011","ibus_before",0,ibus_loss_connect_flag);OBS_EQ("UT-APP-011","mpu_before",0,mpu_loss_flag);
    HAL_TIM_PeriodElapsedCallback(&htim11);OBS_EQ("UT-APP-011","ibus_after",1,ibus_loss_connect_flag);OBS_EQ("UT-APP-011","mpu_after",1,mpu_loss_flag);OBS_EQ("UT-APP-011","ibus_reset",0,ibus_loss_connect_cnt);OBS_EQ("UT-APP-011","mpu_reset",0,mpu_loss_cnt);
}
TEST(Application, APP012_WatchdogOnlySetsFlagThenMainActs)
{motor_lock_flag=0;Motor_Set_Speed(&motor_speed);mpu_loss_cnt=49;HAL_TIM_PeriodElapsedCallback(&htim11);expect_ccr("UT-APP-012",18000,19000,20000,21000);Motor_Safety(&motor_speed);expect_ccr("UT-APP-012",12500,12500,12500,12500);}
TEST(Application, APP013_EscCalibrationRegisterSequence)
{ESC_Calib();OBS_EQ("UT-APP-013","lock_unchanged",1,motor_lock_flag);OBS_EQ("UT-APP-013","delay_count",2,fake.delay_count);OBS_EQ("UT-APP-013","high_ms",7000,fake.delays[0].ms);OBS_EQ("UT-APP-013","low_ms",8000,fake.delays[1].ms);for(unsigned n=0;n<4;n++){OBS_EQ("UT-APP-013","high_CCR",25000,fake.delays[0].ccr[n]);OBS_EQ("UT-APP-013","low_CCR",12500,fake.delays[1].ccr[n]);}expect_ccr("UT-APP-013",12500,12500,12500,12500);}
TEST(Application, APP014_ValidIbusToArmAndFailsafe)
{uint8_t f[32];make_frame(f,1500,1500,1000,1500,2000,1000,0);feed_parser(f);CHECK_EQUAL(1,is_iBUS_Received());ibus_fresh_data=1;Motor_Safety(&motor_speed);OBS_EQ("UT-APP-014","decoded_arm",0,motor_lock_flag);make_frame(f,1500,1500,1500,1500,2000,1000,1);feed_parser(f);CHECK_EQUAL(1,is_iBUS_Received());failsafe_flag=is_failsafe(&fs_i6);Motor_Safety(&motor_speed);expect_ccr("UT-APP-014",13000,13000,13000,13000);}

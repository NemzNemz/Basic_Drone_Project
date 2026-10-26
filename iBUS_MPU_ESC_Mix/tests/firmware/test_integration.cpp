#include "test_support.h"
#include <cstring>
TEST_GROUP(Integration) {
    void setup(){fake_reset();app_reset();}
    void prepare_main(unsigned loops=3){
        fake.drive_main=1;fake.auto_uart=1;fake.auto_mpu=1;fake.max_iterations=loops;
        make_frame(fake.boot_frame,1500,1500,1000,1500,1000,1000,0);
        make_frame(fake.iteration_frame[0],1500,1500,1000,1500,2000,1000,0);
        for(unsigned n=1;n<4;n++)make_frame(fake.iteration_frame[n],1500,1500,1500,1500,2000,1000,0);
    }
};
TEST(Integration, INT001_RawFilterCascade)
{
    MPU_INIT(&hi2c1);put_raw(mpu_mea.raw_buffer,4,2048);put_raw(mpu_mea.raw_buffer,10,655);
    MPU_RAW_MEASUREMENT(&mpu_mea);CONVERT_TO_ORIENT(&mpu_mea,&eul_mea);
    PID_t o={},i={};o.KP=2;i.KP=1;pid_pitch_roll(1500,mpu_mea.gyro.y,eul_mea.pitch,&o,&i);
    const double rate=10.0*3.141592653589793/180*57.295;
    const double angle=.98*.002*rate;
    OBS_NEAR("IT-001","pitch_angle",angle,eul_mea.pitch,1e-6);OBS_NEAR("IT-001","outer",-2*angle,o.PID_OUT,2e-6);OBS_NEAR("IT-001","inner",-2*angle-rate,i.PID_OUT,2e-5);
}
TEST(Integration, INT002_ActualMainNeutralMixer)
{prepare_main();OBS_EQ("IT-002","bounded_main",1,host_run_main());OBS_EQ("IT-002","arm",0,motor_lock_flag);expect_ccr("IT-002",18750,18750,18750,18750);OBS_EQ("IT-002","last_mixer",18750,motor_speed.m1);OBS_EQ("IT-002","irq_restored",0,fake.primask);}
TEST(Integration, INT003_ActualMainPitchMixerAndM1M4Swap)
{prepare_main();for(unsigned n=1;n<4;n++)make_frame(fake.iteration_frame[n],1500,1400,1500,1500,2000,1000,0);CHECK_EQUAL(1,host_run_main());OBS_NEAR("IT-003","Pitch_PID",10,Pitch_PID,1e-5);expect_ccr("IT-003",18760,18740,18740,18760);}
TEST(Integration, INT004_ActualMainRollMixerMapping)
{prepare_main();for(unsigned n=1;n<4;n++)make_frame(fake.iteration_frame[n],1600,1500,1500,1500,2000,1000,0);CHECK_EQUAL(1,host_run_main());OBS_NEAR("IT-004","Roll_PID",10,Roll_PID,1e-5);expect_ccr("IT-004",18740,18760,18740,18760);}
TEST(Integration, INT005_ActualMainYawRateMixer)
{prepare_main();for(unsigned n=1;n<4;n++)make_frame(fake.iteration_frame[n],1500,1500,1500,1600,2000,1000,0);CHECK_EQUAL(1,host_run_main());OBS_NEAR("IT-005","Yaw_PID",36,Yaw_FINAL_PID,1e-4);expect_ccr("IT-005",18786,18786,18714,18714);}
TEST(Integration, INT006_ActualMainCombinedMixer)
{prepare_main();for(unsigned n=1;n<4;n++)make_frame(fake.iteration_frame[n],1600,1400,1500,1600,2000,1000,0);CHECK_EQUAL(1,host_run_main());expect_ccr("IT-006",18786,18786,18694,18734);}
TEST(Integration, INT007_MailboxReplacementAfterSnapshot)
{
    prepare_main(2);make_frame(fake.iteration_frame[1],1600,1500,1500,1500,2000,1000,0);
    make_frame(fake.restore_frame,1000,1000,1800,1800,2000,1000,0);fake.restore_iteration=2;
    CHECK_EQUAL(1,host_run_main());OBS_EQ("IT-007","decoded_snapshot_throttle",1500,fs_i6.L_UD);OBS_EQ("IT-007","decoded_snapshot_roll",1600,fs_i6.R_RL);OBS_EQ("IT-007","next_pending",1,ibus_cplt_flag);MEMCMP_EQUAL(fake.restore_frame,ibus_mailbox,32);MEMCMP_EQUAL(fake.iteration_frame[1],ibus_snapshot,32);OBS_EQ("IT-007","restore_calls",2,fake.restore_calls);
}
TEST(Integration, INT008_ActualMainLowBatteryWarningOnly)
{prepare_main(2);raw_adc_val=900;CHECK_EQUAL(1,host_run_main());OBS_NEAR("IT-008","voltage",12.267,BAT_vol,2e-6);OBS_EQ("IT-008","still_armed",0,motor_lock_flag);OBS_EQ("IT-008","buzzer",50,TIM3->CCR1);expect_ccr("IT-008",13000,13000,13000,13000);}
TEST(Integration, INT009_PreflightHelpersNoCalibration)
{fake.auto_uart=1;make_frame(fake.boot_frame,1500,1500,1000,1500,1000,1000,0);fs_i6ab_init(&huart2);feed_uart(fake.boot_frame);Pre_Flight_Check();OBS_EQ("IT-009","throttle",1000,fs_i6.L_UD);OBS_EQ("IT-009","disarm_switch",1000,fs_i6.SwA);OBS_EQ("IT-009","motor_lock",1,motor_lock_flag);for(unsigned n=0;n<fake.delay_count;n++)CHECK(fake.delays[n].ms!=7000);}
TEST(Integration, INT010_PreflightIntentionalSwbCalibration)
{
    fake.auto_uart=1;make_frame(fake.boot_frame,1500,1500,1000,1500,1000,1000,0);
    uint8_t initial[32];make_frame(initial,1500,1500,1000,1500,1000,2000,0);fs_i6ab_init(&huart2);feed_uart(initial);Pre_Flight_Check();
    unsigned high=0,low=0;
    for(unsigned n=0;n<fake.delay_count;n++){if(fake.delays[n].ms==7000){high++;OBS_EQ("IT-010","calibration_high",25000,fake.delays[n].ccr[0]);}if(fake.delays[n].ms==8000){low++;OBS_EQ("IT-010","calibration_low",12500,fake.delays[n].ccr[0]);}}
    OBS_EQ("IT-010","high_once",1,high);OBS_EQ("IT-010","low_once",1,low);OBS_EQ("IT-010","switch_released",1000,fs_i6.SwB);OBS_EQ("IT-010","lock_unchanged",1,motor_lock_flag);
}
TEST(Integration, INT011_InvalidFrameDoesNotResetBookkeeping)
{prepare_main(2);fake.iteration_frame[1][8]^=1;fake.iteration_hook=[](uint32_t n){if(n==2)ibus_loss_connect_cnt=12;};CHECK_EQUAL(1,host_run_main());OBS_EQ("IT-011","old_throttle",1000,fs_i6.L_UD);OBS_EQ("IT-011","counter_not_reset",12,ibus_loss_connect_cnt);OBS_EQ("IT-011","no_fresh",0,ibus_fresh_data);}
TEST(Integration, INT012_MpuChangedZResetsWatchdog)
{prepare_main(1);mpu_loss_cnt=49;mpu_loss_flag=1;CHECK_EQUAL(1,host_run_main());OBS_EQ("IT-012","raw_z",2048,curren_raw_z);OBS_EQ("IT-012","counter",0,mpu_loss_cnt);OBS_EQ("IT-012","flag",0,mpu_loss_flag);OBS_EQ("IT-012","consumed",0,mpu_data_ready_flag);}
TEST(Integration, INT013_MpuSameZDoesNotResetWatchdog)
{prepare_main(1);put_raw(fake.sample_frame,4,0);mpu_loss_cnt=49;mpu_loss_flag=1;CHECK_EQUAL(1,host_run_main());OBS_EQ("IT-013","counter",49,mpu_loss_cnt);OBS_EQ("IT-013","flag",1,mpu_loss_flag);OBS_EQ("IT-013","lock",1,motor_lock_flag);expect_ccr("IT-013",12500,12500,12500,12500);}
TEST(Integration, INT014_NoMpuCompletionThenTimeout)
{prepare_main(1);fake.auto_mpu=0;mpu_loss_cnt=49;fake.iteration_hook=[](uint32_t){HAL_TIM_PeriodElapsedCallback(&htim11);};CHECK_EQUAL(1,host_run_main());OBS_EQ("IT-014","mpu_loss",1,mpu_loss_flag);OBS_EQ("IT-014","no_completion",0,mpu_data_ready_flag);expect_ccr("IT-014",12500,12500,12500,12500);}
TEST(Integration, INT015_NoIbusFrameThenTimeout)
{prepare_main(2);fake.skip_frame_mask=2;fake.iteration_hook=[](uint32_t n){if(n==2){ibus_loss_connect_cnt=249;HAL_TIM_PeriodElapsedCallback(&htim11);}};CHECK_EQUAL(1,host_run_main());OBS_EQ("IT-015","rc_loss",1,ibus_loss_connect_flag);OBS_EQ("IT-015","no_motor_lock",0,motor_lock_flag);expect_ccr("IT-015",13000,13000,13000,13000);}
TEST(Integration, INT016_ActualMainMixerSaturation)
{prepare_main();for(unsigned n=1;n<4;n++)make_frame(fake.iteration_frame[n],2000,1000,2000,2000,2000,1000,0);CHECK_EQUAL(1,host_run_main());expect_ccr("IT-016",25000,25000,24720,24920);}

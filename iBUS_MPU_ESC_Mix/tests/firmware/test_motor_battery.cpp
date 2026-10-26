#include "test_support.h"
TEST_GROUP(Motor) { MOTOR m; void setup(){fake_reset();app_reset();m={};} };
TEST(Motor, MOTOR001_LowerAndExactBounds)
{Motor_Update_Values(&m,0,12999,13000,25000);OBS_EQ("UT-MOTOR-001","m1",13000,m.m1);OBS_EQ("UT-MOTOR-001","m2",13000,m.m2);OBS_EQ("UT-MOTOR-001","m3",13000,m.m3);OBS_EQ("UT-MOTOR-001","m4",25000,m.m4);}
TEST(Motor, MOTOR002_UpperAndIndependentChannels)
{Motor_Update_Values(&m,25001,65535,17000,19000);OBS_EQ("UT-MOTOR-002","m1",25000,m.m1);OBS_EQ("UT-MOTOR-002","m2",25000,m.m2);OBS_EQ("UT-MOTOR-002","m3",17000,m.m3);OBS_EQ("UT-MOTOR-002","m4",19000,m.m4);}
TEST(Motor, MOTOR003_FieldToCcr)
{m={14000,15000,16000,17000};Motor_Set_Speed(&m);expect_ccr("UT-MOTOR-003",14000,15000,16000,17000);}
TEST(Motor, MOTOR004_LockAndMinimum)
{Motor_Lock(&htim1);expect_ccr("UT-MOTOR-004",12500,12500,12500,12500);Motor_Min_Throttle(&htim1);expect_ccr("UT-MOTOR-004",13000,13000,13000,13000);}
TEST(Motor, MOTOR005_ClampThenRegisterIntegration)
{Motor_Update_Values(&m,1,16000,25001,25000);Motor_Set_Speed(&m);expect_ccr("UT-MOTOR-005",13000,16000,25000,25000);}
TEST_GROUP(Battery) {void setup(){fake_reset();app_reset();}};
TEST(Battery, BAT001_ConversionRange)
{float v=-1;BAT_GET_VOL(0,&v);OBS_NEAR("UT-BAT-001","zero",0,v,0);BAT_GET_VOL(1000,&v);OBS_NEAR("UT-BAT-001","1000",13.63,v,2e-6);BAT_GET_VOL(4095,&v);OBS_NEAR("UT-BAT-001","4095",55.81485,v,1e-5);}
TEST(Battery, BAT002_HysteresisExactBoundaries)
{OBS_EQ("UT-BAT-002","normal_at_13",0,is_bat_low(13));OBS_EQ("UT-BAT-002","below_13",1,is_bat_low(12.999f));OBS_EQ("UT-BAT-002","hold_at_13",1,is_bat_low(13));OBS_EQ("UT-BAT-002","hold_at_13_5",1,is_bat_low(13.5f));OBS_EQ("UT-BAT-002","recover_above",0,is_bat_low(13.5001f));OBS_EQ("UT-BAT-002","hold_normal",0,is_bat_low(13.2f));}
TEST(Battery, BAT003_DmaBoundaryInterface)
{volatile uint32_t raw=0;BAT_INIT(&hadc1,&raw);OBS_EQ("UT-BAT-003","length",1,fake.adc_size);POINTERS_EQUAL((const void *)&raw,fake.adc_buffer);OBS_EQ("UT-BAT-003","calls",1,fake.adc_calls);}
TEST(Battery, BAT004_ConversionToHysteresisSequence)
{float v;BAT_GET_VOL(950,&v);OBS_EQ("UT-BAT-004","low_12_9485",1,is_bat_low(v));BAT_GET_VOL(970,&v);OBS_EQ("UT-BAT-004","hold_13_2211",1,is_bat_low(v));BAT_GET_VOL(1000,&v);OBS_EQ("UT-BAT-004","recover_13_63",0,is_bat_low(v));}

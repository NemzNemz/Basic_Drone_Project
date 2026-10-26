#include "test_support.h"
TEST_GROUP(PID_Math) { PID_t o,i,y; void setup(){o={};i={};y={};} };
TEST(PID_Math, PID002_Neutral)
{o.KP=i.KP=1;pid_pitch_roll(1500,0,0,&o,&i);OBS_NEAR("UT-PID-002","outer",0,o.PID_OUT,1e-6);OBS_NEAR("UT-PID-002","inner",0,i.PID_OUT,1e-6);}
TEST(PID_Math, PID003_RcEndpoints)
{
    o.KP=2;i.KP=1;pid_pitch_roll(1000,0,0,&o,&i);
    OBS_NEAR("UT-PID-003","low_inner",-50,i.PID_OUT,1e-5);OBS_NEAR("UT-PID-003","low_sum",-.05,o.error_sum,1e-6);
    o={};i={};o.KP=2;i.KP=1;pid_pitch_roll(2000,0,0,&o,&i);
    OBS_NEAR("UT-PID-003","high_inner",50,i.PID_OUT,1e-5);OBS_NEAR("UT-PID-003","high_sum",.1,i.error_sum,1e-6);
}
TEST(PID_Math, PID004_IntegralTwoCalls)
{
    o.KI=2;i.KI=3;pid_pitch_roll(1600,0,2,&o,&i);
    OBS_NEAR("UT-PID-004","outer_first",.012,o.PID_OUT,1e-7);OBS_NEAR("UT-PID-004","inner_first",.000072,i.PID_OUT,1e-8);
    pid_pitch_roll(1600,0,2,&o,&i);
    OBS_NEAR("UT-PID-004","outer_second",.024,o.PID_OUT,1e-7);OBS_NEAR("UT-PID-004","inner_second",.000216,i.PID_OUT,1e-8);
}
TEST(PID_Math, PID005_PositiveIntegralClamp)
{o.KI=i.KI=1;o.error_sum=i.error_sum=499.999f;pid_pitch_roll(2000,0,0,&o,&i);OBS_NEAR("UT-PID-005","outer_sum",500,o.error_sum,1e-6);OBS_NEAR("UT-PID-005","inner_sum",500,i.error_sum,1e-6);}
TEST(PID_Math, PID006_NegativeIntegralClamp)
{o.KI=i.KI=1;o.error_sum=i.error_sum=-499.999f;pid_pitch_roll(1000,0,0,&o,&i);OBS_NEAR("UT-PID-006","outer_sum",-500,o.error_sum,1e-6);OBS_NEAR("UT-PID-006","inner_sum",-500,i.error_sum,1e-6);}
TEST(PID_Math, PID007_DerivativeStepAndDecay)
{
    o.KD=2;i.KP=1;i.KD=.1f;const float gyro=10.0f/57.295f;
    pid_pitch_roll(1500,gyro,0,&o,&i);
    OBS_NEAR("UT-PID-007","outer_IIR",7,o.IIR_derivative,1e-5);OBS_NEAR("UT-PID-007","inner_first",-274,i.PID_OUT,2e-4);
    pid_pitch_roll(1500,gyro,0,&o,&i);
    OBS_NEAR("UT-PID-007","outer_second",-18.2,o.PID_OUT,1e-4);OBS_NEAR("UT-PID-007","inner_second",-153.2,i.PID_OUT,2e-4);
}
TEST(PID_Math, PID008_ResetOnlyIntegral)
{o.error_sum=9;o.prev_val=3;o.IIR_derivative=4;o.PID_OUT=5;o.KP=2;reset_error(&o);OBS_NEAR("UT-PID-008","sum",0,o.error_sum,0);OBS_NEAR("UT-PID-008","prev",3,o.prev_val,0);OBS_NEAR("UT-PID-008","IIR",4,o.IIR_derivative,0);OBS_NEAR("UT-PID-008","out",5,o.PID_OUT,0);OBS_NEAR("UT-PID-008","gain",2,o.KP,0);}
TEST(PID_Math, PID009_YawRateEndpoints)
{y.KP=1.5f;pid_yaw_rate(1000,0,&y);OBS_NEAR("UT-PID-009","low",-270,y.PID_OUT,1e-4);y={};y.KP=1.5f;pid_yaw_rate(2000,0,&y);OBS_NEAR("UT-PID-009","high",270,y.PID_OUT,1e-4);}
TEST(PID_Math, PID010_YawRateFeedbackAndIntegralClamp)
{
    y.KP=1;pid_yaw_rate(1500,90.0f/57.295f,&y);OBS_NEAR("UT-PID-010","feedback",-90,y.PID_OUT,1e-4);
    y={};y.KI=1;y.error_sum=499.9f;pid_yaw_rate(2000,0,&y);OBS_NEAR("UT-PID-010","positive_clamp",500,y.error_sum,1e-6);
    y.error_sum=-499.9f;pid_yaw_rate(1000,0,&y);OBS_NEAR("UT-PID-010","negative_clamp",-500,y.error_sum,1e-6);
}
TEST(PID_Math, PID011_YawRateDerivative)
{y.KP=1;y.KD=.1f;pid_yaw_rate(1500,10.0f/57.295f,&y);OBS_NEAR("UT-PID-011","step",-260,y.PID_OUT,2e-4);pid_yaw_rate(1500,10.0f/57.295f,&y);OBS_NEAR("UT-PID-011","constant",-135,y.PID_OUT,2e-4);}
TEST(PID_Math, PID012_YawHeadingWrap)
{y.KP=2;pid_yaw_angle(-179,179,0,&y);OBS_NEAR("UT-PID-012","wrap_positive",4,y.PID_OUT,1e-6);pid_yaw_angle(179,-179,0,&y);OBS_NEAR("UT-PID-012","wrap_negative",-4,y.PID_OUT,1e-6);}
TEST(PID_Math, PID013_YawHeadingBoundaryAndNoIntegral)
{
    y.KP=1;pid_yaw_angle(180,0,0,&y);OBS_NEAR("UT-PID-013","plus180",180,y.PID_OUT,1e-6);pid_yaw_angle(-180,0,0,&y);OBS_NEAR("UT-PID-013","minus180",-180,y.PID_OUT,1e-6);
    y={};y.KP=1;y.KD=2;y.KI=99;y.error_sum=7;pid_yaw_angle(10,0,10.0f/57.295f,&y);
    OBS_NEAR("UT-PID-013","heading_PD",0,y.PID_OUT,1e-5);OBS_NEAR("UT-PID-013","sum_unchanged",7,y.error_sum,0);
}
TEST(PID_Math, PID014_RepeatedPOnly)
{o.KP=2;i.KP=1;for(unsigned n=0;n<1000;n++)pid_pitch_roll(1600,0,2,&o,&i);OBS_NEAR("UT-PID-014","outer",6,o.PID_OUT,1e-5);OBS_NEAR("UT-PID-014","inner",6,i.PID_OUT,1e-5);OBS_NEAR("UT-PID-014","outer_sum",6,o.error_sum,.001);OBS_NEAR("UT-PID-014","inner_sum",12,i.error_sum,.001);}

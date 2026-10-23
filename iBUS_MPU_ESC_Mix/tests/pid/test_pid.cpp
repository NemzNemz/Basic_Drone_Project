#include "CppUTest/TestHarness.h"

extern "C" {
#include "pid.h"
}

TEST_GROUP(PID_PitchRoll)
{
    PID_t outer;
    PID_t inner;

    void setup()
    {
        outer = {};
        inner = {};
    }
};

TEST(PID_PitchRoll, POnly_Cascade)
{
    outer.KP = 2.0f;
    inner.KP = 1.0f;

    pid_pitch_roll(1600, 0.0f, 2.0f, &outer, &inner);

    DOUBLES_EQUAL(6.0, outer.PID_OUT, 0.00001);
    DOUBLES_EQUAL(6.0, inner.PID_OUT, 0.00001);

    DOUBLES_EQUAL(0.006, outer.error_sum, 0.00001);
    DOUBLES_EQUAL(0.012, inner.error_sum, 0.00001);
}
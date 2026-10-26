#ifndef FIRMWARE_TEST_SUPPORT_H
#define FIRMWARE_TEST_SUPPORT_H
#include "CppUTest/TestHarness.h"
#include "fake_hal.h"
#include <cstdio>
#include <cmath>
#define OBS_EQ(id,label,expected,actual) do { \
    const long long e_=(long long)(expected),a_=(long long)(actual); \
    std::printf("OBS|%s|%s|expected=%lld|actual=%lld|tolerance=0\n",id,label,e_,a_); \
    CHECK_EQUAL(e_,a_); \
} while(0)
#define OBS_NEAR(id,label,expected,actual,tol) do { \
    const double e_=(double)(expected),a_=(double)(actual); \
    std::printf("OBS|%s|%s|expected=%.12g|actual=%.12g|tolerance=%.12g\n",id,label,e_,a_,(double)(tol)); \
    CHECK(std::isfinite(a_)); DOUBLES_EQUAL(e_,a_,tol); \
} while(0)
inline void expect_ccr(const char *id,uint32_t a,uint32_t b,uint32_t c,uint32_t d)
{OBS_EQ(id,"CCR1",a,TIM1->CCR1);OBS_EQ(id,"CCR2",b,TIM1->CCR2);OBS_EQ(id,"CCR3",c,TIM1->CCR3);OBS_EQ(id,"CCR4",d,TIM1->CCR4);}
#endif

#ifndef PID_HOST_COMPAT_H
#define PID_HOST_COMPAT_H

#ifndef PID_HOST_TEST
#error "Header nay chi danh cho host-test PID"
#endif

#include <stdint.h>

/* Bo qua main.h de khong keo STM32 HAL vao host-build. */
#ifndef __MAIN_H
#define __MAIN_H
#endif

#endif
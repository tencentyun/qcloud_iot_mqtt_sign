/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "HAL_Platform.h"

void *HAL_Malloc(_IN_ uint32_t size)
{
    return malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    if (ptr)
        free(ptr);
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

#if defined(PLATFORM_OS_LINUX)
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

uint32_t HAL_GetTimeMs(void)
{
    struct timeval time_val = {0};
    uint32_t       time_ms;

    gettimeofday(&time_val, NULL);
    time_ms = time_val.tv_sec * 1000 + time_val.tv_usec / 1000;

    return time_ms;
}

char *HAL_Timer_current(char *time_str)
{
    if (time_str == NULL)
        return " ";

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t now_time = tv.tv_sec;

    struct tm tm_tmp = *localtime(&now_time);
    strftime(time_str, TIME_FORMAT_STR_LEN, "%F %T", &tm_tmp);

    return time_str;
}

long HAL_Timer_current_sec(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec;
}

#elif defined(PLATFORM_OS_WINDOWS)

uint32_t HAL_GetTimeMs(void)
{
    return (uint32_t)GetTickCount64();
}
char *HAL_Timer_current(char *time_str)
{
    time_t    now;
    struct tm tm_val;

    time(&now);
    localtime_s(&tm_val, &now);

    snprintf(time_str, TIME_FORMAT_STR_LEN, "%04d/%02d/%02d %02d:%02d:%02d", tm_val.tm_year + 1900, tm_val.tm_mon + 1,
             tm_val.tm_mday, tm_val.tm_hour, tm_val.tm_min, tm_val.tm_sec);

    return time_str;
}

long HAL_Timer_current_sec(void)
{
    return (long)(GetTickCount64() / 1000);
}

#else

uint32_t HAL_GetTimeMs(void)
{
    // get utc time ms
    return 0;
}

long HAL_Timer_current_sec(void)
{
    return HAL_GetTimeMs() / 1000;
}

char *HAL_Timer_current(char *time_str)
{
    long time_sec;

    time_sec = HAL_Timer_current_sec();
    memset(time_str, 0, TIME_FORMAT_STR_LEN);
    snprintf(time_str, TIME_FORMAT_STR_LEN, "%ld", time_sec);

    return time_str;
}
#endif

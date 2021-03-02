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

#ifndef HAL_PLATFORM_H_
#define HAL_PLATFORM_H_
#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_FORMAT_STR_LEN (20)

#define _IN_ /* indicate an input parameter */
#define _OU_ /* indicate a output parameter */

/**
 * @brief Malloc memory
 *
 * @param size   Expected memory size (unit: byte)
 * @return       pointer to the memory
 */
void *HAL_Malloc(_IN_ uint32_t size);

/**
 * @brief Free memory
 *
 * @param ptr   pointer to the pre-malloc memory
 */
void HAL_Free(_IN_ void *ptr);

/**
 * @brief Print data to console in format
 *
 * @param fmt   print format
 * @param ...   variable number of arguments
 */
void HAL_Printf(_IN_ const char *fmt, ...);

/**
 * @brief Print data to string in format
 *
 * @param str   destination string
 * @param len   Max size of the output
 * @param fmt   print format
 * @param ...   variable number of arguments
 * @return      number of bytes that print successfull
 */
int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...);

/**
 Print data to string in format
  *
  * @param str   destination string
  * @param len   Max size of the output
  * @param fmt   print format
  * @param ap    arguments list
  * @return      number of bytes that print successfull

 */
int HAL_Vsnprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, _IN_ va_list ap);

/**
 * @brief Get timestamp in millisecond
 *
 * @return   timestamp in millisecond
 */
uint32_t HAL_GetTimeMs(void);

/**
 * @brief Get timestamp in second
 *
 * @return   timestamp in second
 */
long HAL_Timer_current_sec(void);

/**
 * @brief Get local time in format: %y-%m-%d %H:%M:%S
 *
 * @return string of formatted time
 */
char *HAL_Timer_current(char *time_str);

#if defined(__cplusplus)
}
#endif
#endif /* HAL_PLATFORM_H_ */

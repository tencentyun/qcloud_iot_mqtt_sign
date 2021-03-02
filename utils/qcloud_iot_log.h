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
#ifndef QCLOUD_IOT_LOG_H_
#define QCLOUD_IOT_LOG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define QCLOUD_ERR_FAILURE -1
/* log print/upload related variables */
/* MAX size of log buffer for one log item including header and content */
#define MAX_LOG_MSG_LEN (511)
#ifdef PLATFORM_OS_LINUX
#undef MAX_LOG_MSG_LEN
#define MAX_LOG_MSG_LEN (1023)
#endif

/**
 * @brief Generate log for print/upload, call LogMessageHandler if defined
 *
 * When LOG_UPLOAD is enabled, the log will be uploaded to cloud server
 *
 * @param file
 * @param func
 * @param line
 * @param level
 */
void IOT_Log_Gen(const char *file, const char *func, const int line, const int level, const char *fmt, ...);

/**
 * SDK log print/upload level
 */
typedef enum { eLOG_DISABLE = 0, eLOG_ERROR = 1, eLOG_WARN = 2, eLOG_INFO = 3, eLOG_DEBUG = 4 } LOG_LEVEL;
/* Simple APIs for log generation in different level */
#define Log_d(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_DEBUG, fmt, ##__VA_ARGS__)
#define Log_i(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_INFO, fmt, ##__VA_ARGS__)
#define Log_w(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_WARN, fmt, ##__VA_ARGS__)
#define Log_e(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_ERROR, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

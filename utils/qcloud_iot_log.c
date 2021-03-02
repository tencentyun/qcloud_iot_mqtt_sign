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

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "HAL_Platform.h"
#include "qcloud_iot_log.h"

static char *level_str[] = {"DIS", "ERR", "WRN", "INF", "DBG"};

LOG_LEVEL g_log_print_level  = eLOG_INFO;
LOG_LEVEL g_log_upload_level = eLOG_DISABLE;

static const char *_get_filename(const char *p)
{
#ifdef WIN32
    char ch = '\\';
#else
    char ch = '/';
#endif
    const char *q = strrchr(p, ch);
    if (q == NULL) {
        q = p;
    } else {
        q++;
    }
    return q;
}

void IOT_Log_Gen(const char *file, const char *func, const int line, const int level, const char *fmt, ...)
{
    if (level > g_log_print_level && level > g_log_upload_level) {
        return;
    }

    char  sg_text_buf[MAX_LOG_MSG_LEN + 1];
    char *tmp_buf = sg_text_buf;
    char *o       = tmp_buf;
    memset(tmp_buf, 0, sizeof(sg_text_buf));
    char time_str[TIME_FORMAT_STR_LEN] = {0};

    /* format log content */
    const char *file_name = _get_filename(file);

    o += HAL_Snprintf(o, sizeof(sg_text_buf), "%s|%s|%s|%s(%d): ", level_str[level], HAL_Timer_current(time_str),
                      file_name, func, line);

    va_list ap;
    va_start(ap, fmt);
    HAL_Vsnprintf(o, MAX_LOG_MSG_LEN - 2 - strlen(tmp_buf), fmt, ap);
    va_end(ap);

    strcat(tmp_buf, "\r\n");

    if (level <= g_log_print_level) {
        /* default log handler: print to console */
        HAL_Printf("%s", tmp_buf);
    }

    return;
}

#ifdef __cplusplus
}
#endif

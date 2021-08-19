#include "limits.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "HAL_Platform.h"
#include "utils_base64.h"
#include "utils_hmac.h"
#include "utils_sha1.h"
#include "utils_md5.h"
#include <time.h>

typedef struct qcloud_iot_http_header_post_sign {
    const char *host;
    const char *uri;
    char *      algorithm;
    uint32_t    timestamp;
    int         nonce;
    char *      request_body_buf;
    int         request_body_buf_len;
    char *      secretkey;
    void *      privatekey;
} QcloudIotHttpHeaderPostSign;

void qcloud_iot_http_body_sha1(char *inbuf, int inbuf_len, char *outbuf)
{
    iot_sha1_context context;
    size_t           i       = 0;
    unsigned char    out[20] = {0};

    utils_sha1_init(&context);
    utils_sha1_starts(&context);
    utils_sha1_update(&context, (unsigned char *)inbuf, inbuf_len);
    utils_sha1_finish(&context, out);
    for (i = 0; i < 20; ++i) {
        outbuf[i * 2]     = utils_hb2hex(out[i] >> 4);
        outbuf[i * 2 + 1] = utils_hb2hex(out[i]);
    }
}

int qcloud_iot_http_post_sign_calc(QcloudIotHttpHeaderPostSign *post_sign, char *signout)
{
    int    ret                          = 0;
    int    sign_len                     = 0;
    char   request_buf_shax[20 * 2 + 1] = {0};
    char * sign                         = NULL;
    size_t olen;

    /*
        StringToSign =
            HTTPRequestMethod + \n +       ----> POST
            CanonicalHost + \n +           ----> HOST NAME
            CanonicalURI + \n +            ----> URI
            CanonicalQueryString + \n +    ----> POST Default ""
            Algorithm + \n +               ----> hmacsha1
            RequestTimestamp + \n +        ----> timestamp
            Nonce + \n +                   ----> nonce
            HashedCanonicalRequest         ----> request body sha1 calc result convert to lowercase hex
    */

    int sign_string_len = (strlen("POST") + 1 + strlen(post_sign->host) + 1 + strlen(post_sign->uri) + 1 + strlen("") +
                           1 + strlen(post_sign->algorithm) + 1 + 20 + 1 + 20 + 1 + 20 * 2);

    char *sign_string = malloc(sign_string_len);
    if (NULL == sign_string) {
        HAL_Printf("malloc sign string fail\r\n");
        return -1;
    }

    /*cal hmacsha1*/
    if (0 == strncmp(post_sign->algorithm, "hmacsha1", sizeof("hmacsha1") - 1)) {
        // 1. http body sha1 result to hex
        qcloud_iot_http_body_sha1(post_sign->request_body_buf, post_sign->request_body_buf_len, request_buf_shax);
        // 2. construct sign string
        HAL_Snprintf(sign_string, sign_string_len, "%s\n%s\n%s\n%s\n%s\n%d\n%d\n%s", "POST", post_sign->host,
                     post_sign->uri, "", post_sign->algorithm, post_sign->timestamp, post_sign->nonce,
                     request_buf_shax);

        HAL_Printf("calcSignString :\r\n%s\r\n\r\n\r\n", sign_string);

        sign_len = 20;
        sign     = malloc(sign_len);
        if (NULL == sign) {
            HAL_Free(sign_string);
            HAL_Printf("malloc sign fail\r\n");
            return -1;
        }

        // 3. dynreg use product secretkey to calc hmacsha1 result, result is raw not hex
        utils_hmac_sha1_hex(sign_string, strlen(sign_string), sign, post_sign->secretkey, strlen(post_sign->secretkey));
    }

    /* 4. sign result base64 encode save in signout*/
    qcloud_iot_utils_base64encode((uint8_t *)signout, sign_len * 2, &olen, (const uint8_t *)sign, sign_len);

    HAL_Free(sign_string);
    HAL_Free(sign);

    return ret;
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        HAL_Printf("please ./qcloud-dynreg-sign product_id product_secretkey device_name\r\n");
        return -1;
    }

    char *product_id        = argv[1];
    char *product_secretkey = argv[2];
    char *device_name       = argv[3];

    char request_body_buf[128];

    // 1. construct dynreg http body
    int request_body_buf_len =
        HAL_Snprintf(request_body_buf, 128, "{\"ProductId\":\"%s\",\"DeviceName\":\"%s\"}", product_id, device_name);

    // 2. set dynreg sign string need data
    QcloudIotHttpHeaderPostSign post_sign;
    memset(&post_sign, 0, sizeof(QcloudIotHttpHeaderPostSign));
    post_sign.host      = "ap-guangzhou.gateway.tencentdevices.com";
    post_sign.uri       = "/device/register";
    post_sign.secretkey = product_secretkey;  // dynreg use product_secretkey

    // 3. use algorithm
    post_sign.algorithm = "hmacsha1";
    int sign_len        = 20;  // hmacsha1 result lenght

    post_sign.request_body_buf     = request_body_buf;
    post_sign.request_body_buf_len = request_body_buf_len;

    // random
    srand((unsigned)time(NULL));
    int nonce           = rand() % 999999999 + 1000;
    int timestamp       = HAL_Timer_current_sec();  // Accurate utc timestamp; otherwise set to 0
    post_sign.timestamp = timestamp;
    post_sign.nonce     = nonce;

    char *sign = malloc(sign_len * 2);
    if (NULL == sign) {
        HAL_Printf("malloc sign fail\r\n");
        return 0;
    }

    memset(sign, 0, sign_len * 2);

    // 4. calc sign
    if (0 != qcloud_iot_http_post_sign_calc(&post_sign, sign)) {
        HAL_Free(sign);
        HAL_Printf("http post sign calc\r\n");
        return 0;
    }

#define QCLOUD_HTTP_HEADER_FORMAT \
    "Accept: %s*/*\r\n"           \
    "X-TC-Algorithm: %s\r\n"      \
    "X-TC-Timestamp: %d\r\n"      \
    "X-TC-Nonce: %d\r\n"          \
    "X-TC-Signature: %s\r\n"      \
    "Content-Type: %s\r\n"        \
    "Content-Length: %d\r\n"

    int http_header_len =
        (sizeof(QCLOUD_HTTP_HEADER_FORMAT) + strlen("application/json;") + strlen(post_sign.algorithm) + 20 + 20 +
         sign_len * 2 + strlen("application/json; charset=utf-8") + 10);

    char *http_header = HAL_Malloc(http_header_len);
    if (NULL == http_header) {
        HAL_Free(sign);
        HAL_Printf("malloc http header fail\r\n");
        return 0;
    }

    // 5. construct http header;
    HAL_Snprintf(http_header, http_header_len, QCLOUD_HTTP_HEADER_FORMAT, "application/json;", post_sign.algorithm,
                 timestamp, nonce, sign, "application/json; charset=utf-8", request_body_buf_len);

    char curl_data[512];

#define QCLOUD_DYNREG_CURL_FORMAT \
    "curl -H \"Accept: %s\""      \
    " -H \"X-TC-Algorithm: %s\""  \
    " -H \"X-TC-Timestamp: %d\""  \
    " -H \"X-TC-Nonce: %d\""      \
    " -H \"X-TC-Signature: %s\""  \
    " -X POST -d '%s' \"%s://%s%s\""

    // construct curl script
    HAL_Snprintf(curl_data, 512, QCLOUD_DYNREG_CURL_FORMAT, "application/json;", post_sign.algorithm, timestamp, nonce,
                 sign, request_body_buf, "http", post_sign.host, post_sign.uri);

    // output http packet (header + body)
    HAL_Printf("http_packet:\r\n");
    HAL_Printf("%s", http_header);
    HAL_Printf("\r\n%s", request_body_buf);
    HAL_Printf("\r\n");

    // output curl script, linux shell exec
    HAL_Printf("\r\ncurl script:\r\n");
    HAL_Printf("%s", curl_data);
    HAL_Printf("\r\n");

    HAL_Free(http_header);
    HAL_Free(sign);
    return 0;
}

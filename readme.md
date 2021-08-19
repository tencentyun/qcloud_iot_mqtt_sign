## 一 介绍
本示例提供了设备端接入腾讯云物联网平台的 MQTT 连接与动态注册参数生成的代码。

## 二 代码文件介绍
```
|---qcloud-mqtt-sign.c     mqtt 连接参数生成示例代码入口文件
|---qcloud-dynreg-sign.c   动态注册参数生成示例代码入口文件
|---cmake.settings         cmake 编译时设置目标编译工具链和系统
|---CMakeLists.txt         cmake 编译时的主脚本文件
|---cmake_build.sh         在 linux 平台上使用 cmake 方式编译的 shell 脚本。
|---make.settings          make 编译时设置目标编译工具链和系统
|---Makefile               make 编译时的主脚本文件
|---readme.md              示例介绍文档
|---platform               目标系统相关文件目录
      |---HAL_Platform.c   适配目标系统的时间获取/动态内存分配释放/字符串格式化相关函数
|---utils                  生成 mqtt 连接参数相关的文件目录
      |---qcloud_iot_log.c 提供 log 输出函数
      |---utils_base64.c   提供 base64 算法
      |---utils_hmac.c     提供 hmac 算法
      |---utils_md5.c      提供 md5 算法
      |---utils_sha1.c     提供 sha1 算法
```

## 三 MQTT 连接参数生成示例编译
1.在 linxu 平台上：

修改 make.settings 替换为目标编译工具：
```
PLATFORM_CC                 	= gcc
PLATFORM_AR                 	= ar
PLATFORM_OS                     = linux
```
然后执行 make

修改 cmake.settings 替换为目标编译工具与系统平台：
```
SET(PLATFORM_CC                 "gcc")
SET(PLATFORM_OS                 "linux")
```
然后执行 `bash cmake_build.sh`

2.在 windows 平台上，使用 vs2019：

修改 cmake.settings 替换为目标编译工具与系统平台：
```
SET(PLATFORM_CC                 "MVSC")
SET(PLATFORM_OS                 "windows")
```

### 1 示例运行
编译完成后，生成可执行文件在 output/bin 目录下。
```
./output/bin/qcloud-mqtt-sign product_id device_name device_secret
```
程序生成的 Client ID username password 用于连接腾讯云物联网平台。
```
$ ./output/bin/qcloud-mqtt-sign  L4WG9V9M2M TEST2 XXXXe65nKPk0Lx+ZThaA==
device_secret base64 decode:0xx 0xxx 0xxx 0xxx 0xd7 0xba 0xe6 0x72 0x8f 0x93 0x42 0xf1 0xf9 0x94 0xe1 0x68
username sign: xxxxea5710f9e585426dcf19e0c8c2d6af484f06
Client ID：L4WG9V9M2MTEST2
username : L4WG9V9M2MTEST2;21010406;5AiF2;92233720368547758
password : xxxxea5710f9e585426dcf19e0c8c2d6af484f06;hmacsha1
```

### 2 腾讯云物联网平台 MQTT 连接参数生成介绍

device_secret 为 psk 通过 base64 编码生成的，使用 `qcloud_iot_utils_base64decode` 解码出 psk。

Client ID 格式为 `productdevicename` 拼接的字符串。

username 格式为  `productdevicename;appid;randomconnid;timestamp` 拼接的字符串，其中 appid 见代码中宏 `QCLOUD_IOT_DEVICE_SDK_APPID`，randomconnid 为随机产生的字符串 ID，timestamp 为 UTC 时间戳，若系统时间戳不准确可使用 `LONG_MAX` 的值。

password 格式为 `username_sign;hmacsha1` 拼接的字符串。签名通过 `utils_hmac_sha1` 函数生成。

### 3 相关链接

[MQTT 接入](https://cloud.tencent.com/document/product/634/32546)

## 四 动态注册连接参数生成示例编译
1.在 linxu 平台上：

修改 make.settings 替换为目标编译工具：
```
PLATFORM_CC                 	= gcc
PLATFORM_AR                 	= ar
PLATFORM_OS                     = linux
```
然后执行 make

修改 cmake.settings 替换为目标编译工具与系统平台：
```
SET(PLATFORM_CC                 "gcc")
SET(PLATFORM_OS                 "linux")
```
然后执行 `bash cmake_build.sh`

2.在 windows 平台上，使用 vs2019：

修改 cmake.settings 替换为目标编译工具与系统平台：
```
SET(PLATFORM_CC                 "MVSC")
SET(PLATFORM_OS                 "windows")
```

### 1 示例运行
编译完成后，生成可执行文件在 output 目录下。
```
./output/bin/qcloud-dynreg-sign product_id product_secretkey device_name
```
程序生成的 calcSignString 是用于计算签名的字符串
程序生成的 http_packet 可用于腾讯云物联网平台进行动态注册。
程序生成的 curl script 可用于 curl 在 linux shell 进行动态注册的测试。

```
./output/bin/qcloud-dynreg-sign PRODUCT_ID1 PRODUCT_SECRETKEY DEVICENAME1
calcSignString :
POST
ap-guangzhou.gateway.tencentdevices.com
/device/register

hmacsha1
1629362589
333464802
88eef715f4c42652ef51b274f174f4f81a393f75


http_packet:
Accept: application/json;*/*
X-TC-Algorithm: hmacsha1
X-TC-Timestamp: 1629362589
X-TC-Nonce: 333464802
X-TC-Signature: 7u3+tAPlt8O4mPpBm3lambv23sw=
Content-Type: application/json; charset=utf-8
Content-Length: 54

{"ProductId":"PRODUCT_ID1","DeviceName":"DEVICENAME1"}

curl script:
curl -H "Accept: application/json;" -H "X-TC-Algorithm: hmacsha1" -H "X-TC-Timestamp: 1629362589" -H "X-TC-Nonce: 333464802" -H "X-TC-Signature: 7u3+tAPlt8O4mPpBm3lambv23sw=" -X POST -d '{"ProductId":"PRODUCT_ID1","DeviceName":"DEVICENAME1"}' "http://ap-guangzhou.gateway.tencentdevices.com/device/register"
```

### 2 腾讯云物联网平台动态注册 http 头部参数生成介绍

构造计算签名的字符串：
```
HTTPRequestMethod + \n +       ----> POST\n
CanonicalHost + \n +           ----> ap-guangzhou.gateway.tencentdevices.com\n
CanonicalURI + \n +            ----> /device/register\n
CanonicalQueryString + \n +    ----> \n
Algorithm + \n +               ----> hmacsha1\n
RequestTimestamp + \n +        ----> utc timestamp\n
Nonce + \n +                   ----> random string\n
HashedCanonicalRequest         ----> request body sha1 calc result convert to lowercase hex
	
"%s\n%s\n%s\n%s\n%s\n%d\n%d\n%s", "POST", ap-guangzhou.gateway.tencentdevices.com, /device/register, "", hmacsha1, timestamp, nonce, body_sha1_convert_lowercase_hex
注： utc timestamp 必须是标准时间戳秒级
```
签名计算：对构造的字符串使用 hmacsha1 + 产品密钥进行签名计算得到 20B原始签名数据


对 20B 的签名做 base64 编码。

使用签名计算过程中的参数构造动态注册 http 必要头参数：
```
"X-TC-Algorithm: hmacsha1\r\n"
"X-TC-Timestamp: utc timestamp\r\n"
"X-TC-Nonce: random string\r\n"
"X-TC-Signature: 签名经过 base64 编码得到的输出\r\n"
```

http body json 格式：
```
{"ProductId":"PRODUCT_ID1","DeviceName":"DEVICENAME1"}
```

### 3 相关链接

[动态注册接口说明](https://cloud.tencent.com/document/product/634/47225)

[动态注册控制台操作](https://cloud.tencent.com/document/product/634/35273)
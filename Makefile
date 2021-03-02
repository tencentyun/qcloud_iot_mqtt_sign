include make.settings

IOTSDK_SRC_FILES := utils/utils_base64.c \
					utils/utils_hmac.c \
					utils/utils_md5.c \
					utils/utils_sha1.c \
					utils/qcloud_iot_log.c \
					platform/HAL_Platform.c
                      
IOTSDK_INCLUDE_FILES := -Iutils/ \
						-Iplatform/
                          
CFLAGS += -Werror -Wall -Wno-error=sign-compare -Wno-error=format -Os -std=gnu99

ifeq (linux,$(strip $(PLATFORM_OS)))
CFLAGS += -DPLATFORM_OS_LINUX
else ifeq (windows,$(strip $(PLATFORM_OS)))
CFLAGS += -DPLATFORM_OS_WINDOWS
else
CFLAGS += -specs=nosys.specs
endif

all:
	$(TOP_Q) \
    mkdir -p output/bin
	$(PLATFORM_CC) $(CFLAGS) ${IOTSDK_INCLUDE_FILES} qcloud-mqtt-sign.c $(IOTSDK_SRC_FILES) -o output/bin/qcloud-mqtt-sign
	
clean:
	rm -rf output

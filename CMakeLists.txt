# 项目信息
project (QCloud_IoT_ConnParam_Sample C)

# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

include(cmake.settings)

# 编译工具链
set(COMPILE_TOOLS ${PLATFORM_CC})

# 设置CMAKE使用编译工具及编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
if (PLATFORM_OS STREQUAL "linux")
	set(CMAKE_C_COMPILER ${COMPILE_TOOLS})
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Werror -Wall -Wno-error=sign-compare -Wno-error=format -Os -pthread -DPLATFORM_OS_LINUX") # 编译选项
elseif (PLATFORM_OS STREQUAL "windows" AND COMPILE_TOOLS STREQUAL "MSVC")
	# 去除相应的警告
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS -DPLATFORM_OS_WINDOWS)
	if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W2" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W2")
	endif()
else ()
	#用户自定义
	set(CMAKE_C_COMPILER ${COMPILE_TOOLS}) # 编译工具
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wno-error=sign-compare -Wno-error=format -Os") # 编译选项
	message(WARNING "User defined platform or compile tools!")
endif()

message(STATUS ${CMAKE_C_FLAGS})

# 头文件目录
include_directories(${PROJECT_SOURCE_DIR}/utils
					${PROJECT_SOURCE_DIR}/platform)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/bin)

file(GLOB utils 	${PROJECT_SOURCE_DIR}/utils/*.c)
file(GLOB platform 	${PROJECT_SOURCE_DIR}/platform/*.c)
file(GLOB src_qcloud_mqtt_sign 	${src_qcloud_mqtt_sign} ${PROJECT_SOURCE_DIR}/qcloud-mqtt-sign.c)
add_executable(qcloud-mqtt-sign 	${utils} ${platform} ${src_qcloud_mqtt_sign})



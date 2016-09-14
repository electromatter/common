cmake_minimum_required(VERSION 2.7)
project(common)

set(CMAKE_C_FLAGS "-std=c99 -Wextra -Wall -pedantic -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fPIC")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(NOT MCP_BASE_PATH)
	set(MCP_BASE_PATH "${PROJECT_SOURCE_DIR}/mcp_base")
	set(BUILD_MCP_BASE 1)
endif()

if(BUILD_MCP_BASE)
	add_subdirectory("${MCP_BASE_PATH}")
endif()

include_directories("${MCP_BASE_PATH}/include" "${PROJECT_SOURCE_DIR}/include")

find_package(EV REQUIRED)

set(COMMON_SRCS
	src/fbuf_printf.c src/fbuf_temp.c src/hash.c
	src/hexdump.c src/net.c src/sha1.c src/sock.c
	src/strbufcpy.c src/uuid.c src/javahash.c
	src/timeutil.c)

if(COMMON_WITH_AES OR COMMON_WITH_AES_NI)
	set(COMMON_SRCS ${COMMON_SRCS} src/aes.c)
	if(COMMON_WITH_AES_NI)
		add_definitions(-DUSE_AES_NI)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -maes")
	else()
		find_package(OpenSSL REQUIRED)
	endif()
endif()

add_library(common ${COMMON_SRCS})

include(CTest)

#enable_testing()

if(BUILD_TESTING)
	add_subdirectory(tests/)
endif()


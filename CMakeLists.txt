cmake_minimum_required(VERSION 2.7)
project(common)

set(CMAKE_C_FLAGS "-std=c99 -Wextra -Wall -pedantic -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fPIC")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

if(NOT MCP_BASE_PATH)
	set(MCP_BASE_PATH "${PROJECT_SOURCE_DIR}/mcp_base")
	set(BUILD_MCP_BASE 1)
endif()

if(BUILD_MCP_BASE)
	add_subdirectory("${MCP_BASE_PATH}")
endif()

include_directories(include/ "${MCP_BASE_PATH}/include")
add_library(common
	src/fbuf_printf.c src/fbuf_temp.c src/hash.c src/heap.c
	src/hexdump.c src/net.c src/rb.c src/sha1.c src/sock.c
	src/strbufcpy.c src/table.c src/uuid.c)

add_dependencies(common mcp_base)

include(CTest)

#enable_testing()

if(BUILD_TESTING)
	add_subdirectory(tests/)
endif()


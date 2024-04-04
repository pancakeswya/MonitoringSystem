cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -O3")

set(SRC_DIR src)
set(INC_DIR include)

include_directories("${INC_DIR}")

find_package(CURL REQUIRED)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -O3")

if (NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(FATAL_ERROR "OS not supported")
endif()

add_library(util STATIC
        ${SRC_DIR}/util.c
        ${SRC_DIR}/util.h)

add_library(cpu SHARED
        ${SRC_DIR}/cpu.c
        ${INC_DIR}/cpu.h)

target_link_libraries(cpu util)
set_target_properties(cpu PROPERTIES OUTPUT_NAME "cpu")

add_library(memory SHARED
        ${SRC_DIR}/memory.c
        ${INC_DIR}/memory.h)

target_link_libraries(memory util)
set_target_properties(memory PROPERTIES OUTPUT_NAME "memory")

add_library(network SHARED
        ${SRC_DIR}/network.c
        ${INC_DIR}/network.h)

target_link_libraries(network CURL::libcurl)
set_target_properties(network PROPERTIES OUTPUT_NAME "network")
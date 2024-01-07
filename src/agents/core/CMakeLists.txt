cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

include_directories("../..")

add_library(AGENT_UTIL STATIC
        util.c
        util.h)

add_library(AGENT_CPU SHARED
        cpu.c
        cpu.h)

target_link_libraries(AGENT_CPU AGENT_UTIL)

add_library(AGENT_MEMORY SHARED
        memory.c
        memory.h)

target_link_libraries(AGENT_MEMORY AGENT_UTIL)

add_library(AGENT_NETWORK SHARED
        network.c
        network.h)
cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

include_directories("../..")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CORE_SRC macos)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CORE_SRC linux)
else()
    message(FATAL_ERROR "OS not supported")
endif()

add_library(AGENT_UTIL STATIC
        util.c
        util.h)

add_library(AGENT_CPU SHARED
        ${CORE_SRC}/cpu.c
        cpu.h)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(AGENT_CPU AGENT_UTIL)
endif()

add_library(AGENT_MEMORY SHARED
        ${CORE_SRC}/memory.c
        memory.h)

target_link_libraries(AGENT_MEMORY AGENT_UTIL)

add_library(AGENT_NETWORK SHARED
        ${CORE_SRC}/network.c
        network.h)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(AGENT_NETWORK AGENT_UTIL)
endif()
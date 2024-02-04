cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)

include_directories("../..")

find_package(CURL REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -O3")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CORE_SRC macos)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CORE_SRC linux)
else()
    message(FATAL_ERROR "OS not supported")
endif()

add_library(agent_util STATIC
        base/util.c
        base/util.h)

add_library(agent_cpu SHARED
        ${CORE_SRC}/cpu.c
        cpu.h)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(agent_cpu agent_util)
endif()

set_target_properties(agent_cpu PROPERTIES OUTPUT_NAME "agent_cpu")

add_library(agent_memory SHARED
        ${CORE_SRC}/memory.c
        memory.h)

target_link_libraries(agent_memory agent_util)

set_target_properties(agent_memory PROPERTIES OUTPUT_NAME "agent_memory")

add_library(agent_network SHARED
        ${CORE_SRC}/network.c
        network.h)

set_target_properties(agent_network PROPERTIES OUTPUT_NAME "agent_network")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(agent_network agent_util)
endif()

target_link_libraries(agent_network CURL::libcurl)

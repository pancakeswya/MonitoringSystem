# please use makefile for building or define variables that are in add_compile_definitions
cmake_minimum_required(VERSION 3.28)
project(MonitoringSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -O3")

set(SRC_DIR src)

include_directories("${SRC_DIR}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src/agents/core)

add_library(AgentsHandler STATIC
        ${SRC_DIR}/agents/handler.cc
        ${SRC_DIR}/agents/handler.h)

add_library(AgentsBuilder STATIC
        ${SRC_DIR}/agents/builder.cc
        ${SRC_DIR}/agents/builder.h)

add_library(ModelUtil STATIC
        ${SRC_DIR}/model/util.cc
        ${SRC_DIR}/model/util.h)

add_library(Model STATIC
        ${SRC_DIR}/model/model.cc
        ${SRC_DIR}/model/model.h)

target_link_libraries(Model ModelUtil AgentsHandler AgentsBuilder)

add_library(Controller STATIC
        ${SRC_DIR}/controller/controller.cc
        ${SRC_DIR}/controller/controller.h)

target_link_libraries(Controller Model)

add_executable(MonitoringSystem
        ${SRC_DIR}/main.cc)

target_link_libraries(MonitoringSystem Controller)

add_compile_definitions("_LOG_PATH=${CMAKE_SOURCE_DIR}/${LOG_PATH}")
add_compile_definitions("_CONFIG_PATH=${CMAKE_SOURCE_DIR}/${CONFIG_PATH}")
add_compile_definitions("_CORELIB_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_PATH}")
add_compile_definitions("_CORELIB_CPU_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_CPU_PATH}")
add_compile_definitions("_CORELIB_MEMORY_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_MEMORY_PATH}")
add_compile_definitions("_CORELIB_NETWORK_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_NETWORK_PATH}")

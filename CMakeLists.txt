# please use makefile for building or define variables that are in add_compile_definitions
cmake_minimum_required(VERSION 3.28)
project(MonitoringSystem)

set(CMAKE_CXX_STANDARD 17)

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

add_library(ConfigParser STATIC
        ${SRC_DIR}/model/config_parser.cc
        ${SRC_DIR}/model/config_parser.h)

add_library(Model STATIC
        ${SRC_DIR}/model/model.cc
        ${SRC_DIR}/model/model.h)

target_link_libraries(Model AgentsHandler AgentsBuilder ConfigParser)

add_library(Controller STATIC
        ${SRC_DIR}/controller/controller.cc
        ${SRC_DIR}/controller/controller.h)

target_link_libraries(Controller Model)

add_executable(MonitoringSystem
        ${SRC_DIR}/main.cc)

target_link_libraries(MonitoringSystem Controller)

add_compile_definitions("_LOG_PATH=${CMAKE_SOURCE_DIR}/${LOG_PATH}")
add_compile_definitions("_CORELIB_CPU_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_CPU_PATH}")
add_compile_definitions("_CORELIB_MEMORY_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_MEMORY_PATH}")
add_compile_definitions("_CORELIB_NETWORK_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_NETWORK_PATH}")

# please use makefile for building or define variables that are in add_compile_definitions
cmake_minimum_required(VERSION 3.28)
project(MonitoringSystem VERSION 1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -O3")

set(SRC_DIR src)

include_directories("${SRC_DIR}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)
find_package(CURL REQUIRED)

add_subdirectory(src/agents/core)

add_library(TelegramBot STATIC
        ${SRC_DIR}/tools/telegram_bot.cc
        ${SRC_DIR}/tools/telegram_bot.h)

target_link_libraries(TelegramBot CURL::libcurl)

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


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(Plot
        ${SRC_DIR}/view/plot.cc
        ${SRC_DIR}/view/plot.h)

target_link_libraries(Plot Qt${QT_VERSION_MAJOR}::Charts)

add_library(Worker
        ${SRC_DIR}/concurrency/worker.cc
        ${SRC_DIR}/concurrency/worker.h)

target_link_libraries(Worker)

set(PROJECT_SOURCES
        ${SRC_DIR}/main.cc
        ${SRC_DIR}/view/monitoringview.cc
        ${SRC_DIR}/view/monitoringview.h
        ${SRC_DIR}/view/monitoringview.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MonitoringSystem
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(MonitoringSystem SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(MonitoringSystem
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MonitoringSystem PRIVATE
                       Plot
                       TelegramBot
                       Worker
                       Controller
                       Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(MonitoringSystem PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MonitoringSystem)
endif()

add_compile_definitions("_LOG_PATH=${CMAKE_SOURCE_DIR}/${LOG_PATH}")
add_compile_definitions("_CONFIG_PATH=${CMAKE_SOURCE_DIR}/${CONFIG_PATH}")
add_compile_definitions("_CORELIB_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_PATH}")
add_compile_definitions("_CORELIB_CPU_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_CPU_PATH}")
add_compile_definitions("_CORELIB_MEMORY_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_MEMORY_PATH}")
add_compile_definitions("_CORELIB_NETWORK_PATH=${CMAKE_SOURCE_DIR}/${CORELIB_NETWORK_PATH}")

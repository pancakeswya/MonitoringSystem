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
        ${SRC_DIR}/telegram/telegram_bot.cc
        ${SRC_DIR}/telegram/telegram_bot.h)

target_link_libraries(TelegramBot CURL::libcurl)

add_library(Config
        ${SRC_DIR}/base/config.h
        ${SRC_DIR}/base/config.cc)

add_library(Agents
        ${SRC_DIR}/agents/agents.h
        ${SRC_DIR}/agents/agents.cc
        ${SRC_DIR}/agents/dll.h
        ${SRC_DIR}/agents/dll.cc)

add_library(Model
        ${SRC_DIR}/model/model.h
        ${SRC_DIR}/model/model.cc)

target_link_libraries(Model Agents Config)

add_library(Controller
        ${SRC_DIR}/controller/controller.h
        ${SRC_DIR}/controller/controller.cc)

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
    add_executable(MonitoringSystem ${PROJECT_SOURCES})
endif()

target_link_libraries(MonitoringSystem PRIVATE
                       Plot
                       TelegramBot
                       Worker
                       Controller
                       Qt${QT_VERSION_MAJOR}::Widgets)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MonitoringSystem)
endif()


OS     := $(shell uname)
NAME   := MonitoringSystem

BUILDER := cmake

SRC_DIR   := src
BUILD_DIR := build

AGENTSCORELIB_DIR := $(SRC_DIR)/agents/lib
AGENTSCORESRC_DIR := $(SRC_DIR)/agents/core

AGENTSCORELIB_CPU_PATH := $(AGENTSCORELIB_DIR)/libAgent_CPU
AGENTSCORELIB_MEMORY_PATH := $(AGENTSCORELIB_DIR)/libAgent_MEMORY
AGENTSCORELIB_NETWORK_PATH := $(AGENTSCORELIB_DIR)/libAgent_NETWORK

ifeq ($(OS), Linux)
AGENTSLIB_PATH := $(addsuffix .so, $(AGENTSCORELIB_CPU_PATH) $(AGENTSCORELIB_MEMORY_PATH) $(AGENTSCORELIB_NETWORK_PATH))
OPEN         := xdg-open
RUN          := ./$(BUILD_DIR)/$(APP)
else ifeq ($(OS), Darwin)
AGENTSLIB_PATH := $(addsuffix .dylib, $(AGENTSCORELIB_CPU_PATH) $(AGENTSCORELIB_MEMORY_PATH) $(AGENTSCORELIB_NETWORK_PATH))
OPEN         := open
RUN          := $(OPEN) $(BUILD_DIR)/$(APP)
endif

RM := rm -rf

all: build run

agentslib:
	mkdir -p $(AGENTSCORELIB_DIR)
	$(BUILDER) -H$(AGENTSCORESRC_DIR) -B$(AGENTSCORELIB_DIR) && make -C $(AGENTSCORELIB_DIR)

build: agentslib
	mkdir -p $(BUILD_DIR)
	$(BUILDER) -B$(BUILD_DIR) \
		-DCORELIB_CPU_PATH=$(word 1, $(AGENTSLIB_PATH)) \
		-DCORELIB_MEMORY_PATH=$(word 2, $(AGENTSLIB_PATH)) \
		-DCORELIB_NETWORK_PATH=$(word 3, $(AGENTSLIB_PATH)) && \
	make -C $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(NAME)

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(AGENTSCORELIB_DIR)

rebuild: clean build
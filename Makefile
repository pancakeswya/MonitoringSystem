OS     := $(shell uname)
NAME   := MonitoringSystem

CMAKE := cmake

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

MAKEFLAGS    += --no-print-directory

all: build run

agent_cpu agent_memory agent_network:
	mkdir -p $(AGENTSCORELIB_DIR)
	$(CMAKE) -H$(AGENTSCORESRC_DIR) -B$(AGENTSCORELIB_DIR) && $(CMAKE) --build $(AGENTSCORELIB_DIR) --target $@

agentslib:
	mkdir -p $(AGENTSCORELIB_DIR)
	$(CMAKE) -H$(AGENTSCORESRC_DIR) -B$(AGENTSCORELIB_DIR) && $(MAKE) -C $(AGENTSCORELIB_DIR)

build: agentslib
	mkdir -p $(BUILD_DIR)
	$(CMAKE) -B$(BUILD_DIR) \
		-DCORELIB_CPU_PATH=$(word 1, $(AGENTSLIB_PATH)) \
		-DCORELIB_MEMORY_PATH=$(word 2, $(AGENTSLIB_PATH)) \
		-DCORELIB_NETWORK_PATH=$(word 3, $(AGENTSLIB_PATH)) && \
	$(MAKE) -C $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(NAME)

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(AGENTSCORELIB_DIR)

rebuild: clean build
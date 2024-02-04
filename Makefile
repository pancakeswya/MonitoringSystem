OS     := $(shell uname)
NAME   := MonitoringSystem

CMAKE := cmake

SRC_DIR   := src
LOG_DIR   := log
BUILD_DIR := build

AGENTSCORELIB_DIR := lib
AGENTSCORESRC_DIR := $(SRC_DIR)/agents/core

CONFIG_PATH := config/config.agents

AGENTSCORELIB_CPU_PATH := $(AGENTSCORELIB_DIR)/libagent_cpu
AGENTSCORELIB_MEMORY_PATH := $(AGENTSCORELIB_DIR)/libagent_memory
AGENTSCORELIB_NETWORK_PATH := $(AGENTSCORELIB_DIR)/libagent_network

ifeq ($(OS), Linux)
DEPENDENCIES   := libcurl4-openssl-dev
AGENTSLIB_PATH := $(addsuffix .so, $(AGENTSCORELIB_CPU_PATH) $(AGENTSCORELIB_MEMORY_PATH) $(AGENTSCORELIB_NETWORK_PATH))
OPEN           := xdg-open
RUN            := ./$(BUILD_DIR)/$(APP)
else ifeq ($(OS), Darwin)
AGENTSLIB_PATH := $(addsuffix .dylib, $(AGENTSCORELIB_CPU_PATH) $(AGENTSCORELIB_MEMORY_PATH) $(AGENTSCORELIB_NETWORK_PATH))
OPEN         := open
RUN          := $(OPEN) $(BUILD_DIR)/$(APP)
endif

RM := rm -rf

MAKEFLAGS += --no-print-directory

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
		-DLOG_PATH=$(LOG_DIR) \
		-DCORELIB_PATH=$(AGENTSCORELIB_DIR) \
		-DCORELIB_CPU_PATH=$(word 1, $(AGENTSLIB_PATH)) \
		-DCORELIB_MEMORY_PATH=$(word 2, $(AGENTSLIB_PATH)) \
		-DCORELIB_NETWORK_PATH=$(word 3, $(AGENTSLIB_PATH)) \
		-DCONFIG_PATH=$(CONFIG_PATH) && \
	$(MAKE) -C $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(NAME)

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(LOG_DIR)
	$(RM) $(AGENTSCORELIB_DIR)

rebuild: clean build

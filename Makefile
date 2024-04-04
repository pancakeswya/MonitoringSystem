OS     := $(shell uname)
NAME   := MonitoringSystem

CMAKE := cmake

SRC_DIR   := src
BUILD_DIR := build

AGENTSCORESRC_DIR := $(SRC_DIR)/agents/core
AGENTSCORELIB_DIR := $(AGENTSCORESRC_DIR)/build

CONFIG_PATH := config/config.agents

DEPENDENCIES   := libcurl4-openssl-dev
OPEN           := xdg-open
RUN            := ./$(BUILD_DIR)/$(APP)

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
	$(CMAKE) -B$(BUILD_DIR) && $(MAKE) -C $(BUILD_DIR)

.PHONY: build

run:
	./$(BUILD_DIR)/$(NAME)

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(LOG_DIR)
	$(RM) $(AGENTSCORELIB_DIR)

rebuild: clean build

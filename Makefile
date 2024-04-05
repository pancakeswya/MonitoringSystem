OS     := $(shell uname)
NAME   := MonitoringSystem

CMAKE := cmake

CURR_DIR  := $(shell pwd)
SRC_DIR   := src
BUILD_DIR := build

TELEGRAM_TOKEN := 6639133805:AAF_DKy2Uq_Zes87Cevbg2f4YqBK11-pNDI
TELEGRAM_CHAT := 961646986

LOG_LOCAL_PATH    := log/metrics.logs
AGENTS_LOCAL_PATH := src/agents/core/build
CONFIG_LOCAL_PATH := config/agents.conf

LOG_DELAY 		  := 1000
AGENTS_LOAD_DELAY := 3000

AGENTSCORESRC_DIR := $(SRC_DIR)/agents/core
AGENTSCORELIB_DIR := $(AGENTSCORESRC_DIR)/build

CONFIG_PATH := config/config.agents

DEPENDENCIES   := libcurl4-openssl-dev libqt5charts5-dev
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

deps:
	sudo apt install $(DEPENDENCIES)

build: agentslib
	mkdir -p $(BUILD_DIR)
	$(CMAKE) -B$(BUILD_DIR) && $(MAKE) -C $(BUILD_DIR)

install: deps build

.PHONY: build

run:
	export LOG_PATH=$(CURR_DIR)/$(LOG_LOCAL_PATH); \
    export AGENTS_PATH=$(CURR_DIR)/$(AGENTS_LOCAL_PATH); \
    export CONFIG_PATH=$(CURR_DIR)/$(CONFIG_LOCAL_PATH); \
    export LOGGING_DELAY=$(LOG_DELAY); \
    export AGENTS_DELAY=$(AGENTS_LOAD_DELAY); \
    export TELE_TOKEN=$(TELEGRAM_TOKEN); \
    export TELE_CHAT=$(TELEGRAM_CHAT); \
	./$(BUILD_DIR)/$(NAME)

check-style:
	find $(SRC_DIR) -name '*.cc' -o -name '*.h' | xargs clang-format -style=google -n

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(LOG_DIR)
	$(RM) $(AGENTSCORELIB_DIR)

rebuild: clean build

SHELL := bash

ROOT_PATH := $(shell readlink -f $(dir $(firstword $(MAKEFILE_LIST))))
OUTDIR := ./out
CACHEDIR := ./.cache

TOOLS_PATH := $(ROOT_PATH)/bin

ECHO_TAG_MESSAGE=printf "\033[36m[%s]\033[0m %s\n"

UNAME_S := $(shell uname -s)

APP_NAME := heliopause

SOURCE_PATH := ./source

HEADER_FILES := $(shell find $(SOURCE_PATH) -name '*.h' -o -name '*.hpp')
C_FILES := $(shell find $(SOURCE_PATH) -name '*.c')
CPP_FILES := $(shell find $(SOURCE_PATH) -name '*.cc' -o -name '*.cpp')
CMAKE_FILES := CMakeLists.txt $(shell find $(SOURCE_PATH) -name CMakeLists.txt)

SOURCES := $(C_FILES) $(CPP_FILES) $(HEADER_FILES)

BLIT_SDK_PATH := $(abspath third_party/32blit-sdk)

COMMON_CMAKE_ARGS := -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D32BLIT_DIR=$(BLIT_SDK_PATH) -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

.PHONY: help
help:  ## show help
	@grep -hE '^[0-9a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

.PHONY: clean
clean:  ## delete out dir
	rm -rf $(OUTDIR)

.PHONY: build-stm32
build-stm32: $(OUTDIR)/stm32/$(APP_NAME).elf  ## build pico device code

.PHONY: build-host
build-host: $(OUTDIR)/host/$(APP_NAME)  ## build host code

.PHONY: build-pico
build-pico: $(OUTDIR)/pico/$(APP_NAME).uf2  ## build pico device code

build: $(OUTDIR)/host/$(APP_NAME) $(OUTDIR)/stm32/$(APP_NAME).elf $(OUTDIR)/pico/$(APP_NAME).uf2

$(OUTDIR)/stm32/Makefile: $(CMAKE_FILES)
	@$(ECHO_TAG_MESSAGE) "GEN" $@
	cmake -S . -B $(OUTDIR)/stm32 $(COMMON_CMAKE_ARGS) -DCMAKE_TOOLCHAIN_FILE=$(BLIT_SDK_PATH)/32blit.toolchain

$(OUTDIR)/host/Makefile: $(CMAKE_FILES)
	@$(ECHO_TAG_MESSAGE) "GEN" $@
	cmake -S . -B $(OUTDIR)/host $(COMMON_CMAKE_ARGS)

$(OUTDIR)/pico/Makefile: $(CMAKE_FILES)
	@$(ECHO_TAG_MESSAGE) "GEN" $@
	cmake -S . -B $(OUTDIR)/pico $(COMMON_CMAKE_ARGS) -DCMAKE_TOOLCHAIN_FILE=$(BLIT_SDK_PATH)/pico.toolchain -DPICO_BOARD=pimoroni_picosystem

.PHONY: gen-stm32
gen-stm32: $(OUTDIR)/stm32/Makefile  ## cmake stm32 Makefiles

.PHONY: gen-host
gen-host: $(OUTDIR)/host/Makefile  ## cmake host Makefiles

.PHONY: gen-pico
gen-pico: $(OUTDIR)/pico/Makefile  ## cmake pico Makefiles

.ONESHELL:
$(OUTDIR)/stm32/$(APP_NAME).elf: $(OUTDIR)/stm32/Makefile $(SOURCES)
	@$(ECHO_TAG_MESSAGE) "BUILD" $@
	$(MAKE) -j 4 -C $(OUTDIR)/stm32

.ONESHELL:
$(OUTDIR)/host/$(APP_NAME): $(OUTDIR)/host/Makefile $(SOURCES)
	@$(ECHO_TAG_MESSAGE) "BUILD" $@
	$(MAKE) -j 4 -C $(OUTDIR)/host

.ONESHELL:
$(OUTDIR)/pico/$(APP_NAME).uf2: $(OUTDIR)/pico/Makefile $(SOURCES)
	@$(ECHO_TAG_MESSAGE) "BUILD" $@
	$(MAKE) -j 4 -C $(OUTDIR)/pico


.PHONY: run-host
run-host: $(OUTDIR)/host/$(APP_NAME)  ## host binary
	$(OUTDIR)/host/$(APP_NAME)

.ONESHELL: flash-pico
flash-pico: build-pico reboot-picosystem-bootloader ## flash pico using picotool
	picotool load out/pico/$(APP_NAME).uf2
	picotool reboot

serial:  ## start pyserial monitor
	python -m serial.tools.miniterm - 115200

.PHONY: reboot-picosystem-bootloader
.ONESHELL: reboot-picosystem-bootloader
reboot-picosystem-bootloader:  ## Reboots picosystem
	@$(ECHO_TAG_MESSAGE) "RESET"
	python3 -c '
	import serial.tools.list_ports;
	import serial; import time; import sys;
	devices = serial.tools.list_ports.comports();
	sys.exit() if not devices else 1;
	device = devices[0].device;
	cur_serial = serial.Serial();
	cur_serial.port = device;
	cur_serial.baudrate = 115200;
	cur_serial.bytesize = serial.EIGHTBITS;
	cur_serial.stopbits = serial.STOPBITS_ONE;
	cur_serial.parity = serial.PARITY_NONE;
	cur_serial.open();
	cur_serial.write("32BLREBT1".encode("utf-8"));
	cur_serial.flush();
	cur_serial.close();
	time.sleep(2)'

# SERIAL_PORT := /dev/ttyACM0
# RESET_BAUD_RATE := 1200
# .ONESHELL: reboot-to-bootloader
# .PHONY: reboot-to-bootloader
# reboot-to-bootloader:  ## Reboots board by touching /dev/ttyACM0 at 1200 bps
# @$(ECHO_TAG_MESSAGE) "RESET" $(SERIAL_PORT)
# python3 -c 'import serial; import time;
# cur_serial = serial.Serial();
# cur_serial.port = "$(SERIAL_PORT)";
# cur_serial.baudrate = $(RESET_BAUD_RATE);
# cur_serial.bytesize = serial.EIGHTBITS;
# cur_serial.stopbits = serial.STOPBITS_ONE;
# cur_serial.parity = serial.PARITY_NONE;
# cur_serial.open();
# cur_serial.setDTR(True);
# time.sleep(0.022);
# cur_serial.setDTR(False);
# cur_serial.close();
# time.sleep(3)'

.ONESHELL:
format:  ## clang-format code
	clang-format -i $(C_FILES) $(H_FILES) $(CPP_FILES) $(CC_FILES)

.PHONY: mkdirs
mkdirs: $(OUTDIR)/

# rule to make a directory
%/:
	@mkdir -v -p $@

SHELL := bash

ROOT_PATH := $(shell readlink -f $(dir $(firstword $(MAKEFILE_LIST))))
OUTDIR := ./out
CACHEDIR := ./.cache

TOOLS_PATH := $(ROOT_PATH)/bin

ECHO_TAG_MESSAGE=printf "\033[36m[%s]\033[0m %s\n"

UNAME_S := $(shell uname -s)

APP_NAME := planet_editor
APP_PATH := apps/$(APP_NAME)/$(APP_NAME)

SOURCE_PATH := ./apps
LIBS_PATH := ./libs

CMAKE_FILES := CMakeLists.txt $(shell find $(SOURCE_PATH) $(LIBS_PATH) -name CMakeLists.txt) $(shell find $(SOURCE_PATH) $(LIBS_PATH) -name '*.yml')
ASSET_FILES := $(shell find $(SOURCE_PATH) -name '*.yml' -o -name '*.tmx' -o -name '*.png')
SOURCE_FILES := $(shell find $(SOURCE_PATH) $(LIBS_PATH) -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cc' -o -name '*.cpp')

BLIT_SDK_PATH := $(abspath third_party/32blit-sdk)

COMMON_CMAKE_ARGS := -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D32BLIT_DIR=$(BLIT_SDK_PATH) -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

.PHONY: help
help:  ## show help
	@grep -hE '^[0-9a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

.PHONY: clean
clean:  ## delete out dir
	rm -rf $(OUTDIR)

.PHONY: build-stm32
build-stm32: $(OUTDIR)/stm32/$(APP_PATH).elf  ## build pico device code

.PHONY: build-host
build-host: $(OUTDIR)/host/$(APP_PATH)  ## build host code

.PHONY: build-wasm
build-wasm: $(OUTDIR)/wasm/$(APP_PATH).wasm  ## build wasm code

.PHONY: build-pico
build-pico: $(OUTDIR)/pico/$(APP_PATH).uf2  ## build pico device code

build: $(OUTDIR)/host/$(APP_PATH) $(OUTDIR)/stm32/$(APP_PATH).elf $(OUTDIR)/pico/$(APP_PATH).uf2

$(OUTDIR)/stm32/Makefile: $(CMAKE_FILES) $(ASSET_FILES)
	@$(ECHO_TAG_MESSAGE) "GEN" $@
	cmake -S . -B $(OUTDIR)/stm32 $(COMMON_CMAKE_ARGS) -DCMAKE_TOOLCHAIN_FILE=$(BLIT_SDK_PATH)/32blit.toolchain

$(OUTDIR)/host/Makefile: $(CMAKE_FILES) $(ASSET_FILES)
	@$(ECHO_TAG_MESSAGE) "GEN" $@
	cmake -S . -B $(OUTDIR)/host $(COMMON_CMAKE_ARGS)

$(OUTDIR)/wasm/Makefile: $(CMAKE_FILES) $(ASSET_FILES)
	@$(ECHO_TAG_MESSAGE) "GEN" $@
	emcmake cmake -S . -B $(OUTDIR)/wasm $(COMMON_CMAKE_ARGS)

$(OUTDIR)/pico/Makefile: $(CMAKE_FILES) $(ASSET_FILES)
	@$(ECHO_TAG_MESSAGE) "GEN" $@
	cmake -S . -B $(OUTDIR)/pico $(COMMON_CMAKE_ARGS) -DCMAKE_TOOLCHAIN_FILE=$(BLIT_SDK_PATH)/pico.toolchain -DPICO_BOARD=pimoroni_picosystem

.PHONY: gen-stm32
gen-stm32: $(OUTDIR)/stm32/Makefile  ## cmake stm32 Makefiles

.PHONY: gen-host
gen-host: $(OUTDIR)/host/Makefile  ## cmake host Makefiles

.PHONY: gen-wasm
gen-wasm: $(OUTDIR)/wasm/Makefile  ## cmake wasm Makefiles

.PHONY: gen-pico
gen-pico: $(OUTDIR)/pico/Makefile  ## cmake pico Makefiles

$(OUTDIR)/stm32/$(APP_PATH).elf: $(OUTDIR)/stm32/Makefile $(SOURCE_FILES)
	@$(ECHO_TAG_MESSAGE) "BUILD" $@ ; \
	$(MAKE) -j 4 -C $(OUTDIR)/stm32

$(OUTDIR)/host/$(APP_PATH): $(OUTDIR)/host/Makefile $(SOURCE_FILES)
	@$(ECHO_TAG_MESSAGE) "BUILD" $@ ; \
	$(MAKE) -j 4 -C $(OUTDIR)/host

$(OUTDIR)/wasm/$(APP_PATH).wasm: $(OUTDIR)/wasm/Makefile $(SOURCE_FILES)
	@$(ECHO_TAG_MESSAGE) "BUILD" $@ ; \
	$(MAKE) -j 4 -C $(OUTDIR)/wasm

$(OUTDIR)/pico/$(APP_PATH).uf2: $(OUTDIR)/pico/Makefile $(SOURCE_FILES)
	@$(ECHO_TAG_MESSAGE) "BUILD" $@ ; \
	$(MAKE) -j 4 -C $(OUTDIR)/pico

.PHONY: run-host
run-host: $(OUTDIR)/host/$(APP_PATH)  ## host binary
	$(OUTDIR)/host/$(APP_PATH)

flash-pico: build-pico reboot-picosystem-bootloader ## flash pico using picotool
	picotool load out/pico/$(APP_PATH).uf2 ; \
	picotool reboot

flash-blit: build-stm32 ## flash 32blit
	$(MAKE) -C $(OUTDIR)/stm32 $(APP_NAME).flash

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

.PHONY: format
format:  ## clang-format code
	clang-format -i $(SOURCE_FILES)

.PHONY: download-%
download-%: $(CACHEDIR)/
	@$(ECHO_TAG_MESSAGE) "DOWNLOAD" "$*" ; \
	mkdir -p $(CACHEDIR)/$* ; \
	cd $(abspath $(CACHEDIR)/$*) ; \
	URL="$($*_url)" ; \
	EXPECTED_SHA256="$($*_sha256)" ; \
	F=`basename $$URL` ; \
	! test -f $$F && $(ECHO_TAG_MESSAGE) CURL $$URL && curl -L -O $$URL ; \
	! test -f $$F && exit 1 ; \
	sha256hash=`sha256sum $$F` ; \
	$(ECHO_TAG_MESSAGE) DOWNLOADED $$sha256hash ; \
	test "$$EXPECTED_SHA256  $$F" != "$$sha256hash" && echo "Invalid SHA256" && echo "   Expected: $$EXPECTED_SHA256" && exit 1 ; \
	exit 0

CD_TO_BUILD_DIR=cd $(abspath $(CACHEDIR)/$(subst build-,,$@)) ; URL="$($(subst build-,,$@)_url)" ; F=`basename $$URL` ; D=`tar tf $$F | head -n 1` ; ! test -d $$D && tar xf $$F ; cd $$D

.PHONY: git-pull-%
git-pull-%:
	@URL=$($(subst git-pull-,,$@)_git_url) ; \
	BRANCH=$($(subst git-pull-,,$@)_git_branch) ; \
	GDIR=$$(basename -s .git $$URL) ; \
	pushd third_party ; \
	$(ECHO_TAG_MESSAGE) "CLONE" $${URL} ; \
	rm -rf $$GDIR ; \
	git clone --depth=1 -b $$BRANCH $$URL ; \
	pushd $$GDIR ; \
	git submodule update --init ; \
	popd ; \
	rm -rf $$GDIR/.git ; \
	find $$GDIR -type f -iname .git -delete ; \
	popd

picosdk_git_url := https://github.com/raspberrypi/pico-sdk
picosdk_git_branch := 1.4.0

openocd_git_url := https://github.com/raspberrypi/openocd.git
openocd_git_branch := picoprobe

picotool_git_url := https://github.com/raspberrypi/picotool
picotool_git_branch := master

arm_mac_url := https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/12.2.mpacbti-bet1/binrel/arm-gnu-toolchain-12.2.mpacbti-bet1-darwin-x86_64-arm-none-eabi.tar.xz
arm_mac_sha256 := 946e5b1b93d48ac71a83bb907a0f8aaa05041ba8eb1007792cc2921475460d4e

arm_linux_url := https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/12.2.mpacbti-bet1/binrel/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi.tar.xz
arm_linux_sha256 := 51d99d11950446ac64a5664c860b9e03b3241757db8dc9f673dbd8fa4a830b18

.PHONY: build-arm_mac
build-arm_mac: download-arm_mac
	@$(ECHO_TAG_MESSAGE) "INSTALL" "arm-none-eabi-gcc" ; \
	$(CD_TO_BUILD_DIR) ; \
	mkdir -p $(abspath $(TOOLS_PATH)/arm-gcc) ; \
	mv * $(abspath $(TOOLS_PATH)/arm-gcc)/

.PHONY: build-arm_linux
build-arm_linux: download-arm_linux
	@$(ECHO_TAG_MESSAGE) "INSTALL" "arm-none-eabi-gcc" ; \
	$(CD_TO_BUILD_DIR) ; \
	mkdir -p $(abspath $(TOOLS_PATH)/arm-gcc) ; \
	mv * $(abspath $(TOOLS_PATH)/arm-gcc)/

ifeq ($(UNAME_S),Linux)
.PHONY: install-arm-gcc
install-arm-gcc: build-arm_linux ## download arm-none-eabi-gcc from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
endif

ifeq ($(UNAME_S),Darwin)
.PHONY: install-arm-gcc
install-arm-gcc: build-arm_mac
endif

.PHONY: mkdirs
mkdirs: $(OUTDIR)/

# rule to make a directory
%/:
	@mkdir -v -p $@

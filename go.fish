#!/usr/bin/env fish
set -x THIRTYTWO_BLIT_PROJECT_ROOT (realpath (dirname (status --filename)))
cd $THIRTYTWO_BLIT_PROJECT_ROOT

set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/pigweed/out/host/host_tools
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/pigweed-venv/bin
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/luci/bin
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/luci
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/python/bin
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/python
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/bazel/bin
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/bazel
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/pigweed/bin
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/pigweed
set -x -a PATH $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd

set -x _PW_ACTUAL_ENVIRONMENT_ROOT $THIRTYTWO_BLIT_PROJECT_ROOT/.environment
set -x PW_BAZEL_CIPD_INSTALL_DIR $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/bazel
set -x PW_BRANDING_BANNER_COLOR magenta
set -x PW_BRANDING_BANNER $THIRTYTWO_BLIT_PROJECT_ROOT/banner.txt
set -x PW_CIPD_INSTALL_DIR $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd
set -x PW_LUCI_CIPD_INSTALL_DIR $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/luci
set -x PW_PIGWEED_CIPD_INSTALL_DIR $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/pigweed
set -x PW_PROJECT_ROOT $THIRTYTWO_BLIT_PROJECT_ROOT
set -x PW_PYTHON_CIPD_INSTALL_DIR $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/cipd/python
set -x PW_ROOT $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/pigweed
set -x THIRTYTWO_BLIT_PROJECT_ROOT $THIRTYTWO_BLIT_PROJECT_ROOT
set -x VIRTUAL_ENV $THIRTYTWO_BLIT_PROJECT_ROOT/.environment/pigweed-venv

set -x PICO_SDK_PATH $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/pico-sdk
set -x PICO_EXTRAS_PATH $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/pico-sdk
set -x BLIT_SDK_PATH $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/32blit-sdk

# If you have ccache installed:
set -x COMMON_ARGS -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D32BLIT_DIR=$BLIT_SDK_PATH
which ccache 2>/dev/null 1>/dev/null && set -x -a COMMON_ARGS -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# ninja -C out/pico
# ninja -C out/stm32
# ninja -C out/host && ./out/host/game

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/pico \
   $COMMON_ARGS \
   -DCMAKE_TOOLCHAIN_FILE=$BLIT_SDK_PATH/pico.toolchain \
   -DPICO_BOARD=pimoroni_picosystem \
   && ninja -C out/pico

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/stm32 \
   $COMMON_ARGS \
   -DCMAKE_TOOLCHAIN_FILE=$BLIT_SDK_PATH/32blit.toolchain \
   && ninja -C out/stm32

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/host \
   $COMMON_ARGS \
   && ninja -C out/host && ./out/host/game

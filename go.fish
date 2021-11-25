#!/usr/bin/env fish
# set -x THIRTYTWO_BLIT_PROJECT_ROOT (realpath (dirname (status --filename)))
# cd $THIRTYTWO_BLIT_PROJECT_ROOT

# set -x PICO_SDK_PATH $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/pico-sdk
# set -x PICO_EXTRAS_PATH $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/pico-sdk
set -x 32BLIT_DIR $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/32blit-sdk

# If you have ccache installed:
set -x COMMON_ARGS -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D32BLIT_DIR=$32BLIT_DIR
which ccache 2>/dev/null 1>/dev/null && set -x -a COMMON_ARGS -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

mkdir -p out/pico
mkdir -p out/stm32
mkdir -p out/host

# ninja -C out/pico
# ninja -C out/stm32
# ninja -C out/host && ./out/host/game

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/pico \
   $COMMON_ARGS \
   -DCMAKE_TOOLCHAIN_FILE=$32BLIT_DIR/pico.toolchain \
   -DPICO_BOARD=pimoroni_picosystem \
   && ninja -C out/pico

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/stm32 \
   $COMMON_ARGS \
   -DCMAKE_TOOLCHAIN_FILE=$32BLIT_DIR/32blit.toolchain \
   && ninja -C out/stm32

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/host \
   $COMMON_ARGS \
   && ninja -C out/host && ./out/host/game

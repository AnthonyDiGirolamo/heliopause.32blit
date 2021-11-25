#!/bin/bash
export THIRTYTWO_BLIT_PROJECT_ROOT=$(realpath $(dirname "${BASH_SOURCE[0]}"))
cd $THIRTYTWO_BLIT_PROJECT_ROOT

export PW_ENVSETUP_QUIET=true
export PW_DOCTOR_SKIP_CIPD_CHECKS=true
source ./activate.sh


export COMMON_ARGS="-DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D32BLIT_DIR=${BLIT_SDK_PATH}"
# # If you have ccache installed:
if test $(which ccache 2>/dev/null); then
    export COMMON_ARGS="${COMMON_ARGS} -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
else
    echo No ccache found.
fi

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
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

function flash_pico () {
  PICO_DIR=/run/media/$USER/RPI-RP2
  if test -d $PICO_DIR ; then
      cp ./out/pico/heliopause.uf2 $PICO_DIR/
  fi
}

function flash_32blit () {
  # ninja -C out/stm32 flash
  DONE=true
}


cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/pico \
   $COMMON_ARGS \
   -DCMAKE_TOOLCHAIN_FILE=$BLIT_SDK_PATH/pico.toolchain \
   -DPICO_BOARD=pimoroni_picosystem \
   && ninja -C out/pico && flash_pico

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/stm32 \
   $COMMON_ARGS \
   -DCMAKE_TOOLCHAIN_FILE=$BLIT_SDK_PATH/32blit.toolchain \
   && ninja -C out/stm32 && flash_32blit

cmake -G Ninja \
   -S $THIRTYTWO_BLIT_PROJECT_ROOT \
   -B ./out/host \
   $COMMON_ARGS \
   && ninja -C out/host && ./out/host/heliopause

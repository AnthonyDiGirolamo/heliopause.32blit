#!/usr/bin/env fish
set -x PW_PROJECT_DIR (realpath (dirname (status --filename)))
cd $PW_PROJECT_DIR

set -x PICO_SDK_PATH $PW_PROJECT_DIR/third_party/pico-sdk
set -x 32BLIT_DIR $PW_PROJECT_DIR/third_party/32blit-sdk

set -x COMMON_ARGS -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D32BLIT_DIR=$32BLIT_DIR

mkdir -p build.pico
mkdir -p build.stm32
mkdir -p build.host

pushd build.pico
cmake .. $COMMON_ARGS \
  -DCMAKE_TOOLCHAIN_FILE=$32BLIT_DIR/pico.toolchain \
  -DPICO_BOARD=pimoroni_picosystem \
  && make -j4

popd

pushd build.stm32
cmake .. $COMMON_ARGS \
  -DCMAKE_TOOLCHAIN_FILE=$32BLIT_DIR/32blit.toolchain \
  && make -j4

popd

pushd build.host
cmake .. $COMMON_ARGS \
  && make -j4 && ./game

popd

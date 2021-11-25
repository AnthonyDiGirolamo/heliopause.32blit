#!/usr/bin/env fish
# set -x THIRTYTWO_BLIT_PROJECT_ROOT (realpath (dirname (status --filename)))
# cd $THIRTYTWO_BLIT_PROJECT_ROOT
# set -x PICO_SDK_PATH $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/pico-sdk

set -x 32BLIT_DIR $THIRTYTWO_BLIT_PROJECT_ROOT/third_party/32blit-sdk

# If you have ccache installed:
# -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
set -x COMMON_ARGS -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -D32BLIT_DIR=$32BLIT_DIR

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

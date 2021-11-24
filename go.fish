#!/usr/bin/env fish
set -x PIGWEED_EXPERIMENTAL_ROOT /mnt/auxdata/anthony/pigweed/experimental
set -x PW_BRANDING_BANNER /mnt/auxdata/anthony/pigweed/experimental/banner.txt
set -x PW_BRANDING_BANNER_COLOR magenta
set -x PW_PROJECT_ROOT /mnt/auxdata/anthony/pigweed/experimental
set -x PW_ROOT /mnt/auxdata/anthony/pigweed/experimental/third_party/pigweed
set -x _PW_ACTUAL_ENVIRONMENT_ROOT /mnt/auxdata/anthony/pigweed/experimental/.environment
set -x PW_CIPD_INSTALL_DIR /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd
set -x CIPD_CACHE_DIR /home/anthony/.cipd-cache-dir
set -x PW_PIGWEED_CIPD_INSTALL_DIR /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/pigweed
set -x PW_BAZEL_CIPD_INSTALL_DIR /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/bazel
set -x PW_PYTHON_CIPD_INSTALL_DIR /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/python
set -x PW_LUCI_CIPD_INSTALL_DIR /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/luci
set -x VIRTUAL_ENV /mnt/auxdata/anthony/pigweed/experimental/.environment/pigweed-venv

set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/pigweed
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/pigweed/bin
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/bazel
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/bazel/bin
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/python
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/python/bin
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/luci
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/cipd/luci/bin
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/.environment/pigweed-venv/bin
set -p PATH /mnt/auxdata/anthony/pigweed/experimental/third_party/pigweed/out/host/host_tools

set -x PICO_SDK_PATH /mnt/auxdata/anthony/pico-sdk
set -x PICO_EXTRAS_PATH /mnt/auxdata/anthony/pico-extras
set -x 32BLIT_DIR /mnt/auxdata/anthony/32blit-sdk

set -x PW_PROJECT_DIR (realpath (dirname (status --filename)))
cd $PW_PROJECT_DIR
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

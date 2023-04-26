#!/bin/sh

# build gflags
function build_gflags()
{
  local source_dir
  local build_dir

  # dir is exists, there is no need to clone again
  if [ -e "gflags" ] && [ -e "gflags/build" ]; then
    return
  fi

  git clone  https://github.com/gflags/gflags.git
  if [ $? -ne 0 ]; then
    echo "git clone gflags failed."
  fi

  cd gflags

  source_dir=$(pwd)
  build_dir=$source_dir/build

  mkdir -p "$build_dir"/ \
    && cd "$build_dir"/ \
    && cmake .. -DCMAKE_INSTALL_PREFIX="$build_dir" -DBUILD_SHARED_LIBS=1 -DBUILD_STATIC_LIBS=1 \
    && make \
    && make install \
    && cd ../../
}

# git submodule update --init --recursive
# build_gflags

SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR"/build
GFLAGS_DIR=gflags/build

# for using multi cores parallel compile
NUM_CPU_CORES=$(grep "processor" -c /proc/cpuinfo)
if [ -z "${NUM_CPU_CORES}" ] || [ "${NUM_CPU_CORES}" = "0" ] ; then
    NUM_CPU_CORES=1
fi

mkdir -p "$BUILD_DIR"/ \
  && cd "$BUILD_DIR"
  
cmake "$SOURCE_DIR" -DCMAKE_BUILD_TYPE=Release -DWITH_SNAPPY=ON -DWITH_TESTS=OFF -DCMAKE_PREFIX_PATH=$GFLAGS_DIR
make -j $NUM_CPU_CORES

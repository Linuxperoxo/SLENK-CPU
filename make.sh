#!/bin/bash

BIN_NAME="CPU"

ROOT_PROJECT="$(pwd)"

SRC_DIR="${ROOT_PROJECT}/src"
BUILD_DIR="${ROOT_PROJECT}/build"

make_bin()
{
  cd "${BUILD_DIR}/obj"
  g++ *.o -v -o "${BUILD_DIR}/bin/${BIN_NAME}" -march=native -O3 -Wall -Wextra 
  strip "${BUILD_DIR}/bin/${BIN_NAME}"
}

make_obj()
{
  cd "${SRC_DIR}"

  for i in *.cpp; do
    out="${i%.*}"
    g++ "${i}" -v -c -o "${BUILD_DIR}/obj/${out}.o" 
  done
}

mkdir -p ./build/{obj,bin}

case "${1}" in
  clean) 
    rm -rf ./build
  ;;
  
  run) 
    ./build/bin/CPU
  ;;

  *) 
    make_obj
    make_bin
  ;;
esac

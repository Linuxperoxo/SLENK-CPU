#!/bin/bash
#
# /*
#  *
#  *
#  *    /--------------------------------------------O
#  *    |                                            |
#  *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
#  *    |  AUTHOR    : Linuxperoxo                   |
#  *    |  FILE      : make.sh                       |
#  *    |  SRC MOD   : 02/11/2024                    |
#  *    |                                            |
#  *    O--------------------------------------------/
#  *
#  *
#  */

BIN_NAME="NANC"
ROOT_PROJECT="$(pwd)"
SRC_DIR="${ROOT_PROJECT}/src"
BUILD_DIR="${ROOT_PROJECT}/build"
OBJ_DIR="${BUILD_DIR}/obj"
BIN_DIR="${BUILD_DIR}/bin"

OTIMIZE_FLAGS="-march=native -O3 -pipe"
OTHER_FLAGS="-Wall -Wextra"

make_bin()
{
  cd "${BUILD_DIR}/obj"
  
  g++ *.o -v -o "${BIN_DIR}/${BIN_NAME}" ${OTIMIZE_FLAGS} ${OTHER_FLAGS}
  
  if [ "$?" != 0 ]; then
    echo -e "\e[31mERROR\e[0m : Failed to make project"
    exit 1;
  fi

  strip "${BIN_DIR}/${BIN_NAME}"
}

make_obj()
{
  cd "${SRC_DIR}"

  for i in *.cpp; do
    out="${i%.*}"
    g++ "${i}" -v -c -o "${BUILD_DIR}/obj/${out}.o" 
  done
}

mkdir -p "${OBJ_DIR}" "${BIN_DIR}"

case "${1}" in
  clean) 
    rm -rf "${BUILD_DIR}"
  ;;
  
  build) 
    make_obj
    make_bin
  ;;

  *)
    echo "/===================================O"
    echo "| build -> to compile source        |"
    echo "| clean -> to clean build directory |"
    echo "O===================================/"
  ;;
esac


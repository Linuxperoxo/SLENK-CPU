#ifndef __UTILS__
#define __UTILS__

#define ASCII_NUM_INIT 48
#define ASCII_NUM_END 58

#include "types.h"

inline __attribute__((always_inline)) __u8 isnum(char __char__)
{
  return __char__ >= ASCII_NUM_INIT && __char__ <= ASCII_NUM_END ? 1 : 0; 
}

#endif

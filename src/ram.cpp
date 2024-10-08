/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : ram.cpp                       |
 *    |  SRC MOD   : 8/10/2024                     |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <iostream>

#include "../include/ram/ram.hpp"

RAM::RAM() noexcept
  : _MEMORY(nullptr)
{
  _MEMORY = static_cast<uint8_t*>(mmap(nullptr, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0));
  
  if(_MEMORY == MAP_FAILED)
  {
    std::cout << "Error to alloc RAM\n";
    exit(EXIT_FAILURE);
  }

  std::memset(_MEMORY, 0, MEMORY_SIZE);
}

RAM::~RAM() noexcept
{
  munmap(_MEMORY, MEMORY_SIZE);
}

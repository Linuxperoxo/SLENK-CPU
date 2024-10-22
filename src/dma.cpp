/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : dma.cpp                       |
 *    |  SRC MOD   : 22/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdint>

#include "../include/dma/dma.hpp"

void DMA::DMA_C(uint16_t _first_addrs, uint8_t _type, uint8_t _data) noexcept
{
  this->_first_addrs = _first_addrs; 
  this->_type        = _type;
  this->_data        = _data;
}


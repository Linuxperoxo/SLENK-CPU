/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.cpp                       |
 *    |  SRC MOD   : 22/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdint>

#include "../include/bus/bus.hpp"

BUS::BUS(CPU* _cpu, MEMORY* _ram, DMA* _dma) noexcept
  : _CPU(_cpu),
    _MEMORY(_ram),
    _DMA(_dma)
{}

uint8_t BUS::read(uint16_t _addrs_to_read) noexcept
{
  return _MEMORY->read(_addrs_to_read);
}

void BUS::write(uint16_t _addrs_to_write, uint8_t _data_to_write) noexcept
{
  _MEMORY->write(_addrs_to_write, _data_to_write); 
}


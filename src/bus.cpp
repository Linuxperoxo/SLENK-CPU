/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.cpp                       |
 *    |  SRC MOD   : 21/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdint>

#include "../include/bus/bus.hpp"

BUS::BUS(CPU* _cpu, RAM* _ram) noexcept
  : _CPU(_cpu),
    _RAM(_ram)
{}

uint8_t BUS::read(uint16_t _addrs_to_read) noexcept
{
  return _RAM->read(_addrs_to_read);
}

void BUS::write(uint16_t _addrs_to_write, uint8_t _data_to_write) noexcept
{
  _RAM->write(_addrs_to_write, _data_to_write); 
}


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

#include "../include/bus/bus.hpp"

BUS::BUS(CPU* _cpu, RAM* _ram) noexcept
  : _CPU(_cpu),
    _RAM(_ram)
{}

/*
 *
 *  Typedef
 *
 * DATA_BITS_SIZE  -> uint8_t
 * ADDRS_BITS_SIZE -> uint16_t
 * NONE            -> void
 *
 */

uint8_t BUS::read(ADDRS_BITS_SIZE _addrs_to_read) noexcept
{
  return _RAM->read(_addrs_to_read);
}

void BUS::write(ADDRS_BITS_SIZE _addrs_to_write, DATA_BITS_SIZE _data_to_write) noexcept
{
  _RAM->write(_addrs_to_write, _data_to_write); 
}


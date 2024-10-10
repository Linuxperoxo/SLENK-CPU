/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : main.cpp                      |
 *    |  SRC MOD   : 9/10/2024                     |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdlib>
#include <iostream>

#include "../include/bus/bus.hpp"
#include "../include/cpu/cpu.hpp"

int main()
{
  CPU* _cpu
  {
    static_cast<CPU*>(std::malloc(sizeof(CPU)))
  };

  RAM* _ram 
  {
    static_cast<RAM*>(std::malloc(sizeof(RAM)))
  };

  BUS* _bus
  {
    static_cast<BUS*>(std::malloc(sizeof(BUS)))  
  };

  if(_cpu == nullptr || _ram == nullptr || _bus == nullptr)
  {
    std::cout << "Error to alloc main structs CPU, RAM or BUS\n";
    exit(EXIT_FAILURE);
  }

  new(_cpu) CPU();
  new(_ram) RAM();
  new(_bus) BUS(_cpu, _ram);

  _cpu->write(0xFD, 0b11111111);
  _cpu->read(0xFD);

  _cpu->~CPU();
  _ram->~RAM();
  _bus->~BUS();

  std::free(_cpu);
  std::free(_ram);
  std::free(_bus);

  return 0;
}


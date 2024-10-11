/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : main.cpp                      |
 *    |  SRC MOD   : 11/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdlib>
#include <iostream>

#include "../include/bus/bus.hpp"
#include "../include/cpu/cpu.hpp"

#ifndef ROM_INIT
#define ROM_INIT 0x8000
#endif

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

  new(_bus) BUS(_cpu, _ram);
  new(_ram) RAM();
  new(_cpu) CPU(_bus);

  _cpu->write(ROM_INIT + 1, 0x0002); // Instrução
  _cpu->write(ROM_INIT + 2, 'H');
  _cpu->write(ROM_INIT + 3, 'E');
  _cpu->write(ROM_INIT + 4, 'L');
  _cpu->write(ROM_INIT + 5, 'L');
  _cpu->write(ROM_INIT + 6, 'O');
  _cpu->write(ROM_INIT + 7, ',');
  _cpu->write(ROM_INIT + 8, ' ');
  _cpu->write(ROM_INIT + 9, 'W');
  _cpu->write(ROM_INIT + 10, 'O');
  _cpu->write(ROM_INIT + 11, 'R');
  _cpu->write(ROM_INIT + 12, 'L');
  _cpu->write(ROM_INIT + 13, 'D');
  _cpu->write(ROM_INIT + 14, '!');
  _cpu->write(ROM_INIT + 15, '\n');

  _cpu->run();

  _cpu->~CPU();
  _ram->~RAM();
  _bus->~BUS();

  std::free(_cpu);
  std::free(_ram);
  std::free(_bus);

  return 0;
}


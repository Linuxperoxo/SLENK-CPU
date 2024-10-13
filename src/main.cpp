/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : main.cpp                      |
 *    |  SRC MOD   : 13/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "../include/bus/bus.hpp"
#include "../include/cpu/cpu.hpp"

#ifndef ROM_INIT
#define ROM_INIT 0x8000
#endif

/*
 *
 * Milissegundo (ms): 1 segundo é igual a 1.000 milissegundos.
 * Microsegundo (µs): 1 milissegundo é igual a 1.000 microsegundos.
 * Nanosegundo (ns): 1 microsegundo é igual a 1.000 nanosegundos.
 * Picosegundo (ps): 1 nanosegundo é igual a 1.000 picosegundos.
 * Femtosegundo (fs): 1 picosegundo é igual a 1.000 femtosegundos.

 * 1 Hz  : 1 evento por segundo
 * 1 KHz : 1000 eventos por segundo
 * 1 MHz : 1000000 eventos por segundo
 * 1 GHz : 1000000000 eventos por segundo
 *
*/

int main()
{
  CPU* _cpu 
  {
    static_cast<CPU*>(std::malloc(sizeof(CPU)))
  };

  if(_cpu == nullptr)
  {
    std::cerr << "Error to alloc memory for CPU class\n";
    exit(EXIT_FAILURE);
  }
  
  RAM* _ram
  {
    static_cast<RAM*>(std::malloc(sizeof(RAM)))
  };

  if(_ram == nullptr)
  {
    std::free(_cpu);
    
    std::cout << "Error to alloc memory for RAM class\n";
    exit(EXIT_FAILURE);
  }

  BUS* _bus
  {
    static_cast<BUS*>(std::malloc(sizeof(BUS)))  
  };

  if(_bus == nullptr)
  {
    std::free(_cpu);
    std::free(_ram);

    std::cout << "Error to alloc memory for BUS class\n";
    exit(EXIT_FAILURE);
  }

  new(_bus) BUS(_cpu, _ram);
  new(_ram) RAM();
  new(_cpu) CPU(_bus);

  /*
   *
   *  Escrevendo ROM na memória
   *
   */

  _cpu->write(ROM_INIT, 0x08); // Escrevendo a instrução PTR no início da rom(0x8000) 
  
  /*
   *
   *  Os caracteres que serão impressos com a instrução PRT
   *
   */

  _cpu->write(ROM_INIT + 1, 'H');
  _cpu->write(ROM_INIT + 3, 'E');
  _cpu->write(ROM_INIT + 5, 'L');
  _cpu->write(ROM_INIT + 7, 'L');
  _cpu->write(ROM_INIT + 9, 'O');
  _cpu->write(ROM_INIT + 11, ',');
  _cpu->write(ROM_INIT + 13, ' ');
  _cpu->write(ROM_INIT + 15, 'W');
  _cpu->write(ROM_INIT + 17, 'O');
  _cpu->write(ROM_INIT + 19, 'R');
  _cpu->write(ROM_INIT + 21, 'L');
  _cpu->write(ROM_INIT + 23, 'D');
  _cpu->write(ROM_INIT + 25, '!');
  _cpu->write(ROM_INIT + 27, '\n');
  
  /*
   *
   * Escrevendo uma instrução de JMP para o endereço 0x8000
   *
   */

  _cpu->write(ROM_INIT + 28, 0x01);
  _cpu->write(ROM_INIT + 29, 0x80);
  _cpu->write(ROM_INIT + 30, 0x1f); 

  /*
   *
   *  Simulando um clock simples
   *
  */

  _cpu->_I = 0;

  uint8_t i { 0 };

  while(true)
  {
    if(_cpu->_I == 0)
    {
      if(_cpu->_B == 1)
      {
        break;
      }
    }
    _cpu->run(); // 1 ciclo
    std::this_thread::sleep_for(std::chrono::nanoseconds(CLOCK_FREQUENCY)); // 1.79 MHz    
    if(i == 3) _cpu->_B = 1;
    ++i;
  }

  _cpu->~CPU();
  _ram->~RAM();
  _bus->~BUS();

  std::free(_cpu);
  std::free(_ram);
  std::free(_bus);

  return 0;
}


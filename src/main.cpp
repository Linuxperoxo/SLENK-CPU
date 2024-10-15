/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : main.cpp                      |
 *    |  SRC MOD   : 14/10/2024                    |
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

int main(int argc, char** argv)
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
   *  Escrevendo a ROM
   *
   */

  
  _cpu->write(ROM_INIT, 0x08); // Escrevendo a instrução PTR no início da rom(0x8000) 
  
  /*
   *
   *  Os caracteres que serão impressos com a instrução PRT
   *
   */

  _cpu->write(ROM_INIT + 1,  'H');
  _cpu->write(ROM_INIT + 2,  'E');
  _cpu->write(ROM_INIT + 3,  'L');
  _cpu->write(ROM_INIT + 4,  'L');
  _cpu->write(ROM_INIT + 5,  'O');
  _cpu->write(ROM_INIT + 6,  ',');
  _cpu->write(ROM_INIT + 7,  ' ');
  _cpu->write(ROM_INIT + 8,  'W');
  _cpu->write(ROM_INIT + 9,  'O');
  _cpu->write(ROM_INIT + 10, 'R');
  _cpu->write(ROM_INIT + 11, 'L');
  _cpu->write(ROM_INIT + 12, 'D');
  _cpu->write(ROM_INIT + 13, '!');
  _cpu->write(ROM_INIT + 14, '\n');

  /*
   *
   * Escrevendo uma instrução de JMP para o endereço 0x803d
   *
   */

  _cpu->write(ROM_INIT + 15, 0x01); // Instrução JMP 
  _cpu->write(ROM_INIT + 16, 0x80);
  _cpu->write(ROM_INIT + 17, 0x3d); 

  /*
   *
   * Testando instrução MOV
   *
   */

  _cpu->write(ROM_INIT + 0x3d, 0x06); // Instrução MOV4
  _cpu->write(ROM_INIT + 0x3e, 0x00);
  _cpu->write(ROM_INIT + 0x3f, 0x80);
  _cpu->write(ROM_INIT + 0x40, 0x01);
  _cpu->write(ROM_INIT + 0x41, 0x01); // Instrução JMP para endereço 0x8000
  _cpu->write(ROM_INIT + 0x42, 0x80);
  _cpu->write(ROM_INIT + 0x43, 0x00);

  /*
   *
   *  Simulando um clock simples
   *
  */

  _cpu->_I = 0;

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
  }

  _cpu->~CPU();
  _ram->~RAM();
  _bus->~BUS();

  std::free(_cpu);
  std::free(_ram);
  std::free(_bus);

  return 0;
}


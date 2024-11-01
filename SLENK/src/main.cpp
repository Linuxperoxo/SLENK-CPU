/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : main.cpp                      |
 *    |  SRC MOD   : 29/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdlib>
#include <iostream>
#include <thread>

#include "../include/bus/bus.hpp"
#include "../include/cpu/cpu.hpp"
#include "../include/display/display.hpp"

#if !defined ROM_INIT
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
  /*
   *
   * Alocando memória para cada classe
   *
   */

  CPU* _cpu { static_cast<CPU*>(std::malloc(sizeof(CPU))) };
  if(_cpu == nullptr) { std::cerr << "Error to alloc memory for CPU class\n"; exit(EXIT_FAILURE); }
  
  MEMORY* _memory { static_cast<MEMORY*>(std::malloc(sizeof(MEMORY))) };
  if(_memory == nullptr) { std::free(_cpu); std::cout << "Error to alloc memory for RAM class\n"; exit(EXIT_FAILURE); }

  DMA* _dma { static_cast<DMA*>(std::malloc(sizeof(DMA))) };
  if(_memory == nullptr) { std::free(_cpu); std::free(_memory); std::cout << "Error to alloc memory for DMA struct\n"; exit(EXIT_FAILURE); }
  
  BUS* _bus { static_cast<BUS*>(std::malloc(sizeof(BUS))) };
  if(_bus == nullptr) { std::free(_cpu); std::free(_memory); std::cout << "Error to alloc memory for BUS class\n"; exit(EXIT_FAILURE); }
  
  DISPLAY* _display { static_cast<DISPLAY*>(std::malloc(sizeof(DISPLAY))) };
  if(_display == nullptr) { std::free(_cpu); std::free(_memory); std::free(_bus); std::cout << "Error to alloc memory for DISPLAY class\n"; exit(EXIT_FAILURE); }

  /*
   *
   * Construindo cada objeto no addrs alocado
   *
   */
  
  new(_dma) DMA();
  new(_bus) BUS(_cpu, _memory, _dma);
  new(_memory) MEMORY();
  new(_cpu) CPU(_bus);
  new(_display) DISPLAY(_bus);
  
  
  #if !defined USE_CUSTOM_FIRMWARE
  _memory->load_firmware(); // Carregando firmware
  #else
  std::cout << "Custom firmware is not supported!\n";
  exit(EXIT_FAILURE);
  #endif
  _memory->load_rom(argv[1]); // Carregando a ROM do programa

  /*
   *
   * Aqui colocamos uma thread para cada parte, já que cada um tem um clock próprio   
   * 
   */

  std::thread _cpu_theard(&CPU::clock_loop, _cpu);
  std::thread _display_theard(&DISPLAY::clock_loop, _display);

  _cpu_theard.join();
  _display_theard.join();

  /*
   *
   * Chamando o destrutor de todos
   *
   */

  _cpu->~CPU();
  _memory->~MEMORY();
  _bus->~BUS();
  _display->~DISPLAY();
  _dma->~DMA();

  /*
   *
   * Liberando memória alocada
   *
   */

  std::free(_cpu);
  std::free(_memory);
  std::free(_bus);
  std::free(_display);
  std::free(_dma);

  return 0;
}


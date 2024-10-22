/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : display.cpp                   |
 *    |  SRC MOD   : 22/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <sstream> // Para buffer de str

#include "../include/display/display.hpp"
#include "../include/bus/bus.hpp"

DISPLAY::DISPLAY(BUS* _bus) noexcept
  : _BUS(_bus)
{}

void DISPLAY::cycle() noexcept
{

  /*
   *
   * Essa função ainda está sendo feita, essa não vai ser a lógica final
   *
   */

  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 0,  0, 'H');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 1,  0, 'E');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 2,  0, 'L');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 3,  0, 'L');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 4,  0, 'O');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 5,  0, ',');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 6,  0, ' ');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 7,  0, 'W');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 8,  0, 'O');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 9,  0, 'R');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 10, 0, 'L');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 11, 0, 'D');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 12, 0, '!');
  _BUS->cpu_interrupt_DMA(DISPLAY_FRAMEBUFFER_ADDRS + 13, 0, '\n');
  
  //std::stringstream _cout_buffer;

  std::cout << "\n+----------------------DISPLAY----------------------+\n\n";
  std::cout.flush();

  //_cout_buffer << "\n+----------------------DISPLAY----------------------+\n\n"; // Borda superior
  
  while(_frammebuffer_addrs <= DISPLAY_FRAMEBUFFER_ADDRS + 13)
  {
    //_cout_buffer << _BUS->cpu_interrupt_DMA(_frammebuffer_addrs, 1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << _BUS->cpu_interrupt_DMA(_frammebuffer_addrs, 1, 0);
    std::cout.flush();
    ++_frammebuffer_addrs;
  }

  std::cout << "\n+---------------------------------------------------+\n\n";

  //_cout_buffer << "\n+--------------------------------------------------+\n\n"; // Borda superior
  
  //std::cout << _cout_buffer.str(); // Mostrando o frame
}

void DISPLAY::clock_loop() noexcept
{
  while(_BUS->CPU_running() != 1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(DISPLAY_FREQUENCY));
    cycle();
  }
}

/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : display.cpp                   |
 *    |  SRC MOD   : 21/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <chrono>
#include <iostream>
#include <thread>

#include "../include/display/display.hpp"
#include "../include/bus/bus.hpp"

DISPLAY::DISPLAY(BUS* _bus) noexcept
  : _BUS(_bus)
{}

void DISPLAY::cycle() noexcept
{
  _BUS->cpu_interrupt_DMA(0x7000, 0, 'H');

  std::cout << "+---------------------------------------------------+"; // Borda superior
  
  while(_frammebuffer_addrs <= DISPLAY_FRAMEBUFFER_ADDRS + FRAMEBUFFER_SIZE)
  {
    std::cout << _BUS->cpu_interrupt_DMA(0x7000, 1, 0) << '\n';
  }

  std::cout << "+---------------------------------------------------+"; // Borda inferior
}

void DISPLAY::clock_loop() noexcept
{
  while(true) // Esse true vai ser temporário, vou usar um sistema de interrupções para gerenciar isso
  {
    cycle();
    std::this_thread::sleep_for(std::chrono::seconds(DISPLAY_FREQUENCY));
  }
}

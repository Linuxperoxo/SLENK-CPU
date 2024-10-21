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

void DISPLAY::cycle() noexcept
{
  std::cout << "+---------------------------------------------------+"; // Borda superior
  
  while(_frammebuffer_addrs <= DISPLAY_FRAMEBUFFER_ADDRS + FRAMEBUFFER_SIZE)
  {
    /*
     *
     * Aqui vai ficar as informações do frame
     *
     */ 
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

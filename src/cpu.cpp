/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.cpp                       |
 *    |  SRC MOD   : 8/10/2024                     |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include "../include/cpu/cpu.hpp"
#include "../include/bus/bus.hpp"

/*
 *
 * Typedef
 *
 * NONE -> void
 *
 */

/*
 *
 * @info   : Essa função é responsável por linkar uma class BUS a CPU
 *
 * @return : Void
 *
 * @param  : Um ponteiro para linkar o barramento a CPU
 *
 */

void CPU::linkbus(BUS* _bus) noexcept
{
  if(_BUS == nullptr) { _BUS = _bus; }
}

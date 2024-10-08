/*
 * 
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.hpp                       |
 *    |  SRC MOD   : 7/10/2024                     | 
 *    |                                            |
 *    O--------------------------------------------/
 *    
 *
 */

#ifndef __CPU_HPP__
#define __CPU_HPP__

#include "../bus/bus.hpp"

typedef void NONE;

class CPU
{
private:

  CPU(const CPU&)            = delete;
  CPU(CPU&&)                 = delete;
  CPU& operator=(const CPU&) = delete;
  CPU& operator=(CPU&&)      = delete;

private:

  BUS* _BUS;

public:

  
  /*
    
    Construtor e destrutor
    
  */


  explicit CPU() noexcept;

  ~CPU() noexcept;

public:
  

  /*
    
    Funções membros
    
  */

  NONE linkbus(BUS* _bus) noexcept;

};

#endif

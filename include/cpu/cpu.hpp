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

class BUS;

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

  explicit CPU() noexcept;

  ~CPU() noexcept;

public:
  
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

  NONE linkbus(BUS* _bus) noexcept;

};

#endif

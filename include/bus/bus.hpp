/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.hpp                       |
 *    |  SRC MOD   : 7/10/2024                     |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#ifndef __BUS_HPP__
#define __BUS_HPP__

#include <cstdint>

typedef uint16_t ADDRS_TYPE;
typedef uint8_t  DATA_TYPE;
typedef void     NONE;

class BUS
{
private: 

  BUS(const BUS&)            = delete;
  BUS(BUS&&)                 = delete;
  BUS& operator=(const BUS&) = delete;
  BUS& operator=(BUS&&)      = delete;

private:


  /*
    
    Dispositivos conectados ao barramento
    
  */

  

public:

  
  /*
    
    Construtor e destrutor
    
  */


  explicit BUS() noexcept;

  ~BUS() noexcept;

public:

  
  /*
    
    Funções membros
    
  */


  DATA_TYPE read(ADDRS_TYPE _addrs_to_read) noexcept;
  NONE      write(ADDRS_TYPE _addrs_to_write, DATA_TYPE _data_to_write);
};

#endif

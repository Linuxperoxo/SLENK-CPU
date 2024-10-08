/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.cpp                       |
 *    |  SRC MOD   : 8/10/2024                     |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdlib>
#include <iostream>

#include "../include/bus/bus.hpp"

BUS::BUS(CPU*& _cpu) noexcept
  : _CPU(nullptr),
    _RAM(nullptr)
{
  _CPU = static_cast<CPU*>(std::malloc(sizeof(CPU)));
  _RAM = static_cast<RAM*>(std::malloc(sizeof(RAM)));

  if(_RAM == nullptr || _CPU == nullptr)
  {
    std::cout << "Error to create system bus\n" << '\n';
    exit(EXIT_FAILURE);
  }

  new(_CPU) CPU();
  new(_RAM) RAM();

  _CPU->linkbus(this);

  _cpu = _CPU;
}

BUS::~BUS() noexcept
{
  _CPU->~CPU();
  _RAM->~RAM();

  std::free(_CPU);
  std::free(_RAM);
}

/*
 *
 *  Typedef
 *
 * DATA_TYPE  -> uint8_t
 * ADDRS_TYPE -> uint16_t
 * NONE       -> void
 *
 */

/*
 *
 *  @info   : Função que vai lé um determinado bloco na memória RAM
 *
 *  @return : uint8_t -> Esse processador é de 8 bits, por isso como
 *            retorno ele retorna um uint8_t, porém, ele usa endereçamento
 *            de 16 bits
 *
 *  @param  : Um uint16_t que será usado como endereço na memória
 *
 */

DATA_TYPE BUS::read(ADDRS_TYPE _addrs_to_read) noexcept
{

}

/*
 *
 *  @info   : Função que vai escrever um dado em um determinado block na
 *            memória RAM
 *
 *  @return : Void
 *
 *  @param  : recebe um bloco que será escrito e o dato que será escrito no
 *            bloco
 *
 */

NONE BUS::write(ADDRS_TYPE _addrs_to_write, DATA_TYPE _data_to_write) noexcept
{
  
}

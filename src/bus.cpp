/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.cpp                       |
 *    |  SRC MOD   : 9/10/2024                     |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include "../include/bus/bus.hpp"

BUS::BUS(CPU* _cpu, RAM* _ram) noexcept
  : _CPU(_cpu),
    _RAM(_ram)
{}

/*
 *
 *  Typedef
 *
 * DATA_BITS_SIZE  -> uint8_t
 * ADDRS_BITS_SIZE -> uint16_t
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

DATA_BITS_SIZE BUS::read(ADDRS_BITS_SIZE _addrs_to_read) noexcept
{
  return _RAM->read(_addrs_to_read);
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

NONE BUS::write(ADDRS_BITS_SIZE _addrs_to_write, DATA_BITS_SIZE _data_to_write) noexcept
{
  _RAM->write(_addrs_to_write, _data_to_write); 
}


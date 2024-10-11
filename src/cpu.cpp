/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.cpp                       |
 *    |  SRC MOD   : 11/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#define FIRST_ADDRS_TO_READ_INSTRUCTION 0xFFFA
#define FIRST_ADDRS_STACK_PTR 0xFF

#include "../include/cpu/cpu.hpp"
#include "../include/bus/bus.hpp"

CPU::CPU() noexcept
  : _STATUS(0x0000)
{
  reset();

  // (this->*_opcode[0]._instruct_ptr)(&_opcode[0]);
}

/*
 *
 * Typedef
 *
 * ADDRS_BITS_SIZE -> uint16_t
 * DATA_BITS_SIZE  -> uint8_t
 * NONE            -> void
 *
 */

/*
 *
 * @info   : Essa função é responsável por linkar uma class BUS a CPU
 *
 * @return : void
 *
 * @param  : Um ponteiro para linkar o barramento a CPU
 *
 */

NONE CPU::linkbus(BUS* _bus) noexcept
{
  if(_BUS == nullptr) { _BUS = _bus; }
}

/*
 *
 * @info   : Essa função escreve um dado em um bloco de memória
 *
 * @return : void
 *
 * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
 *
 */

NONE CPU::write(ADDRS_BITS_SIZE _addrs_to_write, DATA_BITS_SIZE _data_to_write) noexcept
{
  _BUS->write(_addrs_to_write, _data_to_write);
}

/*
 *
 * @info   : Essa função lê um bloco de memória e escreve no registrador X
 *
 * @return : void
 *
 * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
 *
 */

NONE CPU::read(ADDRS_BITS_SIZE _data_to_read) noexcept
{
  _X = _BUS->read(_data_to_read);  
}

/*
 *
 * @info   : Essa função apenas define o primeiro bloco de memória que o registrador
 *           PC vai procurar pela primeira instrução, e configura o ponteiro da stack
 *
 * @return : void
 *
 * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
 *
 */

NONE CPU::reset() noexcept
{
  _PC     = FIRST_ADDRS_TO_READ_INSTRUCTION; 
  _STKPTR = FIRST_ADDRS_STACK_PTR; 
}


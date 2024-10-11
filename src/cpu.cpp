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

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <ios>
#include <iostream>

#include "../include/cpu/cpu.hpp"
#include "../include/bus/bus.hpp"

#define FIRST_ADDRS_TO_READ_INSTRUCTION 0xFFFE
#define FIRST_ADDRS_STACK_PTR 0xFF
#define FIRST_INSTRUCTION_OPCODE 0x00
#define SEC_INSTRUCTION_OPCODE 0x01
#define ROM_INIT 0x8000

constexpr uint16_t CLOCK_FREC {1000000000 / 1790000 };

CPU::CPU(BUS* _bus_to_link) noexcept
{
  linkbus(_bus_to_link);
  
  _PC = FIRST_ADDRS_TO_READ_INSTRUCTION;
  _F  = ROM_INIT;

  // std::cout << std::hex << static_cast<int>(_PC) << '\n';
  
  write(_PC, FIRST_INSTRUCTION_OPCODE);
  run();

  write(_PC, SEC_INSTRUCTION_OPCODE);
  run();

  // std::cout << std::hex << static_cast<int>(_PC) << '\n';
  
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

NONE CPU::run() noexcept
{
  /*
   *
   * O registrador _X vai armazenar o dado
   *
   */

  read(_PC);

  std::cout << "INSTRUCTION : \"" << _opcode[_X]._name << "\" "
            << "SIZE : \"0x" << _opcode[_X]._bytes_to_read + 1 << "\" "
            << "PC : \"0x" << std::hex << _PC << "\"" << '\n'; 

  std::cout << std::dec;

  (this->*_opcode[_X]._instruct_ptr)(&_opcode[_X]);
}

NONE CPU::RST(CPU::INSTRUCTION*) noexcept
{
  _A      = 0x00;
  _X      = 0x00;
  _Y      = 0x00;
  _F      = ROM_INIT;
  _PC     = FIRST_ADDRS_TO_READ_INSTRUCTION; 
  _STKPTR = FIRST_ADDRS_STACK_PTR;
  _STATUS = 0x00;
  
  ++_PC;
}

NONE CPU::ADD(CPU::INSTRUCTION*) noexcept
{
  std::cout << "ADD\n";
}

NONE CPU::SUB(CPU::INSTRUCTION*) noexcept
{
  std::cout << "SUB\n";
}

NONE CPU::JMP(CPU::INSTRUCTION*) noexcept
{
  _PC = _F; 
}

NONE CPU::PRT(CPU::INSTRUCTION* _instruct) noexcept
{
  _A = 1;

  std::cout << _X;

  while(_A < _instruct->_bytes_to_read)
  {  
    read(_PC + _A);    
    
    std::cout << _X;

    if(_X == '\n')
    {
      break;
    }
    ++_A;
  }
  _PC += _A + 1;
}

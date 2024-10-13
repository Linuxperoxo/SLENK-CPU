/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.cpp                       |
 *    |  SRC MOD   : 12/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "../include/cpu/cpu.hpp"
#include "../include/bus/bus.hpp"

#define FIRST_ADDRS_TO_READ_INSTRUCTION 0xFFFD
#define FIRST_INSTRUCTION_OPCODE 0x00

#define SEC_ADDRS_TO_READ_INSTRUCTION 0xFFFE
#define SEC_INSTRUCTION_OPCODE 0x01

#define FIRST_ADDRS_STACK_PTR 0xFF
#define ROM_INIT 0x8000

constexpr uint16_t CLOCK_FREC {1000000000 / 1790000 };

CPU::CPU(BUS* _bus_to_link) noexcept
{
  linkbus(_bus_to_link);

  /*
   *
   * Os primeiros 2 ciclos de inicialização
   *
   */
   
  write(FIRST_ADDRS_TO_READ_INSTRUCTION, FIRST_INSTRUCTION_OPCODE); // Gravando a primeira instrução que é um RST
  run(); // Primeiro ciclo

  write(SEC_ADDRS_TO_READ_INSTRUCTION, SEC_INSTRUCTION_OPCODE); // Gravando a segundo instrução que é JMP para o início da ROM
  
  /*
   *
   * Como o JMP lê os próximos 2 bytes para saber qual endereço ele deve pular, então escrevemos nesse local, o endereço 
   * que vamos querer ir, no caso 0x8000
   *
   */

  write(SEC_ADDRS_TO_READ_INSTRUCTION + 1, 0x80); 
  run(); // Segundo ciclo
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

NONE CPU::sts() noexcept
{
  std::cout << "INSTRUCTION : \"" << _opcode[_X]._name << "\" "
            << "PC ADDRS: \"0x" << std::hex << _PC << "\" "
            << "STACK ADDRS: \"0x" << std::hex << static_cast<int>(_STKPTR) << "\"" 
            << '\n';

  std::cout << std::dec;
}

NONE CPU::linkbus(BUS* _bus) noexcept
{
  if(_BUS == nullptr) { _BUS = _bus; }
}

NONE CPU::write(ADDRS_BITS_SIZE _addrs_to_write, DATA_BITS_SIZE _data_to_write) noexcept
{
  _BUS->write(_addrs_to_write, _data_to_write);
}

NONE CPU::read(ADDRS_BITS_SIZE _data_to_read) noexcept
{
  _X = _BUS->read(_data_to_read);  
}

NONE CPU::run() noexcept
{
  read(_PC); // Lendo a memória que o registrador PC está apontando, essa  memória será armazenada no registrador _X

  sts(); // Mostrando informações de execução

  (this->*_opcode[_X]._instruct_ptr)(&_opcode[_X]); // Executando a instrução
}

NONE CPU::RST(CPU::INSTRUCTION*) noexcept
{
  /*
   *
   * Resetando tudo para o estado inicial
   *
   */

  _A      = 0x00;
  _X      = 0x00;
  _Y      = 0x00;
  _F      = 0x00;
  _H      = 0x00;
  _STKPTR = FIRST_ADDRS_STACK_PTR;
  _PC     = FIRST_ADDRS_TO_READ_INSTRUCTION; 
  
  _STATUS = 0x00;

  ++_PC; // Isso é temporário
}

/*
 *
 * ADDRESS MODE:
 *
 */

NONE CPU::ADDRS8B() noexcept
{
  read(_PC + 1);
  _Y = _X;
}

/*
 *
 * Apenas uma pequena gambiarra para ler um endereço de memória de 16 bits
 *
 */

NONE CPU::ADDRS16B() noexcept
{
  read(_PC + 1);

  _Y = _X;

  read(_PC + 2);

  _F = _X;
}

/*
 *
 *  INSTRUCTIONS
 *
 */

NONE CPU::ADD(CPU::INSTRUCTION*) noexcept
{
  std::cout << "ADD\n";
  ++_PC;
}

NONE CPU::SUB(CPU::INSTRUCTION*) noexcept
{
  std::cout << "SUB\n";
  ++_PC;
}

NONE CPU::JMP(CPU::INSTRUCTION* _instruct) noexcept
{
  (this->*_instruct->_addrs_mode)();
  
  _PC = _PC << 16;

  _PC = (_PC | _Y) << 8;
  _PC = (_PC | _F);
}

NONE CPU::POP(CPU::INSTRUCTION*) noexcept
{
  read(_STKPTR);
  _H = _X;

  if(_STKPTR + 1 <= FIRST_ADDRS_STACK_PTR)
  {
    ++_STKPTR;
  }
  ++_PC;
}

NONE CPU::PSH(CPU::INSTRUCTION*) noexcept
{
  write(_STKPTR, _H);
  
  if(_STKPTR - 1 >= 0x00)
  {
    --_STKPTR;
  }
  ++_PC;
}


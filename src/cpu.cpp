/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.cpp                       |
 *    |  SRC MOD   : 18/10/2024                    |
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

#define BRK_INSTRUCTION_OPCODE 0x09

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

/*
 *
 * Funções Legais >:)
 *
 */

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

/*
 *
 * Função que representa 1 Ciclo
 *
 */

NONE CPU::run() noexcept
{
  read(_PC); // Lendo a memória que o registrador PC está apontando, essa  memória será armazenada no registrador _X

  if(_X > OPCODE_NUM - 1 || _X < 0) { _X = BRK_INSTRUCTION_OPCODE; } // Vendo se é uma instrução válida, caso não, vamos ter uma instrução de BRK

#if defined(CPU_LOG)
  sts(); // Mostrando informações de execução
#endif
  
  (this->*_opcode[_X]._instruct_ptr)(); // Executando a instrução
}

/*
 *
 * Funções para facilitar minha vida
 *
 */

NONE CPU::BYTE1() noexcept
{  
  read(_PC + 1);
  _Y = _X;
}

NONE CPU::BYTE2() noexcept
{
  read(_PC + 2);
  _F = _X;
}

NONE CPU::BYTE3() noexcept
{
  read(_PC + 3);
  _Q = _X;
}

/*
 *
 *  INSTRUCTION RST:
 *
 */

NONE CPU::RST() noexcept
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

  ++_PC;
}

/*
 *
 *  INSTRUCTION ADD:
 *
 */

NONE CPU::ADD() noexcept
{
  std::cout << "ADD\n";
  ++_PC;
}

/*
 *
 * INSTRUCTION SUB:
 *
 */

NONE CPU::SUB() noexcept
{
  std::cout << "SUB\n";
  ++_PC;
}

/*
 *
 * INSTRUCTION JMP:
 *
 */

NONE CPU::JMP() noexcept
{
  BYTE1();
  BYTE2();

  _PC = 0;
  
  _PC = (_PC | _Y) << 8;
  _PC = (_PC | _F);
}

/*
 *
 * INSTRUCTION POP: 
 *
 */

NONE CPU::POP() noexcept
{
  read(_STKPTR);
  _H = _X;

  if(_STKPTR + 1 <= FIRST_ADDRS_STACK_PTR)
  {
    ++_STKPTR;
  }
  ++_PC;
}

/*
 *
 * INSTRUCTION PSH:
 *
 */

NONE CPU::PSH() noexcept
{ 
  if(_STKPTR -1 >= 0x00)
  {
    --_STKPTR;
  }

  write(_STKPTR, _H);

  ++_PC;
}

/*
 *
 *  INSTRUCTION PRT:
 *
 */

NONE CPU::PRT() noexcept
{ 
  BYTE1();

  ++_PC;

  while(_Y != '\n')
  {  
    std::cout << _Y;

    BYTE1();
      
    ++_PC;
  }
  std::cout << _Y;

  ++_PC;
}

/*
 *
 * INSTRUCTION BRK:
 *
 */

NONE CPU::BRK() noexcept
{
  _B = 1;
}

/*
 *
 * INSTRUCTION INC/DEC:
 *
 */

NONE CPU::INC() noexcept
{
  BYTE1();

  *_regcode[_Y] += 1;
  
  _PC += 3;
}

NONE CPU::DEC() noexcept
{
  BYTE1();

  *_regcode[_Y] -= 1;

  _PC += 3;
}

/*
 *
 * INSTRUCTION MOV: 
 *
 */

NONE CPU::MOV() noexcept
{
  BYTE1();
  BYTE2();

  *_regcode[_Y] = _F;

  _PC += 3;
}

NONE CPU::MOV2() noexcept
{
  BYTE1();
  BYTE2();

  *_regcode[_Y] = *_regcode[_F];

  _PC += 3;
}

NONE CPU::MOV3() noexcept
{
  BYTE1();
  BYTE2();
  BYTE3();

  /*
   *
   * Decidi usar a stack tanto para Instrução quando para a MOV4, eu poderia simplificar muito isso, 
   * mas quis deixar assim para ser mais "fiel", compensa o overhead adicional
   *
   */

  _H = _PC;

  PSH();

  _H = (_PC >> 8);

  PSH();

  _PC = 0;
  _PC = (_PC | _F) << 8;
  _PC = (_PC | _Q);

  read(_PC);

  *_regcode[_Y] = _X;

  POP();
 
  _PC = 0;
  _PC = (_PC | _H) << 8;
 
  POP();

  _PC = (_PC | _H);

  _PC += 4;  
}

NONE CPU::MOV4() noexcept
{
  BYTE1();
  BYTE2();
  BYTE3();

  _H = _PC;

  PSH();

  _H = (_PC >> 8);
  
  PSH();

  _PC = 0;
  _PC = (_PC | _F) << 8;
  _PC = (_PC | _Q);

  write(_PC, *_regcode[_Y]);

  POP();

  _PC = 0;
  _PC = (_PC | _H) << 8;

  POP();

  _PC = (_PC | _H);

  _PC += 4;
}


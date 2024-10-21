/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.cpp                       |
 *    |  SRC MOD   : 20/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cctype>
#include <chrono>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <thread>

#include "../include/cpu/cpu.hpp"
#include "../include/bus/bus.hpp"

#define FIRST_ADDRS_TO_READ_INSTRUCTION 0xFFFD
#define FIRST_INSTRUCTION_OPCODE 0x00
#define SEC_ADDRS_TO_READ_INSTRUCTION 0xFFFE
#define SEC_INSTRUCTION_OPCODE 0x01
#define FIRST_ADDRS_STACK_PTR 0xFF
#define ROM_INIT 0x8000
#define BRK_INSTRUCTION_OPCODE 0x09

CPU::CPU(BUS* _bus_to_link) noexcept
{
  linkbus(_bus_to_link);

  /*
   *
   * Os primeiros 2 ciclos de inicialização
   *
   */
   
  write(FIRST_ADDRS_TO_READ_INSTRUCTION, FIRST_INSTRUCTION_OPCODE); // Gravando a primeira instrução que é um RST
  cycle();

  write(SEC_ADDRS_TO_READ_INSTRUCTION, SEC_INSTRUCTION_OPCODE); // Gravando a segundo instrução que é JMP para o início da ROM

  /*
   *
   * Como o JMP lê os próximos 2 bytes para saber qual endereço ele deve pular, então escrevemos nesse local, o endereço 
   * que vamos querer ir, no caso 0x8000
   *
   */

  write(SEC_ADDRS_TO_READ_INSTRUCTION + 1, 0x80);
  cycle();
}

/*
 *
 * Pequeno LOG
 *
 */

void CPU::sts(INSTRUCTION* _instruct) noexcept
{
  std::cout << "INSTRUCTION : \"" << _instruct->_name << "\" "
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

void CPU::linkbus(BUS* _bus) noexcept
{
  if(_BUS == nullptr) { _BUS = _bus; }
}

void CPU::write(uint16_t _addrs_to_write, uint8_t _data_to_write) noexcept
{
  _BUS->write(_addrs_to_write, _data_to_write);
}

uint8_t CPU::read(uint16_t _data_to_read) noexcept
{
  return _BUS->read(_data_to_read);  
}

/*
 *
 * Função para o clock
 *
 */

void CPU::cycle() noexcept
{
  INSTRUCTION* _instruction
  {
    instruction_decoder(read(_PC)) // Esse read está lendo o endereço que o _PC aponta
  };

  #if defined(CPU_LOG)
    sts(_instruction); // Mostrando informações de execução
  #endif
  
  (this->*_instruction->_instruct_ptr)(); // Executando instrução selecionada pelo instruction decoder  
}

void CPU::clock_loop() noexcept
{
  while(true)
  {
    if(_I == 0)
    {
      if(_B == 1)
      {
        break;
      }
    }
    cycle(); // 1 ciclo
    std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_FREQUENCY));
  }
}

/*
 *
 * Funções para facilitar minha vida
 *
 */

uint8_t CPU::BYTE1() noexcept
{   
  return read(_PC + 1);
}

uint8_t CPU::BYTE2() noexcept
{
  return read(_PC + 2);
}

uint8_t CPU::BYTE3() noexcept
{
  return read(_PC + 3);
}

/*
 *
 *  INSTRUCTION RST:
 *
 */

void CPU::RST() noexcept
{
  /*
   *
   * Resetando tudo para o estado inicial
   *
   */

  _A      = 0x00;
  _X      = 0x00;
  _Y      = 0x00;
  _S      = 0x00;
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

void CPU::ADD() noexcept
{
  std::cout << "ADD\n";
  ++_PC;
}

/*
 *
 * INSTRUCTION SUB:
 *
 */

void CPU::SUB() noexcept
{
  std::cout << "SUB\n";
  ++_PC;
}

/*
 *
 * INSTRUCTION JMP:
 *
 */

void CPU::JMP() noexcept
{
  uint16_t _buffer = (_buffer | BYTE1()) << 8;
  _buffer          = (_buffer | BYTE2());
  
  _PC = _buffer;
}

/*
 *
 * INSTRUCTION POP: 
 *
 */

void CPU::POP() noexcept
{
  _S = read(_STKPTR);

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

void CPU::PSH() noexcept
{ 
  if(_STKPTR -1 >= 0x00)
  {
    --_STKPTR;
  }

  write(_STKPTR, _S);

  ++_PC;
}

/*
 *
 *  INSTRUCTION PRT:
 *
 */

void CPU::PRT() noexcept
{ 
  _Y = BYTE1();

  ++_PC;

  while(_Y != '\n')
  {  
    std::cout << _Y;

    _Y = BYTE1();
      
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

void CPU::BRK() noexcept
{
  _B = 1;
}

/*
 *
 * INSTRUCTION INC/DEC:
 *
 */

/*
 *
 * Usamos o 1 byte após a instrução para definir qual registrador
 * será incrementado
 *
 */

void CPU::INC() noexcept
{
  *register_decoder(BYTE1()) += 1;
  
  _PC += 3;
}

void CPU::DEC() noexcept
{
  *register_decoder(BYTE1()) -= 1;

  _PC += 3;
}

/*
 *
 * INSTRUCTION MOV: 
 * 
 */

void CPU::MOV() noexcept
{
  *register_decoder(BYTE1()) = BYTE2();

  _PC += 3;
}

void CPU::MOV2() noexcept
{
  *register_decoder(BYTE1()) = *register_decoder(BYTE2());

  _PC += 3;
}

void CPU::MOV3() noexcept
{
  uint16_t _buffer;

  _buffer = (_buffer | BYTE2()) << 8;
  _buffer = (_buffer | BYTE3());

  *register_decoder(BYTE1()) = read(_buffer);

  _PC += 4;  
}

void CPU::MOV4() noexcept
{
  uint16_t _buffer;

  _buffer = (_buffer | BYTE2()) << 8;
  _buffer = (_buffer | BYTE3());

  write(_buffer, *register_decoder(BYTE1()));

  _PC += 4;
}


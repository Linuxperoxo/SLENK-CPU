/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.cpp                       |
 *    |  SRC MOD   : 30/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <thread>

#include "../include/cpu/cpu.hpp"
#include "../include/bus/bus.hpp"

#if defined CPU_LOG
#include <iomanip>
#include <iostream>
#include <sstream>
  constexpr double HMZ_FREQUENCY { 1790000.0 / 1000000.0 }; 
#endif

#if !defined DISPLAY_FRAMEBUFFER_ADDRS
#define DISPLAY_FRAMEBUFFER_ADDRS 0x7000
#endif

#if !defined DISPLAY_FRAMEBUFFER_SIZE
#define DISPLAY_FRAMEBUFFER_SIZE 0x12C
#endif

#if !defined STACK_PTR_ADDRS
#define STACK_PTR_ADDRS 0xFF
#endif

#define BRK_INSTRUCTION_OPCODE 0x09
#define ROM_INIT               0x8000
#define NANO_PER_SEC           1e9
#define MILLISECONDS_HOUSE     0x03
#define NANOSECONDS_HOUSE      0x06

typedef std::stringstream sstr;

CPU::CPU(BUS* _bus_to_link) noexcept
{
  _BUS = _bus_to_link;
  _FRAMEBUFFER_PTR = DISPLAY_FRAMEBUFFER_ADDRS;
}

/*
 *
 * Funções Legais >:)
 *
 */

void CPU::write(uint16_t _addrs_to_write, uint8_t _data_to_write) noexcept
{

  /*
   *
   * Aqui bloquemos o acesso ao barramento até que o DMA pare de ser usado
   *
   */

  while(_U != 0) { }
  
  _BUS->write(_addrs_to_write, _data_to_write);
}

uint8_t CPU::read(uint16_t _data_to_read) noexcept
{

  /*
   *
   * A mesma coisa aqui
   *
   */

  while(_U != 0) { }
  
  return _BUS->read(_data_to_read);  
}

/*
 *
 * Funções para o DMA
 *
 */

uint8_t CPU::DMA_interruption(uint16_t _first_addrs, uint8_t _type, uint8_t _data, uint8_t _IO_device_addrs_to_use_DMA) noexcept
{
  while(_U != 0) { if(_DMA_DEV == _IO_device_addrs_to_use_DMA || _DMA_DEV == 0) { break; }}
  
  /*
   *
   *  Aqui vamos usar um endereço para cada dev, cada chamado o DMA nós vamos armazenar o addrs do I/O device e somente ele terá acesso ao barramento
   *
   */
 
  _DMA_DEV = _IO_device_addrs_to_use_DMA;


  /*
   *
   * Configurando DMA para o device
   *
   */

  _BUS->configure_DMA(_first_addrs, _type, _data);

  if(_type)
  {
    return _BUS->DMA_R(_first_addrs);
  }

  _BUS->DMA_W(_first_addrs, _data);
  
  return 0;
}

void CPU::DMA_stopped() noexcept
{
  _DMA_DEV = 0;
  _U       = 0;
}

void CPU::DMA_started() noexcept
{
  _U = 1;
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
    
    /*
     *
     * Um pequeno LOG so para ajudar no desenvolvimento
     *
     */
    
    /*
     *
     * sstr -> std::stringstream
     *
     */
    
    static uint64_t _cycle_counter { 0 };
    static double   _runtime_sec   { 0 };
    static sstr     _cpu_log; 
     
    /*
     *
     * Simples contador de runtime só para ter uma noção do tempo de execução
     *
     */

    /*
     *
     * Aqui faço incremento de 558 nanosegundos ao _runtime, 558 que é o tempo de cada ciclo
     *
     * Como estamos trabalhando com segundos dividimos 558.0 por 1.10^9 já que 1 nanosegundo é
     * 0.000000001 segundos
     *
     */
    
    _runtime_sec += (static_cast<double>(CPU_FREQUENCY) / NANO_PER_SEC);

    _cpu_log << "INSTRUCTION   : \"" << _instruction->_name << "\" \n";
    _cpu_log << "PC ADDRS      : \"0x" << std::hex << _PC << "\" \n";
    _cpu_log << "STACK ADDRS   : \"0x" << std::hex << static_cast<int>(_STKPTR) << "\" \n";
    _cpu_log << "CYCLE COUNTER : \"" << std::dec << ++_cycle_counter << "\" \n";
    _cpu_log << "CPU CLOCK     : \"" << std::fixed << std::setprecision(2) << HMZ_FREQUENCY << "MHz\" \n";
    _cpu_log << "CPU RUNTIME   : \"" << std::fixed << std::setprecision(NANOSECONDS_HOUSE) << _runtime_sec << "sec\"\n";
    _cpu_log << "REAL RUNTIME  : \"" << std::fixed << std::setprecision(NANOSECONDS_HOUSE) << _runtime_sec * 115 << "sec\"\n";

    std::cout << "\n+--------CPU-INSTRUCTION-LOG--------+\n";
    std::cout << _cpu_log.str();
    std::cout << "+-----------------------------------+\n";
    std::cout.flush();  

    _cpu_log.str("");
    _cpu_log.clear();
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

uint8_t CPU::BYTE4() noexcept
{
  return read(_PC + 4);
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

  _A               = 0x00;
  _X               = 0x00;
  _Y               = 0x00;
  _S               = 0x00;
  _STKPTR          = 0x00;
  _PC              = 0x0000; 
  _FRAMEBUFFER_PTR = 0x0000;
  _STATUS          = 0x00;
}

/*
 *
 *  INSTRUCTION ADD:
 *
 */

void CPU::ADD() noexcept
{
  _A  += BYTE1();
  _PC += 2;
}

void CPU::ADD2() noexcept
{
  _A  += *register_decoder(BYTE1());
  _PC += 2;
}

void CPU::ADD3() noexcept
{
  _A   = BYTE1() + BYTE2();
  _PC += 3;
}

void CPU::ADD4() noexcept
{
  _A   = *register_decoder(BYTE1()) + *register_decoder(BYTE2());
  _PC += 3;
}

/*
 *
 * INSTRUCTION SUB:
 *
 */

void CPU::SUB() noexcept
{
  _A  -= BYTE1();
  _PC += 2;
}

void CPU::SUB2() noexcept
{
  _A  -= *register_decoder(BYTE1());
  _PC += 2;
}

void CPU::SUB3() noexcept
{
  _A   = BYTE1() - BYTE2();
  _PC += 3;
}

void CPU::SUB4() noexcept
{
  _A   = *register_decoder(BYTE1()) - *register_decoder(BYTE2());
  _PC += 3;
}

/*
 *
 * INSTRUCTION JMP:
 *
 */

void CPU::JMP() noexcept
{
  uint16_t _buffer; 
  
  _buffer  = (_buffer | BYTE1()) << 8;
  _buffer |= BYTE2();
  
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

  if(_STKPTR + 1 <= STACK_PTR_ADDRS)
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
  _Y = { BYTE1() };
  _A = { 0 };

  /*
   *
   * Como estamos trabalhando com usigned int por seguraça decidi usar 
   * um registrador incrementando 
   *
   */

  while(_A <= _Y)
  {
    write(_FRAMEBUFFER_PTR, BYTE2());
    
    ++_PC;
    ++_FRAMEBUFFER_PTR;
    ++_A;

    if(_FRAMEBUFFER_PTR > DISPLAY_FRAMEBUFFER_ADDRS + DISPLAY_FRAMEBUFFER_SIZE)
    {
      _FRAMEBUFFER_PTR = DISPLAY_FRAMEBUFFER_ADDRS;
    }
  }

  /*
   *
   * Copiando o Byte faltante
   *
   */

  write(_FRAMEBUFFER_PTR, BYTE2());
  
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

  _buffer  = (_buffer | BYTE2()) << 8;
  _buffer |= BYTE3();

  *register_decoder(BYTE1()) = read(_buffer);

  _PC += 4;  
}

void CPU::MOV4() noexcept
{
  uint16_t _buffer;

  _buffer  = (_buffer | BYTE1()) << 8;
  _buffer |= BYTE2();

  write(_buffer, *register_decoder(BYTE3()));

  _PC += 4;
}

void CPU::MOV5() noexcept
{
  uint16_t _buffer;

  _buffer  = (_buffer | BYTE1()) << 8; 
  _buffer |= BYTE2();

  write(_buffer, BYTE3());

  _PC += 4;
}

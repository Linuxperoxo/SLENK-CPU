/*
 * 
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.hpp                       |
 *    |  SRC MOD   : 9/10/2024                     | 
 *    |                                            |
 *    O--------------------------------------------/
 *    
 *
 */

/*
 *
 * Esse processador será um processador de 8 bits com endereçamento de 16 bits, ele vai
 * ser parecido com o NES (Nintendo Entertainment System), mas com suas próprias caracteristicas,
 * instruções e registradores.
 *
 * OBS: Por enquanto não irei implementar CACHES nem MULTICORES 
 *
 * MAP: 
 *
 * +-------+                      RAM
 * |       |    +-----------------------------------------+
 * |  CPU  |    | 0x00 | 0x01 | 0x02 | 0x03 | 0x04 | 0x05 | 
 * |       |    +-----------------------------------------+
 * +-------+                          ^   |
 *   ^  |                             |   |
 *   |  |                             |   |
 *   |  v               SYSTEM BUS    |   v
 *  +-------------------------------------------------+
 *  | +-------------+ +-------------+ +-------------+ |  
 *  | |             | |             | |             | | ---> +------------+
 *  | | CONTROL BUS | | ADDRESS BUS | |  DATA BUS   | |      | OTHERS I/O |
 *  | |             | |             | |             | | <--- +------------+
 *  | +-------------+ +-------------+ +-------------+ |
 *  +-------------------------------------------------+
 *
 * ALU: Componente que realiza todas as operações matemáticas e 
 * lógicas, como adição, subtração, operações lógicas (AND, OR, NOT)
 *
 * REGISTRADORES: pequenas áreas de memória de alta velocidade dentro 
 * da CPU que armazenam dados temporários usados durante a execução das instruções.
 *
 */

#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <cstdint>

class BUS;

typedef uint16_t ADDRS_BITS_SIZE;
typedef uint8_t  DATA_BITS_SIZE;
typedef void     NONE;

class CPU
{
private:

  CPU(const CPU&)            = delete;
  CPU(CPU&&)                 = delete;
  CPU& operator=(const CPU&) = delete;
  CPU& operator=(CPU&&)      = delete;

private:

  BUS* _BUS { nullptr };

private:

  /*
   *
   * A      : Usado para armazenar o resultado de operações aritmética
   * X      :
   * Y      : 
   * STKPTR : Armazenar o endereço para o top da pilha 
   * PC     : Armazena o endereço para a próxima instrução a ser executada pelo processador
   *
   */

  uint8_t _A       { 0x00 };
  uint8_t _X       { 0x00 };
  uint8_t _Y       { 0x00 };
  uint16_t _STKPTR { 0x0000 };
  uint16_t _PC     { 0x0000 };

  /*
  * REGISTRADOR DE STATUS:
  *
  *              BYTE
  *  +---+---+---+---+---+---+---+---+
  *  | N | V | U | B | D | I | Z | C | <- BITS
  *  +---+---+---+---+---+---+---+---+
  *    0   0   0   0   0   0   0   0
  *
  *  CARRAY BIT    : É usado nas operações aritméticas para indicar se houve um "carry out" ou seja subir 1
  *  ZERO BIT      : Setada quando um resultado de uma operação é aritmética é 0
  *  INTERRUPT BIT : É usada para controlar se o processador deve responder ou não interrupções externas
  *                  1 para ignorar qualquer interrupção
  *  DECIMAL BIT   : É usado para operações decimais, 1 para ativar
  *  BREAK BIT     : É usado para indicar uma instrução de interrupção(BRK) por software, 1 para indicar 
  *                  interrupção por software
  *  UNUSED BIT    : Esse bit não é usado porém nesse processador vamos arrumar alguma utilidade para ele
  *  OVERFLOW BIT  : Indica se houve um overflow na operação aritmética 
  *                  EXEMPLO : Digamos que ocorra uma soma (127 + 1) resultado é 128 digamos que seja permitido
  *                            números com sinais ou seja, bit N é define se o número é negativo ou possitivo, 
  *                            o 128 é um overflow já que usando bit N conseguimos representar apenas de -128 - 127
  *  NEGATIVE BIT  : Esse bit diz se o número é negativo ou possitivo, 1 para negativo 0 para possitivo
  */

  union
  {
    uint8_t _STATUS;
    struct
    {
      uint8_t _C : 1; 
      uint8_t _Z : 1; 
      uint8_t _I : 1; 
      uint8_t _D : 1; 
      uint8_t _B : 1; 
      uint8_t _U : 1; 
      uint8_t _V : 1; 
      uint8_t _N : 1; 
    };
  };

public:

  explicit CPU() noexcept;

  ~CPU() noexcept = default;

public:
  
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

  NONE linkbus(BUS*) noexcept;
  
  /*
   *
   * @info   : Essa função escreve um dado em um bloco de memória
   *
   * @return : void
   *
   * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
   *
  */

  NONE write(ADDRS_BITS_SIZE, DATA_BITS_SIZE) noexcept;
  
  /*
   *
   * @info   : Essa função lê um bloco de memória e escreve no registrador X
   *
   * @return : void
   *
   * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
   *
  */

  NONE read(ADDRS_BITS_SIZE) noexcept;
  
  /*
   *
   * @info   : Essa função apenas define o primeiro bloco de memória que o registrador
   *          PC vai procurar pela primeira instrução, e configura o ponteiro da stack 
   *          para o início
   *
   * @return : void
   *
   * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
   *
  */

  NONE reset() noexcept;
};

#endif


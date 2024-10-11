/*
 * 
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.hpp                       |
 *    |  SRC MOD   : 11/10/2024                    | 
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
 * OBS: Por enquanto não irei implementar CACHES nem MULTICORES. 
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
 * lógicas, como adição, subtração, operações lógicas (AND, OR, NOT).
 *
 * REGISTRADORES: pequenas áreas de memória de alta velocidade dentro 
 * da CPU que armazenam dados temporários usados durante a execução das instruções.
 *
 */

#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <cstdint>
#include <string>

#define INSTRUCTIONS_NUM 12

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
   * A      : Usado para armazenar o resultado de operações aritmética;
   * X      : Principalmente usado para armazenar valores para instruções aritmética, para instruções de 2 bytes, sempre vamos usar ele. Também armazena dados de leitura da memória;
   * Y      : Mesma coisa do registrador X;
   * F      : Armazena 
   * STKPTR : Armazenar o endereço para o top da pilha;
   * PC     : Armazena o endereço para a próxima instrução a ser executada pelo processador.
   *
   */

  DATA_BITS_SIZE  _A      { 0x00 };
  DATA_BITS_SIZE  _X      { 0x00 };
  DATA_BITS_SIZE  _Y      { 0x00 };

  ADDRS_BITS_SIZE _F      { 0x0000 };
  ADDRS_BITS_SIZE _STKPTR { 0x0000 };
  ADDRS_BITS_SIZE _PC     { 0x0000 };

  /*
  * REGISTRADOR DE STATUS:
  *
  *              BYTE
  *  +---+---+---+---+---+---+---+---+
  *  | N | V | U | B | D | I | Z | C | <- BITS
  *  +---+---+---+---+---+---+---+---+
  *    0   0   0   0   0   0   0   0
  *
  *  CARRAY BIT    : É usado nas operações aritméticas para indicar se houve um "carry out" ou seja subir 1;
  *  ZERO BIT      : Setada quando um resultado de uma operação é aritmética é 0;
  *  INTERRUPT BIT : É usada para controlar se o processador deve responder ou não interrupções externas
  *                  1 para ignorar qualquer interrupção;
  *  DECIMAL BIT   : É usado para operações decimais, 1 para ativar;
  *  BREAK BIT     : É usado para indicar uma instrução de interrupção(BRK) por software, 1 para indicar 
  *                  interrupção por software;
  *  UNUSED BIT    : Esse bit não é usado porém nesse processador vamos arrumar alguma utilidade para ele;
  *  OVERFLOW BIT  : Indica se houve um overflow na operação aritmética 
  *                  EXEMPLO : Digamos que ocorra uma soma (127 + 1) resultado é 128 digamos que seja permitido
  *                            números com sinais ou seja, bit N é define se o número é negativo ou possitivo, 
  *                            o 128 é um overflow já que usando bit N conseguimos representar apenas de -128 - 127;
  *  NEGATIVE BIT  : Esse bit diz se o número é negativo ou possitivo, 1 para negativo 0 para possitivo.
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

  /*
   *
   * Aqui eu fiz uma pequena struct para cada instrução, ela vai servir para organizar 
   * cada uma.
   *
   * O primeiro membro é um ponteiro para uma função, que no caso, é a nossa instrução.
   *
   * O segundo é quantos bytes nos vamos ler para executar uma instrução, ou seja, dependendo
   * do tipo da instrução, nos vamos ou apenas ler dados na memória como informações para a instrução,
   * ou pegar dados de registradores para a instrução.
   */
  
  struct INSTRUCTION
  {
    std::string _name;
    NONE            (CPU::*_instruct_ptr)(INSTRUCTION*); // Ponteiro para função
    DATA_BITS_SIZE  _bytes_to_read; // Quantos bytes ler após o endereço do opcode
    DATA_BITS_SIZE  _have_out; // Tem saída?
    DATA_BITS_SIZE* _out; // Onde vamos armazenar a saída
  };

  /*
   *
   * Aqui são os opcode. Cada instrução tem um opcode, que é um código binário que será interpretado como instrução.
   *
   * EXEMPLO: digamos que o program counter(PC) esteja apontando para um local na memória, esse local será interpretado 
   * como uma instrução. Digamos que PC esteja apontando para o o endereço 0x0004.
   *
   * +-------------------------+
   * |0x0004 | 0x0005 | 0x0006 | -> Memória onde está as instruções, pode ser a ram ou caches do processador
   * +-------------------------+
   * 
   * Como o PC está apontando para o 0x0004 ele é o início da nossa instrução, digamos que temos o valor em binário 0001
   * nesse endereço, então esse é o nosso opcode, se jogamos esse opcode para o array abaixo a instrução seria um ADD lendo
   * o primeiro byte depois do opcode, então essa instrução é de 2 bytes.
   *
   * Vamos ilustrar como vai funcionar:
   *
   * - Primeiro vamos ler para onde o PC está apontando (0x0004);
   * - Depois vamos ler esse endereço e interpretar o dado escrito como um opcode;
   * - Com base nesse opcode a instrução vai ser definida. Como exemplo, vamos usar nosso array _opcode abaixo.
   *   Jogando o opcode para o array, digamos que o opcode seja 0001 em binário, a instrução retornada pelo array é
   *   {&CPU::ADD, 1, 1, &_A}, vamos quebrar ela e entender cada parte.
   *
   *   - A primeira é a instrução que vai ser executa;
   *   - Quantos bytes ler após o endereço do opcode, nesse caso vamos ler o 0x0005 também;
   *   - Se a instrução vai ter uma saída que precisa ser guardada, lugar onde vai ser guardada, pode ser um endereço de 
   *     memória, ou registrador
   *   - Lugar onde vai ser aramazenado o dado de saída.
   *
   * Nesse exemplo, nossa instrução tem uma dependência, que no caso é o próximo byte após o opcode. 
   * 
   * Para ficar mais claro:
   *  * Em uma soma precisamos de 2 valores, para essa instrução nos temos o primeiro, ele está em 0x0005, mas, e o segundo?
   *    O segundo está em um registrador, vamos usar os registradores _X e _Y, para esse instrução, vamos usar o registrador _X, 
   *    e então somar o valor armazenado em 0x0005 com o valor aramazenado em _X. Para instruções que tem 0 nos vamos ler o _X e _Y,
   *    e para instruções de 2 bytes, vamos ler os próximos 2 bytes após o opcode e somar os 2.
   */

  INSTRUCTION _opcode[INSTRUCTIONS_NUM]
  {
    {"RST", &CPU::RST, 0, 0, nullptr}, {"JMP", &CPU::JMP, 0, 0, nullptr}, {"PRT", &CPU::PRT, 128, 0, nullptr},
    {"ADD", &CPU::ADD, 0, 1, &_A},     {"ADD", &CPU::ADD, 1, 1, &_A},     {"ADD", &CPU::ADD, 2, 1, &_A},    
    {"SUB", &CPU::SUB, 0, 1, &_A},     {"SUB", &CPU::SUB, 1, 1, &_A},     {"SUB", &CPU::SUB, 2, 1, &_A}
  };

public:

  explicit CPU(BUS*) noexcept;

  ~CPU() noexcept = default;

private:

  /*
   *
   * Typedef
   *
   * ADDRS_BITS_SIZE -> uint16_t
   * DATA_BITS_SIZE  -> uint8_t
   * NONE            -> void
   *
   */

  NONE RST(CPU::INSTRUCTION*) noexcept;
  NONE ADD(CPU::INSTRUCTION*) noexcept;  
  NONE SUB(CPU::INSTRUCTION*) noexcept;
  NONE JMP(CPU::INSTRUCTION*) noexcept;
  NONE PRT(CPU::INSTRUCTION*) noexcept;

public:

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

  NONE run() noexcept;
};

#endif


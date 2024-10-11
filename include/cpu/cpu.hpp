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

constexpr uint32_t CLOCK_FREQUENCY { 1000000000 / 1790000 }; // 1.79 MHz

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
   * F      : Armazena endereços de memória, usado pela instrução JMP;
   * H      : Armazena dados da stack, usado pela instrução POP, e joga dados para a stack usado pela instrução PUH;
   * STKPTR : Armazenar o endereço para o top da pilha;
   * PC     : Armazena o endereço para a próxima instrução a ser executada pelo processador.
   *
   */

public:

  DATA_BITS_SIZE  _A      { 0x00 };
  DATA_BITS_SIZE  _X      { 0x00 };
  DATA_BITS_SIZE  _Y      { 0x00 };
  DATA_BITS_SIZE  _H      { 0x00 };

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
   * Aqui eu criei uma struct simples para representar cada instrução.
   * Ela basicamente vai nos ajudar a organizar todas elas direitinho.
   *
   * O primeiro membro da struct é um ponteiro para uma função, que no caso, 
   * é a própria instrução que queremos executar.
   *
   * O segundo membro indica quantos bytes a gente vai precisar ler para executar 
   * essa instrução. Isso depende do tipo da instrução, já que algumas vão ler 
   * dados da memória (como parâmetros) ou pegar informações dos registradores.
   *
   */
  
  struct INSTRUCTION
  {
    std::string     _name;
    NONE            (CPU::*_instruct_ptr)(INSTRUCTION*); // Ponteiro para função
    DATA_BITS_SIZE  _bytes_to_read; // Quantos bytes ler após o endereço do opcode
    DATA_BITS_SIZE  _have_out; // Tem saída?
    DATA_BITS_SIZE* _out; // Onde vamos armazenar a saída
  };

  /*
   * Aqui estão os opcodes. Cada instrução tem um opcode, que é basicamente um código binário
   * que o processador vai interpretar como uma instrução específica.
   *
   * EXEMPLO: Imagine que o program counter (PC) esteja apontando para um endereço na memória,
   * esse endereço vai ser tratado como uma instrução. Vamos supor que o PC esteja apontando para
   * o endereço 0x0004.
   *
   * +--------------------------+
   * | 0x0004 | 0x0005 | 0x0006 | -> Memória onde estão as instruções, podendo ser RAM ou cache do processador
   * +--------------------------+
   *
   * O PC aponta para o 0x0004, que será o início da nossa instrução. Se nesse endereço estiver 
   * o valor binário `0001`, então esse é o opcode. A partir disso, buscamos esse opcode no array 
   * abaixo, e encontramos a instrução ADD, que lê o primeiro byte depois do opcode. Então, essa 
   * é uma instrução de 2 bytes.
   *
   * Como isso funciona:
   *
   * - Primeiro, a gente lê o endereço para onde o PC está apontando (0x0004);
   * - Depois, interpretamos o dado desse endereço como um opcode;
   * - Com base no opcode, a instrução é definida. Usando o array `_opcode` abaixo, 
   *   se o opcode for `0001` em binário, a instrução será:
   *   {&CPU::ADD, 1, 1, &_A}. Vamos entender cada parte:
   *
   *   - Primeiro: a instrução que será executada (nesse caso, `ADD`);
   *   - Segundo: quantos bytes serão lidos após o endereço do opcode, aqui lemos o byte em 0x0005;
   *   - Terceiro: se a instrução tem uma saída para ser armazenada e onde ela vai ser guardada 
   *     (pode ser um endereço de memória ou registrador);
   *   - Quarto: onde o resultado final será armazenado (nesse caso, no registrador `_A`).
   *
   * Nesse exemplo, a instrução `ADD` depende do próximo byte após o opcode.
   *
   * Para deixar mais claro:
   *  * Em uma soma, precisamos de dois valores. O primeiro está em 0x0005, mas e o segundo?
   *    Ele está num registrador, e vamos usar o registrador `_X` para essa instrução. 
   *    Assim, somamos o valor de 0x0005 com o que está em `_X`. Para instruções de 1 byte, 
   *    lemos os registradores `_X` e `_Y`, e para instruções de 2 bytes, lemos os dois 
   *    próximos bytes após o opcode e fazemos a soma.
   */

  INSTRUCTION _opcode[INSTRUCTIONS_NUM]
  {
    {"RST", &CPU::RST, 0, 0, nullptr}, {"JMP", &CPU::JMP, 0, 0, nullptr}, {"PRT", &CPU::PRT, 127, 0, nullptr},
    {"POP", &CPU::POP, 0, 0, nullptr}, {"PUH", &CPU::PUH, 0, 0, nullptr},
    {"ADD", &CPU::ADD, 0, 1, &_A},     {"ADD", &CPU::ADD, 1, 1, &_A},     {"ADD", &CPU::ADD, 2, 1, &_A},    
    {"SUB", &CPU::SUB, 0, 1, &_A},     {"SUB", &CPU::SUB, 1, 1, &_A},     {"SUB", &CPU::SUB, 2, 1, &_A}
  };

  /*
   *
   * OPCODES:
   *  RST : 0x00   JMP : 0x01   PRT: 0x02
   *  POP : 0x03   PUH : 0x04   
   *  ADD : 0x05   ADD : 0x06   ADD : 0x07
   *  SUB : 0x08   SUB : 0x09   SUB : 0x0A
   *
   */

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

  /*
   * 
   * Instruções: 
   *
   */

  NONE RST(CPU::INSTRUCTION*) noexcept; // Reseta o processador, modificando o PC e STKPTR
  NONE ADD(CPU::INSTRUCTION*) noexcept; // Soma
  NONE SUB(CPU::INSTRUCTION*) noexcept; // Subtração
  NONE JMP(CPU::INSTRUCTION*) noexcept; // Pula PC para um endereço de memória 
  NONE PRT(CPU::INSTRUCTION*) noexcept; // Exibe caracteres na tela. MÁXIMO DE 127 Bytes de leitura
  NONE POP(CPU::INSTRUCTION*) noexcept; // Desempilha elemento da stack 
  NONE PUH(CPU::INSTRUCTION*) noexcept; // Empilha elemento na stack

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

  /*
   *
   * @info   : Executa a instrução que está no contador de programas PC
   *
   * @return : void
   *
   * @param  : void
   *
   */

  NONE run() noexcept;
};

#endif


/*
 * 
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.hpp                       |
 *    |  SRC MOD   : 18/10/2024                    | 
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
 *                  +-------------+
 *       +--------> |             | ----------------+
 *       |          |  DATA BUS   |                 |              +-----------+
 *       |  +------ |             | <------------+  |              |   MEMORY  | <-+
 *       |  |       +-------------+              |  |              +-----------+   |
 *       |  v              |                     |  v                    |         |
 *  +--------+      +-------------+        +---------------+             |         |
 *  |        |      |             |        |               |             |         |
 *  |  CPU   |----> | CONTROL BUS |------> | ADDRS DECODER |-------------+         |
 *  |        |      |             |        |               |             |         |
 *  +--------+      +-------------+        +---------------+             |         |
 *    ^ |                  |                       ^                     |         |
 *    | |           +-------------+                |               +-----------+   |
 *    | |           |             |                |               |  I/O DEV  |   |
 *    | +---------> |  ADDRS BUS  |----------------+               +-----------+   |
 *    | |           |             |                                      | ^       |
 *    | |           +-------------+                                      | |       |
 *    | |                  |                                             | |       |
 *    | |                  | CONECTADO A TODOS OS BARRAMENTOS            | |       |
 *    | |                  |                                             | |       |
 *    | |           +-------------+                                      | |       |
 *    | +---------> |     DMA     | <------------------------------------+ |       |
 *    +------------ | CONTROLLER  | ----------------------------------------       |
 *                  +-------------+                                                |
 *                        |                                                        |
 *                        |                                                        |
 *                        +--------------------------------------------------------+
 * 
 * DMA: Ele é responsável por fazer um device I/O ter acesso direto a memória sem
 * envolver o processador no meio. OBS: Quando o DMA está sendo usado ele emite uma
 * interrupção ao processador, não deixando-o acessar o barramento, mas o processador 
 * vai ter o cache para poder acessar dados
 *
 * ADDRS DECODER: Ele faz a "tradução" do endereço enviado pelo barramento de endereço,
 * ele verifica se o endereço é para um dispositivo mapeado na memória ou é acesso a memória,
 * assim enviando dados ou lendo para memória ou I/O DEV
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

#define OPCODE_NUM 14
#define REGCODE_NUM 5

#define REG_A 0x00
#define REG_X 0x01
#define REG_Y 0x02
#define REG_H 0x03
#define REG_Q 0x04

#define CPU_LOG // Comente essa linha se não quiser o log de cada instrução

constexpr uint32_t CLOCK_FREQUENCY { 1000000000 / 1790000 }; // 1.79 MHz

class BUS;

typedef uint16_t  ADDRS_BITS_SIZE;
typedef uint8_t   DATA_BITS_SIZE;
typedef void      NONE;

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
   * X      : Uso geral, armazena dados de leitura da memória;
   * Y      : Uso geral, é usado pela função BYTE1;
   * F      : Uso geral, é usado pela função BYTE2;
   * Q      : Uso geral, é usado pela função BYTE3;
   * H      : Armazena dados da stack, é usado pela instrução POP, joga dados para a stack através da instrução PUH;
   * STKPTR : Armazenar o endereço para o top da pilha;
   * PC     : Armazena o endereço da instrução a ser executada.
   *
   */

public:

  DATA_BITS_SIZE  _A      { 0x00 };
  DATA_BITS_SIZE  _X      { 0x00 };
  DATA_BITS_SIZE  _Y      { 0x00 };
  DATA_BITS_SIZE  _F      { 0x00 };
  DATA_BITS_SIZE  _Q      { 0x00 };
  DATA_BITS_SIZE  _H      { 0x00 };
  DATA_BITS_SIZE  _STKPTR { 0x00 };

  ADDRS_BITS_SIZE _PC     { 0x0000 };

  /*
  * REGISTRADOR DE STATUS:
  *
  *              BYTE
  *  +---+---+---+---+---+---+---+---+
  *  | N | V | U | B | D | I | Z | C | <- BITS
  *  +---+---+---+---+---+---+---+---+
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
   * INSTRUCTION struct:
   *
   * Aqui eu criei uma struct simples para representar cada instrução.
   * Ela basicamente vai nos ajudar a organizar todas elas direitinho.
   *
   */
  
  struct INSTRUCTION
  {
    std::string     _name; // nome da instrução que vai aparecer no LOG
    NONE            (CPU::*_instruct_ptr)(); // Ponteiro para função 
  };

  /*
   *
   * Essa struct INSTRUCTION basicamente representa cada instrução que o processador vai executar.
   *
   * Cada instrução tem um nome (tipo "ADD"), um ponteiro pra função que faz o trabalho de verdade (via _instruct_ptr),
   * e um modo de endereçamento (_addrs_mode) caso precise acessar a memória de uma forma especial. O campo _regop
   * guarda qual registrador está envolvido (pode ser _A, _X, etc.).
   *
   * O _instruct_ptr é um ponteiro pra uma função que vive dentro da classe CPU e que vai rodar a instrução.
   * O _addrs_mode também é um ponteiro pra função, mas esse aqui cuida de como vamos acessar a memória pra
   * pegar os dados pra instrução (se necessário).
   *
   */
 
  /*
   *
   * OPCODE:
   *
   * Agora aqui temos o array de opcodes. O opcode é o código binário que representa uma instrução.
   * Por exemplo, se o PC (Program Counter) estiver apontando pra 0x0004 na memória e nesse endereço
   * o valor for '0001', esse é o opcode que a CPU vai interpretar como uma instrução.
   *
   * Exemplo:
   * Se o PC tá em 0x0004 e o valor ali é 0001, isso é um opcode. Aí a gente pega esse opcode e
   * procura ele no array _opcode pra ver o que é. Se for 'ADD', por exemplo, essa instrução vai
   * ler o byte seguinte (0x0005) e fazer alguma operação (tipo somar) com o valor desse endereço.
   * 
   * O processo geral:
   * 1. A CPU lê o endereço que o PC aponta (0x0004).
   * 2. Interpreta o valor lá como um opcode (0001).
   * 3. Com esse opcode, ela sabe qual instrução executar e segue o resto dos bytes, se necessário.
   * 
   * A cada opcode, o array _opcode define o que fazer:
   * - O nome da instrução (ex.: "ADD").
   * - O ponteiro pra função que executa a instrução.
   * - O modo de endereçamento, se necessário.
   * - Qual registrador está envolvido, se for o caso.
   *
   */

  INSTRUCTION _opcode[OPCODE_NUM]
  {
    {"RST", &CPU::RST}, {"JMP", &CPU::JMP},  {"POP", &CPU::POP},  {"PSH", &CPU::PSH},
    {"MOV", &CPU::MOV}, {"MOV", &CPU::MOV2}, {"MOV", &CPU::MOV3}, {"MOV", &CPU::MOV4},
    {"PRT", &CPU::PRT}, {"BRK", &CPU::BRK}
  };

  /*
   *
   * Esse array _opcode define como cada opcode vai ser tratado. Cada entrada tem:
   * - O nome da instrução ("RST", "JMP", "POP", etc.).
   * - Um ponteiro pra função que faz a instrução acontecer (tipo &CPU::ADD).
   * 
   */

  /*
   *
   * Agora, o array _regcode armazena os registradores que o processador vai usar.
   * Cada um deles aponta pra um registrador específico na CPU.
   *
   */

  uint8_t* _regcode[REGCODE_NUM]
  {
    &_A,  &_X, &_Y, &_H, &_Q
  };

  /*
   *
   * OPCODES:
   *  RST : 0x00   JMP : 0x01    POP : 0x02    PSH : 0x03   
   *  MOV : 0x04   MOV : 0x05    MOV : 0x06    MOV : 0x07
   *  PRT : 0x08   BRK : 0x09
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
   * @info   : Essa função mostra algumas informações, como, endereço atual apontado por PC, 
   *           endereço atual da STACK e INSTRUÇÃO que vai ser executada 
   *
   * @return : void
   *
   * @param  : void
   */

  NONE sts() noexcept; 

  /*
   * 
   * INSTRUCTIONS: 
   * 
   * RST : Reseta o processador para o estado inicial;
   * ADD : Instrução de soma, pode tanto somar os primeiro e o segundo byte após a instrução, quanto somar 2 registradores, qualquer soma será armazenado no registrador A;
   * SUB : A mesma lógica do ADD porém para subtração;
   * JMP : Lê os 2 bytes após a instrução e pula para o endereço;
   *       EXEMPLO:
   *                +---------------------------------+
   *                | JMP | 0x80 | 0x00 | 0xAA | 0xFA | 
   *                +---------------------------------+
   *       Vamos ler os 2 bytes após a instrução, no caso o 0x80 e 0x00, com isso vamos pular para esse endereço JMP 0x8000.
   *
   *       OBS : Lembre que esse processador é de 8 bits, então vamos armazenar o high byte no registrador Y e o lower byte no registrador F, lembre se jogar o valor de cada
   *             um para a stack antes de continuar.
   *
   * POP : Desempilha um elemento da stack e armazena no registrador H; 
   * PSH : Empilha o dado de H para a stack.
   *
   */

  NONE RST() noexcept; // Reseta o processador, modificando o PC e STKPTR
  NONE ADD() noexcept; // Soma
  NONE SUB() noexcept; // Subtração
  NONE JMP() noexcept; // Pula PC para um endereço de memória 
  NONE POP() noexcept; // Desempilha elemento da stack 
  NONE PSH() noexcept; // Empilha elemento na stack
  NONE PRT() noexcept; // Imprimi caracteres na tela até encontrar um caractere nulo('\n'). OBS : Essa instrução é temporária
  NONE BRK() noexcept; // Interrompe execução do programa
  NONE INC() noexcept; // Incrementa 1 a um registrador
  NONE DEC() noexcept; // Decrementa 1 a um registrador

  /*
   *
   * Nesse caso a instrução MOV vai ter 4 variações
   *
   * 1. Mover dado bruto para um registrador(MOV) 
   *    EXEMPLO : MOV A, 1
   * 
   * 2. Mover dado de um registrador para outro (MOV2) 
   *    EXEMPLO : MOV A, X
   * 
   * 3. Mover dado de uma área da memória para um registrador(MOV3)
   *    EXEMPLO : MOV A, 0x80FF
   *
   * 3. Mover dado de um registrador para uma área da memória
   *    EXEMPLO : MOV 0x80FF, A
   *
   */

  NONE MOV() noexcept; NONE MOV2() noexcept; NONE MOV3() noexcept; NONE MOV4() noexcept;


  /*
   *
   * Funções para facilitar 
   *
   * Aqui podemos ler o 1, 2 e 3 byte após a instrução 
   *
   */

  NONE BYTE1() noexcept; // Armazena no registrador Y
  NONE BYTE2() noexcept; // Armazena no registrador F
  NONE BYTE3() noexcept; // Armazena no registrador Q

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

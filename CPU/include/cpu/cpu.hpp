/*
 * 
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : cpu.hpp                       |
 *    |  SRC MOD   : 02/11/2024                    | 
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
#include <cstdlib>
#include <string>

#define REGCODE_NUM 0x05
#define OPCODE_NUM  0x15
#define BRK_OPCODE  0x0A

/*
 *
 * Descomente a linha abaixo para ativar o log do CPU
 *
 * Por enquanto está bem rústico, recomendo deixar assim mesmo
 *
 * Eu uso ele para debug
 *
 */

//#define CPU_LOG

constexpr uint16_t CPU_FREQUENCY { 1000000000 / 1790000 }; // 558 nanosegundos de delay

class BUS;

class CPU
{
private:

  CPU(const CPU&)            = delete;
  CPU(CPU&&)                 = delete;
  CPU& operator=(const CPU&) = delete;
  CPU& operator=(CPU&&)      = delete;

private:

  BUS* _BUS { nullptr };

public:

  /*
   *
   * A      : Usado para armazenar o resultado de operações aritmética, e é usado pela instruçao PTR como INC;
   * X      : Uso geral
   * Y      : Uso geral, é usado pela instrução PRT
   * S      : Armazena dados da stack através da instrução POP e joga dados para a stack através da instrução PUH;
   * STKPTR : Armazenar o endereço para o top da pilha;
   * PC     : Armazena o endereço da instrução a ser executada.
   *
   */

  uint8_t  _A               { 0x00 };
  uint8_t  _X               { 0x00 };
  uint8_t  _Y               { 0x00 };
  uint8_t  _S               { 0x00 };
  uint8_t  _STKPTR          { 0x00 };
  uint8_t  _DMA_DEV         { 0x00 };
  uint16_t _PC              { 0x0000 };
  uint16_t _FRAMEBUFFER_PTR { 0x0000 };

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
    void            (CPU::*_instruct_ptr)(); // Ponteiro para função 
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
   * Agora aqui temos o array de OPTABLE. O opcode é o código binário que representa uma instrução.
   * Por exemplo, se o PC (Program Counter) estiver apontando pra 0x0004 na memória e nesse endereço
   * o valor for '0001', esse é o opcode que a CPU vai interpretar como uma instrução.
   *
   * Exemplo:
   * Se o PC tá em 0x0004 e o dado armazenado nesse endereço é 0001, esso é o nosso opcode. Aí a gente 
   * pega esse opcode e procura ele no array _opcode pra ver o que é. Se for 'ADD', por exemplo, essa 
   * instrução vai ler o byte seguinte (0x0005) e fazer alguma operação (tipo somar) com o valor desse 
   * endereço.
   * 
   * O processo geral:
   * 1. A CPU lê o endereço que o PC aponta (0x0004).
   * 2. Interpreta o valor lá como um opcode (0001).
   * 3. Com esse opcode, ela sabe qual instrução executar e segue o resto dos bytes, se necessário.
   * 
   * A cada opcode, o array OPTABLE define o que fazer:
   * - O nome da instrução (ex.: "ADD").
   * - O ponteiro pra função que executa a instrução.
   *
   */

  inline INSTRUCTION* instruction_decoder(uint8_t _opcode) noexcept
  { 

    /*
    *
    * Esse array _OPTABLE define como cada opcode vai ser tratado. Cada entrada tem:
    * - O nome da instrução ("RST", "JMP", "POP", etc.).
    * - Um ponteiro pra função que faz a instrução acontecer (tipo &CPU::ADD).
    * 
    */
    
    static INSTRUCTION _OPTABLE[OPCODE_NUM]
    {
      {"RST", &CPU::RST},  {"JMP", &CPU::JMP},  {"POP", &CPU::POP},  {"PSH", &CPU::PSH},
      {"MOV", &CPU::MOV},  {"MOV", &CPU::MOV2}, {"MOV", &CPU::MOV3}, {"MOV", &CPU::MOV4},
      {"MOV", &CPU::MOV5}, {"PRT", &CPU::PRT},  {"BRK", &CPU::BRK},  {"ADD", &CPU::ADD},  
      {"ADD", &CPU::ADD2}, {"ADD", &CPU::ADD3}, {"ADD", &CPU::ADD4}, {"SUB", &CPU::SUB},  
      {"SUB", &CPU::SUB2}, {"SUB", &CPU::SUB3}, {"SUB", &CPU::SUB4}, {"INC", &CPU::INC},
      {"DEC", &CPU::DEC}
    }; 
    
    /*
     *
     * OPCODES:
     *  RST : 0x00   JMP : 0x01    POP : 0x02    PSH : 0x03   
     *  MOV : 0x04   MOV : 0x05    MOV : 0x06    MOV : 0x07
     *  MOV : 0x08   PRT : 0x09    BRK : 0x0A    ADD : 0x0B    
     *  ADD : 0x0C   ADD : 0x0D    ADD : 0x0E    SUB : 0x0F    
     *  SUB : 0x10   SUB : 0x11    SUB : 0x12    INC : 0x13
     *  DEC : 0x14
     *
     */

    return _opcode >= 0 && _opcode < OPCODE_NUM? &_OPTABLE[_opcode] : &_OPTABLE[BRK_OPCODE]; 
  }

  inline uint8_t* register_decoder(uint8_t _regcode) noexcept
  {

    /*
     * 
     * Agora, o array _regcode armazena os registradores que o processador vai usar.
     * Cada um deles aponta pra um registrador específico da CPU.
     *
     */

    static uint8_t* _REGTABLE[REGCODE_NUM]
    {
      &_A, &_X, &_Y, &_S, &_STKPTR
    };

    /*
     *
     * REGCODES:
     *  A      : 0x00   X : 0x01    Y : 0x02    S : 0x03
     *  STKPTR : 0x04   
     *
     */

    return _regcode >= 0 && _regcode < REGCODE_NUM ? _REGTABLE[_regcode] : nullptr;    
  }

public:

  explicit CPU(BUS*) noexcept;

  ~CPU() noexcept = default;

private:

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

  void RST() noexcept; // Reseta o processador, modificando o PC e STKPTR
  
  /*
   */

  void ADD() noexcept; void ADD2() noexcept; void ADD3() noexcept; void ADD4() noexcept;


  /*
   */

  void SUB() noexcept; void SUB2() noexcept; void SUB3() noexcept; void SUB4() noexcept;

  void JMP() noexcept; // Pula PC para um endereço de memória 
  void POP() noexcept; // Desempilha elemento da stack 
  void PSH() noexcept; // Empilha elemento na stack
  void PRT() noexcept; // Imprimi caracteres na tela até encontrar um caractere nulo('\n'). OBS : Essa instrução é temporária
  void BRK() noexcept; // Interrompe execução do programa
  void INC() noexcept; // Incrementa 1 a um registrador
  void DEC() noexcept; // Decrementa 1 a um registrador

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
   * 4. Mover dado de um registrador para uma área da memória
   *    EXEMPLO : MOV 0x80FF, A
   *
   * 5. Mover dado bruto para uma área da memória
   *    EXEMPLO : MOV 0x80FF, 1
   *
   */

  void MOV() noexcept; void MOV2() noexcept; void MOV3() noexcept; void MOV4() noexcept; void MOV5() noexcept;


  /*
   *
   * Funções para facilitar 
   *
   * Aqui podemos ler o 1, 2, 3 e 4 byte após a instrução 
   *
   */

  uint8_t BYTE1() noexcept;
  uint8_t BYTE2() noexcept; 
  uint8_t BYTE3() noexcept; 
  uint8_t BYTE4() noexcept;

  /*
   *
   * @info   : Executa a instrução que está no contador de programa PC, 1 ciclo
   *
   * @return : void
   *
   * @param  : void
   *
   */

  void cycle() noexcept;

public:

  /*
   *
   * @info   : Essa função escreve um dado em um bloco de memória
   *
   * @return : void
   *
   * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
   *
  */

  void write(uint16_t, uint8_t) noexcept;
  
  /*
   *
   * @info   : Essa função lê um bloco de memória e escreve no registrador X
   *
   * @return : uint8_t
   *
   * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
   *
  */

  uint8_t read(uint16_t) noexcept;

  /*
   *
   * @info   : Vai chamar a função cycle com uma frequência
   *
   * @return : void
   *
   * @param  : void
   *
   */ 

  void clock_loop() noexcept;

  /*
   *
   * @info   : Uma interrupção que configura o DMA para uma operação
   *
   * @return : uint8_t
   *
   * @param  : void
   *
   */

  uint8_t DMA_interruption(uint16_t, uint8_t, uint8_t, uint8_t) noexcept;
 
  /*
   *
   * @info    : Uma interrupção que avisa que o DMA não está sendo usado 
   *            e que o barramento está livre
   *
   * @return : void
   *
   * @param  : void
   *
   */

  void DMA_stopped() noexcept;

  /*
   *
   * @info   : Uma interrupção que avisa que o DMA está sendo usado
   *           e que o barramento não pode ser usado
   *
   * @return : void
   *
   * @param  : void
   *
   */ 

  void DMA_started() noexcept;
};

#endif

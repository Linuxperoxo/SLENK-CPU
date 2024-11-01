/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : nanc.cpp                      |
 *    |  SRC MOD   : 01/11/2024                    |
 *    |  VERSION   : 0.0-1                         |
 *    |                                            |
 *    O--------------------------------------------/
 *    
 *
 */

/*
 * Para essa primeira versão a otimização não vai ser o nosso foco, pretendo deixar nosso compilador
 * funcional antes de qualquer coisa otimização, seja em estruturas de dados ou algoritmos.
 *
 * OTIMIZAÇÂO PRÉMATURA É A RAIZ DE TODO MAL!
 *
 *    CHANGE LOG 0.0-1:
 *      * Adicionado -> Funções básicas como: Remoção de espaços e quebra de linhas no arquivo e divisão do arquivo em tokens;
 *      * Adicionado -> Parsing para todas as instruções
 *      * Adicionado -> Tradução das instruções para opcode binário
 *      * Melhoria   -> Estrutura do código
 *
 *    TO DOS 0.0-1:
 *      * Traduzir os argumentos para binário;
 *
 *    TO DOS 0.1-0:
 *      * Aplicar diversas otimizações e melhorias no código;
 *      * Usar estruturas de dados mais complexas, como hashtable;
 *
 */

/*
 *
 * Esse será nosso compilador, com eles vamos conseguir escrever código de forma mais humana.
 *
 * Fazer um pequeno compilador foi uma ideia desde sempre, programar em binário é extremamente legal
 * porém é muito fácil errar algum 0 ou 1 e perder o programa por completo.
 *
 * Fazer um compilador é um projeto extremamente gratificante, porém possue várias dificuldades, mas a ideia
 * desse projeto é realmente me desafiar e explorar os funcionamentos de cada coisa.
 *
 * O compilador é a parte mais importante de qualquer linguagem, ele vai dividir o código em 3 partes:
 *
 * - Lexer      : Nessa parte o código vai ser convertido em tokens. Tokens são as menores unidades significativas 
 *                de um programa (como palavras-chave, identificadores, números, operadores, etc.).
 *
 * - Parsing   : Parte que os tokens serão analisados e organizados em árvores, é aqui onde temos erro de syntax caso 
 *               algo não esteja correto.
 *
 * - Tradução : A maioria dos compiladores traduzem os tokens em assembly, por isso conseguimos usar -O3, -O2 em compiladores
 *              como o GCC e o LLVM, a ideia é reduzir o tamanho do código assembly, e depois de gerar todo o código assembly
 *              vamos traduzir esse código para instrução de máquina.
 *
 */

/*
 *
 * COMPILER for canembly lang
 *
 */

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "../include/utils.hpp"
#include "../include/instructions_parsing.hpp"

#define SOURCE_FILE       argv[1]
#define CANEBLY_EXTENSION "ceb" 

#define OPTABLE_SIZE         0x13
#define REGTABLE_SIZE        0x05
#define INSTRUCTIONS         0x09
#define FUNCTIONS_DECODER    0x03

/*
 *
 * Error codes
 *
 */

#define SYNTAX_ERROR      0xFF
#define SOURCE_FILE_ERROR 0xFE

/*
 *
 * Essa parte pode ser bem complexa, talvez até mais que devia, mas foi a
 * forma que eu encontrei para lidar com instruções que podem ter várias 
 * variações, como o MOV
 *
 */

static std::string _cpu_possible_instructions_names[OPTABLE_SIZE]
{
  "RST",  "JMP",  "POP",  "PSH",
  "MOV1", "MOV2", "MOV3", "MOV4",
  "MOV5", "PRT",  "BRK",  "ADD1",
  "ADD2", "ADD3", "ADD4", "SUB1",
  "SUB2", "SUB3", "SUB4"
};

static std::string _reg_names[REGTABLE_SIZE]
{
  "A", "X", "Y", "S", 
  "STK" 
};

static std::string _source_possible_instructions_names[INSTRUCTIONS]
{
  "RST", "JMP", "POP", "PSH",
  "MOV", "PRT", "BRK", "ADD",
  "SUB"
};

static void (*_instructions_functions_decoder[INSTRUCTIONS])(std::string* __restrict, const std::string* __restrict, const std::string* __restrict)
{
  &rst_instruction, &jmp_instruction, &pop_instruction, &psh_instruction,
  &mov_instruction, &prt_instruction, &brk_instruction,  &add_instruction, 
  &sub_instruction
};

void instruction_parsing(std::string* _instruction_name, const std::string* _arg1, const std::string* _arg2) noexcept
{
  /*
   *
   * Definimos com um valor qualquer só para ver se ele foi alterado
   *
   */

  for(uint8_t _i { 0 }; _i < INSTRUCTIONS; _i++)
  {
    if(_source_possible_instructions_names[_i] == *_instruction_name) { _instructions_functions_decoder[_i](_instruction_name, _arg1, _arg2); return; } 
  }
  std::cerr << "Syntax Error -> " << "Invalid instruction -> " << *_instruction_name << ' ' << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR);
}

void convert_instruction(const std::string* _instruction_name, std::string* _token_bin) noexcept
{
  for(uint8_t _i { 0 }; _i < OPTABLE_SIZE; _i++)
  {
    if(*_instruction_name == _cpu_possible_instructions_names[_i]) { decimal_in_string_bin(_i, _token_bin); return; }
  }
}

struct Token
{
  /*
   *
   * Aqui é a nossa "árvore", vamos guardar o nome e os argumentos do token que o Lexer organizou 
   *
   */

  std::string _name;
  std::string _arg1;
  std::string _arg2;

  /*
   *
   * Aqui vai ficar o binário desse token, ou seja, o binário que será interpretado como instrução
   * pelo processador
   *
   */

  std::string _bin;

  void parsing() noexcept
  {
    /*
     *
     * Fazendo o parsing do nosso token
     *
     */

    instruction_parsing(&_name, &_arg1, &_arg2);
  }

  void translate() noexcept
  {
    /*
     *
     * Convertendo para binário
     *
     */

    convert_instruction(&_name, &_bin);
    
    std::cout << _name << ' ' << _bin << '\n';
  }
};

void lexer(const uint8_t* _source_file, std::vector<Token>* _tokens, const uint32_t _source_file_size) noexcept
{
  /*
   *
   * Aqui fica o nosso lexer, é aqui onde o código é organizado.
   *
   * EXEMPLO:
   *  ARQUIVO FONTE:
   *    MOV [A] [X];
   *    MOV [B] [*0x8010];
   *
   * Devemos organizar a instrução e seus argumentos, no final ficará mais ou menos assim:
   *  
   *  INSTRUCTION : MOV
   *  ARG1        : A
   *  ARG2        : X
   *
   *  INSTRUCTION : MOV
   *  ARG1        : B
   *  ARG2        : *0x8010
   *
   */

  std::string _current_instruction_name;
  std::string _current_instruction_arg1;
  std::string _current_instruction_arg2;

  bool _inside_key_arg1       { false };
  bool _inside_key_arg1_close { false };
  bool _inside_key_arg2       { false };
  bool _inside_key_arg2_close { false };

  for(uint32_t _i { 0 }; _i < _source_file_size; _i++)
  {
    /*
     *
     * Tenho que admitir que essa parte está bem confusa mas no futuro pretendo melhorar :D.
     *
     * Nessa parte estamos divindo o que é instrução e o que é argumento, onde se inicia o 1 e o 2 
     * argumento e onde acaba, onde começa e termina uma instrução, também pretendo adicionar mais funções
     * nessa parte >:)
     *
     */

    if(std::isalpha(_source_file[_i]) || _source_file[_i] == '*')
    {
      if(_inside_key_arg1)
      {
        _current_instruction_arg1.push_back(_source_file[_i]);  
      } else if (_inside_key_arg2){
        _current_instruction_arg2.push_back(_source_file[_i]);
      } else {
        _current_instruction_name.push_back(_source_file[_i]);
        _current_instruction_name.push_back(_source_file[++_i]);  
        _current_instruction_name.push_back(_source_file[++_i]);  
      }
    } else if(std::isalnum(_source_file[_i])){
      if(_inside_key_arg1)
      {
        _current_instruction_arg1.push_back(_source_file[_i]);
      } else if(_inside_key_arg2){
        _current_instruction_arg2.push_back(_source_file[_i]);
      }
    } else if(_source_file[_i] == '['){
      if(_inside_key_arg1_close){
        _inside_key_arg2 = true;
      } else {
        _inside_key_arg1 = true;
      }
    } else if(_source_file[_i]  == ']'){
      if(_inside_key_arg1_close){
        _inside_key_arg2_close = true;
        _inside_key_arg2       = false;
      } else {
        _inside_key_arg1_close = true;
        _inside_key_arg1       = false;
      }
    } else if(_source_file[_i] == ';'){
      _tokens->push_back({_current_instruction_name, _current_instruction_arg1, _current_instruction_arg2});

      _current_instruction_name.clear();
      _current_instruction_arg1.clear();
      _current_instruction_arg2.clear();

      _inside_key_arg1_close = false;
      _inside_key_arg2_close = false;
    }
  }
}

/*
 *
 * Para verificar se é um registrador válido
 *
 */

bool check_valid_reg(const std::string* _reg) noexcept
{
  for(uint8_t _i { 0 }; _i < REGTABLE_SIZE; _i++)
  {
    if(_reg_names[_i] == *_reg) { return true; }
  }
  return false;
}

int main (int argc, char** argv) noexcept 
{
  /*
   *
   * Pequena verificação para ver se o arquivo foi passado como parâmetro 
   *
   */

  if(argc < 2) 
  { std::cerr << "Specify .ceb source file\n"; exit(SOURCE_FILE_ERROR); }

  /*
   *
   * Carregando o arquivo como apenas leitura
   *
   */

  int _file { open(SOURCE_FILE, O_RDONLY) };
  
  struct stat _file_infos; 

  if(_file == -1)
  { std::cerr << "Error to load source -> " << SOURCE_FILE << '\n'; exit(SOURCE_FILE_ERROR); }

  /*
   *
   * Pegando algumas informações do arquivo, nesse caso apenas o seu tamanho
   *
   */

  if(fstat(_file, &_file_infos) == -1)
  { std::cerr << "Error to get file infos -> " << SOURCE_FILE << '\n'; exit(SOURCE_FILE_ERROR); }
  
  /*
   *
   * Mapeando arquivo na memória para facilitar a manipulação de leitura
   *
   * O _file_mmap_dest é o arquivo já sem os espaços e quebra de linhas
   *
   */

  uint8_t* _file_mmap      { static_cast<uint8_t*>(mmap(nullptr, _file_infos.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, _file, 0)) };
  uint8_t* _file_mmap_dest { static_cast<uint8_t*>(mmap(nullptr, _file_infos.st_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0)) };
  

  /*
   *
   * Um vetor para armazenar cada token
   *
   */

  std::vector<Token> _tokens;
  
  /*
   *
   * Removemos qualquer quebra de linha e espaços no arquivo source usando essa função
   *
   */

  remove_file_spaces(_file_mmap, _file_mmap_dest, _file_infos.st_size); munmap(_file_mmap, _file_infos.st_size);
  
  /*
   *
   * Depois de fazer a limpaza dos espaços e quebra de linhas vamos organizar os tokens
   *
   */

  lexer(_file_mmap_dest, &_tokens, _file_infos.st_size);

  for(uint32_t _i { 0 }; _i < _tokens.size(); _i++)
  {
    /*
     *
     * Primeiro vamos fazer o parsing do nosso token, ou seja, 
     * se ele está seguindo as regras de syntax da linguagem
     *
     */

    _tokens[_i].parsing();

    /*
     *
     * Se tudo estiver OK vamos converter o token para código binário
     *
     */

    _tokens[_i].translate();

    /*
     *
     * Parte para verificar se os tokens estão sendo organizado certinho 
     *
     * std::cout << "===================TOKEN-" << _i << "==================\n";
     * std::cout << "INSTRUCTION : " << _tokens[_i]._name << '\n';
     * std::cout << "ARG1        : " << _tokens[_i]._arg1 << '\n';
     * std::cout << "ARG2        : " << _tokens[_i]._arg2 << '\n';
     * std::cout << "============================================\n";
     *
     */
  }

  munmap(_file_mmap_dest, _file_infos.st_size);

  return 0;
}


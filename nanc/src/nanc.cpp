/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : nanc.cpp                      |
 *    |  SRC MOD   : 31/10/2024                    |
 *    |  VERSION   : 0.0-1                         |
 *    |                                            |
 *    O--------------------------------------------/
 *    
 *
 */

/*
 *
 *    CHANGE LOG 0.0-1:
 *      * Adicionado funções básicas como: Remoção de espaços e quebra de linhas no arquivo e dividindo o arquivo em tokens;
 *      * Adicionado um decoder para saber qual variação da instrução MOV usar;
 *
 *   TO DOS 0.0-1:
 *    * Fazer o decoder para as outras instruçoes;
 *    * Fazer o decoder para as outras instruções que possue diversas variações;
 *    * Traduzir para o binário;
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
#include <iostream>
#include <string>
#include <vector>

#define SOURCE_FILE       argv[1]
#define CANEBLY_EXTENSION "ceb" 

#define OPTABLE_SIZE         0x13
#define REGTABLE_SIZE        0x05
#define INSTRUCTION_VARIABLE 0x03
#define FUNCTIONS_DECODER    0x03

/*
 *
 * Error codes
 *
 */

#define SYNTAX_ERROR      0xFF
#define SOURCE_FILE_ERROR 0xFE

void mov_decoder(std::string* __restrict, const std::string* __restrict, const std::string* __restrict);
void add_decoder(std::string* __restrict, const std::string* __restrict, const std::string* __restrict) {}
void sub_decoder(std::string* __restrict, const std::string* __restrict, const std::string* __restrict) {}

/*
 *
 * Essa parte pode ser bem complexa, talvez até mais que devia, mas foi a
 * forma que eu encontrei para lidar com instruções que podem ter várias 
 * variações, como o MOV
 *
 */

static std::string _instruction_name[OPTABLE_SIZE]
{
  "RST",  "JMP",  "POP",  "PSH",
  "MOV",  "MOV2", "MOV3", "MOV4",
  "MOV5", "PRT",  "BRK",  "ADD",
  "ADD2", "ADD3", "ADD4", "SUB",
  "SUB2", "SUB3", "SUB4"
};

static std::string _reg_name[REGTABLE_SIZE]
{
  "A", "X", "Y", "S", 
  "STK" 
};

static std::string _instructions_with_variable[INSTRUCTION_VARIABLE]
{
  "MOV", "ADD", "SUB"
};

static uint8_t _optable[OPTABLE_SIZE]
{
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12
};

static uint8_t _regtable[REGTABLE_SIZE]
{
  0x00, 0x01, 0x02, 0x03, 
  0x04
};

static void (*_functions_decoder[FUNCTIONS_DECODER])(std::string* __restrict, const std::string* __restrict, const std::string* __restrict)
{
  &mov_decoder, &add_decoder, &sub_decoder
};

void instruction_decoder(std::string* _instruction_name, const std::string* _arg1, const std::string* _arg2) noexcept
{
  /*
   *
   * Definimos com um valor qualquer só para ver se ele foi alterado
   *
   */

  uint8_t _instruction_variable { 10 };

  for(uint8_t _i { 0 }; _i < INSTRUCTION_VARIABLE; _i++)
  {
    if(_instructions_with_variable[_i] == *_instruction_name) { _instruction_variable = _i; break; } 
  }

  if(_instruction_variable == 10) { return; }

  _functions_decoder[_instruction_variable](_instruction_name, _arg1, _arg2);
}

struct Token
{
  /*
   *
   * Isso séria a árvore, aqui vamos guardar o nome e os argumentos do token que o Lexer organizou 
   *
   */

  std::string _name;
  std::string _arg1;
  std::string _arg2;

  void translate() noexcept
  {
    /*
     *
     * Ainda vou fazer :)
     *
     */

    instruction_decoder(&_name, &_arg1, &_arg2);
  }
};

void remove_spaces(uint8_t* __restrict _source_file, uint8_t* __restrict _source_file_dest, const uint32_t _source_file_size) noexcept
{
  /*
   *
   * Essa função serve para tirar qualquer rastro de espaços e quebra de linhas do nosso arquivo fonte
   *
   */

  for(uint32_t _i { 0 }; _i < _source_file_size; _i++)
  {
    if(_source_file[_i] != ' ' && _source_file[_i] != '\n') _source_file_dest[_i] = _source_file[_i];
  }
}

void tokenize(const uint8_t* _source_file, std::vector<Token>* _tokens, const uint32_t _source_file_size) noexcept
{
  /*
   *
   * Aqui fica o nosso lexer, aqui o código é organizado.
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
    if(_reg_name[_i] == *_reg) { return true; }
  }
  return false;
}

/*
 *
 * Funções para verificar qual variação vamos usar, isso serve
 * para apenas algumas instruções
 *
 */

void mov_decoder(std::string* __restrict _instruction_name, const std::string* __restrict _arg1, const std::string* __restrict _arg2)
{
  if(check_valid_reg(_arg1) && !check_valid_reg(_arg2) && (*_arg2)[0] != '*') 
  { *_instruction_name = "MOV1"; }
  else if(check_valid_reg(_arg1) && check_valid_reg(_arg2))
  { *_instruction_name = "MOV2"; }
  else if(check_valid_reg(_arg1) && !check_valid_reg(_arg2) && (*_arg2)[0] == '*')
  { *_instruction_name = "MOV3"; }
  else if(check_valid_reg(_arg2) && !check_valid_reg(_arg1) && (*_arg1)[0] == '*')
  { *_instruction_name = "MOV4"; }
  else
  { std::cerr << "Syntax Error -> " << *_instruction_name << ' ' << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR); }
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

  remove_spaces(_file_mmap, _file_mmap_dest, _file_infos.st_size); munmap(_file_mmap, _file_infos.st_size);
  tokenize(_file_mmap_dest, &_tokens, _file_infos.st_size);

  /*
   *
   * Essa parte que estou usando para ver se tudo foi organizado como esperado
   *
   */ 

  for(uint32_t _i { 0 }; _i < _tokens.size(); _i++)
  {
    _tokens[_i].translate();

    std::cout << "===================TOKEN-" << _i << "==================\n";
    std::cout << "INSTRUCTION : " << _tokens[_i]._name << '\n';
    std::cout << "ARG1        : " << _tokens[_i]._arg1 << '\n';
    std::cout << "ARG2        : " << _tokens[_i]._arg2 << '\n';
    std::cout << "============================================\n";
  }

  munmap(_file_mmap_dest, _file_infos.st_size);

  return 0;
}


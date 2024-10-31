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
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>

#define COMPILER_EXEC     argv[0]
#define SOURCE_FILE       argv[1]
#define CANEBLY_EXTENSION "ceb" 

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

void tokenize(const uint8_t* _source_file, std::vector<Token>& _tokens, const uint32_t _source_file_size) noexcept
{
  /*
   *
   * Aqui fica o nosso lexer, aqui o código é organizado.
   *
   * EXEMPLO:
   *  ARQUIVO FONTE:
   *    MOV [A] [X];
   *    MOV [B] [0x8010];
   *
   * Devemos organizar a instrução e seus argumentos, no final ficará mais ou menos assim:
   *  
   *  INSTRUCTION : MOV
   *  ARG1        : A
   *  ARG2        : X
   *
   *  INSTRUCTION : MOV
   *  ARG1        : B
   *  ARG2        : 0x8010
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

    if(std::isalpha(_source_file[_i]))
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
      _tokens.push_back({_current_instruction_name, _current_instruction_arg1, _current_instruction_arg2});

      _current_instruction_name.clear();
      _current_instruction_arg1.clear();
      _current_instruction_arg2.clear();

      _inside_key_arg1_close = false;
      _inside_key_arg2_close = false;
    }
  }
}

int main (int argc, char** argv) noexcept 
{ 
  /*
   *
   * Carregando o arquivo como apenas leitura
   *
   */

  int _file { open(SOURCE_FILE, O_RDONLY) };
  
  struct stat _file_infos; 

  if(_file == -1)
  {
    std::cerr << COMPILER_EXEC << " : Error to load source -> " << SOURCE_FILE << '\n'; 
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Pegando algumas informações do arquivo, nesse caso apenas o seu tamanho
   *
   */

  if(fstat(_file, &_file_infos) == -1)
  {
    std::cerr << COMPILER_EXEC << " : Error to get file infos -> " << SOURCE_FILE << '\n';
    exit(EXIT_FAILURE);
  }
  
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
  tokenize(_file_mmap_dest, std::ref(_tokens), _file_infos.st_size);

  /*
   *
   * Essa parte que estou usando para ver se tudo foi organizado como esperado
   *
   */ 

  for(uint32_t _i { 0 }; _i < _tokens.size(); _i++)
  {
    std::cout << "===================TOKEN-" << _i << "==================\n";
    std::cout << "INSTRUCTION : " << _tokens[_i]._name << '\n';
    std::cout << "ARG1        : " << _tokens[_i]._arg1 << '\n';
    std::cout << "ARG2        : " << _tokens[_i]._arg2 << '\n';
    std::cout << "============================================\n";
  }

  munmap(_file_mmap_dest, _file_infos.st_size);

  return 0;
}


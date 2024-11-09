/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : nanc.cpp                      |
 *    |  SRC MOD   : 09/11/2024                    |
 *    |  VERSION   : 0.1-0                         |
 *    |                                            |
 *    O--------------------------------------------/
 *    
 *
 */

/*
 *
 * Canembly Lang assembler :)
 *
 */


/*
 * Para essa primeira versão a otimização não vai ser o nosso foco, pretendo deixar nosso assembler
 * funcional antes de qualquer otimização, seja em estruturas de dados ou algoritmos.
 *
 * OTIMIZAÇÂO PRÉMATURA É A RAIZ DE TODO MAL!
 *
 *    CHANGE LOG 0.1-0:
 *      * Adicionado -> Suporte a comentários no código usando os operadores < Isso é um comentário >;
 *      * Adicionado -> Instruções INC e DEC para incremento e decremento de registradores;
 *      * Adicionado -> Instruções CMP e JFZ para controle de fluxo do código
 *      * Melhoria   -> Função lexer totalmente refeita;
 *      * Melhoria   -> Algumas otimizações no código em geral;
 *      * Melhoria   -> Agora as instruções não são mais limitadas a 3 caracteres;
 *      * Correção   -> Problema em formatação de hexadecimais;
 *      * Correçao   -> Problemas com instruções que não precisam do '[]', agora voce pode usar por exemplo BRK; ao invés de BRK [];, mas caso queira usar o '[]' pode usar sem problemas :D;
 *      * Removido   -> Instrução PRT agora foi totalmente removida;
 *
 *    TO DOS 0.1-0:
 *      * Aplicar diversas otimizações e melhorias no código;
 *      * Usar estruturas de dados mais complexas, como hashtable;
 *      * Adicionar mais instruções ao compilador;
 *      * Melhorar manipulações de erro com throw;
 *
 */

/*
 *
 * Esse será nosso assembler, com eles vamos conseguir escrever código de forma mais humana.
 *
 * Fazer um pequeno assembler foi uma ideia desde sempre, programar em binário é extremamente legal
 * porém é muito fácil errar algum 0 ou 1 e perder o programa por completo.
 *
 * Fazer um assembler é um projeto extremamente gratificante, porém possue várias dificuldades, mas a ideia
 * desse projeto é realmente me desafiar e explorar os funcionamentos de cada coisa.
 *
 * O assembler é o montador, ou seja, ele vai traduzir o nosso código para binário.
 *
 * Ele vai ter 3 etapas: 
 *
 * - Lexer      : Nessa parte o código vai ser convertido em tokens. Tokens são as menores unidades significativas 
 *                de um programa (como palavras-chave, identificadores, números, operadores, etc.).
 *
 * - Parsing   : Parte que os tokens serão analisados e organizados em árvores, é aqui onde temos erro de syntax caso 
 *               algo não esteja correto.
 *
 * - Tradução : A maioria dos compiladores traduzem os tokens em assembly, por isso conseguimos usar -O3, -O2 em compiladores
 *              como o GCC e o LLVM, a ideia é reduzir o tamanho do código assembly, e depois de gerar todo o código assembly
 *              vamos usar o assembler para traduzir esse código para instrução de máquina.
 *
 */

#include <cctype>
#include <cstdint>
#include <cstdio>
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
#define BIN_FILE_DEST     "./anc.bin"

#define OPTABLE_SIZE         0x17
#define REGTABLE_SIZE        0x05
#define INSTRUCTIONS         0x0D
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
  "MOV5", "BRK",  "ADD1", "ADD2", 
  "ADD3", "ADD4", "SUB1", "SUB2", 
  "SUB3", "SUB4", "INC",  "DEC",
  "CMP",  "JFZ",  "JFNZ"
};

static std::string _reg_names[REGTABLE_SIZE]
{
  "A", "X", "Y", "S", "STK" 
};

static std::string _source_possible_instructions_names[INSTRUCTIONS]
{
  "RST", "JMP", "POP", "PSH",
  "MOV", "BRK", "ADD", "SUB", 
  "INC", "DEC", "CMP", "JFZ",
  "JFNZ"
};

static void (*_instructions_functions_decoder[INSTRUCTIONS])(std::string* __restrict, const std::string* __restrict, const std::string* __restrict)
{
  &rst_instruction,     &jmp_instruction,     &pop_instruction,     &psh_instruction,
  &mov_instruction,     &brk_instruction,     &add_instruction,     &sub_instruction, 
  &inc_dec_instruction, &inc_dec_instruction, &cmp_dec_instruction, &jfz_dec_instruction,
  &jfnz_dec_instruction
};

void instruction_parsing(std::string* _instruction, const std::string* _arg1, const std::string* _arg2) noexcept
{
  /*
   *
   * Definimos com um valor qualquer só para ver se ele foi alterado
   *
   */

  for(uint8_t _i { 0 }; _i < INSTRUCTIONS; _i++)
  {
    if(_source_possible_instructions_names[_i] == *_instruction) { _instructions_functions_decoder[_i](_instruction, _arg1, _arg2); return; } 
  }
  std::cerr << "Syntax Error -> " << "Invalid instruction -> " << *_instruction << ' ' << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR);
}

void convert_instruction(const std::string* __restrict _instruction, std::string* __restrict _token_bin) noexcept
{
  for(uint8_t _i { 0 }; _i < OPTABLE_SIZE; _i++)
  {
    if(*_instruction == _cpu_possible_instructions_names[_i]) { decimal_in_string_bin(_i, _token_bin); return; }
  }
}

void convert_args(std::string* __restrict _arg, std::string* __restrict _token_bin) noexcept
{
  /*
/   *
   * Essa função vai ser melhorada no futuro, está sendo apenas para testes
   *
   */

  if(_arg->empty()) 
  { return; }

  if(_arg->size() == 1)
  {
    if(check_valid_reg(_arg) != REG_NOT_FOUND)
    { decimal_in_string_bin(check_valid_reg(_arg), _token_bin); return; }
    
    if(std::isalnum((*_arg)[0]))
    { decimal_in_string_bin((*_arg)[0] - '0', _token_bin); return; }
    
    std::cerr << "Syntax Error -> Undefined instruction -> " << '[' << *_arg << ']' << '\n'; exit(SYNTAX_ERROR);
  }
  
  uint32_t _i        { 0 };
  bool     _is_addrs { false };

  if((*_arg)[_i] == '*')
  { ++_i; _is_addrs = true; }

  if(std::isalnum((*_arg)[_i]))
  {
    if((*_arg)[_i] == '0' && std::tolower((*_arg)[_i + 1]) == 'x')
    {
      _arg->erase(0, _is_addrs ? 3 : 2);
       
      if(_arg->size() < 4) { std::cout << "Syntax Error -> hexa nums is Undefined -> 0x" << *_arg << " use this format -> \"0x0000\"\n"; exit(SYNTAX_ERROR); }

      /*
       *
       * Convertendo hexadecimal em decimal
       *
       * Primeiro pegamos o high byte para converter e depois o lower byte
       *
       */
       
      decimal_in_string_bin(std::stoi(_arg->substr(0, 2), nullptr, 16), _token_bin);
      decimal_in_string_bin(std::stoi(_arg->substr(2, 2), nullptr, 16), _token_bin);
      
      } else {
      
      _arg->erase(0, _is_addrs ? 1 : 0);
       
      /*
       *
       * Convertendo decimal string para decimal
       *
       */
       
      decimal_in_string_bin(std::stoi(*_arg), _token_bin);
    }
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
    convert_args(&_arg1, &_bin);
    convert_args(&_arg2, &_bin);
  }
};

void lexer(const std::string* _source_file, std::vector<Token>* _tokens) noexcept
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

  std::string _current_instruction;
  std::string _current_instruction_arg1;
  std::string _current_instruction_arg2;

  uint64_t _instruction_index { 0 };

  bool _inside_key_arg1_close { false };
  bool _inside_key_arg2_close { false };

  for(uint32_t _i { 0 }; _i < _source_file->size(); _i++)
  {

    /*
     *
     * Nessa parte estamos divindo o que é instrução e o que são os argumetos, onde se inicia o 1 argumento onde acaba,
     * onde inicia o 2 arumentos e onde acaba, onde começa e termina uma instrução, também pretendo adicionar mais funções
     * nessa parte >:)
     *
     */

    if(std::isalpha((*_source_file)[_i]))
    {
      while(std::isalpha((*_source_file)[_i])) 
      {
        _current_instruction.push_back((*_source_file)[_i++]);
      }

      define_arg:
      
      if((*_source_file)[_i] == '[')
      {

        /*
         *
         * Selecionando Argumento
         *
         * Esse é o poder dos ponteiros >:D
         *
         */

        std::string* _arg_selector       { !_inside_key_arg1_close ? &_current_instruction_arg1 : &_current_instruction_arg2 };
        bool*        _arg_close_selector { !_inside_key_arg1_close ? &_inside_key_arg1_close : &_inside_key_arg2_close };
        
        /*
         *
         * Pulando o '['
         *
         */

        ++_i;

        while((*_source_file)[_i] != ']')
        {
          _arg_selector->push_back((*_source_file)[_i]);
          ++_i;
        }

        /*
         *
         * Pulando o caractere ']'
         *
         */

        ++_i;

        /*
         *
         * Fechando o argumnto
         *
         */
        
        *_arg_close_selector = true;
        
        /*
         * 
         * Coloquei esse goto que não é muito recomendado usar porém aqui vai funcionar muito bem
         *
         * Ele vai ser muito útil caso eu queira adicionar mais parâmetros para as instruções
         *
         */
        
        goto define_arg;
      }

      if((*_source_file)[_i] == ';')
      {
        _tokens->push_back({_current_instruction, _current_instruction_arg1, _current_instruction_arg2, ""}); 
        
        _inside_key_arg1_close = false;
         _inside_key_arg2_close = false;

        _current_instruction.clear();
        _current_instruction_arg1.clear();
        _current_instruction_arg2.clear(); 

        ++_instruction_index;
      } else {
        std::cout << "Syntax Error -> operator ';' not found! -> line " << _instruction_index << '\n'; exit(SYNTAX_ERROR);
      }
    }
  }
}

/*
 *
 * Para verificar se é um registrador válido
 *
 */

int8_t check_valid_reg(const std::string* _reg) noexcept
{
  for(uint8_t _i { 0 }; _i < REGTABLE_SIZE; _i++)
  {
    if(_reg_names[_i] == *_reg) { return _i; }
  }
  return REG_NOT_FOUND;
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
   * Criando um arquivo que vai ser o binário
   *
   */

  int _bin_file { open(BIN_FILE_DEST, O_TRUNC | O_CREAT | O_RDWR, S_IWUSR | S_IRUSR) };

  if(_bin_file == -1)
  { std::cerr << "Error to create bin file -> " << BIN_FILE_DEST << '\n'; close(_file); exit(SOURCE_FILE_ERROR); }
  
  /*
   *
   * Mapeando arquivo na memória para facilitar a manipulação de leitura
   *
   * O _file_pure é o arquivo já sem os espaços e quebra de linhas
   *
   */

  uint8_t*     _file_mmap             { static_cast<uint8_t*>(mmap(nullptr, _file_infos.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, _file, 0)) };
  std::string* _file_without_segments { nullptr };

  /*
   *
   * Como mapeamos o arquivo na memória podemos fecha-lo
   *
   */

  close(_file);

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

  remove_source_segments(_file_mmap, _file_infos.st_size, _file_without_segments);

  /*
   *
   * Como já tiramos todos os espaços do arquivo podemos liberar essa memória 
   *
   */

  munmap(_file_mmap, _file_infos.st_size);
  
  /*
   *
   * Depois de fazer a limpaza dos espaços e quebra de linhas vamos organizar os tokens
   *
   */

  lexer(_file_without_segments, &_tokens);

  for(uint64_t _i { 0 }; _i < _tokens.size(); _i++)
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
     * Adicionando uma quebra de linha no final do arquivo binário
     *
     * OBS : Isso é só para arrumar um bug
     *
     */

    if(_i + 1 == _tokens.size()) { _tokens[_i]._bin.push_back('\n'); }

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

    if(write(_bin_file, _tokens[_i]._bin.c_str(), _tokens[_i]._bin.size()) == -1)
    { 
      std::cerr << "Error to write in binary file -> " << BIN_FILE_DEST << '\n'; 
      
      close(_bin_file); 
      
      remove(BIN_FILE_DEST);

      break;
    }
  }

  delete _file_without_segments;

  return 0;
}


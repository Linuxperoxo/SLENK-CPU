/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : instructions_parsing.hpp      |
 *    |  SRC MOD   : 03/11/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#ifndef __PARSING_HPP__
#define __PARSING_HPP__

#define REG_NOT_FOUND -1
#define SYNTAX_ERROR  0xFF

#include <iostream>
#include <string>
#include <cstdint>

int8_t check_valid_reg(const std::string* _reg) noexcept;

/*
 *
 * Essa função eu uso para funções que não precisam de parâmetros extras, como RST que é uma instrução de reinicialização,
 * não faz sentido essa instrução ter parâmetros
 *
 */

inline void instruction_with_empty_args(const std::string* __restrict _instruction, 
                                        const std::string* __restrict _arg1, 
                                        const std::string* __restrict _arg2) noexcept
{
  if(_arg1->empty() && _arg2->empty()) { return; }
  
  std::cerr << "Syntax Error -> " 
            << *_instruction 
            << " Invalid args -> " 
            << '[' << *_arg1 << ']' 
            << ' ' 
            << '[' << *_arg2 << ']' 
            << '\n';
}

/*
 *
 * Parsing para cada instrução
 *
 * OBS : Algumas não precisam de parsing já que não usam
 *       nenhum parâmetro
 *
 */

inline void rst_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  instruction_with_empty_args(_instruction, _arg1, _arg2);
}

inline void jmp_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  if((*_arg1)[0] == '*' && _arg2->empty()) { return; }   
  std::cerr << "Syntax Error -> " << *_instruction << " Invalid args -> " << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR);
}

inline void pop_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  instruction_with_empty_args(_instruction, _arg1, _arg2);
}

inline void psh_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  instruction_with_empty_args(_instruction, _arg1, _arg2);
}

inline void mov_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  if(check_valid_reg(_arg1) != REG_NOT_FOUND && check_valid_reg(_arg2) == REG_NOT_FOUND && (*_arg2)[0] != '*' && !_arg2->empty()) 
  { *_instruction = "MOV1"; }
  else if(check_valid_reg(_arg1) != REG_NOT_FOUND && check_valid_reg(_arg2) != REG_NOT_FOUND)
  { *_instruction = "MOV2"; }
  else if(check_valid_reg(_arg1) != REG_NOT_FOUND && check_valid_reg(_arg2) == REG_NOT_FOUND && (*_arg2)[0] == '*')
  { *_instruction = "MOV3"; }
  else if(check_valid_reg(_arg2) != REG_NOT_FOUND && check_valid_reg(_arg1) == REG_NOT_FOUND && (*_arg1)[0] == '*')
  { *_instruction = "MOV4"; }
  else if(check_valid_reg(_arg2) == REG_NOT_FOUND && (*_arg2)[0] != '*' && check_valid_reg(_arg1) == REG_NOT_FOUND && (*_arg1)[0] == '*')
  { *_instruction = "MOV5"; }
  else
  { std::cerr << "Syntax Error -> " << *_instruction << " Invalid args -> " << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR); }
}

inline void brk_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  instruction_with_empty_args(_instruction, _arg1, _arg2);
}

inline void add_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  if(check_valid_reg(_arg1) == REG_NOT_FOUND && (*_arg1)[0] != '*' && _arg2->empty())
  { *_instruction = "ADD1"; }
  else if(check_valid_reg(_arg1) != REG_NOT_FOUND && _arg2->empty())
  { *_instruction = "ADD2"; }
  else if(check_valid_reg(_arg1) == REG_NOT_FOUND && (*_arg1)[0] != '*' && check_valid_reg(_arg2) == REG_NOT_FOUND && (*_arg2)[0] != '*')
  { *_instruction = "ADD3"; }
  else if(check_valid_reg(_arg1) != REG_NOT_FOUND && check_valid_reg(_arg2) != REG_NOT_FOUND)
  { *_instruction = "ADD4"; }
  else
  { std::cerr << "Syntax Error -> " << *_instruction << " Invalid args -> " << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR); }
}

inline void sub_instruction(std::string* __restrict _instruction, 
                            const std::string* __restrict _arg1, 
                            const std::string* __restrict _arg2) noexcept
{
  if(check_valid_reg(_arg1) == REG_NOT_FOUND && (*_arg1)[0] != '*' && _arg2->empty())
  { *_instruction = "SUB1"; }
  else if(check_valid_reg(_arg1) != REG_NOT_FOUND && _arg2->empty())
  { *_instruction = "SUB2"; }
  else if(check_valid_reg(_arg1) == REG_NOT_FOUND && (*_arg1)[0] != '*' && check_valid_reg(_arg2) == REG_NOT_FOUND && (*_arg2)[0] != '*')
  { *_instruction = "SUB3"; }
  else if(check_valid_reg(_arg1) != REG_NOT_FOUND && check_valid_reg(_arg1) != REG_NOT_FOUND)
  { *_instruction = "SUB4"; }
  else
  { std::cerr << "Syntax Error -> " << *_instruction << " Invalid args -> " << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR); }
}

inline void inc_dec_instruction(std::string *__restrict _instruction, 
                            const std::string *__restrict _arg1, 
                            const std::string *__restrict _arg2) noexcept
{
  if(check_valid_reg(_arg1) != REG_NOT_FOUND && _arg2->empty())
  { return; }

  std::cerr << "Syntax Error -> " << *_instruction << " Invalid args -> " << '[' << *_arg1 << ']' << ' ' << '[' << *_arg2 << ']' << '\n'; exit(SYNTAX_ERROR);
}

#endif


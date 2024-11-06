/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : utils.hpp                     |
 *    |  SRC MOD   : 05/11/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#ifndef __UTILS_HPP___
#define __UTILS_HPP___

#include <cstdint>
#include <string>
#include <iostream>
#include <sys/mman.h>

inline void decimal_in_string_bin(const uint8_t _byte, std::string* _dest) noexcept
{
  for(uint8_t _i { 8 }; _i >= 1; _i--)
  {
    _dest->push_back((_byte >> (_i - 1) & 1) + '0');
  }
}

inline void remove_source_segments(const uint8_t* __restrict _file, const uint32_t _file_size, std::string*& _dest) noexcept
{
  
  /*
   *
   * Vamos ignorar os coméntarios e qualquer espaço ou quebra de linha que tenha no arquivo source,
   * ou seja, armazenar somente os caracteres, e vamos colocar tudo em um buffer e mandar para um
   * ponteiro
   *
   */

  std::string* _buffer_without_segments { new std::string() };

  for(uint64_t _i { 0 }; _i < _file_size; _i++)
  {
    if(_file[_i] == '<') 
    { 
      while(_file[_i++] != '>')
      {
        if(_i + 1 == _file_size)
        { std::cerr << "Syntax Error -> " << "comment scope was not closed!\n"; delete _buffer_without_segments; exit(EXIT_FAILURE); }
      } 
    }
    
    if(_file[_i] == '>') { std::cerr << "Syntax Error -> " << "comment scope was closed but at no point was it opened!\n"; exit(EXIT_FAILURE); }

    if(_file[_i] != ' ' && _file[_i] != '\n') { _buffer_without_segments->push_back(_file[_i]); }
  }
  _dest = _buffer_without_segments;
}

#endif

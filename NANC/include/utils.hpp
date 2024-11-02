/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : utils.hpp                     |
 *    |  SRC MOD   : 02/11/2024                    |
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

inline void decimal_in_string_bin(const uint8_t _byte, std::string* _dest) noexcept
{
  for(uint8_t _i { 8 }; _i >= 1; _i--)
  {
    _dest->push_back((_byte >> (_i - 1) & 1) + '0');
  }
}

inline void remove_source_segments(const uint8_t* __restrict _file, uint8_t* __restrict _file_dest, const uint32_t _file_size) noexcept
{
  bool _inside_comment_tuple { false };

  for(uint32_t _i { 0 }; _i < _file_size; _i++)
  {
    if(_file[_i] == '<') 
    { 
      if(_inside_comment_tuple)
      { break; }

      _inside_comment_tuple = true; 
    }
    
    if(_file[_i] == '>')
    { 
      if(!_inside_comment_tuple)
      { std::cerr << "Syntax Error -> " << "comment scope was closed but at no point was it opened!\n"; exit(EXIT_FAILURE); }
      
      _inside_comment_tuple = false; ++_i; 
    }
    
    if(_file[_i] != ' ' && _file[_i] != '\n' && !_inside_comment_tuple) { _file_dest[_i] = _file[_i]; }
  }
  
  if(_inside_comment_tuple)
  { std::cerr << "Syntax Error -> " << "comment scope was not closed!\n"; exit(EXIT_FAILURE); }
}

#endif

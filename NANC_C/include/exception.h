#ifndef __EXCEPTION__
#define __EXCEPTION__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct exception
{
  char* _bin_name;
  char* _error;
  char* _file_error;
}exception;

__attribute__((noreturn, always_inline)) inline void throw_exception(exception* _exception)
{
  printf("%s : %s %s\n", _exception->_bin_name, _exception->_error, _exception->_file_error != NULL ? _exception->_file_error : "");
  
  free(_exception->_bin_name);
  free(_exception->_error);
  free(_exception->_file_error);
  free(_exception);

  exit(EXIT_FAILURE);
}

exception* create_exception(const char*,
                            const char*,
                            const char*);

#endif

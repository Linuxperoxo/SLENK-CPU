#ifndef __EXCEPTION__
#define __EXCEPTION__

#define INTERNAL_ERROR_CODE 22
#define ALLOCATIONS 3

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "types.h" 

typedef struct exception
{
  char* __bin_name;
  char* __error;
  char* __file_error;
}exception;

__attribute__((noreturn, always_inline)) inline void throw_exception(exception* __exception__)
{
  printf("%s : %s %s\n", __exception__->__bin_name, __exception__->__error, __exception__->__file_error != NULL ? __exception__->__file_error : "");
  
  free(__exception__->__bin_name);
  free(__exception__->__error);
  free(__exception__->__file_error);
  free(__exception__);

  exit(EXIT_FAILURE);
}

inline __attribute__((always_inline)) exception* create_exception(const char* __bin_name__,
                                                                  const char* __error__,
                                                                  const char* __file__)
{
  exception* __exception = (exception*)malloc(sizeof(exception));

  if(__exception == NULL)
  {
    INTERNAL_ERROR:
    printf("INTERNAL FAIL: MALLOC\n");
    exit(INTERNAL_ERROR_CODE);
  }
  
  char** __members[ALLOCATIONS] = 
  {
    &__exception->__bin_name, 
    &__exception->__error, 
    &__exception->__file_error
  };

  const char* __str_to_cpy[ALLOCATIONS] = 
  {
    __bin_name__,
    __error__,
    __file__
  };

  for(__i8 _i = 0; _i < ALLOCATIONS; ++_i)
  {
    if(__str_to_cpy[_i] != NULL)
    {
      *__members[_i] = (char*)malloc(strlen(__bin_name__));
      
      if(*__members[_i] == NULL)
      {
        goto INTERNAL_ERROR;  
      }
      strcpy(*__members[_i], __str_to_cpy[_i]);
    }
  }
  return __exception;
}

#endif

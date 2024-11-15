#include "../include/exception.h"

#define MEMBERS_ALLOC 3
#define INTERNAL 22

exception* create_exception(const char* _bin_name,
                            const char* _error,
                            const char* _file_error)
{
  exception* _exception = (exception*)malloc(sizeof(exception));

  if(_exception == NULL)
  {
    INTERNAL_ERROR:
    printf("INTERNAL FAIL: MALLOC\n");
    exit(INTERNAL);
  }

  char** _members[MEMBERS_ALLOC] = 
  {
    &_exception->_bin_name, 
    &_exception->_error, 
    &_exception->_file_error
  };

  const char* _str_to_cpy[MEMBERS_ALLOC] = 
  {
    _bin_name,
    _error,
    _file_error
  };

  for(int8_t _i = 0; _i < MEMBERS_ALLOC; ++_i)
  {
    if(_str_to_cpy[_i] != NULL)
    {
      *_members[_i] = (char*)malloc(strlen(_bin_name));
      
      if(*_members[_i] == NULL)
      {
        goto INTERNAL_ERROR;  
      }
      strcpy(*_members[_i], _str_to_cpy[_i]);
    }
  }
  return _exception;
}

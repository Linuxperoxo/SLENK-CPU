#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../include/exception.h"

int main(int _argc, char** _argv)
{
  
  /*
   *
   * Verificando se um arquivo foi passado para compilação
   *
   */

  if(_argc < 2)
    throw_exception(create_exception(_argv[0], "no input file", NULL));

  /*
   *
   * Abrindo o arquivo source
   *
   */

  int8_t _org_source_file = open(_argv[1], O_RDONLY);

  /*
   *
   * Verificando se o arquivo foi aberto com sucesso
   *
   */

  if(_org_source_file == -1)
    throw_exception(create_exception(_argv[0], "file not found ->", _argv[1]));
  
  /*
   *
   * Pegando informações do arquivo 
   *
   */

  struct stat* _source_stat = (struct stat*)malloc(sizeof(struct stat));
  fstat(_org_source_file, _source_stat);

  /*
   *
   * Mapeando arquivo na memória para facilitar a manipulação
   *
   */

  uint8_t* _source_file_map = (uint8_t*)mmap(NULL, _source_stat->st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, _org_source_file, 0);

  if(_source_file_map == MAP_FAILED)
    throw_exception(create_exception(_argv[0], "failed to map file ->", _argv[1]));

  printf("%s", _source_file_map);

  return 0;
}

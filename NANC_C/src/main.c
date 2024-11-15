#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../include/exception.h"

int main(int __argc__, char** __argv__)
{
  
  /*
   *
   * Verificando se um arquivo foi passado para compilação
   *
   */

  if(__argc__ < 2)
    throw_exception(create_exception(__argv__[0], "no input file", NULL));

  /*
   *
   * Abrindo o arquivo source
   *
   */

  int8_t __org_source_file = open(__argv__[1], O_RDONLY);

  /*
   *
   * Verificando se o arquivo foi aberto com sucesso
   *
   */

  if(__org_source_file == -1)
    throw_exception(create_exception(__argv__[0], "file not found ->", __argv__[1]));
  
  /*
   *
   * Pegando informações do arquivo 
   *
   */

  struct stat* __source_stat = (struct stat*)malloc(sizeof(struct stat));
  fstat(__org_source_file, __source_stat);

  /*
   *
   * Mapeando arquivo na memória para facilitar a manipulação
   *
   */

  uint8_t* __source_file_map = (uint8_t*)mmap(NULL, __source_stat->st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, __org_source_file, 0);

  if(__source_file_map == MAP_FAILED)
    throw_exception(create_exception(__argv__[0], "failed to map file ->", __argv__[1]));

  printf("%s", __source_file_map);

  return 0;
}

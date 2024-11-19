/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : file.h                        |
 *    |  SRC MOD   : 15/11/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *    
 *
 */

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "types.h"



inline __attribute__((always_inline)) __u8* rm_file_seg(const __u8* __src__, __u64 __src_size__, __u64* __dest_size__)
{
  if(__src__ == MAP_FAILED)
     return (__u8*)MAP_FAILED;

  __u8* __dest = NULL;
  __u64 __push = 0;
  __u64 __dest_size = 0;

  /*
   *
   * Essa parte não é tão necessária, porém queria consumir menos memória
   *
   */

  for(__u64 __i = 0; __i < __src_size__; ++__i)
  {
    if(__src__[__i] != ' ' && __src__[__i] != '\n')
      ++__dest_size;
  }

  /*
   *
   * A flag MAP_PRIVATE significa que as alterações feitas na memória mapeada 
   * não serão refletidas no arquivo subjacente (se houver) e não serão vistas 
   * por outros processos que mapearem o mesmo arquivo.
   *
   * Qualquer modificação feita no bloco de memória será privada para o processo 
   * e não afetará o conteúdo original do arquivo, funcionando como uma cópia sob 
   * demanda (copy-on-write).
   *
   * A flag MAP_ANON (ou MAP_ANONYMOUS, que é a nomenclatura mais comum) indica 
   * que a região de memória mapeada não estará associada a nenhum arquivo. 
   * Em vez disso, ela será inicializada com zeros.
   * 
   * Ao usar MAP_ANONYMOUS, você não precisa (e não deve) fornecer um descritor 
   * de arquivo (o argumento fd deve ser definido como -1).
   *
   */

  __dest = (__u8*)mmap(NULL, __dest_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

  if(__dest == MAP_FAILED)
    return (__u8*)MAP_FAILED;
  
  /*
   *
   * Tirando todos os segmentos que não queremos 
   *
   */

  for(__u64 __i = 0; __i < __src_size__; ++__i)
  {
    if(__src__[__i] != ' ' && __src__[__i] != '\n')
      __dest[__push++] = __src__[__i];
  }
  *__dest_size__ = __dest_size; 
  return __dest;
}

/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : utils.h                       |
 *    |  SRC MOD   : 15/11/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *    
 *
 */

#ifndef __UTILS__
#define __UTILS__

#define ASCII_NUM_INIT 48
#define ASCII_NUM_END 58
#define IS_NOT_NUM 0
#define IS_NUM

#define ASCII_ALP_UP_INIT 65
#define ASCII_ALP_UP_END 90
#define ASCII_ALP_LOW_INIT 97
#define ASCII_ALP_LOW_END 122

#define IS_ALP_LOW 2
#define IS_ALP_UP 1
#define IS_NOT_ALP 0

#define HEX16 4

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifndef __TYPES__
#define __TYPES__
typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef int8_t __i8;
typedef int16_t __i16;
typedef int32_t __i32;
typedef int64_t __i64;
#endif

/*
 *
 * Função verifica se um caractere é um número
 *
 */

inline __attribute__((always_inline)) __u8 isnum(char __char__)
{
  return __char__ >= ASCII_NUM_INIT && __char__ <= ASCII_NUM_END ? IS_NUM : IS_NOT_NUM; 
}

/*
 *
 * Função que verifica se um caractere é uma letra do alfabeto
 *
 * retorna 1 se for uma letra upper e retorna 2 se for uma letra lower
 *
 */

inline __attribute__((always_inline)) __u8 isalp(char __char__)
{
  return __char__ >= ASCII_ALP_UP_INIT && __char__ <= ASCII_ALP_UP_END ? IS_ALP_UP : __char__ >= ASCII_ALP_LOW_INIT && __char__ <= ASCII_ALP_LOW_END ? IS_ALP_LOW : IS_NOT_ALP; 
}

/*
 *
 * Função que converte hexadecimal de 16 bits em decimal
 *
 */

inline __attribute__((always_inline)) __u16 hex16dec(const char* __str_hex__)
{
  
  /*
   *
   * Verificando se o tamanho é válido
   *
   */

  if(strlen(__str_hex__) < 4 || strlen(__str_hex__) > 4)
    return -1;

  __u16 __hex = 0;

  for(__u8 __i = HEX16; __i > 1; __i--)
  {
    /*
     *
     * Primeiro convertemos o char para decimal e depois movemos os bits
     *
     */

    __hex |= (__str_hex__[HEX16 - __i] - '0') << (HEX16 * __i - HEX16);
  }
  return __hex;
}

/*
 *
 * Essa função imprime cada bit de um byte
 *
 * OBS : Fiz apenas para testar algumas coisas, como ela foi útil para mim
 *       pode ser para outra pessoa também :)
 *
 */

inline __attribute__((always_inline)) void byte(__u16 __byte__)
{
  for(__u8 __i = 8; __i >= 1; __i--)
  {

    /*
     *
     * Aqui vamos dando espaço a cada 4 bits 
     *
     */

    printf("%s", __i != 8 && __i % 4 == 0 ? " " : "");

    /*
     *
     * Aqui vamos movendo o bit até chegar no bit menos significativo,
     * e depois comparamos ele como o decimal 1, que em binário é 0001
     *
     * EXEMPLO : 
     *  
     *  Queremos ver se o 3 bit de um decimal está habilitado
     *
     *  0100 (8 em decimal)
     *
     *  quero ver se o bit 3 está habilitado, como posso fazer isso?
     *
     *  0001 (1 em decimal)
     *
     *  Podemos usar AND. O AND funciona da seguinte maneira:
     *  
     *  +-----------+
     *  |  1  |  1  | = 1
     *  |  1  |  0  | = 0
     *  |  0  |  1  | = 0
     *  |  0  |  0  | = 0
     *  +-----------+
     *
     *  Usamos shift para mover o bit que queremos pegar, conte sempre começando do 0 
     *  e da esquerda para a direita
     *
     *  0 1 0 0
     *  -------
     *  3 2 1 0
     *
     *  Quero ver se o bit 2 está ativo
     *
     *  8 >> 2 & 1
     *
     *  Estamos simplesmente fazendo o seguinte
     *
     *  8 >> 1 = 0010
     *  8 >> 2 = 0001
     *   
     *  O nosso 8 virou 1, usarmos o AND agora 
     *
     *   0001
     * & 0001
     * --------
     *   0001
     *
     * Retornou 1
     *
     * Agora quero ver se o bit 3 está habilitado
     *
     * 8 >> 1 = 0010
     * 8 >> 2 = 0001
     * 8 >> 3 = 0000
     *
     *   0000
     * & 0001
     * --------
     *   0000
     *
     * Retornou 0
     *
     * Explicar isso é bem difícil, a melhor forma de você entender é testando você mesmo :D
     *
     */

    printf("%d", (__byte__ >> (__i - 1)) & 1);
  }
  printf("\n");
}

#endif

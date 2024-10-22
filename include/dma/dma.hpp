/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : dma.hpp                       |
 *    |  SRC MOD   : 22/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

/*
 *
 * O DMA Direct Memory Acess é responsável por deixar um device I/O acessar a memória diretamente,
 * isso é muito útil para livrar o processador de ficar fazendo esse trabalho, imagina só ficar usando 
 * o processador para escrever uma quantidade de dados enorme da ram para um disco, isso é extremamente
 * inviável, então o DMA se encarrega de fazer isso enquanto o processador está livre para fazer outra coisa
 * 
 * OBS : Enquanto o DMA está sendo usado o processador não poderá usar o barramento
 *
 */

#ifndef __DMA_HPP__
#define __DMA_HPP__

#include <cstdint>

class BUS;

struct DMA
{
private:

  DMA(const DMA&)            = delete;
  DMA(DMA&&)                 = delete;
  DMA& operator=(const DMA&) = delete;
  DMA& operator=(DMA&&)      = delete;

private:

  /*
   *
   * Aqui fica as informações que o DMA vai usar
   *
   * _first_addrs   : Primeiro endereço de onde os dados vão ser escrito ou lidos;
   *
   * _type          : Tipo da operação R/W;
   * 
   * _data          : Se o tipo de operação for leitura, aqui vai ficar armazenado o byte lido, se for escrita
   *                  aqui vai ficar o valor a ser escrito no byte.
   *                  
   */

  uint16_t _first_addrs   { 0 };
  uint8_t  _type          { 0 };
  uint8_t  _data          { 0 };    

  void    DMA_C(uint16_t, uint8_t, uint8_t) noexcept; // Configura o DMA

public:

  explicit DMA() noexcept = default;
  
  ~DMA() noexcept = default;
  
  friend class BUS;
};

#endif


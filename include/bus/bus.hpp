/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.hpp                       |
 *    |  SRC MOD   : 21/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

/*
 *
 * O bus é responsável por enviar informações do processador para diferentes 
 * dispositivos conectados
 *
 * O system bus é constituido por 3 partes
 *
 *
 *          SYSTEM BUS
 * +---------------------------+
 * | +-----------------------+ |
 * | |                       | |
 * | |      Control Bus      | | -> Responsável por indicar a operação R/W
 * | |                       | |   
 * | +-----------------------+ |
 * |                           |
 * | +-----------------------+ |
 * | |                       | |
 * | |      Address Bus      | | -> Indica qual dispositivo ou posição da memória           
 * | |                       | |
 * | +-----------------------+ |
 * |                           |
 * | +-----------------------+ |
 * | |                       | |
 * | |       Data Bus        | | -> Se for um tipo de escrita (W) ele vai ter o dado que será escrito
 * | |                       | |
 * | +-----------------------+ |
 * +---------------------------+
 *
 */


#ifndef __BUS_HPP__
#define __BUS_HPP__

#include <cstdint>

#include "../cpu/cpu.hpp"
#include "../ram/ram.hpp"
#include "../dma/dma.hpp"
#include "../display/display.hpp"

class BUS
{
private: 

  BUS(const BUS&)            = delete;
  BUS(BUS&&)                 = delete;
  BUS& operator=(const BUS&) = delete;
  BUS& operator=(BUS&&)      = delete;

private:

  /*
    
    Dispositivos conectados ao barramento
    
  */

  CPU* _CPU;
  RAM* _RAM;
  DISPLAY* _DISPLAY;

public:

  explicit BUS(CPU* _cpu, RAM* _ram) noexcept;

  ~BUS() noexcept = default;
 
private:
  
  /*
   *  
   *  Typedef
   *
   * uint16_t -> uint16_t
   * uint8_t  -> uint8_t
   * void            -> void
   *
   */

  /*
   * 
   *  @info   : Função que vai lé um determinado bloco na memória RAM
   *
   *  @return : uint8_t -> Esse processador é de 8 bits, por isso como 
   *            retorno ele retorna um uint8_t, porém, ele usa endereçamento 
   *            de 16 bits
   *
   *  @param  : Um uint16_t que será usado como endereço na memória
   *
   */

  uint8_t read(uint16_t _addrs_to_read) noexcept;

  /*
   *
   *  @info   : Função que vai escrever um dado em um determinado bloco na
   *            memória RAM
   *
   *  @return : Void
   *
   *  @param  : recebe um bloco que será escrito e o dato que será escrito no
   *            bloco
   *
   */

  void write(uint16_t _addrs_to_write, uint8_t _data_to_write) noexcept; 

  friend class CPU;
  friend class DMA;
};

#endif


/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.hpp                       |
 *    |  SRC MOD   : 20/10/2024                    |
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

typedef uint16_t ADDRS_BITS_SIZE;
typedef uint8_t  DATA_BITS_SIZE;
typedef void     NONE;

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

public:

  explicit BUS(CPU* _cpu, RAM* _ram) noexcept;

  ~BUS() noexcept = default;
 
private:
  
  /*
   *  
   *  Typedef
   *
   * ADDRS_BITS_SIZE -> uint16_t
   * DATA_BITS_SIZE  -> uint8_t
   * NONE            -> void
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

  DATA_BITS_SIZE read(ADDRS_BITS_SIZE _addrs_to_read) noexcept;

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

  NONE write(ADDRS_BITS_SIZE _addrs_to_write, DATA_BITS_SIZE _data_to_write) noexcept; 

public:

  /*
   *
   * Aqui é 2 funções simples para o DMA
   *
   */ 
  
  inline DATA_BITS_SIZE DNA_R(ADDRS_BITS_SIZE _addrs_to_read) noexcept { return read(_addrs_to_read); }
  inline NONE DNA_W(ADDRS_BITS_SIZE _addrs_to_write, DATA_BITS_SIZE _data_to_write) noexcept { write(_addrs_to_write, _data_to_write); }

  friend class CPU;
};

#endif


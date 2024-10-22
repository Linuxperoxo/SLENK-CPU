/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.hpp                       |
 *    |  SRC MOD   : 22/10/2024                    |
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

  CPU*     _CPU;
  RAM*     _RAM;
  DMA*     _DMA;

public:

  explicit BUS(CPU*, RAM*, DMA*) noexcept;

  ~BUS() noexcept = default;
 
private:
  
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

  uint8_t read(uint16_t) noexcept;

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

  void write(uint16_t, uint8_t) noexcept; 

  /*
   *
   * DMA FUNCTIONS:
   *
   * Aqui eu fiz todo esse jogo de chamada de funções para simular da forma mais real possível, caso
   * o desempenho fique muito ruim vou melhorar isso.
   *
   * Deixei todos inline para diminuir ao máximo o overhead das chamadas, porém o compilador pode ignorar
   * esses inlines
   *
   */
  
  /*
   *
   * A função _CPU->DMA_interruption() vai chamar essa configure_DMA(), fiz isso para deixar o mais parecido possível, por
   * mais que fique muito confuso e desnecessário.
   *
   */ 

  inline void    configure_DMA(uint16_t _first_addrs, uint8_t _type, uint8_t _data) noexcept { _DMA->DMA_C(_first_addrs, _type, _data); }

public: 

  /*
   *
   * Enviamos a interrupção para o processador configurar o DMA
   *
   */

  inline uint8_t    cpu_interrupt_DMA(uint16_t _first_addrs, uint8_t _type, uint8_t _data) noexcept { return _CPU->DMA_interruption(_first_addrs, _type, _data); }
  
private:

  /*
   *
   * E aqui fica o tipo da operação R/W
   *
   */

  inline void    DMA_W(uint16_t _addrs_to_write, uint8_t _data) noexcept { write(_addrs_to_write, _data); }
  inline uint8_t DMA_R(uint16_t _addrs_to_read) noexcept                 { return read(_addrs_to_read); }
  
public:

  /*
   *
   * Essa função vai ser simplesmente para saber so o programa da ROM ainda está rodando,
   * como não existe drivers para fazer esse trabalho de ver registrador de status do processador 
   * vou fazer da forma mais simples possível
   *
   */

  inline uint8_t CPU_running() noexcept { return _CPU->_B; };

  friend class CPU;
  friend class DMA;
};

#endif


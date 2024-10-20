/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : bus.cpp                       |
 *    |  SRC MOD   : 20/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

/*
 *
 * Aqui vamos ter nosso primeiro dispositivo I/O, um simples display. 
 *
 * Ele vai ser um display bem simples, vamos usar um framebuffer que 
 * vai ser lido pelo DMA.
 *
 * FRAMEBUFFER : Framebuffer é um bloco de memória responsável por guardar
 * informações do próximo frame, então o display consegue acessar esse bloco, 
 * normalmente pelo DMA, e mostrar essa informação na tela.
 *
 */

#ifndef __DISPLAY_HPP__
#define __DISPLAY_HPP__

#include <cstdint>

#define DISPLAY_FRAMEBUFFER_ADDRS 0x6000
#define FRAMEBUFFER_SIZE 0x1000

#define DISPLAY_LOG // Informações que eu vou usar no desenvolvimento, deixarei comentado por padrão 

constexpr uint16_t DISPLAY_FREQUENCY { 1000 / 1000 }; // Frequencia de 1Hz   

typedef void NONE;

class DISPLAY
{
private:
  
  DISPLAY(const DISPLAY&)            = delete;
  DISPLAY(DISPLAY&&)                 = delete;
  DISPLAY& operator=(const DISPLAY&) = delete;
  DISPLAY& operator=(DISPLAY&&)      = delete;

private:

  /*
   *
   * Essa variável vai servir para saber onde a leitura do framebuffer está
   *
   */ 

  uint16_t _frammebuffer_addrs { DISPLAY_FRAMEBUFFER_ADDRS }; 

  /*
   *
   * Typedef
   *
   * NONE -> void
   *
   */

  /*
   *
   * @info   : Essa função corresponde a 1 ciclo do display, ou seja, como nos temos
   *           10Hz vamos ter 10 ciclos por segundo, para ficar melhor de entender,
   *           vamos dizer que cada ciclo é um frame, um monitor simples tem 60Hz, já
   *           esse tem 1Hz, por enquanto :)
   *
   * @param  : void
   *
   * @return : void
   *
   */ 

  NONE cycle() noexcept;

public:

  DISPLAY() noexcept = default;

  ~DISPLAY() noexcept = default;
  
  /*
   *
   * @info   : Aqui fica o loop principal, essa função vai chamar a função cycle 10 
   *           vezes por segundo
   *
   * @param  : void
   *
   * @return : void
   *
   */ 

  NONE clock_loop() noexcept;
};

#endif


/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : display.hpp                   |
 *    |  SRC MOD   : 24/10/2024                    |
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

class BUS;

#define DISPLAY_FRAMEBUFFER_ADDRS 0x7000
#define DISPLAY_FRAMEBUFFER_SIZE 0x12C
#define DISPLAY_DEVICE_ADDRS 0xFA

/*
 *
 * Essa parte vai servir para debugar o código, caso você não vá fazer alteração
 * no código pode deixar do jeito que está
 *
 */

// #define DISPLAY_LOG // Informações que eu vou usar no desenvolvimento, deixarei comentado por padrão 
#define SHOW_DISPLAY // Caso não queira que o display seja mostrado comente essa linha

constexpr uint16_t DISPLAY_FREQUENCY { 1000 / 1000 }; // Frequencia de 1Hz   

class DISPLAY
{
private:
  
  DISPLAY(const DISPLAY&)            = delete;
  DISPLAY(DISPLAY&&)                 = delete;
  DISPLAY& operator=(const DISPLAY&) = delete;
  DISPLAY& operator=(DISPLAY&&)      = delete;

private:

  BUS* _BUS; 

  /*
   *
   * Essa variável vai servir para saber onde a leitura do framebuffer está
   *
   */ 

  uint16_t _frammebuffer_addrs { DISPLAY_FRAMEBUFFER_ADDRS };

  /*
   *
   * @info   : Essa função corresponde a 1 ciclo do display, ou seja, um frame por segundo,
   *           como ainda não terminei por completo ele está fazendo 10 caracteres por segundo.
   *
   * @param  : void
   *
   * @return : void
   *
   */ 

  void cycle() noexcept;

public:

  explicit DISPLAY(BUS*) noexcept;

  ~DISPLAY() noexcept = default;
  
  /*
   *
   * @info   : Aqui fica o loop principal, essa função vai chamar a função cycle 1 
   *           vezes por segundo
   *
   * @param  : void
   *
   * @return : void
   *
   */ 

  void clock_loop() noexcept;
};

#endif


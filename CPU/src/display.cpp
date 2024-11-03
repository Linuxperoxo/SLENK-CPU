/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : display.cpp                   |
 *    |  SRC MOD   : 03/11/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <sstream> // Para buffer de str

#include "../include/display/display.hpp"
#include "../include/bus/bus.hpp"

#define MAX_CHAR_FOR_LINE 50
#define DELAY_MS 50
#define READ_OP 1

DISPLAY::DISPLAY(BUS* _bus) noexcept
  : _BUS(_bus)
{}

/*
 *
 * Por enquanto não vou usar um buffer para cada frame, deixei com esse delay
 * para ter uma animação legal :^)
 *
 */

void DISPLAY::cycle() noexcept
{
  char     _display_char    { '\0' };
  uint8_t  _render_for_line { 0 };
  uint16_t _count           { 0 };

  /*
   *
   * Para armazenar o frame em um buffer
   *
   */

  static std::stringstream _frame;

  /*
   *
   * Para o display log
   *
   */

#if defined DISPLAY_LOG 
  static uint64_t _frames_render    { 0 };
  static uint64_t _dma_interruption { 0 };
  static std::stringstream _log;
#endif

  /*
   *
   * Início do display
   *
   */

  _frame << "\n+=======================DISPLAY======================+\n| ";
  
  while(_count <= DISPLAY_FRAMEBUFFER_SIZE)
  {
    _display_char = _BUS->cpu_interrupt_DMA(_frammebuffer_addrs, READ_OP, 0, DISPLAY_DEVICE_ADDRS);
    
    #if defined DISPLAY_LOG
      ++_dma_interruption;
    #endif
    
    if(_display_char == '\n')
    {
      
      /*
       * 
       * Serve para manipular as linhas quando usamos o '\n'
       *
       * OBS: Isso não seria necessário se nao tivesse os '|' no final das linhas
       *      mas coloquei para ficar mais agradável
       *
       */ 

      while(_render_for_line < MAX_CHAR_FOR_LINE)
      {
        _frame << ' ';
        
        ++_render_for_line;
        ++_count;
      }
    }
    
    /*
     *
     * A cada 50 caracteres vamos quebrar a linha
     *
     */

    if(_render_for_line % MAX_CHAR_FOR_LINE == 0 && _render_for_line > 0)
    { 
      _frame << " |\n";

      /*
       *
       * Adicionei esse if para corrigir um bug de quebra de linha que estava me incomodando
       * 
       * Ele rodava o loop mais uma vez quebrando a linha e imprimindo o próximo caractere, 
       * apenas usei esse if para quebrar o loop quando quebrar a linha e não imprimir o caractere
       *
       */

      if(_count >= DISPLAY_FRAMEBUFFER_SIZE) { break; }
      
      _frame << "| ";

      /*
       *
       * Como não queremos o '\n' da ROM pegamos o próximo caractere 
       *
       */
      
      _display_char = _BUS->cpu_interrupt_DMA(++_frammebuffer_addrs, READ_OP, 0, DISPLAY_DEVICE_ADDRS);
      
      #if defined DISPLAY_LOG
        ++_dma_interruption;
      #endif
      
      /*
       *
       * Zeramos essa variável já que é uma linha nova
       *
       */

      _render_for_line = 0;
    }
    
    if(_display_char == '\0')
      _display_char = ' ';
    
    _frame << _display_char;

    ++_frammebuffer_addrs;
    ++_render_for_line;
    ++_count;
  }

  _frame << "+====================================================+\n";

  /*
   *
   * Depois de armazenar todas as informações do frame vamos jogar ele para o display
   *
   */

#if defined SHOW_DISPLAY
  std::cout << _frame.str();
  std::cout.flush();
#endif

#if defined DISPLAY_LOG
  
  /*
   *
   * Parte para a depuração do código
   *
   */

  _log << "FRAMEBUFFER_PTR_ADDRS : " << std::hex << "0x" << _frammebuffer_addrs << '\n';
  _log << "DISPLAY FREQUENCY     : " << std::dec << DISPLAY_FREQUENCY/1000 << "Hz" << '\n';
  _log << "FRAME COUNTER         : " << std::dec << ++_frames_render << '\n';
  _log << "DMA INTERRUPTION      : " << std::dec << _dma_interruption << '\n'; 

  std::cout << "\n+---------DISPLAY-LOG---------+\n";
  std::cout << _log.str();
  std::cout << "+-----------------------------+\n";

  _log.str("");
  _log.clear();
#endif
  
  /*
   *
   * Definindo o ponteiro para o início do framebuffer
   *
   */

  _frammebuffer_addrs = DISPLAY_FRAMEBUFFER_ADDRS;
}

void DISPLAY::clock_loop() noexcept
{

  /*
   *
   * Fica rodando até uma instrução BRK ser executada
   *
   */ 

  while(_BUS->CPU_running() != 1)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(DISPLAY_FREQUENCY)); // Deixei ele em cima de cycle() para o processador conseguir rodar o primeiro clock dele
    _BUS->DMA_started();
    cycle(); // 1 ciclo 
    _BUS->DMA_stopped();
  }
}

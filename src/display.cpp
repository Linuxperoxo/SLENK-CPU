/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : display.cpp                   |
 *    |  SRC MOD   : 24/10/2024                    |
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
  uint8_t  _render_for_line { 1 };
  uint16_t _count           { 1 };

  /*
   *
   * Para armazenar o frame em um buffer
   *
   */

  std::stringstream _frame;

  /*
   *
   * Início do display
   *
   */

  _frame << "\n+=====================DISPLAY=====================+\n|";
  
  while(_count <= DISPLAY_FRAMEBUFFER_SIZE)
  {
    _display_char = _BUS->cpu_interrupt_DMA(_frammebuffer_addrs, READ_OP, 0, DISPLAY_DEVICE_ADDRS);

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
      
      /*
       *
       * Fazendo correção da linha
       *
       */

      _frame << ' ' << ' ';
    }
    
    /*
     *
     * Isso serve para colocar o '|' quando os outros bytes do framebuffer são 0, isso vai ser melhorado
     *
     */

    if(_display_char == '\0' && _BUS->cpu_interrupt_DMA(_frammebuffer_addrs + 1, READ_OP, 0, DISPLAY_DEVICE_ADDRS) == '\0')
    {
      while(_BUS->cpu_interrupt_DMA(++_frammebuffer_addrs + 1, READ_OP, 0, DISPLAY_DEVICE_ADDRS) == '\0' && _render_for_line < MAX_CHAR_FOR_LINE)
      {
        _frame << ' ';
        
        ++_render_for_line;
        ++_count;
      }

      /*
       *
       * Fazendo uma correção na primeira linha
       *
       */

      if(_count <= MAX_CHAR_FOR_LINE) _frame << ' ' << ' ';
    }

    /*
     *
     * A cada 50 caracteres vamos quebrar a linha
     *
     */

    if(_render_for_line % MAX_CHAR_FOR_LINE == 0)
    { 
      _frame << "|\n";

      /*
       *
       * Adicionei esse if para corrigir um bug de quebra de linha que estava me incomodando
       * 
       * Ele rodava o loop mais uma vez quebrando a linha e imprimindo o próximo caractere, 
       * apenas usei esse if para quebrar o loop quando quebrar a linha e não imprimir o caractere
       *
       */

      if(_count >= DISPLAY_FRAMEBUFFER_SIZE) { break; }
      
      _frame << '|';

      /*
       *
       * Como não queremos o '\n' da ROM pegamos o próximo caractere 
       *
       */
      
      _display_char = _BUS->cpu_interrupt_DMA(++_frammebuffer_addrs, READ_OP, 0, DISPLAY_DEVICE_ADDRS);

      /*
       *
       * Zeramos essa variável já que é uma linha nova
       *
       */

      _render_for_line = 1;
      
      /*
       *
       * Adicionamos mais 1 no count já que pulamos um caractere
       *
       */

      ++_count;
    }
     
    _frame << _display_char;

    ++_frammebuffer_addrs;
    ++_render_for_line;
    ++_count;
  }

  _frame << "+=================================================+\n";

  _frammebuffer_addrs = DISPLAY_FRAMEBUFFER_ADDRS;

  /*
   *
   * Depois de armazenar todas as informações do frame vamos jogar ele para o display
   *
   */

#if defined SHOW_DISPLAY
  std::cout << _frame.str();
  std::cout.flush();
#endif
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
    std::this_thread::sleep_for(std::chrono::seconds(DISPLAY_FREQUENCY)); // Deixei ele em cima de cycle() para o processador conseguir rodar o primeiro clock dele
    _BUS->DMA_started();
    cycle(); // 1 ciclo 
    _BUS->DMA_stopped();
  }
}


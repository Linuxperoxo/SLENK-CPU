/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : ram.hpp                       |
 *    |  SRC MOD   : 8/10/2024                     |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

/*
 *
 * Aqui é a memória principal, e vai ser aqui também que nós vamos
 * manipular dispositivos mapeados na memória
 *
 * A memória RAM é dividida em célular de 1 byte de tamanho
 *
 * +--------------------------------------------------------------+
 * | 0x00 | 0x01 | 0x02 | 0x03 | 0x04 | 0x05 | 0x06 | 0x07 | 0x08 | -> RAM 
 * +--------------------------------------------------------------+
 *
 * Normalmente dispositivos são mapeados nos endereços mais altos(depende do OS)
 *
 * Digamos que um dispositivos qualquer esteja mapeado no endereço 0x05 - 0x07, com o tamanho
 * de 3 bytes
 *
 * +--------------------------------------------------------------+
 * | 0x00 | 0x01 | 0x02 | 0x03 | 0x04 | 0x05 | 0x06 | 0x07 | 0x08 | -> RAM 
 * +--------------------------------------------------------------+
 *                                       ^             ^
 *                                       |             |
 *                                     Início        Final
 *
 * Então com o dispositivo mapeado na memória, qualquer dado que é escrito nesse endereço
 * será uma manipulação com o dispositivo mapeado
 *
 * Exemplo, digamos que é um teclado mapeado nesse endereço, digamos também que todo dato de input
 * do teclado seja armazenado nesse endereço em expecífico, podemos pedir para ler esse bloco para 
 * saber qual tecla foi pressionada
 *
 * Mas não é como se o dispositivo também tivesse acesso a esse lugar na memória, por baixo dos panos
 * o que realmente acontece é que após traduzir o endereço fornecido pelo processador, o barramento
 * percebe que aquele endereço não é para um lugar na memória, e sim para um dispositivo, então ele 
 * redireciona para o dispositivo
 *
 */

#ifndef __RAM_HPP__
#define __RAM_HPP__

#include <cstdint>

constexpr uint32_t MEMORY_SIZE { 1024 * 64 }; 

class RAM
{
private:

  RAM(const RAM&)            = delete;
  RAM(RAM&&)                 = delete;
  RAM& operator=(const RAM&) = delete;
  RAM& operator=(RAM&&)      = delete;

private:

  uint8_t* _MEMORY;

public:

  explicit RAM() noexcept;

  ~RAM() noexcept;
};

#endif


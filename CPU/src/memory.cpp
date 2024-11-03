/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : memory.cpp                    |
 *    |  SRC MOD   : 03/11/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h> // mmap()
#include <iostream>
#include <fcntl.h> // open()
#include <unistd.h> // close()
#include <sys/stat.h> // fstat()

#include "../include/memory/memory.hpp"

#define FIRMWARE_ROM_SIZE 0xFF

constexpr uint16_t PROGRAM_ROM_SIZE { 0xFFFF - 0x8000 };
constexpr uint16_t RAM_MEMORY_SIZE  { 0x6FFF - 0x100 };
constexpr uint16_t IO_MEMORY_SIZE   { 0x7FFF - 0x7000 };

MEMORY::MEMORY() noexcept
  : _FIRMWARE_ROM(nullptr),
    _PROGRAM_ROM(nullptr),
    _IO_DEVICE(nullptr),
    _RAM(nullptr),
    _CE(1)
{
  /*
   *
   * Cada memória essa dessa vai responder a uma determinada faixa de endereços, isso é definido pelo addrs decoder
   *
   */

  _FIRMWARE_ROM = static_cast<uint8_t*>(mmap(nullptr, FIRMWARE_ROM_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0));
  _PROGRAM_ROM  = static_cast<uint8_t*>(mmap(nullptr, PROGRAM_ROM_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0));
  _IO_DEVICE    = static_cast<uint8_t*>(mmap(nullptr, IO_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0));
  _RAM          = static_cast<uint8_t*>(mmap(nullptr, RAM_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0));

  if(_FIRMWARE_ROM == MAP_FAILED || _PROGRAM_ROM == MAP_FAILED || 
     _IO_DEVICE == MAP_FAILED || _RAM == MAP_FAILED)
  { std::cout << "Error to alloc memory in class MEMORY\n"; exit(EXIT_FAILURE); }

  /*
   *
   * Inicializando todas a memórias com 0
   *
   */

  std::memset(_FIRMWARE_ROM, 0, FIRMWARE_ROM_SIZE);
  std::memset(_PROGRAM_ROM, 0, PROGRAM_ROM_SIZE);
  std::memset(_IO_DEVICE, 0, IO_MEMORY_SIZE);
  std::memset(_RAM, 0, RAM_MEMORY_SIZE);
  
}

MEMORY::~MEMORY() noexcept
{
  munmap(_FIRMWARE_ROM, FIRMWARE_ROM_SIZE);
  munmap(_PROGRAM_ROM, PROGRAM_ROM_SIZE);
  munmap(_IO_DEVICE, IO_MEMORY_SIZE);
  munmap(_RAM, RAM_MEMORY_SIZE);
}

uint8_t MEMORY::addrs_decoder(uint16_t _addrs, uint8_t _data, uint8_t _op) noexcept
{
  switch(_op)
  {
    case 0:
      if(_addrs >= FIRMWARE_INIT && _addrs <= FIRMWARE_END && _CE == 1)
      { return _FIRMWARE_ROM[_addrs]; }
      else if(_addrs >= FIRMWARE_INIT && _addrs <= FIRMWARE_END)
      { _addrs += RAM_MEMORY_INIT; }
      
      if(_addrs >= RAM_MEMORY_INIT && _addrs <= RAM_MEMORY_END)
      { return _RAM[_addrs - RAM_MEMORY_INIT]; }

      if(_addrs >= IO_MEMORY_INIT && _addrs <= IO_MEMORY_END)
      { return _IO_DEVICE[_addrs - IO_MEMORY_INIT]; }

      if(_addrs >= PROGRAM_ROM_INIT && _addrs <= PROGRAM_ROM_END)
      { _CE = 0; return _PROGRAM_ROM[_addrs - PROGRAM_ROM_INIT]; }
    break;

    case 1:
      if(_addrs >= FIRMWARE_INIT && _addrs <= FIRMWARE_END)
      { _addrs += RAM_MEMORY_INIT; }
      
      if(_addrs >= RAM_MEMORY_INIT && _addrs <= RAM_MEMORY_END)
      { _RAM[_addrs - RAM_MEMORY_INIT] = _data; return 0; }
      
      if(_addrs >= IO_MEMORY_INIT && _addrs <= IO_MEMORY_END)
      { _IO_DEVICE[_addrs - IO_MEMORY_INIT] = _data; return 0; }
    break;
  }
  return 0;
}

void MEMORY::load_rom(const char* _rom_file) noexcept
{
  int _file
  {
    open(_rom_file, O_RDONLY) // Abrindo arquivo para apenas escrita
  };

  if(_file == -1)
  {
    std::cout << "Error to load ROM file -> " << _rom_file << '\n';
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Usamos a função fstat para pegar o tamanho da ROM 
   *
   */

  struct stat* _rom_file_info { new struct stat() }; // Struct necessária para guardar informações do arquivo

  if(fstat(_file, _rom_file_info) == -1) // Pegando as informações e verificando se ocorreu algum problema
  {
    std::cout << "Error to get file infos -> " << _rom_file << '\n';
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Aqui fiz uma pequena verificação para ver se o tamanho do arquivo não é maior que o tamanho
   * máximo que no caso é de 0x8000 até 0xFFFF 
   *
   */

  if(_rom_file_info->st_size / 8 > PROGRAM_ROM_SIZE)
  {
    std::cout << "ROM file -> " << _rom_file << " is very long he has " << _rom_file_info->st_size << " Bytes of size, he most have at most " << PROGRAM_ROM_SIZE * 8 << '\n';
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Como eu so quero saber o tamanho do arquivo, apenas armazeno o tamanho em uma variável para poder fechar
   * arquivo já que ele vai ser mapeado na memória
   *
   */

  uint16_t _rom_file_size { static_cast<uint16_t>(_rom_file_info->st_size) };

  uint8_t* _ROM_BUFFER
  {
    /*
     * 
     * Mapeado arquivo na memória com permissão de apenas escrita e privado, ou seja, qualquer alteração no seu 
     * conteúdo não vai ser escrito no arquivo
     *
     */
    
    static_cast<uint8_t*>(mmap(nullptr, _rom_file_size, PROT_READ, MAP_PRIVATE, _file, 0))
  };

  if(_ROM_BUFFER == MAP_FAILED)
  {
    std::cout << "Error to alloc memory for ROM BUFFER\n";
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Já pegamos a informação que eu queria, já podemos liberar essa memória
   *
   */

  delete _rom_file_info;
  
  /*
   *
   * Já mapeamos o arquivo na memória então podemos fecha-ló
   *
   */

  close(_file);

  uint16_t _bits  { 0 }; // Vendo quantos bits nos copiamos
  uint8_t  _byte  { 0 }; // Aqui onde vamos mover cada "bit" do arquivo  
  uint8_t  _flip  { 0 }; // Quantas vezes vamos flipar o bit para a esquerda, isso serve para montar nosso byte

  for(; _bits < _rom_file_size; _bits++, _flip++) // Copiando 32 bytes do arquivo, mais pra frente vamos aumentar essa quantidade
  {
    if(_bits % 8 == 0 && _bits > 0)
    {

      /*
       *
       * Quando copiamos 1 byte do arquivo jogamos esse byte para a memória da rom
       *
       */
      
      _PROGRAM_ROM[_bits / 8 - 1] = _byte;
      _byte                       = 0;
      _flip                       = 0;
    }
    
    /*
     *
     * Aqui convertemos as strings '0', '1' do arquivo rom em numeral 0, 1
     *
     */

    _byte = _byte | (_ROM_BUFFER[_bits] - '0') << (7 - _flip);
  }

  munmap(_ROM_BUFFER, _rom_file_size);

  /*
   *
   * ÁREA DEBUG:
   *
   * Pequenos testes xD
   *
   * std::cout << reinterpret_cast<void*>(&_ROM[0]) << '\n';
   * std::cout << static_cast<int>(*reinterpret_cast<uint8_t*>(_MEMORY + ROM_INIT)) << '\n';
   * std::cout << *reinterpret_cast<int*>(_MEMORY + ROM_INIT + 1) << '\n';
   *
   *
   *
   * std::cout << std::hex;

   * std::cout << static_cast<int>(_ROM[0]) << '\n';
   * std::cout << static_cast<int>(_ROM[1]) << '\n';
   * std::cout << static_cast<int>(_ROM[2]) << '\n';
   * exit(EXIT_FAILURE);
   *
   */
}

void MEMORY::load_firmware() noexcept
{
  /*
   *
   * Não queria fazer um arquivo separado para ser usado como firmware
   * então fiz assim mesmo :)
   *
   */

  /*
   *
   * Configurando o registrador STKPTR para o início da stack que é 0xFF 
   *
   * MOV [STK] [0xFF];
   *
   */
  
  _FIRMWARE_ROM[0x00] = 0x04; // MOV
  _FIRMWARE_ROM[0x01] = 0x04; // STK
  _FIRMWARE_ROM[0x02] = 0xFF; // Valor a mover

  /*
   *
   * Configurando o registrador PC para o início da ROM do programa
   *
   * JMP [0x8000];
   *
   */ 

  _FIRMWARE_ROM[0x03] = 0x01; // JMP
  _FIRMWARE_ROM[0x04] = 0x80; // HIGH BYTE 
  _FIRMWARE_ROM[0x05] = 0x00; // LOWER BYTE

  /*
   *
   * Definindo esse bit como 0 já que não podemos mais acessar qualquer local da memória, 
   * apenas usamos ele para escrever um firmware de maneira mais fácil
   *
   */
}


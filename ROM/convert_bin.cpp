#include <cstdint>
#include <cstdlib>
#include <iostream>

#define CHAR_NUM 14

union Byte{
  uint8_t _byte;
  struct{
    uint8_t _bit1 : 1;
    uint8_t _bit2 : 1;
    uint8_t _bit3 : 1;
    uint8_t _bit4 : 1;
    uint8_t _bit5 : 1;
    uint8_t _bit6 : 1;
    uint8_t _bit7 : 1;
    uint8_t _bit8 : 1;
  };

  void printbits() noexcept{
    for(int8_t i { 7 }; i >= 0; i--)
    {
      std::cout << (_byte >> i & 1);
    }
  }
};

int main()
{
  Byte _byte;

  /*
   *
   * Você pode usar esse programa para imprimir alguma frase na tela, nessa caso pegar
   * binário de cada char e fazer seu print em binário! :D 
   *
   */ 


  char _char_to_convert[CHAR_NUM]
  {
    'H', 'E', 'L', 'L', 'O', ',', ' ', 'W', 'O', 'R', 'L', 'D', '!', '\n'
  };

  for(int i { 0 }; i < CHAR_NUM; i++){
    _byte._byte = _char_to_convert[i];

    std::cout << "+-----------------------------+\n" 
              << "CHAR   : " << _byte._byte << '\n' 
              << "BINARY : "; _byte.printbits(); std::cout << '\n' 
              << "+-----------------------------+" << '\n';
  }
  return 0;
}

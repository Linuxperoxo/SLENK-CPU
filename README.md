# Projeto LITTLE_CPU

Este projeto foi criado com o intuito de estudo e compreensão dos princípios de funcionamento de um processador. O objetivo não é desenvolver um processador funcional, mas sim simular e entender suas operações.

## O que é o processador 6502?

O **6502** é um microprocessador de 8 bits que teve grande influência e popularidade nas décadas de 1970 e 1980. Desenvolvido pela **MOS Technology** em 1975, foi um dos processadores mais acessíveis de sua época, sendo amplamente utilizado em diversos computadores e consoles clássicos, como:

- Apple II
- Commodore 64
- Atari 2600
- NES (Nintendo Entertainment System)

## Por que escolhi o 6502?

A escolha do 6502 como base para este projeto deve-se à sua documentação abrangente disponível em [nesdev.org](https://www.nesdev.org). Embora este projeto não replique um NES, utilizei essa documentação para compreender seu funcionamento e implementar minhas próprias ideias.

## Compilação

Para compilar e executar o projeto, siga os passos abaixo:

1. Clone o repositório:
    ```bash
    git clone https://github.com/Linuxperoxo/LITTLE_CPU.git
    cd LITTLE_CPU
    ```

2. Limpe quaisquer arquivos de compilação existentes:
    ```bash
    ./make.sh clean
    ```

3. Compile o projeto:
    ```bash
    ./make.sh build
    ```

4. Execute o projeto:
    ```bash
    cd build/bin/
    ./CPU ../../ROM/Hello, World.rom
    ```

## Documentação

Como documentação você pode ler os arquivos .hpp em include/.

Ordem de leitura recomendada:  

1. include/ram/ram.hpp;  
2. include/bus/bus.hpp;  
3. include/cpu/cpu.hpp;  

## Observações

O projeto ainda está em desenvolvimento, e várias funcionalidades e lógicas ainda não foram implementadas. Agradeço qualquer feedback ou contribuição!



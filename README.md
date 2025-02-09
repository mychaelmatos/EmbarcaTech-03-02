# Projeto TAREFA U4C6 - Display OLED e Matriz LED

Este projeto é parte da residência do CEPEDI de Sistemas Embarcados. Ele demonstra a integração de múltiplos componentes e protocolos de comunicação usando a Raspberry Pi Pico, incluindo display OLED, matriz LED WS2812, botões e comunicação serial.

## Descrição

O projeto implementa um sistema interativo que permite entrada de caracteres via Serial Monitor, controle de LEDs RGB através de botões, exibição de caracteres em um display OLED SSD1306 e representação de números em uma matriz LED WS2812 5x5.

## Funcionalidades Principais

1. **Entrada de Caracteres via PC**
   - Recebe caracteres individuais através do Serial Monitor
   - Exibe os caracteres no display OLED SSD1306
   - Quando números (0-9) são digitados, exibe padrões correspondentes na matriz LED WS2812

2. **Controle via Botões**
   - Botão A: Controla o LED RGB Verde
   - Botão B: Controla o LED RGB Azul
   - Feedback visual no display OLED e mensagens no Serial Monitor
   - Implementação de debouncing via software

3. **Display OLED**
   - Suporte a caracteres maiúsculos e minúsculos
   - Exibição de mensagens de status
   - Interface I2C para comunicação

4. **Matriz LED WS2812**
   - Padrões 5x5 para representação de números
   - Controle individual de cada LED RGB

## Estrutura do Projeto

- `blink.c`: Código principal do projeto
- `inc/font.h`: Biblioteca de fontes modificada com caracteres minúsculos
- `inc/ssd1306.h`: Biblioteca para controle do display OLED
- `ws2812.pio`: Arquivo de configuração PIO para LEDs WS2812
- `CMakeLists.txt`: Arquivo de configuração do CMake

## Requisitos de Hardware

- Placa BitDogLab com RP2040
- Display OLED SSD1306 128x64
- Matriz LED WS2812 5x5
- 2 Botões (A e B)
- LEDs RGB
- Computador com porta USB

## Como Usar

1. Clone o repositório
2. Configure o ambiente de desenvolvimento para Raspberry Pi Pico
3. Compile o projeto usando CMake
4. Conecte a placa no modo BOOTSEL
5. Carregue o firmware compilado
6. Use o Serial Monitor para interagir com o sistema

## Protocolos Utilizados

- UART: Comunicação serial com o computador
- I2C: Comunicação com o display OLED
- PIO: Controle dos LEDs WS2812

## Licença

Este projeto é licenciado sob a [MIT License](LICENSE).

---

Desenvolvido como parte da Residência do CEPEDI de Sistemas Embarcados, Embarcatech.

**Projeto RP2040 com LEDs, Botões e Display SSD1306**

Este é um projeto que utiliza a *BitDogLab*, LEDs, botões e um display OLED SSD1306. 

**Requisitos do Projeto**

- Uso de Interrupções e *Debouncing*;
- Controle de LEDs;
- Utilização do Display 128x64 SSD1306 (I2C):
- Comunicação Serial via UART.

**Funcionamento:**

- Ao clicar no Botão A, aciona/desliga o LED verde.
- Ao clicar no Botão B, aciona/desliga o LED azul.
- Os botões alternam o estado do LED correspondente, exibindo a informação no display OLED e no Serial monitor.
- O programa aguarda a entrada de um caractere pelo serial monitor e exibe-o no display.

**Como usar:**

- Compile o código utilizando o VSCode.
- Carregue o código na placa *BitDogLab*.
- Faça o envio de caracteres no Serial Monitor.
- Pressione os botões A e B.

Link de funcionamento:
https://drive.google.com/drive/folders/13AvdmWvfJ9prlMlQaiIborEdul3V-gYp?usp=sharing
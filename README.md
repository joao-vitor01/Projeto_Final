# Projeto_Final: Jogo da Cobrinha em Matriz de LEDs 5x5

## Sobre o Projeto
Este projeto consiste na implementação de uma versão simplificada do jogo da cobrinha utilizando uma matriz de LEDs 5x5, controlada por um joystick analógico e um Raspberry Pi Pico W.

## Objetivos
- Criar um jogo simples e funcional utilizando um sistema embarcado.
- Utilizar um joystick para movimentar a cobrinha.
- Exibir o jogo em uma matriz de LEDs 5x5.
- Implementar uma lógica básica de crescimento e colisão.

## Componentes Utilizados
- **Microcontrolador:** Raspberry Pi Pico W (RP2040)
- **Matriz de LEDs:** WS2812 5x5
- **Controle:** Joystick analógico (eixo X, eixo Y e botão de seleção)
- **Alimentação:** 5V via USB

## Funcionamento
- O jogador controla a cobrinha usando o joystick.
- A comida aparece em posições aleatórias na matriz.
- Ao comer a comida, a cobrinha cresce.
- O jogo termina quando a cobrinha colide com as bordas da matriz.
- O botão do joystick pode ser usado para reiniciar o jogo.

## Como Jogar
- Mova a cobrinha utilizando o joystick.
- Evite colidir com as bordas.
- Pegue os alimentos para crescer.
- Pressione o botão do joystick para reiniciar.

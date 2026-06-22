# Projeto 2 – Aferidor de Temperatura de Forno Industrial

## Introdução

Este projeto foi desenvolvido por mim, Lucas Garcia Pereira NºUSP: 15496307,  na disciplina **SEL0433 – Aplicação de Microprocessadores** com o objetivo de implementar um sistema embarcado para monitoramento de temperatura e tempo de operação de um forno industrial utilizando o microcontrolador **PIC18F4550**.

O sistema integra diferentes periféricos do microcontrolador, incluindo conversor analógico-digital (ADC), temporizadores (Timers), interrupções externas e display LCD. A temperatura do forno é medida por meio de um sensor **LM35** (simulado por um potenciômetro durante os testes) e exibida continuamente em um display LCD no formato **XX.X °C**. Além disso, o usuário pode selecionar diferentes tempos de aferição por meio de botões externos, iniciando contagens regressivas de **10 segundos** ou **60 segundos**.

O projeto foi desenvolvido utilizando a linguagem C no compilador MikroC PRO e simulado no simulIDE.

## Descrição do Projeto

O sistema possui os seguintes recursos principais:

* Leitura contínua da temperatura através do canal analógico AN0 do PIC18F4550;
* Utilização de tensão de referência externa de 1 V para melhorar a resolução da medição do LM35;
* Exibição da temperatura em tempo real em um display LCD 16x2;
* Seleção de dois modos de contagem regressiva:

  * Curta duração: 10 segundos;
  * Longa duração: 60 segundos;
* Utilização de interrupções externas nos botões de seleção;
* Utilização do **Timer0** para geração da base de tempo de 1 segundo da contagem de 60 segundos;
* Utilização do **Timer1** para geração da base de tempo de 250 ms da contagem de 10 segundos;
* Acionamento de um LED que representa a resistência de aquecimento do forno.

A lógica de controle da resistência foi implementada utilizando histerese simples:

* LED ligado para temperaturas inferiores a **60 °C**;
* LED desligado para temperaturas superiores a **80 °C**.

Essa estratégia evita oscilações frequentes da saída quando a temperatura se encontra próxima ao valor de referência.

## Implementação

O firmware foi estruturado utilizando interrupções para garantir que a contagem de tempo ocorra independentemente da execução das demais tarefas do sistema.

As interrupções externas INT0 e INT1 são responsáveis por iniciar as contagens regressivas de 60 s e 10 s, respectivamente. Durante a execução da contagem, os temporizadores geram interrupções periódicas que decrementam o valor exibido no display.

## Resultados e Discussão

A leitura da temperatura apresentou comportamento consistente ao variar o potenciômetro utilizado para simular o sensor LM35, permitindo a visualização contínua dos valores no display LCD com uma casa decimal de precisão.

As contagens regressivas de 10 s e 60 s foram executadas corretamente por meio dos temporizadores Timer1 e Timer0, respectivamente. A utilização de interrupções permitiu que a atualização do tempo ocorresse de forma precisa sem interferir na aquisição da temperatura.

O LED que representa a resistência do forno também apresentou comportamento adequado, ligando-se quando a temperatura estava abaixo de 60 °C e desligando-se acima de 80 °C, simulando um sistema simples de controle térmico.

De modo geral, o projeto permitiu aplicar conceitos importantes de sistemas embarcados, como configuração de registradores, utilização de periféricos do PIC18F4550, programação de interrupções, conversão analógico-digital e integração de múltiplos dispositivos em uma única aplicação.

## Conclusão

O desenvolvimento do aferidor de temperatura de forno industrial permitiu consolidar os conhecimentos relacionados ao microcontrolador PIC18F4550 e seus principais recursos internos. A integração entre ADC, temporizadores, interrupções externas, display LCD e saídas digitais resultou em um sistema funcional capaz de monitorar simultaneamente temperatura e tempo de operação.

Os resultados obtidos demonstraram que a solução atende aos requisitos propostos, fornecendo uma interface simples para monitoramento e simulação do funcionamento de um forno industrial, além de servir como base para aplicações embarcadas mais complexas envolvendo controle e supervisão de processos.

## Compilação e Simulação

![Captura de Tela da compilação]./assets/compilacao

![Captura de Tela do SimulIDE]./assets/simulacao

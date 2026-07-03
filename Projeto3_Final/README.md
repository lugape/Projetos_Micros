# Projeto 3: Controle PWM e Comunicação com ESP32

**Disciplina:** SEL0433 - APLICAÇÃO DE MICROPROCESSADORES  
**Autor:** Lucas Garcia Pereira - Nº USP: 15496307  

Este repositório contém os códigos e a documentação do Projeto 3, desenvolvido para exercitar a programação de microcontroladores de 32 bits utilizando a plataforma ESP32 DevKit. O projeto explora comunicação serial, modulação por largura de pulso (PWM) e o uso de bibliotecas otimizadas.

---

## Parte 1: Controle PWM de LED RGB

**Objetivo:** Implementar o controle PWM de um LED RGB utilizando a biblioteca LEDC (LED Control PWM) com resolução de 8 bits e frequência de 5 kHz.

### Conceitos Envolvidos e Bibliotecas Empregadas
* **Biblioteca:** `Arduino.h` (utilizando a API nativa `ledc` embutida no framework para ESP32).
* **Conceito:** O código utiliza *duty cycles* independentes para cada cor (Vermelho, Verde e Azul) com taxas de incremento distintas. A cor verde incrementa em passos de 5%, a azul no dobro desse valor (10%) e a vermelha o triplo (15%). Para evitar travamentos, o controle de tempo foi feito utilizando a função `millis()` no lugar de `delay()`, garantindo uma execução fluida.

### Principais Trechos de Código
A configuração do PWM é feita associando a frequência e resolução aos pinos:
`ledcAttach(PIN_RED, PWM_FREQ, PWM_RES);`

O mapeamento da porcentagem (0-100%) para a resolução de 8 bits (0-255) é calculado dinamicamente:
`uint16_t dutyRed8 = map(dutyRed_atual, 0, 100, 0, PWM_MAX);`
`ledcWrite(PIN_RED, dutyRed8);`

### Discussão dos Resultados
A simulação demonstrou que as cores variam de forma assíncrona devido às diferentes taxas de incremento. O uso de um temporizador não bloqueante (`millis()`) garantiu que as informações fossem enviadas via UART (115200 de baud rate) no momento exato, sem interferir na modulação do sinal visual do LED.

---

## Parte 2 - Exercício 1: Controle de Servomotor via Potenciômetro

**Objetivo:** Controlar a posição de um servomotor variando o *duty cycle* por meio da leitura analógica de um potenciômetro.

### Conceitos Envolvidos e Bibliotecas Empregadas
* **Biblioteca:** `ESP32Servo.h`. Abstrai a complexidade dos temporizadores de hardware da ESP32 para controle de servos convencionais.
* **Conceitos:** Utilização do ADC (Conversor Analógico-Digital) de 12 bits da ESP32 para leitura de tensão. O valor lido é convertido em graus (0° a 180°) e aplicado ao motor. Filtros de software (média de 8 amostras) e zona morta (atualização apenas se variação >= 1°) foram implementados para evitar *jitter* (trepidação mecânica).

### Principais Trechos de Código
Média de leitura do ADC para suavização do sinal:
`long sumAdc = 0;`
`for (int i = 0; i < 8; i++) {`
`  sumAdc += analogRead(PIN_POT);`
`  delayMicroseconds(500);`
`}`
`int adcVal = (int)(sumAdc / 8);`

### Discussão dos Resultados
O controle manual apresentou resposta rápida e estável. A conversão da leitura de 12 bits (0 a 4095) para ângulos limitou o ruído elétrico do potenciômetro na simulação, proporcionando um deslocamento suave do eixo do motor.

---

## Parte 2 - Exercício 2: Sistema Avançado de Controle MCPWM com OLED e Buzzer

**Objetivo:** Desenvolver uma aplicação própria com foco na biblioteca nativa **MCPWM** (Motor Control PWM) da ESP32. O sistema controla um servomotor com um potenciômetro, conta ciclos completos de movimento, exibe dados em um display OLED I2C e aciona um buzzer de alerta a cada 3 ciclos concluídos.

### Conceitos Envolvidos e Bibliotecas Empregadas
* **Bibliotecas Nativas ESP-IDF:** O projeto exige a biblioteca nativa MCPWM[cite: 1]. Diferente do exercício anterior, utilizou-se acesso de baixo nível ao periférico de controle de motores para maior precisão.
* **Bibliotecas de Display:** O uso do barramento I2C para um display OLED é exigido para visualizar parâmetros de funcionamento e estados do sistema.
* **Conceitos:**
    * Geração de sinais PWM utilizando timers e operadores nativos em microssegundos.
    * Lógica de "Máquina de Estados" para rastrear o movimento de ida e volta do servo (0° -> 180° -> 0°) e contabilizar um ciclo completo.
    * Integração de múltiplos periféricos (ADC, PWM, I2C, UART) simultaneamente sem bloqueios críticos.

### Principais Trechos de Código
Inicialização da estrutura nativa MCPWM:
`mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_PIN);`
`mcpwm_config_t pwm_config;`
`pwm_config.frequency = 50;`
`pwm_config.cmpr_a = 0;`
`pwm_config.counter_mode = MCPWM_UP_COUNTER;`
`pwm_config.duty_mode = MCPWM_DUTY_MODE_0;`
`mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);`

Ajuste contínuo da largura do pulso em microssegundos com base no ADC:
`uint32_t pulse_width = map(angulo, 0, SERVO_MAX_DEGREE, SERVO_MIN_PULSEWIDTH_US, SERVO_MAX_PULSEWIDTH_US);`
`mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, pulse_width);`

### Discussão dos Resultados
O uso direto da biblioteca MCPWM provou ser eficiente, explorando de forma adequada os recursos de PWM avançado. O barramento I2C funcionou em perfeita sincronia com a UART, atualizando o OLED e o terminal serial com o ângulo atual e a contagem de ciclos sem causar atrasos que pudessem desestabilizar o sinal PWM do servo.

---

## Reflexão sobre a Escolha Arquitetural: Microcontroladores de 32-bits vs 8-bits

Com base na execução do projeto, o uso de um microcontrolador de 32 bits como a ESP32 é justificado quando a aplicação exige concorrência de recursos e eficiência. 

Enquanto um microcontrolador de 8 bits atenderia a tarefas isoladas, ele teria dificuldades para manter um sinal PWM por software estável enquanto realiza cálculos, atende interrupções de um conversor ADC e atualiza um painel gráfico OLED simultaneamente. A arquitetura de 32 bits fornece a capacidade de processamento necessária para que soluções complexas rodem em paralelo sem sacrificar a responsividade do sistema. Como exigido pela reflexão, a solução mais tecnicamente avançada é justificada pelos requisitos simultâneos da aplicação.

---

## Diagramas e Registros de Simulação
![simu_wokwi_parte1](Projeto3_Final/assets/simu_wokwi_final.png)

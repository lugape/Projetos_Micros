/*
  SEL0433 - Aplicação de Microprocessadores
  Lucas Garcia Pereira - Nº USP: 15496307
  Projeto 3 - Parte 1: Ativação de LED RGB usando biblioteca LEDC na ESP32
*/

#include <Arduino.h>
#include <stdio.h> // Biblioteca para usar o printf

// ---------------- Pinos do LED RGB ----------------
const int PIN_RED   = 25;
const int PIN_GREEN = 26;
const int PIN_BLUE  = 27;

// ---------------- Parâmetros do PWM ----------------
const uint32_t PWM_FREQ = 5000;   // 5 kHz
const uint8_t  PWM_RES  = 8;      // 8 bits de resolução -> duty de 0 a 255 em decimal
const uint16_t PWM_MAX  = 255;    // Valor máximo do PWM

// ---------------- Incrementos de duty cycle (em %) ----------------
const uint8_t STEP_GREEN = 5;               // passo base do LED verde
const uint8_t STEP_BLUE  = STEP_GREEN * 2;  // passo do LED azul
const uint8_t STEP_RED   = STEP_GREEN * 3;  // passo do LED vermelho

// ---------------- Duty cycle atual de cada canal (em %) ----------------
uint8_t dutyRed_atual   = 0;
uint8_t dutyGreen_atual = 0;
uint8_t dutyBlue_atual  = 0;

unsigned long tempo_atual = 0;
const unsigned long intervalo_inc = 300; // intervalo entre incrementos

void setup() {
  delay(300);

  printf("=== Projeto 3 - Parte 1: PWM RGB ===\n");
  printf("Incrementos -> R: %u%% | G: %u%% | B: %u%%\n",
                STEP_RED, STEP_GREEN, STEP_BLUE);

  // Parametros da biblioteca
  ledcAttach(PIN_RED,   PWM_FREQ, PWM_RES);
  ledcAttach(PIN_GREEN, PWM_FREQ, PWM_RES);
  ledcAttach(PIN_BLUE,  PWM_FREQ, PWM_RES);
}

void loop() {
  if (millis() - tempo_atual >= intervalo_inc) {
    tempo_atual = millis();

    // Incrementa cada canal com seu próprio passo; reinicia ao passar de 100%
    dutyRed_atual += STEP_RED;
    if (dutyRed_atual > 100) dutyRed_atual = 0;

    dutyGreen_atual += STEP_GREEN;
    if (dutyGreen_atual > 100) dutyGreen_atual = 0;

    dutyBlue_atual += STEP_BLUE;
    if (dutyBlue_atual > 100) dutyBlue_atual = 0;

    // Converte de % (0-100) para valor de 8 bits (0-255) usando o mapeamento proporcional
    uint16_t dutyRed8   = map(dutyRed_atual,   0, 100, 0, PWM_MAX);
    uint16_t dutyGreen8 = map(dutyGreen_atual, 0, 100, 0, PWM_MAX);
    uint16_t dutyBlue8  = map(dutyBlue_atual,  0, 100, 0, PWM_MAX);

    // Aplica os duty cycles nos respectivos pinos PWM
    ledcWrite(PIN_RED,   dutyRed8);
    ledcWrite(PIN_GREEN, dutyGreen8);
    ledcWrite(PIN_BLUE,  dutyBlue8);

    // Printa os valores no serial monitor
    printf(
      "R: %3u%% (%3u/255) [passo %2u%%] | "
      "G: %3u%% (%3u/255) [passo %2u%%] | "
      "B: %3u%% (%3u/255) [passo %2u%%]\n",
      dutyRed_atual,   dutyRed8,   STEP_RED,
      dutyGreen_atual, dutyGreen8, STEP_GREEN,
      dutyBlue_atual,  dutyBlue8,  STEP_BLUE);
  }
}

/*
  SEL0433 - Aplicação de Microprocessadores
  Lucas Garcia Pereira - N°USP: 15496307
  Projeto 3 - Parte 2: Exercício 1
  Controle de posição de servomotor via potenciômetro
*/

#include <Arduino.h>
#include <ESP32Servo.h>

// ---- Pinos ----
const int PIN_POT   = 34;   // ADC1_CH6 - entrada analógica do potenciômetro
const int PIN_SERVO = 18;   // sinal PWM do servomotor

// ---- Parâmetros do servomotor ----
const int SERVO_MIN_DEG = 0;
const int SERVO_MAX_DEG = 180;

// Largura de pulso mínima e máxima em microssegundos (típico para SG90)
const int SERVO_MIN_US = 500;
const int SERVO_MAX_US = 2400;

Servo myServo;

// ---- Variáveis de controle ----
int lastAngle     = -1;          // ângulo anterior (para detectar mudança)
unsigned long lastPrint = 0;     // controle do intervalo de log serial

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println(F("=== Proj.3 - Parte 2 - Ex.1: Servo + Potenciômetro ==="));
  Serial.println(F("Biblioteca: ESP32Servo"));
  Serial.println(F("Gire o potenciômetro para controlar o ângulo do servo.\n"));

  // Aloca um timer PWM para o servo e associa ao pino
  myServo.setPeriodHertz(50);                   // frequência padrão de servo: 50 Hz
  myServo.attach(PIN_SERVO, SERVO_MIN_US, SERVO_MAX_US);

  // Posição inicial: centro (90°)
  myServo.write(90);
  Serial.println(F("Servo inicializado em 90°."));
}

void loop() {
  // Leitura do ADC (média de 8 amostras para reduzir ruído)
  long sumAdc = 0;
  for (int i = 0; i < 8; i++) {
    sumAdc += analogRead(PIN_POT);
    delayMicroseconds(500);
  }
  int adcVal = (int)(sumAdc / 8);

  // Converte de 0–4095 (12 bits ADC) para 0°–180°
  int angle = map(adcVal, 0, 4095, SERVO_MIN_DEG, SERVO_MAX_DEG);

  // Aplica apenas se houve variação >= 1° (evita jitter no servo)
  if (abs(angle - lastAngle) >= 1) {
    myServo.write(angle);
    lastAngle = angle;
  }

  // Log serial a cada 200 ms
  if (millis() - lastPrint >= 200) {
    lastPrint = millis();

    // Calcula duty cycle equivalente do sinal PWM do servo
    // Período = 20.000 µs (50 Hz)
    // Pulso = map(angle, 0, 180, MIN_US, MAX_US)
    int pulsoUs = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
    float dutyCycle = (float)pulsoUs / 20000.0f * 100.0f;

    Serial.printf("ADC: %4d | Angulo: %3d graus | Pulso: %4d us | Duty: %.2f%%\n",
                  adcVal, angle, pulsoUs, dutyCycle);
  }

  delay(20);  // taxa de atualização ~50 Hz, compatível com o servo
}

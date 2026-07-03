/*
  Projeto: Controle PWM e Comunicação - Parte 2, Exercício 2 (FINAL)
  Disciplina: SEL0433 - Aplicação de Microprocessadores
  Lucas Garcia Pereira - NºUSP: 15496307
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

// Configurações do Display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Definição dos Pinos
#define POT_PIN 34
#define SERVO_PIN 18
#define BUZZER_PIN 5

// Parâmetros do Servomotor (Valores padrão para a maioria dos servos SG90)
#define SERVO_MIN_PULSEWIDTH_US 500  // Largura de pulso mínima em microssegundos
#define SERVO_MAX_PULSEWIDTH_US 2500 // Largura de pulso máxima em microssegundos
#define SERVO_MAX_DEGREE        180  // Ângulo máximo em graus

// Variáveis de controle
int ciclos = 0;
int estado_ciclo = 0; // Máquina de estados: 0 (início), 1 (foi ao máximo), 2 (retornou ao mínimo)

void setup() {
  // Inicialização da Comunicação Serial exigida pelo projeto
  Serial.begin(115200);
  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Inicialização do Display OLED I2C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha na inicialização do OLED"));
    for(;;);
  }
  display.clearDisplay();

  // Configuração da biblioteca nativa MCPWM para o Servomotor
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_PIN);
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 50;    // Frequência de 50 Hz padrão para servomotores
  pwm_config.cmpr_a = 0;        // Duty cycle inicial
  pwm_config.cmpr_b = 0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
}

void loop() {
  // Leitura do Potenciômetro (ADC) e mapeamento para graus
  int pot_val = analogRead(POT_PIN);
  int angulo = map(pot_val, 0, 4095, 0, SERVO_MAX_DEGREE);

  // Calcula e aplica a largura de pulso em microssegundos via MCPWM
  uint32_t pulse_width = map(angulo, 0, SERVO_MAX_DEGREE, SERVO_MIN_PULSEWIDTH_US, SERVO_MAX_PULSEWIDTH_US);
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, pulse_width);

  // Lógica de detecção de ciclo completo
  if (estado_ciclo == 0 && angulo < 10) {
    estado_ciclo = 1; // Registrou posição inicial
  } else if (estado_ciclo == 1 && angulo > 170) {
    estado_ciclo = 2; // Chegou na extremidade oposta (meio ciclo)
  } else if (estado_ciclo == 2 && angulo < 10) {
    ciclos++;         // Retornou à origem: 1 ciclo completo
    estado_ciclo = 1; // Prepara para a contagem do próximo
  }

  // Monitoramento via Serial
  Serial.print("Ângulo do Servo: "); 
  Serial.print(angulo);
  Serial.print("° | Ciclos Concluídos: "); 
  Serial.println(ciclos);

  // Monitoramento via Display OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Controle MCPWM");
  display.printf("Angulo: %d\n", angulo);
  display.printf("Ciclos: %d\n", ciclos);
  display.display();

  // Rotina de Alerta (3 ciclos)
  if (ciclos >= 3) {
    Serial.println("ALERTA: 3 ciclos atingidos! Acionando buzzer.");
    
    // Atualiza o OLED para mostrar o alerta
    display.setCursor(0, 40);
    display.println("ALERTA ATIVADO!");
    display.display();

    digitalWrite(BUZZER_PIN, HIGH);
    delay(3000); // Mantém o alerta sonoro por 3 segundos
    digitalWrite(BUZZER_PIN, LOW);

    // Reinicia a rotina em loop conforme solicitado
    ciclos = 0; 
    estado_ciclo = 0;
  }

  delay(50); // Pequeno atraso para estabilidade da leitura e simulação
}
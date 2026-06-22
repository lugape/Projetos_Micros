// Projeto 2: Aferidor de temperatura de forno industrial
// Lucas Garcia Pereira  NUSP: 15496307

sbit LCD_RS at LATD4_bit;
sbit LCD_EN at LATD5_bit;
sbit LCD_D4 at LATD0_bit;
sbit LCD_D5 at LATD1_bit;
sbit LCD_D6 at LATD2_bit;
sbit LCD_D7 at LATD3_bit;

sbit LCD_RS_Direction at TRISD4_bit;
sbit LCD_EN_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD0_bit;
sbit LCD_D5_Direction at TRISD1_bit;
sbit LCD_D6_Direction at TRISD2_bit;
sbit LCD_D7_Direction at TRISD3_bit;

// LED do forno

sbit LED_RESISTENCIA at LATC0_bit;
sbit LED_RESISTENCIA_Direction at TRISC0_bit;

// Variáveis Globais

volatile unsigned char tempo_restante = 0;
volatile unsigned char flag_atualiza  = 0;
volatile unsigned char contador_tmr1  = 0;

void interrupt() {

    // Interrupcao externa INT0: botao de 60 segundos
    if (INTCON.INT0IF == 1) {
        tempo_restante = 60;
        flag_atualiza  = 1;

        // Para o Timer1 antes de ligar o Timer0
        T1CON.TMR1ON   = 0;
        PIR1.TMR1IF    = 0;
        contador_tmr1  = 0;

        // Carrega e liga Timer0
        TMR0H = 0xC2;
        TMR0L = 0xF7;
        INTCON.TMR0IF  = 0;
        T0CON.TMR0ON   = 1;

        INTCON.INT0IF = 0;
    }

    // Interrupcao externa INT1: botao de 10 segundos
    if (INTCON3.INT1IF == 1) {
        tempo_restante = 10;
        flag_atualiza  = 1;
        contador_tmr1  = 0;

        // Para o Timer0 antes de ligar o Timer1
        T0CON.TMR0ON   = 0;
        INTCON.TMR0IF  = 0;

        TMR1H = 0x0B;
        TMR1L = 0xDC;
        PIR1.TMR1IF    = 0;
        T1CON.TMR1ON   = 1;

        INTCON3.INT1IF = 0;
    }

    // Overflow do Timer0: dispara a cada 1 segundo
    if (INTCON.TMR0IF == 1) {
        INTCON.TMR0IF = 0;

        TMR0H = 0xC2;
        TMR0L = 0xF7;

        if (tempo_restante > 0) {
            tempo_restante--;
            flag_atualiza = 1;
        }
        // Quando chega a zero, para o timer
        if (tempo_restante == 0) {
            T0CON.TMR0ON  = 0;
        }
    }

    // Overflow do Timer1: dispara a cada 250 ms
    if (PIR1.TMR1IF == 1) {
        PIR1.TMR1IF = 0;

        TMR1H = 0x0B;
        TMR1L = 0xDC;

        contador_tmr1++;

        if (contador_tmr1 >= 4) {
            contador_tmr1 = 0;

            if (tempo_restante > 0) {
                tempo_restante--;
                flag_atualiza = 1;
            }
            // Quando chega a zero, para o timer
            if (tempo_restante == 0) {
                T1CON.TMR1ON  = 0;
            }
        }
    }
}

void main() {
    char buffer_conversao[4];
    unsigned int  leitura_adc;
    unsigned long temp_x10;
    unsigned char parte_inteira;
    unsigned char parte_decimal;

    // Força a operação em 8MHz
    OSCCON = 0x72;

    // Configurações das portas digitais
    TRISB.RB0 = 1; // Botao 1 (60 s) como entrada - INT0
    TRISB.RB1 = 1; // Botao 2 (10 s) como entrada - INT1

    LED_RESISTENCIA_Direction = 0; // RC0 como saida (LED da resistencia)
    LED_RESISTENCIA = 0;

    CMCON  = 7;    // Comparadores desligados

    // Configuracao do ADC (leitura do LM35)
    TRISA.RA0 = 1; // RA0/AN0  -> entrada analogica
    TRISA.RA2 = 1; // RA2/AN2  -> entrada (Vref-)
    TRISA.RA3 = 1; // RA3/AN3  -> entrada (Vref+)

    ADC_Init();    // Inicializa o modulo ADC (biblioteca MikroC)

    ADCON1 = 0x3B;
    ADCON2 = 0x92;

    // Timer0: 16 bits, clock interno, prescaler 1:128
    T0CON = 0b00000110;

    // Timer1: prescaler 1:8, clock interno, TMR1ON=0
    T1CON = 0b10110000;

    // Interrupcoes
    RCON.IPEN         = 0; // Sem prioridade de interrupcao
    INTCON.GIE        = 1; // Habilita interrupcoes globais
    INTCON.PEIE       = 1; // Habilita interrupcoes de perifericos

    INTCON2.INTEDG0   = 0; // INT0 dispara ao pressionar o botao
    INTCON2.INTEDG1   = 0; // INT1 dispara ao pressionar o botao

    INTCON.INT0IE     = 1; // Habilita INT0
    INTCON3.INT1IE    = 1; // Habilita INT1
    INTCON.TMR0IE     = 1; // Habilita interrupcao do TMR0
    PIE1.TMR1IE       = 1; // Habilita interrupcao do TMR1

    // Limpa todas as flags
    INTCON.INT0IF     = 0;
    INTCON3.INT1IF    = 0;
    INTCON.TMR0IF     = 0;
    PIR1.TMR1IF       = 0;

    // Inicializacao do display LCD
    Delay_ms(100);

    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    
    Delay_ms(10);

    Lcd_Out(1, 1, "Tempo:");
    Lcd_Out(1, 10, "s");
    Lcd_Out(1, 7, "  0");

    Lcd_Out(2, 1, "Temp:");

    while (1) {

        // ------------------- Contagem regressiva (LCD linha 1) -------------
        if (flag_atualiza == 1) {
            flag_atualiza = 0;

            ByteToStr(tempo_restante, buffer_conversao);

            Lcd_Chr(1, 7, buffer_conversao[0]);
            Lcd_Chr(1, 8, buffer_conversao[1]);
            Lcd_Chr(1, 9, buffer_conversao[2]);
        }

        // ------------------- Leitura continua de temperatura ---------------
        leitura_adc = ADC_Get_Sample(0);

        temp_x10 = ((unsigned long)leitura_adc * 1000UL) / 1023UL;

        parte_inteira  = (unsigned char)(temp_x10 / 10);
        parte_decimal  = (unsigned char)(temp_x10 % 10);

        ByteToStr(parte_inteira, buffer_conversao);
        Lcd_Chr(2, 6, buffer_conversao[0]);
        Lcd_Chr(2, 7, buffer_conversao[1]);
        Lcd_Chr(2, 8, buffer_conversao[2]);
        Lcd_Chr(2, 9, '.');
        Lcd_Chr(2, 10, parte_decimal + 48);
        Lcd_Chr(2, 11, ' ');
        Lcd_Chr(2, 12, 0xDF);
        Lcd_Chr(2, 13, 'C');

        // ------------------- LED da resistencia do forno --------------------
        if (temp_x10 < 600) {
            LED_RESISTENCIA = 1;
        } else if (temp_x10 > 800) {
            LED_RESISTENCIA = 0;
        }

        Delay_ms(200);
    }
}
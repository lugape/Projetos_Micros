;Lucas Garcia Pereira - NºUSP: 15496307
;Alexsandra Pavani Xavier - NºUSP: 14681372

;Entrega Projeto - Final

ORG 0000h
    LJMP inicio

; Interrupção do Timer 1
ORG 001Bh
    ACALL reset_timer          ; Chama a subrotina de reset quando atinge 10 voltas
    RETI                       ; Retorna da interrupção

ORG 0030h
inicio:
    ; Configuração do Timer 1: Modo 1 (16 bits), Contador Externo (C/T=1)
    MOV TMOD, #01010000b
    ACALL reset_timer          ; Inicializa o timer com o valor de contagem
    
    ; Habilita Interrupções
    SETB ET1                   ; Habilita interrupção do Timer 1
    SETB EA                    ; Habilita interrupção global
    SETB TR1                   ; Liga o contador

    ; Sincronização inicial da direção
    MOV C, P2.0
    MOV F0, C

loop:
    ACALL verifica_direcao     ; Verifica mudança de chave e atualiza motor
    ACALL atualiza_display     ; Atualiza o número e o ponto decimal
    SJMP loop

; Subrotina de Verificação de Direção (Com detecção de mudança)
verifica_direcao:
    MOV C, P2.0
    JNB F0, era_zero           ; Se era 0, verifica se mudou para 1

    ; Caso anterior era 1
    JC motor_conf               ; Continua igual
    ACALL reset_timer          ; MUDOU: Reseta contador
    CLR F0
    SJMP motor_conf

era_zero:
    ; Caso anterior era 0
    JNC motor_conf             ; Continua igual
    ACALL reset_timer          ; MUDOU: Reseta contador
    SETB F0

motor_conf:
    JB F0, antihorario
    ; Horário
    SETB P3.1
    CLR P3.0
    RET
antihorario:
    ; Anti-horário
    SETB P3.0
    CLR P3.1
    RET

; --- Subrotina de Reset (Chamada por Interrupção ou Mudança de Direção) ---
reset_timer:
    CLR TR1
    ; Para contar 10 eventos até o overflow (0FFFFh + 1):
    ; 65536 - 10 = 65526 (FFF6h)
    MOV TH1, #0FFh             
    MOV TL1, #0F6h             
    SETB TR1
    RET

; Subrotina de Atualização do Display com Ponto Decimal
atualiza_display:
    ; Calcula o valor atual da contagem (Diferença entre atual e o preset F6h)
    MOV A, TL1
    CLR C
    SUBB A, #0F6h              ; Transforma o valor bruto (F6-FF) em 0-9
    
    MOV DPTR, #tabela
    MOVC A, @A+DPTR            ; Pega o padrão do número (0-9)
    
    ; Inserção do Ponto Decimal (P1.7) baseado no F0
    JNB F0, ponto_off          ; Se F0=0, ponto apaga (em Anodo Comum, bit 7 = 1)
    CLR ACC.7                  ; Se F0=1, ponto acende (bit 7 = 0)
    SJMP envia_1
ponto_off:
    SETB ACC.7                 ; Garante bit 7 em 1 (apagado)
envia_1:
    MOV P1, A
    RET

; Tabela de Conversão (Ânodo Comum)
tabela:
    DB 0C0h, 0F9h, 0A4h, 0B0h, 099h, 092h, 082h, 0F8h, 080h, 090h ; em ordem de 0 a 9

END
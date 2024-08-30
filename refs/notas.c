#include <msp430.h>

// Definição das frequências das notas musicais (em Hz)
#define NOTA_DO  261
#define NOTA_RE  294
#define NOTA_MI  330
#define NOTA_FA  349
#define NOTA_SOL 392
#define NOTA_LA  440
#define NOTA_SI  494

// Função para configurar o PWM no pino do buzzer
void setupPWM(unsigned int frequency) {
    TA0CTL = TASSEL_2 + MC_1;      // SMCLK, modo up
    TA0CCR0 = 1000000 / frequency; // Configura o período do PWM (1MHz / frequência)
    TA0CCTL1 = OUTMOD_7;           // Saída PWM reset/set
    TA0CCR1 = TA0CCR0 / 2;         // Duty cycle 50%
    P1DIR |= BIT2;                 // P1.2 como saída
    P1SEL |= BIT2;                 // P1.2 com função alternativa (PWM)
}

void stopPWM() {
    TA0CTL = MC_0;                 // Para o timer
    P1SEL &= ~BIT2;                // Desativa a função alternativa para P1.2
    P1OUT &= ~BIT2;                // Desliga o pino do buzzer
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    while(1) {
        // Toca Dó
        setupPWM(NOTA_DO);
        __delay_cycles(1000000); // 1 segundo
        stopPWM();
        __delay_cycles(1000000); // Pausa de 1 segundo

        // Toca Ré
        setupPWM(NOTA_RE);
        __delay_cycles(1000000); // 1 segundo
        stopPWM();
        __delay_cycles(1000000); // Pausa de 1 segundo

        // Toca Mi
        setupPWM(NOTA_MI);
        __delay_cycles(1000000); // 1 segundo
        stopPWM();
        __delay_cycles(1000000); // Pausa de 1 segundo

        // Toca Fá
        setupPWM(NOTA_FA);
        __delay_cycles(1000000); // 1 segundo
        stopPWM();
        __delay_cycles(1000000); // Pausa de 1 segundo

        // Toca Sol
        setupPWM(NOTA_SOL);
        __delay_cycles(1000000); // 1 segundo
        stopPWM();
        __delay_cycles(1000000); // Pausa de 1 segundo

        // Toca Lá
        setupPWM(NOTA_LA);
        __delay_cycles(1000000); // 1 segundo
        stopPWM();
        __delay_cycles(1000000); // Pausa de 1 segundo

        // Toca Si
        setupPWM(NOTA_SI);
        __delay_cycles(1000000); // 1 segundo
        stopPWM();
        __delay_cycles(1000000); // Pausa de 1 segundo
    }

    return 0;
}

#include <msp430.h>
#include "def_principais.h"
#include "LCD.h"

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

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    unsigned int i, j = 0;
    unsigned char tecla;
    unsigned char teclado[4][4] =  {{'1', '2', '3', 'A'},
                                    {'4', '5', '6', 'B'},
                                    {'7', '8', '9', 'C'},
                                    {'*', '0', '#', 'D'}};

    P6DIR = 0xFF;            // P6.0-P6.3 como saída para dados do LCD
    P3DIR = 0xFF;            // P3.2-P3.4 como saída para controle do LCD

    P4DIR |= 0b00011110;     // P4.1-P4.4 como saída para colunas do teclado, P4.0 LED para debug
    P4OUT |= 0b00011110;

    P2DIR &= 0b10000111;     // P2.3-P2.6 como entrada para linhas do teclado
    P2REN |= 0b01111000;
    P2OUT |= 0b01111000;

    set_bit(CONTR_LCD, BkL);

    inic_LCD_4bits(); // Inicializa o LCD

    cmd_LCD(1,0); // Limpa o display
    __delay_cycles(100000); // Delay para estabilização

    escreve_LCD("Teclado Musical!");
    cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
    escreve_LCD("# # # # # # # #");

    unsigned char menu_option = 0; // Variável para rastrear a opção do menu
    unsigned char menu_active = 0; // Variável para saber se o menu está ativo
    int seq = 0;

    while(1)
    {
        for(i=0; i<4; i++) // Varredura das colunas
        {
            clr_bit(P4OUT, i+1); // Testa cada coluna (P4.1-P4.4)
            for(j=0; j<4; j++) // Varredura das linhas
            {
                if(!tst_bit(P2IN, j+3)) // Testa se a linha está pressionada
                {
                    tecla = teclado[j][i]; // Captura a tecla pressionada

                    if (menu_active) {
                        // Se o menu estiver ativo, processa as opções do menu
                        switch(tecla) {
                            case '6':  // Mais
                                menu_option++;
                                if (menu_option > 2) menu_option = 0; // Loop de opções
                                break;
                            case '9':  // Menos
                                menu_option--;
                                if (menu_option > 2) menu_option = 2; // Loop de opções
                                break;
                            case '#':  // Selecionar
                                if (menu_option == 0) {
                                    cmd_LCD(1, 0);
                                    escreve_LCD("DoReMiFa");
                                    // Música "DoReMiFa"
                                    int doremi[] = {NOTA_DO, NOTA_RE, NOTA_MI, NOTA_FA, NOTA_FA, NOTA_FA, NOTA_DO, NOTA_RE, NOTA_DO, NOTA_RE, NOTA_RE, NOTA_RE};
                                    int num_notas = sizeof(doremi) / sizeof(doremi[0]);

                                    for (seq = 0; seq < num_notas; seq++) {
                                        setupPWM(doremi[seq]);
                                        __delay_cycles(150000);
                                        stopPWM();
                                        __delay_cycles(150000);
                                    }

                                } else if (menu_option == 1) {
                                    cmd_LCD(1, 0);
                                    escreve_LCD("Happy Birthday");
                                    // Música "Happy Birthday"
                                    int happy[] = {NOTA_DO, NOTA_DO, NOTA_RE, NOTA_DO, NOTA_FA, NOTA_MI, NOTA_DO, NOTA_DO, NOTA_RE, NOTA_DO, NOTA_RE, NOTA_RE};
                                    int num_notas = sizeof(happy) / sizeof(happy[0]);

                                    for (seq = 0; seq < num_notas; seq++) {
                                        setupPWM(happy[seq]);
                                        __delay_cycles(150000); // Ajuste o delay conforme necessário
                                        stopPWM();
                                        __delay_cycles(150000); // Ajuste o delay conforme necessário
                                    }

                                } else if (menu_option == 2) {
                                    cmd_LCD(1, 0);
                                    escreve_LCD("Ode The Joy");
                                    // Música "Ode to Joy"
                                    int ode[] = {NOTA_MI, NOTA_MI, NOTA_FA, NOTA_SOL, NOTA_SOL, NOTA_FA, NOTA_RE, NOTA_DO, NOTA_DO, NOTA_RE, NOTA_MI, NOTA_RE, NOTA_RE, NOTA_DO, NOTA_DO};
                                    int num_notas = sizeof(ode) / sizeof(ode[0]);

                                    for (seq = 0; seq < num_notas; seq++) {
                                        setupPWM(ode[seq]);
                                        __delay_cycles(150000); // Ajuste o delay conforme necessário
                                        stopPWM();
                                        __delay_cycles(150000); // Ajuste o delay conforme necessário
                                    }
                                }

                                // Aguarda até que a tecla seja liberada
                                while(!tst_bit(P2IN, j+3));
                                break;
                            case '3':  // Voltar
                                menu_active = 0; // Sai do menu
                                cmd_LCD(1, 0);
                                escreve_LCD("Teclado Musical!");
                                cmd_LCD(0xC0, 0);
                                break;
                        }

                        // Atualiza o menu exibido
                        if (menu_active) {
                            cmd_LCD(1, 0); // Limpa o display
                            if (menu_option == 0) {
                                cmd_LCD(1, 0); // Limpa o display
                                escreve_LCD("    Musics");
                                cmd_LCD(0xC0, 0); // Move o cursor para a segunda linha
                                escreve_LCD("DoReMi-_____-___");
                            } else if (menu_option == 1) {
                                cmd_LCD(1, 0); // Limpa o display
                                escreve_LCD("    Musics");
                                cmd_LCD(0xC0, 0); // Move o cursor para a segunda linha
                                escreve_LCD("______-Happy-___");
                            } else if (menu_option == 2) {
                                cmd_LCD(1, 0); // Limpa o display
                                escreve_LCD("    Musics");
                                cmd_LCD(0xC0, 0); // Move o cursor para a segunda linha
                                escreve_LCD("______-_____-Ode");
                            }
                        }

                    } else {
                        // Se o menu não estiver ativo
                        switch(tecla) {
                            case '*':
                                menu_active = 1; // Ativa o menu
                                cmd_LCD(1, 0); // Limpa o display
                                escreve_LCD("    Musics");
                                cmd_LCD(0xC0, 0); // Move o cursor para a segunda linha
                                escreve_LCD("DoReMi-Happy-Ode");
                                break;
                            case '7':
                                // Toca Dó
                                setupPWM(NOTA_DO);
                                cmd_LCD(1, 0);  // Comando para limpar o display
                                escreve_LCD("    Nota: Do");
                                cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
                                escreve_LCD("Frequencia:261Hz");
                                break;
                            case '4':
                                // Toca Ré
                                setupPWM(NOTA_RE);
                                cmd_LCD(1, 0);
                                escreve_LCD("    Nota: Re");
                                cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
                                escreve_LCD("Frequencia:294Hz");
                                break;
                            case '1':
                                // Toca Mi
                                setupPWM(NOTA_MI);
                                cmd_LCD(1, 0);
                                escreve_LCD("    Nota: Mi");
                                cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
                                escreve_LCD("Frequencia:330Hz");
                                break;
                            case '0':
                                // Toca Fá
                                setupPWM(NOTA_FA);
                                cmd_LCD(1, 0);
                                escreve_LCD("    Nota: Fa");
                                cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
                                escreve_LCD("Frequencia:349Hz");
                                break;
                            case '8':
                                // Toca Sol
                                setupPWM(NOTA_SOL);
                                cmd_LCD(1, 0);
                                escreve_LCD("    Nota: Sol");
                                cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
                                escreve_LCD("Frequencia:392Hz");
                                break;
                            case '5':
                                // Toca Lá
                                setupPWM(NOTA_LA);
                                cmd_LCD(1, 0);
                                escreve_LCD("    Nota: La");
                                cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
                                escreve_LCD("Frequencia:440Hz");
                                break;
                            case '2':
                                // Toca Si
                                setupPWM(NOTA_SI);
                                cmd_LCD(1, 0);
                                escreve_LCD("    Nota: Si");
                                cmd_LCD(0xC0,0); // Move o cursor para a segunda linha
                                escreve_LCD("Frequencia:494Hz");
                                break;
                            case '#':
                                // Selecionar
                                cmd_LCD(1, 0);
                                escreve_LCD("Selecionar");
                                break;
                            case '3':
                                // Voltar
                                cmd_LCD(1, 0);
                                escreve_LCD("Voltar");
                                break;
                            default:
                                stopPWM();
                                break;
                        }
                        // Aguarda até que a tecla seja liberada
                        while(!tst_bit(P2IN, j+3));
                    }

                    // Aguarda até que a tecla seja liberada
                    while(!tst_bit(P2IN, j+3));
                    stopPWM();
                }
            }
            set_bit(P4OUT, i+1); // Restaura o estado da coluna (P4.1-P4.4)
        }
    }

    return 0;
}

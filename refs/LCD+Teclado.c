#include <msp430.h>
#include "def_principais.h"
#include "LCD.h"

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    unsigned int i, j = 0;
    unsigned char teclado[4][3] =  {{'1', '2', '3', 'A'},
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

    while(1)
    {
        for(i=0; i<4; i++) // Varredura das colunas
        {
            clr_bit(P4OUT, i+1); // Testa cada coluna (P4.1-P4.4)
            for(j=0; j<4; j++) // Varredura das linhas
            {
                if(!tst_bit(P2IN, j+3)) // Testa se a linha está pressionada
                {
                    cmd_LCD(0x80,0); // Move o cursor para o início da primeira linha
                    escreve_LCD("Nota: ");
                    cmd_LCD(teclado[j][i],1); // Exibe a tecla pressionada no LCD

                    // Aguarda até que a tecla seja liberada
                    while(!tst_bit(P2IN, j+3));
                }
            }
            set_bit(P4OUT, i+1); // Restaura o estado da coluna (P4.1-P4.4)
        }
    }

    return 0;
}

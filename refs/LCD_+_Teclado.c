#include <msp430.h> 

#include "def_principais.h"
#include "LCD.h"

/**
 * main.c
 */
int main(void)
{
    unsigned int k,i,j = 0;
    unsigned char teclado[4][4] =  {{'1', '4', '7', '*'},
                                    {'2', '5', '8', '0'},
                                    {'3', '6', '9', '#'},
                                    {'A', 'B', 'C', 'D'}};

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P6DIR = 0xFF;           // P6.0-P6.3 dedicados aos dados do LCD
    P3DIR = 0xFF;           // P3.2-P3.4 dedicados ao controle do LCD

    P1DIR |= 0b00111101;    // P1.2-P1.5 dedicados à escrita nas colunas do teclado, P1.0 LED para debug
    P1OUT |= 0b00111100;

    P2DIR &= 0b10000111;    // P2.3-P2.6 dedicados à leitura das linhas do teclado
    P2REN |= 0b01111000;
    P2OUT |= 0b01111000;

    set_bit(CONTR_LCD, BkL);

    inic_LCD_4bits(); // inicializa o LCD

    cmd_LCD(1,0);
    __delay_cycles(100000);     // delay


    escreve_LCD(" Teclado + LCD!");

    cmd_LCD(0xC0,0);

    while(1)
    {
        for(i=0; i<4; i++) // varredura coluna
        {
            clr_bit(P1OUT, i+2); // testa cada coluna
            for(j=0; j<4; j++) // varredura linha
            {
                if(!tst_bit(P2IN, j+3)) // testa a linha
                {
                    cpl_bit(P1OUT,0);
                    cmd_LCD(teclado[j][i],1); // coloca no display o valor definido pela função
                    while(!tst_bit(P2IN, j+3));
                }
            }
            set_bit(P1OUT, i+2);
        }
    }
    return 0;
}

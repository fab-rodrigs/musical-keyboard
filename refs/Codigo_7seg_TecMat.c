#include <msp430.h> 

#define set_bit(y,bit)  (y|=(1<<bit))   //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit)  (y&=~(1<<bit))  //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit)  (y^=(1<<bit))   //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit)  (y&(1<<bit))    //retorna 0 ou 1 conforme leitura do bit

#define DISPLAY P6OUT  // P6.0-P6.7 dedicados ao display 7seg

//const unsigned char Tabela[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E};
const unsigned char Tabela[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    unsigned int k,i,j = 0;
    unsigned char teclado[4][4] =  {{1, 2, 3, 11},
                                    {4, 5, 6, 12},
                                    {7, 8, 9, 13},
                                    {14, 0, 15, 16}};

    P6DIR = 0xFF;           // Port 6 dedicado à escrita no display 7seg
    P6OUT = 0x00;

    P1DIR |= 0b00111101;    // P1.2-P1.5 dedicados à escrita nas colunas do teclado, P1.0 LED para debug
    P1OUT |= 0b00111100;

    P2DIR &= 0b10000111;    // P2.3-P2.6 dedicados à leitura das linhas do teclado
    P2REN |= 0b01111000;
    P2OUT |= 0b01111000;

    while(1)
    {
        /*
        P6OUT = Tabela[0];
        k++;
        if(k>15)
        {
            k=0;
        }
        // Delay
        for(i=65000; i>0; i--);
        */

        for(i=0; i<4; i++) // varredura coluna
        {
            clr_bit(P1OUT, i+2); // testa cada coluna

            for(j=0; j<4; j++) // varredura linha
            {
                if(!tst_bit(P2IN, j+3)) // testa a linha
                {
                    cpl_bit(P1OUT,0);
                    DISPLAY = Tabela[teclado[j][i]]; // coloca no display o valor definido pela função
                    while(!tst_bit(P2IN, j+3));
                }
            }
            set_bit(P1OUT, i+2);
        }
    }
}

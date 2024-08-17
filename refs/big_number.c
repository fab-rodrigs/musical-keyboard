#include <msp430.h> 
#include "def_principais.h"
#include "LCD.h"

//informações para criar novos caracteres, armazenadas na memória flash
const unsigned char novos_caract[]={0b00000001,//0
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00011111,
                                            0b00011111,//1
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00011111,//2
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,//3
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00011111,//4
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00011111,
                                            0b00011111,//5
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00011111,
                                            0b00011111,//6
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00011111,//7
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00011111};

const unsigned char nr_grande[10][4] = { {0x01, 0x02, 0x4C, 0x00}, //nr. 0
                                                {0x20, 0x7C, 0x20, 0x7C}, //nr. 1
                                                {0x04, 0x05, 0x4C, 0x5F}, //nr. 2
                                                {0x06, 0x05, 0x5F, 0x00}, //nr. 3
                                                {0x4C, 0x00, 0x20, 0x03}, //nr. 4
                                                {0x07, 0x04, 0x5F, 0x00}, //nr. 5
                                                {0x07, 0x04, 0x4C, 0x00}, //nr. 5
                                                {0x06, 0x02, 0x20, 0x03}, //nr. 7
                                                {0x07, 0x05, 0x4C, 0x00}, //nr. 8
                                                {0x07, 0x05, 0x20, 0x03}};//nr. 9
//-----------------------------------------------------------------------------------------------------------
void cria_novos_caract()//criação dos 8 novos caracteres
{
    unsigned char i, k, j=0, n=0x40;

    for(i=0;i<8;i++)
    {
        cmd_LCD(n,0);       //endereco base para gravar novo segmento
        for(k=0;k<7;k++)
            cmd_LCD(novos_caract[k+j],1);

        cmd_LCD(0x00,1);    //apaga ultima posição do end. da CGRAM para evitar algum dado espúrio
        j += 7;
        n += 8;
    }
}
//-----------------------------------------------------------------------------------------------------------
void escreve_BIG(unsigned char end, unsigned char nr)//escreve um número grandes no LCD
{
     cmd_LCD(end,0);                                //endereco de incio de escrita (1a linha)
     cmd_LCD(nr_grande[nr][0],1);
     cmd_LCD(nr_grande[nr][1],1);
     cmd_LCD(end+64,0);                             //desloca para a 2a linha
     cmd_LCD(nr_grande[nr][2],1);
     cmd_LCD(nr_grande[nr][3],1);
}


/**
 * main.c
 */
int main(void)
{
    unsigned char k,i,j = 0;
    unsigned char vet1[] = {1,3,7,15,31,63,127};
    unsigned char teclado[4][4] =  {{'1', '4', '7', '*'},
                                    {'2', '5', '8', '0'},
                                    {'3', '6', '9', '#'},
                                    {'A', 'B', 'C', 'D'}};

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P6DIR = 0xFF;           // P6.0-P6.3 dedicados aos dados do LCD
    P3DIR = 0xFF;           // P3.2-P3.4 dedicados ao controle do LCD

    set_bit(CONTR_LCD, BkL);

    inic_LCD_4bits(); // inicializa o LCD

    cria_novos_caract();            //cria os 8 novos caracteres

    //escreve os números 0, 1, 2, 3, 4 e 5
    escreve_BIG(0x80,0);
    escreve_BIG(0x82,1);
    escreve_BIG(0x85,2);
    escreve_BIG(0x88,3);
    escreve_BIG(0x8B,4);
    escreve_BIG(0x8E,5);

    while(1)
    {
        
    }
    return 0;
}

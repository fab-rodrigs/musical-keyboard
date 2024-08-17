/*
 * 1ª. Atividade Avaliativa (MCA20304 – Microcontroladores I)
 *
 * Aluno: Fabrício Rodrigues de Santana
 *
 * Desenvolva um programa para o MSP430 de maneira a implementar um relógio digital utilizando números grandes
 * no LCD. Um botão deve modificar o conteúdo da informação apresentada no LCD, de maneira a alternar entre os
 * valores da data (dd/mm/aa), horário (hh:mm:ss) e cronômetro (sss:ms). Um segundo botão deve ser empregado para
 * iniciar ou parar a contagem do cronômetro. Ambos os botões devem ser configurados para fazer uso de interrupções.
 *
 */

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

const unsigned char nr_grande[12][4] = { {0x01, 0x02, 0x4C, 0x00}, //nr. 0
                                        {0x20, 0x7C, 0x20, 0x7C}, //nr. 1
                                        {0x04, 0x05, 0x4C, 0x5F}, //nr. 2
                                        {0x06, 0x05, 0x5F, 0x00}, //nr. 3
                                        {0x4C, 0x00, 0x20, 0x03}, //nr. 4
                                        {0x07, 0x04, 0x5F, 0x00}, //nr. 5
                                        {0x07, 0x04, 0x4C, 0x00}, //nr. 5
                                        {0x06, 0x02, 0x20, 0x03}, //nr. 7
                                        {0x07, 0x05, 0x4C, 0x00}, //nr. 8
                                        {0x07, 0x05, 0x20, 0x03}, //nr. 9
                                        {0x01, 0x02, 0x4C, 0x00},
                                        {0x20, 0x20, 0x20, 0x20}};//vazio


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
//-----------------------------------------------------------------------------------------------------------


int state, stop = 0;

// Inicio no dia 30/12/23
int dia_uni = 0;
int dia_dez = 3;
int mes_uni = 2;
int mes_dez = 1;
int ano_dez = 2;
int ano_uni = 3;

// Inicio na hora 23:59:00
int hor_dez = 2;
int hor_uni = 3;
int min_uni = 9;
int min_dez = 5;
int seg_uni, seg_dez = 0;

int s_uni, s_dez, s_cen, ms_dez, ms_uni = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P6DIR = 0xFF;               // P6.0-P6.3 dedicados aos dados do LCD
    P3DIR = 0xFF;               // P3.2-P3.4 dedicados ao controle do LCD

    // Configura interrupções botão 1 (alterna o que é mostrado no display)
    P1DIR = 0b00000000;         // Define P1.1 como entrada
    P1REN |= 0b00000010;        // Habilita o resistor de pull-up/pull-down para P1.1
    P1OUT |= 0b00000010;        // Configura P1.1 como pull-up (nível alto)
    P1IES = 0b00000000;         // Configura borda da interrupção no P1.1
    P1IE = 0b00000010;          // Habilita interrupção externa no P1.1

    // Configura interrupções botão 2 (inicia ou para a contagem do cronômetro)
    P2DIR = 0b00000000;         // Define P2.1 como entrada
    P2REN |= 0b00000010;        // Habilita o resistor de pull-up/pull-down para P2.1
    P2OUT |= 0b00000010;        // Configura P2.1 como pull-up (nível alto)
    P2IES = 0b00000000;         // Configura borda da interrupção no P2.1
    P2IE = 0b00000010;          // Habilita interrupção externa no P2.1

    __bis_SR_register(GIE);     // Habilita interrupções globais

    set_bit(CONTR_LCD, BkL);
    inic_LCD_4bits();           // Inicializa o LCD
    cria_novos_caract();        // Cria os 8 novos caracteres

    while(1)
    {
        if(state == 1 || state == 2)
            __delay_cycles(1000000); // Tempo seg
        else
            __delay_cycles(1000);    // Tempo ms
        seg_uni++;
        if(seg_uni>9)
        {
            seg_uni = 0;
            seg_dez++;
            if(seg_dez>5)
            {
                seg_dez = 0;
                min_uni++;
                if(min_uni>9)
                {
                    min_uni = 0;
                    min_dez++;
                    if(min_dez>5)
                    {
                        min_dez = 0;
                        hor_uni++;
                        if(hor_uni>9)
                        {
                            hor_uni = 0;
                            hor_dez++;
                            if(hor_dez>5)
                                hor_dez = 5;
                        }
                        else if(hor_dez==2 && hor_uni==4)
                        {
                            hor_uni = 0;
                            hor_dez = 0;
                            dia_uni++;
                            if(dia_dez<3 && dia_uni>9)
                            {
                                dia_uni = 0;
                                dia_dez++;
                            }
                            else if(dia_dez==3 && dia_uni==1)
                            {
                                dia_uni = 1;
                                dia_dez = 0;
                                mes_uni++;
                                if(mes_uni>9 && mes_dez==0)
                                {
                                    mes_uni = 0;
                                    mes_dez++;
                                }
                                else if(mes_uni>2 && mes_dez==1)
                                {
                                    mes_dez = 0;
                                    mes_uni = 1;
                                    ano_uni++;
                                    if(ano_uni>9)
                                    {
                                        ano_uni = 0;
                                        ano_dez++;
                                        if(ano_dez>9)
                                            ano_dez = 9;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        switch(state)
        {
            case 0:             // início
                escreve_BIG(0x80,11);
                escreve_BIG(0x82,11);
                escreve_BIG(0x85,0);
                escreve_BIG(0x88,0);
                escreve_BIG(0x8B,11);
                escreve_BIG(0x8E,11);
                break;
            case 1:             // data (dd/mm/aa)
                escreve_BIG(0x80,dia_dez);
                escreve_BIG(0x82,dia_uni);
                cmd_LCD(0x84,0);
                cmd_LCD('/',1);
                cmd_LCD(0xC4,0);
                cmd_LCD('/',1);
                escreve_BIG(0x85,mes_dez);
                escreve_BIG(0x88,mes_uni);
                cmd_LCD(0x8A,0);
                cmd_LCD('/',1);
                cmd_LCD(0xCA,0);
                cmd_LCD('/',1);
                escreve_BIG(0x8B,ano_dez);
                escreve_BIG(0x8E,ano_uni);
                break;
            case 2:             // horário (hh:mm:ss)
                escreve_BIG(0x80,hor_dez);
                escreve_BIG(0x82,hor_uni);
                cmd_LCD(0x84,0);
                cmd_LCD('.',1);
                cmd_LCD(0xC4,0);
                cmd_LCD('.',1);
                escreve_BIG(0x85,min_dez);
                escreve_BIG(0x88,min_uni);
                cmd_LCD(0x8A,0);
                cmd_LCD('.',1);
                cmd_LCD(0xCA,0);
                cmd_LCD('.',1);
                escreve_BIG(0x8B,seg_dez);
                escreve_BIG(0x8E,seg_uni);
                break;
            case 3:             // cronômetro (sss:ms)
                escreve_BIG(0x80,11);
                escreve_BIG(0x82,s_cen);
                cmd_LCD(0x84,0);
                cmd_LCD(' ',1);
                cmd_LCD(0xC4,0);
                cmd_LCD(' ',1);
                escreve_BIG(0x85,s_dez);
                escreve_BIG(0x88,s_uni);
                cmd_LCD(0x8A,0);
                cmd_LCD('.',1);
                cmd_LCD(0xCA,0);
                cmd_LCD('.',1);
                escreve_BIG(0x8B,ms_dez);
                escreve_BIG(0x8E,ms_uni);
                if(stop)
                {
                    escreve_BIG(0x80,10);
                    escreve_BIG(0x82,s_cen);
                    escreve_BIG(0x85,s_dez);
                    escreve_BIG(0x88,s_uni);
                    escreve_BIG(0x8B,ms_dez);
                    escreve_BIG(0x8E,ms_uni);

                    ms_uni++;
                    if(ms_uni>9)
                    {
                        ms_uni = 0;
                        ms_dez++;
                        if(ms_dez>9)
                        {
                            ms_dez = 0;
                            s_uni++;
                            if(s_uni>9)
                            {
                                s_uni = 0;
                                s_dez++;
                                if(s_dez>9)
                                {
                                    s_dez = 0;
                                    s_cen++;
                                    if(s_cen>9)
                                        s_cen = 9;
                                }
                            }
                        }
                    }
                }
                break;
        }
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    state++;  // troca estado
    if(state>3)
        state = 1;
    __delay_cycles(10000);
    P1IFG = 0; // limpa historico interrupção
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    stop ^= 1; // troca estado stop
    __delay_cycles(10000);
    P2IFG = 0; // limpa historico interrupção
}

#include <msp430.h>
#include <stdint.h>
#include "st7735.h"

// Definições de pinos para o MSP430F5529
#define CS_PIN     BIT0   // Chip Select // P2.0
#define DC_PIN     BIT4   // Data/Command // A0 // P2.4
#define RST_PIN    BIT2   // Reset // P2.2
#define MOSI_PIN   BIT0   // MOSI (UCB0SIMO) // P3.0
#define SCLK_PIN   BIT2   // SCLK (UCB0CLK) // P3.2
#define LED        BIT1   // BackLight      // P3.1

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160

void ST7735_init(void);
void SPI_init(void);
void ST7735_sendData(uint8_t data);
void ST7735_sendCommand(uint8_t command);
void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
void ADC_init(void);

int main(void)
{
    unsigned int i,j,dados=0;

    WDTCTL = WDTPW | WDTHOLD;   // Para o watchdog timer

    ADC_init();

    SPI_init();      // Inicializa o SPI

    ST7735_init();   // Inicializa o display

    __delay_cycles(50000);

    P3OUT |= 0x02;  // Backlight LCD on

    uint16_t color = 0xFFE0; // Cor amarela em formato RGB565

    //drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, color);

    for(i=0; i<128; i++)
        for(j=0;j<160;j++)
            drawPixel(i,j,color);

    color = 0;
    i = 0;
    j=0;
    while(1)
    {

        ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
        while (!(ADC12IFG & BIT0));
        dados = ADC12MEM0;

        //drawPixel(dados*3,j,color);
        drawPixel(dados/32,j,color);
        j++;
        if(j>160)
            j=0;
        __delay_cycles(100000);

    }

    return 0;
}

void ADC_init(void)
{
   P6SEL |= 0x01;                            // P6.0,1 ADC option select
   ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
   ADC12CTL1 = ADC12SHP;                     // Use sampling timer
   ADC12MCTL0 = ADC12INCH_1;                 // ref+=AVcc, channel = A0
   //ADC12IE = 0x01;                           // Enable interrupt
   ADC12CTL0 |= ADC12ENC;
}

void drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;

    P2OUT &= ~CS_PIN;
    ST7735_sendCommand(ST7735_CASET);  // Column address set
    ST7735_sendData(0x00);
    ST7735_sendData(x);                // Start column
    ST7735_sendData(0x00);
    ST7735_sendData(x);                // End column

    ST7735_sendCommand(ST7735_RASET);  // Row address set
    ST7735_sendData(0x00);
    ST7735_sendData(y);                // Start row
    ST7735_sendData(0x00);
    ST7735_sendData(y);                // End row

    ST7735_sendCommand(ST7735_RAMWR);  // Write to RAM
    ST7735_sendData(color >> 8);
    ST7735_sendData(color);
    P2OUT |= CS_PIN;
}

void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 0;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}


void SPI_init(void) {
    // Configuração dos pinos de SPI
    P3SEL |= MOSI_PIN + SCLK_PIN;
    P3DIR |= MOSI_PIN + SCLK_PIN + LED;

    // Configuração do SPI
    UCB0CTL1 |= UCSWRST;                          // Coloca o módulo em reset
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;  // Fase de clock, MSB primeiro, mestre, síncrono
    UCB0CTL1 |= UCSSEL_2;                         // Seleciona SMCLK como fonte de clock
    UCB0BR0 = 0x02;                               // Divide o clock (SMCLK / 2)
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                         // Sai do reset
}

void ST7735_init(void) {
    // Configuração dos pinos de controle
    P2DIR |= CS_PIN + DC_PIN + RST_PIN;
    P2OUT |= CS_PIN;
    P2OUT &= ~RST_PIN;
    __delay_cycles(50000);
    P2OUT |= RST_PIN;

    // Inicialização do display
    P2OUT &= ~CS_PIN;

    ST7735_sendCommand(ST7735_SWRESET); // Software reset
    __delay_cycles(150000);

    ST7735_sendCommand(ST7735_SLPOUT);  // Sleep out
    __delay_cycles(150000);

    ST7735_sendCommand(ST7735_FRMCTR1); // Frame rate control - normal mode
    ST7735_sendData(0x01);
    ST7735_sendData(0x2C);
    ST7735_sendData(0x2D);

    ST7735_sendCommand(ST7735_FRMCTR2); // Frame rate control - idle mode
    ST7735_sendData(0x01);
    ST7735_sendData(0x2C);
    ST7735_sendData(0x2D);

    ST7735_sendCommand(ST7735_FRMCTR3); // Frame rate control - partial mode
    ST7735_sendData(0x01);
    ST7735_sendData(0x2C);
    ST7735_sendData(0x2D);
    ST7735_sendData(0x01);
    ST7735_sendData(0x2C);
    ST7735_sendData(0x2D);

    ST7735_sendCommand(ST7735_INVCTR);  // Display inversion control
    ST7735_sendData(0x07);

    ST7735_sendCommand(ST7735_PWCTR1);  // Power control
    ST7735_sendData(0xA2);
    ST7735_sendData(0x02);
    ST7735_sendData(0x84);

    ST7735_sendCommand(ST7735_PWCTR2);  // Power control
    ST7735_sendData(0xC5);

    ST7735_sendCommand(ST7735_PWCTR3);  // Power control
    ST7735_sendData(0x0A);
    ST7735_sendData(0x00);

    ST7735_sendCommand(ST7735_PWCTR4);  // Power control
    ST7735_sendData(0x8A);
    ST7735_sendData(0x2A);

    ST7735_sendCommand(ST7735_PWCTR5);  // Power control
    ST7735_sendData(0x8A);
    ST7735_sendData(0xEE);

    ST7735_sendCommand(ST7735_VMCTR1);  // VCOM control
    ST7735_sendData(0x0E);

    ST7735_sendCommand(ST7735_GMCTRP1); // Positive Gamma correction
    ST7735_sendData(0x02);
    ST7735_sendData(0x1C);
    ST7735_sendData(0x07);
    ST7735_sendData(0x12);
    ST7735_sendData(0x37);
    ST7735_sendData(0x32);
    ST7735_sendData(0x29);
    ST7735_sendData(0x2D);
    ST7735_sendData(0x29);
    ST7735_sendData(0x25);
    ST7735_sendData(0x2B);
    ST7735_sendData(0x39);
    ST7735_sendData(0x00);
    ST7735_sendData(0x01);
    ST7735_sendData(0x03);
    ST7735_sendData(0x10);

    ST7735_sendCommand(ST7735_GMCTRN1); // Negative Gamma correction
    ST7735_sendData(0x03);
    ST7735_sendData(0x1D);
    ST7735_sendData(0x07);
    ST7735_sendData(0x06);
    ST7735_sendData(0x2E);
    ST7735_sendData(0x2C);
    ST7735_sendData(0x29);
    ST7735_sendData(0x2D);
    ST7735_sendData(0x2E);
    ST7735_sendData(0x2E);
    ST7735_sendData(0x37);
    ST7735_sendData(0x3F);
    ST7735_sendData(0x00);
    ST7735_sendData(0x00);
    ST7735_sendData(0x02);
    ST7735_sendData(0x10);

    ST7735_sendCommand(ST7735_COLMOD);  // Interface pixel format
    ST7735_sendData(0x05); // 16-bit/pixel RGB 5-6-5

    ST7735_sendCommand(ST7735_MADCTL);  // Memory data access control
    ST7735_sendData(0xC8);

    ST7735_sendCommand(ST7735_CASET);   // Column address set
    ST7735_sendData(0x00);
    ST7735_sendData(0x00);
    ST7735_sendData(0x00);
    ST7735_sendData(0x7F);  // X address: 0 to 127

    ST7735_sendCommand(ST7735_RASET);   // Row address set
    ST7735_sendData(0x00);
    ST7735_sendData(0x00);
    ST7735_sendData(0x00);
    ST7735_sendData(0x9F);  // Y address: 0 to 159

    ST7735_sendCommand(ST7735_NORON);   // Normal display mode on
    __delay_cycles(10000);

    ST7735_sendCommand(ST7735_DISPON);  // Display on
    __delay_cycles(10000);

    P2OUT |= CS_PIN;
}

// Função para enviar comandos ao display
void ST7735_sendCommand(uint8_t command) {
    P2OUT &= ~DC_PIN; // Command mode
    UCB0TXBUF = command;
    while (!(UCB0IFG & UCTXIFG)); // Aguarda transmissão
}

// Função para enviar dados ao display
void ST7735_sendData(uint8_t data) {
    P2OUT |= DC_PIN; // Data mode
    UCB0TXBUF = data;
    while (!(UCB0IFG & UCTXIFG)); // Aguarda transmissão
}



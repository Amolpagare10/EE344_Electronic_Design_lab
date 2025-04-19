#include <xc.h>                         // Include processor header
#include <stdio.h>                      // For printf and snprintf
#include <stdint.h>                     // For uint types
#include <stdlib.h>                     // For standard utilities
#define FCY 100000000UL                 // Define instruction cycle frequency (Fosc/2 = 200MHz / 2)
#include <libpic30.h>                   // For __delay_us and other helpers
#include <math.h>                       // Math operations
#include "mcc_generated_files/system/clock.h"  // Clock initialization
#include "protocol_def.h"                      // For cmd protocol
// Define ports for DAC and multiplexer
#define DATA_PORT LATA        
#define DATA_TRIS TRISA       

// Define control lines for 3-bit multiplexer
#define MUX_A0 LATAbits.LATA9
#define MUX_A1 LATAbits.LATA10
#define MUX_A2 LATAbits.LATA2
#define MUX_EN LATAbits.LATA3

// Define DAC chip select and data lines
#define DAC_CSn LATDbits.LATD12
#define DAC_data LATD

// Define ADC data and clock lines
#define ADC_data LATC
#define ADC_clk LATAbits.LATA4

// Define Chip Select line for digital logic
#define CS_TRIS  TRISCbits.TRISC0
#define CS_LAT  LATCbits.LATC0
    
#define BAUDRATE 9600                   // UART baudrate
#define BRGVAL (((FCY / BAUDRATE) / 16) - 1)  // Calculate UART baud rate register value

// Define shift register connections (Edge Sequencer)
#define ES_DIN LATAbits.LATA8
#define ES_CLK LATAbits.LATA11
#define ES_LEn LATAbits.LATA1
#define ES_CLR LATAbits.LATA7
#define ES_DOUT LATAbits.LATA0

// UART receiver initialization
void uart_rx_init() {
    _U1RXR = 59;                        // Map RX to RP59
    U1CONbits.MODE = 0;                // 8-bit UART, asynchronous mode
    U1CONbits.BRGS = 0;                // Low speed baud
    U1CONbits.CLKSEL = 0;              // Use system clock/2
    U1CONbits.STP = 0;                 // 1 stop bit
    U1BRG = BRGVAL;                    // Set baud rate
    U1CONbits.ON = 1;                  // Enable UART module
    U1CONbits.RXEN = 1;                // Enable RX
}

// UART transmitter initialization
void uart_tx_init() {
    _RP58R = 9;                        // Map TX to RP58
    U1CONbits.TXEN = 1;               // Enable TX
}

// Transmit a single character over UART
void uart_transmit_char(char c) {
    while (U1STATbits.UTXBF);         // Wait if TX buffer full
    U1TXB = c;                         // Load character to TX buffer
}

// Transmit a null-terminated string over UART
void uart_transmit_string(const char *str) {
    while (*str) {
        uart_transmit_char(*str++);   // Send each character
    }
}

// Transmit an integer as a string
void uart_transmit_no(const uint16_t data) {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", data);  // Convert to string
    uart_transmit_string(buffer);                  // Send string
}

// Hanning window (50 points)
const uint16_t hanningTable[50] = {
    0x800,0x812,0x842,0x872,0x867,0x7EF,0x718,0x641,0x5F9,0x6B1,
    0x867,0xA87,0xC12,0xC17,0xA36,0x6F0,0x38F,0x19E,0x233,0x554,
    0x9E1,0xDF8,0xFCF,0xE81,0xA85,0x57B,0x17F,0x031,0x208,0x61F,
    0xAAC,0xDCD,0xE62,0xC71,0x910,0x5CA,0x3E9,0x3EE,0x579,0x799,
    0x94F,0xA07,0x9BF,0x8E8,0x811,0x799,0x78E,0x7BE,0x7EE,0x800
};

// Sine wave (20 points)
const uint16_t sineTable[20] = {
    0x7fd, 0x814, 0x829, 0x83a, 0x844, 0x848, 0x844, 0x83a,
    0x829, 0x814, 0x7fd, 0x7e6, 0x7d1, 0x7c0, 0x7b6, 0x7b2,
    0x7b6, 0x7c0, 0x7d1, 0x7e6
};

// Function declarations
void DAC_init(void);
void DAC_write(uint16_t data);
void DAC_Controller(uint16_t amplitude);
void Timer1_Init(void);

volatile uint16_t cnt = 0;              // Toggle counter for DAC waveform
volatile uint16_t amplitude = 0xFFFF;   // Default amplitude

// Write 12-bit data to DAC
void DAC_write(uint16_t data) {
    DATA_PORT = (DATA_PORT & 0xF000) | (data & 0x0FFF);  // Mask upper bits
    __delay_us(1);
    CS_LAT = 0;             // Pull chip select low
    __delay_us(1);
    CS_LAT = 1;             // Pull chip select high
    __delay_us(1);
}

// Generate waveform using DAC
void DAC_Controller(uint16_t amplitude) {
    uint16_t normalized_value = (uint16_t)(((uint32_t)amplitude * 4095) / 65536);  // Scale to 12-bit
    uint16_t Data;

    if (cnt) {
        Data = normalized_value;
        __delay_us(1);
    }
    else {
        Data = 0;
        __delay_us(1);
    }
    cnt ^= 1;               // Toggle cnt
    DAC_write(Data);        // Send to DAC
}

// Initialize multiplexer control lines
void TMUX_init(){
    TRISAbits.TRISA9 =  0;
    TRISAbits.TRISA10 = 0;
    TRISAbits.TRISA2 =  0;
    TRISAbits.TRISA3 =  0; 
}

// Initialize DAC
void DAC_init(){
    TRISD = 0;              // Set PORTD as output
    DAC_CSn = 0;            // Enable DAC chip
}

// Control 3-bit address of multiplexer
void TMUX_Control(uint8_t address) {
    MUX_A0 = address & 0x01;
    MUX_A1 = (address & 0x02)>>1;
    MUX_A2 = (address & 0x04)>>2;
}

// Initialize ADC input and clock
void ADC_init(){
    TRISC = 0xFFF;          // Set PORTC as input
    TRISAbits.TRISA4 = 0;   // Set clock pin as output
    ADC_clk = 0;            // Initialize clock low
}

// Initialize edge sequencer shift register
void ES_init(){
    TRISAbits.TRISA8 = 0;   // DIN
    TRISAbits.TRISA11 = 0;  // CLK
    TRISAbits.TRISA1 = 0;   // LE
    TRISAbits.TRISA7 = 0;   // CLR
    TRISAbits.TRISA0 = 1;   // DOUT as input

    ES_LEn = 1;             // Disable latch
    ES_CLK = 0;
    ES_CLR = 0;             // Clear shift register
    __delay_us(1);
    ES_CLR = 1;             // Stop clearing
    __delay_us(1);
    ES_CLR = 0;             // Reassert
}

int main(void) {
    CLOCK_Initialize();     // Initialize system clock
    DAC_init();             // Initialize DAC
    TMUX_init();            // Initialize multiplexer
    MUX_EN = 1;             // Enable multiplexer
    TMUX_Control(MUX_data);        // Set MUX to channel 0
    ADC_init();             // Initialize ADC
    ES_init();              // Initialize edge sequencer

    uart_rx_init();         // Init UART RX
    uart_tx_init();         // Init UART TX

    // Edge sequencer bit pattern
    uint8_t es_seq[8];
    for(int i=0;i<8;i++){
    es_seq[i] = ES_data[i];}
    //uint8_t es_seq[8] = {0,1,0,0,0,0,0,0};  // Order of channels
    ES_LEn = 0;              // Enable shift register load
    __delay_us(10);
    for (int i = 0; i < 8; i++) {
        ES_DIN = es_seq[i];                 // Load bit
        __delay_us(10);
        ES_CLK = !ES_CLK;                   // Clock pulse high
        __delay_us(10);
        ES_CLK = !ES_CLK;                   // Clock pulse low
    }
    __delay_us(10);
    ES_LEn = 1;                             // Latch the sequence

    uint16_t recvd[400];                   // Store ADC values

    while(1){
        for (int i = 0; i < 50; i++) {
            DAC_data = hanningTable[i % 50] & 0x0fff; // Set DAC data
            __delay_us(1);
            DAC_CSn = !DAC_CSn;            // Toggle chip select
            DAC_CSn = !DAC_CSn;            // Toggle again

            ADC_clk = !ADC_clk;            // Toggle ADC clock
            // Read 10-bit ADC value from specific pins
            recvd[i] = (PORTCbits.RC11 << 9 | PORTCbits.RC10 << 8 | 
                        PORTCbits.RC8 << 7 | PORTCbits.RC7 << 6 |
                        PORTCbits.RC6 << 5 | PORTCbits.RC5 << 4 |
                        PORTCbits.RC4 << 3 | PORTCbits.RC3 << 2 |
                        PORTCbits.RC2 << 1 | PORTCbits.RC0) & 0x03ff;
        }
        __delay_us(50000);  // Delay for observation
    }

    for (int i = 50; i < 400; i++) {
        ADC_clk = !ADC_clk;            // Toggle ADC clock
        // Continue reading remaining ADC values
        recvd[i] = (PORTCbits.RC11 << 9 | PORTCbits.RC10 << 8 |
                    PORTCbits.RC8 << 7 | PORTCbits.RC7 << 6 |
                    PORTCbits.RC6 << 5 | PORTCbits.RC5 << 4 |
                    PORTCbits.RC4 << 3 | PORTCbits.RC3 << 2 |
                    PORTCbits.RC2 << 1 | PORTCbits.RC0) & 0x03ff;
    }

    // Transmit alternate ADC values over UART
    for (int i = 1; i < 400; i += 2) {
        uart_transmit_no(recvd[i]);
        uart_transmit_string(",");
    }

    while (1);  // Infinite loop
}

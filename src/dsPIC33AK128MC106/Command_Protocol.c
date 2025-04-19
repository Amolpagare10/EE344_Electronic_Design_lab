#include <xc.h>
#define FCY 60000000UL  // Fosc = 120MHz, FCY = Fosc/2
#include <libpic30.h>
#include "protocol_def.h"
#define FP 4000000       
#define BAUDRATE 9600
#define BRGVAL (((FP / BAUDRATE) / 16) - 1)

#define RX_BUFFER_SIZE 12 

int ES_data[8] = {0};
int MUX_data = 0;

void uart_rx_init();
void uart_tx_init();
char uart_receive_char();
void uart_receive_string(uint8_t max_length);
void uart_transmit_char(char c);
void uart_transmit_string(char *str);
void process_received_data(void);  

char tx[8] = {'0'};
char rx[4] = {'0'};

void uart_rx_init() {
    _U1RXR = 59;        // Assign U1RX to pin RP59
    U1CONbits.MODE = 0; // Asynchronous 8-bit UART mode
    U1CONbits.BRGS = 0; // Low-Speed Baud mode
    U1CONbits.CLKSEL = 0; // System Clock/2 as Baud Clock source
    U1CONbits.STP = 0;  // 1 stop bit
    U1BRG = BRGVAL;     // Baud rate setting for 9600

    U1CONbits.ON = 1;   // Enable UART
    U1CONbits.RXEN = 1; // Enable UART RX
}

void uart_tx_init() {
    _RP58R = 9;        // Assign U1TX to pin RP58
    U1CONbits.TXEN = 1; // Enable UART TX
}

char uart_receive_char() {
    while (U1STATbits.RXBE); 
    return U1RXB;           
}

// Function to receive a string (stops at newline or buffer limit)
void uart_receive_string(uint8_t max_length) {
    uint8_t i = 0;
    char received_char;

    while (i < max_length - 1) {
        received_char = uart_receive_char(); 
        if (received_char == '\n' || received_char == '\r') break; 

        if (i < 7) {
            tx[i++] = received_char;
        } else if (i < 10) {
            rx[i - 7] = received_char;  // shift index
        }
        i++;
    }

    tx[7] = '\0';
    rx[3] = '\0';
}


void uart_transmit_char(char c) {
    while (U1STATbits.UTXBF); 
    U1TXB = c; 
}

void uart_transmit_string(const char *str) {
    while (*str) {
        uart_transmit_char(*str++);
    }
}

void process_received_data() {
   for(int i=0;i<8;i++){
        ES_data [i] = tx[i]-'0';
       if(i>=3)continue;
        MUX_data += (rx[i]-'0')*(1<<i);
   }
}


int main(void) {
    uart_rx_init();  
    uart_tx_init();  
    
    
    while (1) {
        uart_transmit_string("\nReceiving es and mux"); 
        uart_receive_string(RX_BUFFER_SIZE); 
        uart_transmit_string("\nES is set mux is set");
        uart_transmit_string(tx);
        uart_transmit_string(rx);              
        process_received_data();  
    }

    return 0;
}

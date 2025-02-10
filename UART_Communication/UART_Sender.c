#include "UART_Config.h"

// Global variables
volatile unsigned int colorState = 0;                           // Current color index
char txBuffer[10];                                              // UART transmission buffer
volatile unsigned int j = 0;                                    // Index for transmission buffer

// Function prototypes
inline void transmitString(const char* str);

int main(void) 
{
    WDTCTL = WDTPW | WDTHOLD;                                   // Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;                                      // Set clock to 1MHz
    DCOCTL = CALDCO_1MHZ;                                       // Set clock to 1MHz

    P2DIR |= RED | GREEN | BLUE;                                // Set RGB LED pins as output
    P2OUT &= ~(RED | GREEN | BLUE);                             // Turn off LEDs initially

    initConfigurations();                                       // Initialize UART, timer, and ports
    _enable_interrupt();                                        // Enable global interrupts

    while (1) {
        // Main loop (no action needed here)
    }
}

//**************************************************************
// Switch Button ISR to send the button press to the other board
//**************************************************************
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void) 
{
    P1IFG &= ~SWITCH;  // Clear interrupt flag
    transmitString(seven_colors[colorState]);                   // Send the current color string to the other board
}

//*************************************************************************
// Once button pressed, to send the color string over UART (Transmission part)
//*************************************************************************
inline void transmitString(const char* str) 
{
    strcpy(txBuffer, str);                                      // Copy string to tx buffer
    j = 0;                                                      // Resets the index before transmission of every string
    IE2 |= UCA0TXIE;                                            // Enables UART transmit interrupt
}

//*************************************************************************
// Transmission ISR for UART transmission which serves when TX interrupt occurs
//*************************************************************************
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) 
{
    if (txBuffer[j]) {
        UCA0TXBUF = txBuffer[j++];                              // Sends the next character of the string Color
    } else {
        UCA0TXBUF = '\0';
        IE2 &= ~UCA0TXIE;                                       // Clears TX interrupt when done
    }
}

//*******************************************************************************************************
// Initialize port 1 and interrupts for button, timer for 1ms intervals and UART for serial communication
//*******************************************************************************************************
void initConfigurations(void) 
{
    // Ports Initialization
    P1DIR &= ~SWITCH;                                           // Set button pin as input
    P1OUT |= SWITCH;                                            // Enable pull-up resistor
    P1IE |= SWITCH;                                             // Enable interrupt on button press
    P1IES |= SWITCH;                                            // High-to-low transition
    P1IFG &= ~SWITCH;                                           // Clear interrupt flag

    // UART initialization
    P1SEL |= TXDATA + RXDATA;                                   // UART function is selected
    P1SEL2 |= TXDATA + RXDATA;
    UCA0CTL1 &= ~UCSWRST;                                       // Initialize USCI state machine
    UCA0CTL1 |= UCSSEL_2;                                       // Source clock - SMCLK
    UCA0BR0 = 104;                                              // 9600 baud rate
    UCA0MCTL = UCBRS0;
    UCA0BR1 = 0;
    IE2 |= UCA0TXIE;                                            // Enable TX interrupt
}


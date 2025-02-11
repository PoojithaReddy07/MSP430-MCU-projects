#include "I2C_Config.h"

// Global variables
volatile int colorState = 0;                                 // Current array index for the color cycle

// Function prototypes
void Master_Mode(void);
void transmitNextColor(unsigned int send_color);

int main(void) 
{
    WDTCTL = WDTPW | WDTHOLD;                               // Stop watchdog timer
    initConfigurations();                                   // Initialize ports, timer, and I2C
    __enable_interrupt();                                   // Enable global interrupts

    while (1) 
    {
        // Main loop (no action needed here)
    }
}

//****************************************************************************************
// Initializing registers of I2C communication for master board for transmitting the color
//*****************************************************************************************
void Master_Mode(void) 
{
    UCB0CTL1 |= UCSWRST;                                    // Enable the Software reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;                   // Select I2C Master mode and synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                          // Use SMCLK and Software reset
    UCB0BR0 = 12;                                           // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = 0x48;                                       // Slave Address is 0x048
    UCB0CTL1 &= ~UCSWRST;                                   // Disable the Software reset
    IE2 &= ~UCB0RXIE;                                       // Disable Receiver interrupt
}

//***********************************************************************************************************************************
// Once button pressed, to send the color string over I2C (Transmission part) and revert back to slave mode once transmission completed
//***********************************************************************************************************************************
void transmitNextColor(unsigned int send_color) 
{
    Master_Mode();
    while (UCB0CTL1 & UCTXSTP); // Ensure stop condition is sent
    UCB0CTL1 |= UCTR + UCTXSTT;  // Start condition for I2C transmission
    UCB0TXBUF = send_color;  // Send the color data to be transmitted
    IE2 |= UCB0TXIE;   // Enable TX interrupt
}

//**********************************************************************************************************************
// Transmission ISR for I2C communication where master transmits data to slave and stored data from received buffer
//**********************************************************************************************************************
#pragma vector = USCIAB0TX_VECTOR
__interrupt void TX_ISR(void) 
{
    if (IFG2 & UCB0TXIFG) 
    {                                                       // Check if TX buffer is ready
        UCB0CTL1 |= UCTXSTP;                                // Stop condition for I2C transmission
        IFG2 &= ~UCB0TXIFG;                                 // Clear TX interrupt flag
        while (UCB0CTL1 & UCTXSTP);                         // Ensure stop condition is complete
        IE2 &= ~UCB0TXIE;                                   // Disable TX interrupt
    }
}

//**************************************************************
// Switch Button ISR to send the button press to the other board
//**************************************************************
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void) 
{
    if (P1IFG & BIT3) 
    {
        P1IFG &= ~BIT3;                                     // Clear button interrupt flag
        colorState = (colorState + 1) % 7;                  // Increment to the next color
        transmitNextColor(colors_7[colorState]);            // Send next color to the slave board
    }
}

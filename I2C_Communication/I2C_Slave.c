#include "I2C_Config.h"

// Global variables
volatile LED_COLORS current_color = RED;                    // Initial color to be blinked
volatile unsigned char changeColor = 0, rx_ColorData = 0;   // Flags to track new color received

// Function prototypes
void Slave_Mode(void);

int main(void) 
{
    WDTCTL = WDTPW | WDTHOLD;                               // Stop watchdog timer
    initConfigurations();                                   // Initialize ports, timer, and I2C
    Slave_Mode();                                           // Start in Slave mode
    __enable_interrupt();                                   // Enable global interrupts
    display_LED(current_color);                             // Set initial LED color as RED

    while (1) 
    {
        if (changeColor) 
        {
            changeColor = 0;                                // Reset the color change flag
            current_color = decodeColor(rx_ColorData & 0xFF);
            display_LED(current_color);                     // Update the LED based on the received color
        }
    }
}

//*************************************************************
// Initializing registers of I2C communication for Slave board
//**************************************************************
void Slave_Mode(void) 
{
    UCB0CTL1 |= UCSWRST;                                       // Enable the Software reset
    UCB0CTL0 = UCMODE_3 + UCSYNC;                              // Select I2C Slave mode and synchronous mode
    UCB0I2COA = 0x48;                                          // Slave address is 0x048
    UCB0CTL1 &= ~UCSWRST;                                      // Disable the Software reset
    IE2 |= UCB0RXIE;                                           // Enable Receiver interrupt
}

//********************************************************************************
// Converts the received data into the LED_COLORS type.
//*********************************************************************************
LED_COLORS decodeColor(unsigned int colorValue) 
{
    return (LED_COLORS)colorValue;
}

//******************************************
// Function to display the current LED color
//*****************************************
void display_LED(LED_COLORS color) 
{
    P2OUT &= ~(RED | GREEN | BLUE);                             // Turn off all LEDs
    P2OUT |= color;                                             // Set LED color based on current state
}

//**********************************************************************************************************************
// Transmission ISR for I2C communication where master transmits data to slave and stored data from received buffer
//**********************************************************************************************************************
#pragma vector = USCIAB0TX_VECTOR
__interrupt void TX_ISR(void)
{
    if (IFG2 & UCB0RXIFG) {                                     // Check if RX flag is set
        rx_ColorData = UCB0RXBUF;                               // Store received data in buffer
        changeColor = 1;                                        // Set flag to update LED color
        IFG2 &= ~UCB0RXIFG;                                     // Clear RX interrupt flag
    }
}

//********************************************************************
// Timer ISR to toggle RGB LED with 0.9 seconds delay at 50% duty cycle
//********************************************************************
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void) 
{
    static unsigned int counter = 0;                                    // Overflow counter
    counter++;
    if (counter >= DELAY) 
    {
        counter = 0;                                                    // Reset the counter
        if (P2OUT & (RED | GREEN | BLUE)) {                             // If LED is on
            P2OUT &= ~(RED | GREEN | BLUE);                             // Turn off LED
        } else {
            display_LED(current_color);                                 // Turn on LED with the current color
        }
    }
}

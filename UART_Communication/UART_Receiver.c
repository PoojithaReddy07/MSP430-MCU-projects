#include "UART_Config.h"

// Global variables
volatile unsigned int colorState = 0;                           // Current color index
volatile unsigned int OverFlowCounter = 0;                      // Overflow counter for timer
volatile unsigned char changeColor = 0;                         // Flag to track color change
volatile unsigned char ledState = 0;                            // Flag to track LED state
volatile unsigned char rxFlag = 0;                              // Flag to track receiver buffer status
char rxBuffer[10];                                              // UART reception buffer

// Function prototypes
inline void display_LED(void);

int main(void) 
{
    WDTCTL = WDTPW | WDTHOLD;                                   // Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;                                      // Set clock to 1MHz
    DCOCTL = CALDCO_1MHZ;                                       // Set clock to 1MHz

    P2DIR |= RED | GREEN | BLUE;                                // Set RGB LED pins as output
    P2OUT &= ~(RED | GREEN | BLUE);                             // Turn off LEDs initially

    initConfigurations();                                       // Initialize UART, timer, and ports
    _enable_interrupt();                                        // Enable global interrupts

    while (1) 
    {
        if (changeColor) 
        {
            display_LED();                                      // Update the LED based on the received color
            changeColor = 0;                                    // Reset the color change flag
        }
    }
}

//*************************************************************************
// UART Receiver ISR to receive the color string and change the LED on the other board
//*************************************************************************
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) 
{
    char receivedChar = UCA0RXBUF;                               // Read the received character
    unsigned int i = 0;
    if (rxFlag < sizeof(rxBuffer) - 1) 
    {
        rxBuffer[rxFlag++] = receivedChar;                       // Store the received character in the buffer
    }
    if (receivedChar == '\0') 
    {                                  // Check if NULL character is received (end of string)
        rxBuffer[rxFlag] = '\0';
        for (i = 0; i < 7; i++) 
        {
            if (!strcmp(rxBuffer, seven_colors[i])) 
            {            // Compare with each color string
                colorState = i;                                 // Update the color state based on received string
                changeColor = 1;                                // Set flag to update the LED color on this board
                break;
            }
        }
        rxFlag = 0;                                             // Reset receiver buffer index for the next color update
    }
}

//**********************************
// Function to display the current LED color
//**********************************
inline void display_LED(void) 
{
    P2OUT &= ~(RED | GREEN | BLUE);                             // Turn off all LEDs
    P2OUT |= colors_7[colorState];                              // Set the LED based on the current color state
}

//********************************************************************
// Timer ISR to toggle RGB LED with 0.9 seconds delay at 50% duty cycle
//********************************************************************
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void) 
{
    OverFlowCounter++;
    if (OverFlowCounter >= DELAY) 
    {
        if (ledState) {
            P2OUT &= ~(RED | GREEN | BLUE);                      // Turn OFF LED after 450ms
            ledState = 0;                                        // Disable LED state
        } else {
            display_LED();                                       // Turn ON LED based on the current color state
            ledState = 1;                                        // Enable LED state
        }
        OverFlowCounter = 0;                                     // Reset the overflow count
    }
}

//*******************************************************************************************************
// Initialize port 1 and interrupts for button, timer for 1ms intervals and UART for serial communication
//*******************************************************************************************************
void initConfigurations(void) 
{
    // Timer Initialization
    TACCR0 = 1000 - 1;  // 1ms timer interval
    TACCTL0 = CCIE;  // Enable interrupt for timer
    TACTL = TASSEL_2 + MC_1 + ID_0;  // Source clock - SMCLK, Mode - Up mode

    // UART initialization
    P1SEL |= TXDATA + RXDATA;                                     // UART function is selected
    P1SEL2 |= TXDATA + RXDATA;
    UCA0CTL1 &= ~UCSWRST;                                         // Initialize USCI state machine
    UCA0CTL1 |= UCSSEL_2;                                         // Source clock - SMCLK
    UCA0BR0 = 104;                                                // 9600 baud rate
    UCA0MCTL = UCBRS0;
    UCA0BR1 = 0;
    IE2 |= UCA0RXIE;                                              // Enable RX interrupt
}

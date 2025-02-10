/***********************************************************************************
 * MSP430: Interrupts & Timers
 * Blink each of 7 colors of LED after each button press with 1.5 seconds at 50% duty cycle
 ***********************************************************************************/
#include <msp430.h>

// Define LED colors using an enum
typedef enum {
    RED = 0x02,                                                 // P2.1
    GREEN = 0x08,                                               // P2.3
    BLUE = 0x20,                                                // P2.5
    YELLOW = RED | GREEN,
    CYAN = GREEN | BLUE,
    MAGENTA = RED | BLUE,
    WHITE = RED | GREEN | BLUE
} LED_COLORS_7;

#define SWITCH 0x08   					                        // P1.3 (Switch button)
#define DELAY 750     					                        // 750ms delay (1.5 seconds at 50% duty cycle)

// Global variables
volatile unsigned int OFCount = 0;                              // Overflow count for Timer
volatile unsigned char changeColor = 0;                         // Flag to track color change (0 = OFF, 1 = ON)
volatile unsigned int colorState = 0;                           // Tracks the current color in the enum array

// Array of 7 LED colors
LED_COLORS_7 colors[] = { RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE };

// Function prototypes
void initTimer_A(void);
void init_ports(void);

void main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set Port 2 pins 1, 3, and 5 as output pins
    P2DIR |= RED | GREEN | BLUE;

    // Set MCLK = SMCLK = 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // Timer configuration
    TACCR0 = 1000 - 1;                                          // Timer count limit for 1ms (1MHz clock)
    TACCTL0 |= CCIE;                                            // Enable interrupt for CCR0
    TACTL = TASSEL_2 + ID_0 + MC_1;                             // SMCLK, no divider, Up Mode

    // Enable global interrupts
    _enable_interrupt();

    // Initialize ports for button press
    init_ports();

    // Initially blink the first color (RED)
    P2OUT = colors[colorState];

    // Main loop
    while (1) {
    // Update LED color if changeColor flag is set
        if (changeColor) {
            P2OUT = colors[colorState];                         // Set the new color
            changeColor = 0;                                    // Reset the flag
        }
    }
}

// Timer ISR for delay
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void) {
    OFCount++;                                                  // Increment overflow counter

    // Toggle the current color after the delay
    if (OFCount >= DELAY && changeColor == 0) {
        P2OUT ^= colors[colorState];                            // Toggle the current color
        OFCount = 0;                                            // Reset the overflow counter
    }
}

// Ports initialization for switch
void init_ports(void) {
    P1DIR &= ~SWITCH;                                           // Configure P1.3 as input pin
    P1OUT |= SWITCH;                                            // Enable pull-up resistor
    P1IFG &= ~SWITCH;                                           // Clear interrupt flag
    P1IE |= SWITCH;                                             // Enable interrupt
    P1IES |= SWITCH;                                            // Set interrupt edge to high-to-low
}

// Switch ISR to change color
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void) {
    P1IFG &= ~SWITCH;                                            // Clear interrupt flag

    // Move to the next color state
    if (colorState == 6) {
        colorState = 0;                                          // Reset to the first color after 7 colors
    } else {
        colorState++;                                            // Move to the next color
    }
    changeColor = 1;                                             // Set the flag to indicate color change
}

// Blinking 7 colors of tri-color LED found on the MSP430G2ET
// Each Color Cycle at a rate of 1.33Hz at a duty cycle of 100%
#include <msp430.h>

#define RED_LED   0x02    // P2.1
#define GREEN_LED 0x08    // P2.3
#define BLUE_LED  0x20    // P2.5

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    // Set P2.1, P2.3, and P2.5 as output
    P2DIR |= RED_LED | GREEN_LED | BLUE_LED;

    // Array to hold all colors
    volatile unsigned char colors[] = {RED_LED, GREEN_LED, BLUE_LED, RED_LED | GREEN_LED, RED_LED | BLUE_LED, GREEN_LED | BLUE_LED, RED_LED | GREEN_LED | BLUE_LED};

    volatile unsigned int i = 0;

    while(1)
    {
        for(i = 0; i < 7; i++)
        {
            P2OUT = colors[i];
            __delay_cycles(790800); // inbuilt delay, 790ms (1.33Hz)
        }
    }
}


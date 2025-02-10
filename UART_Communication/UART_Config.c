
#ifndef UART_CONFIG_H
#define UART_CONFIG_H

#include <msp430.h>
#include <string.h>

// Pin definitions
#define TXDATA 0x04                                         // TXD on P1.2
#define RXDATA 0x02                                         // RXD on P1.1
#define SWITCH 0x08                                         // Button on P1.3
#define DELAY 450                                           // 450 ms (0.9 seconds delay at 50% duty cycle)

// LED color definitions
typedef enum {
    RED = 0x02,                                             // P2.1
    GREEN = 0x08,                                           // P2.3
    BLUE = 0x20,                                            // P2.5
    YELLOW = RED | GREEN,
    CYAN = GREEN | BLUE,
    MAGENTA = RED | BLUE,
    WHITE = RED | GREEN | BLUE
} LED_COLORS;

// Array of 7 LED colors
LED_COLORS colors_7[7] = { RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE };

// Array of color names
const char* seven_colors[] = {"RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA", "WHITE"};

// Function prototypes
void initConfigurations(void);

#endif



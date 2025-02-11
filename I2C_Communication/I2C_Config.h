#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#include <msp430.h>

// Pin definitions
#define SWITCH 0x08                                             // Button on P1.3
#define DELAY 450                                               // 450 ms (0.9 seconds delay at 50% duty cycle)

// LED color definitions
typedef enum {
    RED = 0x02,                                                 // P2.1
    GREEN = 0x08,                                               // P2.3
    BLUE = 0x20,                                                // P2.5
    YELLOW = RED | GREEN,
    CYAN = GREEN | BLUE,
    MAGENTA = RED | BLUE,
    WHITE = RED | GREEN | BLUE
} LED_COLORS;

// Array of 7 LED colors
const LED_COLORS colors_7[7] = { RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE };

// Function prototypes
void initConfigurations(void);
void display_LED(LED_COLORS color);
LED_COLORS decodeColor(unsigned int colorValue);

#endif

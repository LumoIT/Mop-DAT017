#ifndef GRAPHICDISPLAY_H
#define GRAPHICDISPLAY_H

#include "delay.h"
#include "gpio.h"

#define B_E         0x40
#define B_RST       0x20
#define B_CS2       0x10
#define B_CS1       0x8
#define B_SELECT    0x4
#define B_RW        0x2
#define B_RS        0x1

#define LCD_ON			0x3F
#define LCD_OFF			0x3E
#define LCD_DISP_START	0xC0
#define LCD_SET_ADD		0x40
#define LCD_SET_PAGE	0xB8
#define LCD_BUSY		0x80

typedef unsigned char uint8_t;

// ***Prototypes***

// Bit handling, control register
void graphic_ctrl_bit_set(uint8_t);
void graphic_ctrl_bit_clear(uint8_t);

// Controller handling
void select_controller(uint8_t);

// Delay handling
void graphic_wait_ready(void);

// Output handling, data register
void graphic_write(uint8_t, uint8_t);
void graphic_write_command(uint8_t, uint8_t);
void graphic_write_data(uint8_t, uint8_t);

// Input handling, data register
uint8_t graphic_read(uint8_t);
uint8_t graphic_read_data(uint8_t);

// Initialisation
void init_app(void);
void graphic_initialize(void);

// Clear screen
void graphic_clear_screen(void);

// Coordinate handling
void pixel(uint8_t, uint8_t, uint8_t);

#endif // GRAPHICDISPLAY_H
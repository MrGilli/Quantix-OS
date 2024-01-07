#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "multiboot.h"
#include "stdint-gcc.h"

// Function declarations
void init_tty(multiboot_info_t *mbi, uint32_t fg, uint32_t bg);
void framebuffer_putchar(char ch, uint32_t color);
void framebuffer_putpixel(uint32_t x, uint32_t y, uint32_t color);
void framebuffer_clscr(uint32_t color);
void gotoxy(uint32_t x, uint32_t y);

// Constants
#define FONT_HEIGHT 8
#define FONT_WIDTH 8

// Define any additional structures or constants needed for framebuffer operations

#endif /* FRAMEBUFFER_H */

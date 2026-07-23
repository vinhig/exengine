/* vga
  Simple vga style font rendering specifically
  for debug information, not for production use.
*/

#pragma once

#include <inttypes.h>
#include <stdlib.h>

extern uint32_t ex_vga_fg;
extern uint32_t ex_vga_bg;

/**
 * @brief Initialize VGA systems.
 */
void ex_vga_init();

/**
 * @brief Blit text to the VGA buffer.
 * @param x   x character index
 * @param y   y character index
 * @param str ASCII string
 */
void ex_vga_print(size_t x, size_t y, const char *str);

/**
 * @brief Render VGA to the screen.
 */
void ex_vga_render();

/**
 * @brief Clear the VGA pixel data.
 */
void ex_vga_clear();

/**
 * @brief Set the foreground color.
 * @param r red component
 * @param g green component
 * @param b blue component
 * @param a alpha component
 */
void ex_vga_setfg(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * @brief Set the background color.
 * @param r red component
 * @param g green component
 * @param b blue component
 * @param a alpha component
 */
void ex_vga_setbg(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * @brief Clean up VGA systems.
 */
void ex_vga_destroy();

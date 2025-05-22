#ifndef LCD_H
#define LCD_H

#include "main.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetTextColor(uint32_t color);
void LCD_SetBackColor(uint32_t color);
void LCD_DisplayString(uint16_t line, uint16_t column, uint8_t *text);
void LCD_DisplayTemp(float temperature, const char* classification);

#endif

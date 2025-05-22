#include "lcd.h"
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery_lcd.h"
#include <stdio.h>

void LCD_Init(void) {
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_DisplayOn();
    BSP_LCD_Clear(LCD_COLOR_WHITE);
}

void LCD_Clear(void) {
    BSP_LCD_Clear(LCD_COLOR_WHITE);
}

void LCD_SetTextColor(uint32_t color) {
    BSP_LCD_SetTextColor(color);
}

void LCD_SetBackColor(uint32_t color) {
    BSP_LCD_SetBackColor(color);
}

void LCD_DisplayString(uint16_t line, uint16_t column, uint8_t *text) {
    BSP_LCD_DisplayStringAt(column, line, text, LEFT_MODE);
}

void LCD_DisplayTemp(float temperature, const char* classification) {
    char tempStr[20];
    char classStr[20];

    LCD_Clear();

    // Display title
    LCD_SetTextColor(LCD_COLOR_BLUE);
    LCD_DisplayString(10, 5, (uint8_t*)"Temperature Monitor");

    // Display temperature
    LCD_SetTextColor(LCD_COLOR_BLACK);
    sprintf(tempStr, "Temp: %.1f C", temperature);
    LCD_DisplayString(50, 5, (uint8_t*)tempStr);

    // Display classification with appropriate color
    if (strcmp(classification, "Cold") == 0) {
        LCD_SetTextColor(LCD_COLOR_BLUE);
    } else if (strcmp(classification, "Warm") == 0) {
        LCD_SetTextColor(LCD_COLOR_ORANGE);
    } else if (strcmp(classification, "Hot") == 0) {
        LCD_SetTextColor(LCD_COLOR_RED);
    }

    sprintf(classStr, "Status: %s", classification);
    LCD_DisplayString(90, 5, (uint8_t*)classStr);

    // Footer
    LCD_SetTextColor(LCD_COLOR_DARKGRAY);
    LCD_DisplayString(200, 5, (uint8_t*)"TFLM Demo");
}

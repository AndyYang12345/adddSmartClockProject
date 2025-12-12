/**
  ******************************************************************************
  * @file    lcd1602.h
  * @brief   LCD1602 8-bit mode driver for STM32F103C8T6
  ******************************************************************************
  */

#ifndef __LCD1602_H
#define __LCD1602_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_WriteString(char *str);
void LCD_WriteChar(char ch);
void LCD_WriteNumber(int32_t num);
void LCD_DisplayControl(uint8_t display, uint8_t cursor, uint8_t blink);
void LCD_CreateChar(uint8_t location, uint8_t charmap[]);
void LCD_HardReset(void);
void LCD_RecoveryInit(void);
#ifdef __cplusplus
}
#endif

#endif /* __LCD1602_H */

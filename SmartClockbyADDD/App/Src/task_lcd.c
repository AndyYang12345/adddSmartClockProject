#include "task_lcd.h"

void TimeToLCD(int row, int col, char *time_str){
	LCD_SetCursor(row, col);
	LCD_WriteString(time_str);
}

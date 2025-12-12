#include "test.h"

void test_btn(void){
    static uint8_t last_key = 0;
    uint8_t current_key;

    // 扫描当前按键状态
    current_key = GetKeyVal();  // 直接读取当前按键值，不依赖状态变化

    // 只有当按键状态变化时才更新显示
    if(current_key != last_key) {
        last_key = current_key;

        LCD_SetCursor(0, 0);
        if(current_key == 0) {
            LCD_WriteString("No Key      ");  // 清空显示
        } else {
            LCD_WriteString("Key: ");
            LCD_WriteNumber((int32_t)current_key);  // 显示2位数字
            LCD_WriteString("   ");  // 清空剩余部分
        }
    }
}

void test_btn_detailed(void){
    static uint8_t last_key = 0;
    uint8_t current_key;

    // 直接测试每一列的原始状态
    LCD_SetCursor(0, 0);
    LCD_WriteString("Cols:");

    // 测试第一行，检查各列状态
    RowInit(1);  // 激活第一行
    HAL_Delay(1); // 增加稳定时间

    // 读取各列状态并显示
    uint8_t col1 = (HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_RESET);
    uint8_t col2 = (HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_RESET);
    uint8_t col3 = (HAL_GPIO_ReadPin(KEY7_GPIO_Port, KEY7_Pin) == GPIO_PIN_RESET);
    uint8_t col4 = (HAL_GPIO_ReadPin(KEY8_GPIO_Port, KEY8_Pin) == GPIO_PIN_RESET);

    LCD_SetCursor(0, 6);
    LCD_WriteNumber(col1);
    LCD_WriteNumber(col2);
    LCD_WriteNumber(col3);
    LCD_WriteNumber(col4);

    // 显示当前按键值
    current_key = GetKeyVal();
    LCD_SetCursor(1, 0);
    if(current_key != last_key) {
        last_key = current_key;
        if(current_key == 0) {
            LCD_WriteString("Key: --        ");
        } else {
            LCD_WriteString("Key: ");
            LCD_WriteNumber((int32_t)current_key);
            LCD_WriteString("   ");
        }
    }
}

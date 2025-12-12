#include "lcd1602.h"
#include "main.h"

/* 引脚定义 */
#define LCD_DB7_PIN    GPIO_PIN_1
#define LCD_DB7_PORT   GPIOA
#define LCD_DB6_PIN    GPIO_PIN_2
#define LCD_DB6_PORT   GPIOA
#define LCD_DB5_PIN    GPIO_PIN_3
#define LCD_DB5_PORT   GPIOA
#define LCD_DB4_PIN    GPIO_PIN_4
#define LCD_DB4_PORT   GPIOA
#define LCD_DB3_PIN    GPIO_PIN_5
#define LCD_DB3_PORT   GPIOA
#define LCD_DB2_PIN    GPIO_PIN_6
#define LCD_DB2_PORT   GPIOA
#define LCD_DB1_PIN    GPIO_PIN_7
#define LCD_DB1_PORT   GPIOA
#define LCD_DB0_PIN    GPIO_PIN_0
#define LCD_DB0_PORT   GPIOB

#define LCD_RS_PIN     GPIO_PIN_11
#define LCD_RS_PORT    GPIOB
#define LCD_RW_PIN     GPIO_PIN_10
#define LCD_RW_PORT    GPIOB
#define LCD_EN_PIN     GPIO_PIN_1
#define LCD_EN_PORT    GPIOB

/* 私有函数 */
static void LCD_WriteCommand(uint8_t command);
static void LCD_WriteData(uint8_t data);

/**
  * @brief  微秒延时
  */
static void LCD_Delay(uint32_t us)
{
    uint32_t ticks = us * (SystemCoreClock / 1000000) / 10;
    while(ticks--);
}

/**
  * @brief  写命令
  */
static void LCD_WriteCommand(uint8_t command)
{
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_DB7_PORT, LCD_DB7_PIN, (command & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB6_PORT, LCD_DB6_PIN, (command & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB5_PORT, LCD_DB5_PIN, (command & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB4_PORT, LCD_DB4_PIN, (command & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB3_PORT, LCD_DB3_PIN, (command & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB2_PORT, LCD_DB2_PIN, (command & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB1_PORT, LCD_DB1_PIN, (command & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB0_PORT, LCD_DB0_PIN, (command & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_Delay(10);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_Delay(100);
}

/**
  * @brief  写数据
  */
static void LCD_WriteData(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_DB7_PORT, LCD_DB7_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB6_PORT, LCD_DB6_PIN, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB5_PORT, LCD_DB5_PIN, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB4_PORT, LCD_DB4_PIN, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB3_PORT, LCD_DB3_PIN, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB2_PORT, LCD_DB2_PIN, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB1_PORT, LCD_DB1_PIN, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB0_PORT, LCD_DB0_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_Delay(10);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_Delay(100);
}
void LCD_RecoveryInit(void) {
    /* 专门用于从异常状态恢复的初始化 */

    // 步骤1：硬件复位
    LCD_HardReset();

    // 步骤2：延长上电等待时间
    HAL_Delay(100);  // 确保LCD完全稳定

    // 步骤3：执行标准的8位初始化序列（与您的版本一致）
    LCD_WriteCommand(0x38); // Function Set: 8-bit, 2-line, 5x8 dots
    HAL_Delay(15);   // 官方要求至少4.1ms

    LCD_WriteCommand(0x38); // 重复确保稳定
    HAL_Delay(5);

    LCD_WriteCommand(0x38); // 第三次
    HAL_Delay(5);

    // 步骤4：显示控制
    LCD_WriteCommand(0x08); // Display Off
    LCD_WriteCommand(0x01); // Clear Display
    HAL_Delay(10);   // 清屏需要更长时间

    // 步骤5：输入模式设置
    LCD_WriteCommand(0x06); // Entry Mode Set: Increment, No shift

    // 步骤6：最终显示开启
    LCD_WriteCommand(0x0C); // Display On, Cursor Off, Blink Off
    HAL_Delay(10);

    // 步骤7：再次清屏确保干净状态
    LCD_WriteCommand(0x01);
    HAL_Delay(10);
}

void LCD_HardReset(void) {
    /* 完全按照LCD1602数据手册的复位时序 */

    // 1. 确保所有控制线处于已知状态
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // 2. 数据线置于已知状态（不是高阻态，因为我们是输出模式）
    HAL_GPIO_WritePin(LCD_DB0_PORT, LCD_DB0_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB1_PORT, LCD_DB1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB2_PORT, LCD_DB2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB3_PORT, LCD_DB3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB4_PORT, LCD_DB4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB5_PORT, LCD_DB5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB6_PORT, LCD_DB6_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB7_PORT, LCD_DB7_PIN, GPIO_PIN_RESET);

    // 3. 上电延时（数据手册要求VCC上升到4.5V后等待15ms）
    HAL_Delay(15);

    // 4. 执行数据手册的8位初始化序列
    // 第一次发送功能设置命令（8位模式）
    HAL_GPIO_WritePin(LCD_DB7_PORT, LCD_DB7_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB6_PORT, LCD_DB6_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB5_PORT, LCD_DB5_PIN, GPIO_PIN_SET);   // 0x30
    HAL_GPIO_WritePin(LCD_DB4_PORT, LCD_DB4_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_DB3_PORT, LCD_DB3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB2_PORT, LCD_DB2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB1_PORT, LCD_DB1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DB0_PORT, LCD_DB0_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_Delay(10);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // 等待4.1ms以上
    HAL_Delay(5);

    // 第二次发送功能设置命令
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_Delay(10);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // 等待100us以上
    HAL_Delay(1);

    // 第三次发送功能设置命令
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_Delay(10);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // 等待40us以上
    LCD_Delay(100);

    // 5. 现在LCD已经进入8位模式，可以使用正常的写命令函数
    // 设置8位接口，2行显示，5x8点阵
    LCD_WriteCommand(0x38);
    HAL_Delay(1);

    // 显示关闭
    LCD_WriteCommand(0x08);
    HAL_Delay(1);

    // 清屏
    LCD_WriteCommand(0x01);
    HAL_Delay(2);

    // 输入模式设置：增量，不移位
    LCD_WriteCommand(0x06);
    HAL_Delay(1);

    // 显示开启，光标关闭，闪烁关闭
    LCD_WriteCommand(0x0C);
    HAL_Delay(1);
}

/**
  * @brief  LCD初始化
  */
void LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = LCD_DB7_PIN | LCD_DB6_PIN | LCD_DB5_PIN | LCD_DB4_PIN |
                         LCD_DB3_PIN | LCD_DB2_PIN | LCD_DB1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD_DB0_PIN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD_RS_PIN | LCD_RW_PIN | LCD_EN_PIN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET);

    HAL_Delay(50);

    LCD_WriteCommand(0x38);
    LCD_WriteCommand(0x38);
    LCD_WriteCommand(0x0C);
    LCD_WriteCommand(0x01);
    HAL_Delay(4);
    LCD_WriteCommand(0x06);
}

/**
  * @brief  清屏
  */
void LCD_Clear(void)
{
    LCD_WriteCommand(0x01);
    HAL_Delay(2);
}

/**
  * @brief  设置光标位置
  */
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    LCD_WriteCommand(address + col);
}

/**
  * @brief  显示字符串
  */
void LCD_WriteString(char *str)
{
    while (*str) {
        LCD_WriteData(*str++);
    }
}

/**
  * @brief  显示单个字符
  */
void LCD_WriteChar(char ch)
{
    LCD_WriteData(ch);
}

/**
  * @brief  显示数字
  */
void LCD_WriteNumber(int32_t num)
{
    if (num < 0) {
        LCD_WriteChar('-');
        num = -num;
    }

    if (num >= 10) {
        LCD_WriteNumber(num / 10);
    }
    LCD_WriteChar('0' + (num % 10));
}

/**
  * @brief  显示控制
  */
void LCD_DisplayControl(uint8_t display, uint8_t cursor, uint8_t blink)
{
    uint8_t command = 0x08;
    if (display) command |= 0x04;
    if (cursor)  command |= 0x02;
    if (blink)   command |= 0x01;
    LCD_WriteCommand(command);
}

/**
  * @brief  创建自定义字符
  */
void LCD_CreateChar(uint8_t location, uint8_t charmap[])
{
    LCD_WriteCommand(0x40 | (location << 3));
    for (uint8_t i = 0; i < 8; i++) {
        LCD_WriteData(charmap[i]);
    }
    LCD_WriteCommand(0x80);
}

#include "get_time.h"
#define TIME_FULL   0
#define TIME_DATE   1
#define TIME_CLOCK  2

static uint8_t current_pos = 0;//当前光标所处位置

static struct tm settingTime;
static struct tm originalTime;
static bool is_editing = false;
static bool has_unsaved_changes = false;  // 是否有未保存的修改
static bool direct_input_mode = false;

char weekdays[7][4] = {
    "SUN\0", "MON\0", "TUE\0", "WED\0", "THU\0", "FRI\0", "SAT\0"
};

char *GetTime(int mode, struct tm* time){
    static char message[3][50] = {0};  // 为三种模式分别提供缓冲区
    static uint8_t current_buf = 0;

    current_buf = (current_buf + 1) % 3;  // 循环使用三个缓冲区

    switch(mode){
        case TIME_DATE:
            sprintf(message[current_buf], "%d-%d-%d",
                time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
            break;
        case TIME_CLOCK:
            sprintf(message[current_buf], "%02d:%02d:%02d",
                time->tm_hour, time->tm_min, time->tm_sec);
            break;
        case TIME_FULL:
        default:
            sprintf(message[current_buf], "%d-%d-%d %02d:%02d:%02d",
                time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
                time->tm_hour, time->tm_min, time->tm_sec);
            break;
    }
    return message[current_buf];
}

char *GetWeekday(struct tm* time){
	return weekdays[time->tm_wday];
}

void ShowTime(struct tm* time){
    char line1[17] = {0};
    char line2[17] = {0};

    // 使用 unsigned int 消除范围警告
    unsigned int year = (unsigned int)(time->tm_year + 1900);
    unsigned int month = (unsigned int)(time->tm_mon + 1);
    unsigned int day = (unsigned int)time->tm_mday;
    unsigned int hour = (unsigned int)time->tm_hour;
    unsigned int minute = (unsigned int)time->tm_min;
    unsigned int second = (unsigned int)time->tm_sec;

    // 第一行：日期 + 星期
    snprintf(line1, sizeof(line1), "%04u-%02u-%02u %s",
             year, month, day, weekdays[time->tm_wday]);

    // 第二行：闹钟图标(左) + 时间 + 秒表图标(右)
    uint8_t alarm_enabled = IsAlarmEnabled();
    uint8_t stopwatch_running = IsStopwatchRunningInBackground();

    if(alarm_enabled && stopwatch_running) {
        // 同时显示闹钟和秒表
        snprintf(line2, sizeof(line2), "%c %02u:%02u:%02u %c",
                 4, hour, minute, second, 3);  // 闹钟在左，秒表在右
    }
    else if(alarm_enabled) {
        // 只显示闹钟
        snprintf(line2, sizeof(line2), "%c %02u:%02u:%02u   ",
                 4, hour, minute, second);  // 闹钟在左
    }
    else if(stopwatch_running) {
        // 只显示秒表
        snprintf(line2, sizeof(line2), "  %02u:%02u:%02u %c",
                 hour, minute, second, 3);  // 秒表在右
    }
    else {
        // 都不显示
        snprintf(line2, sizeof(line2), "  %02u:%02u:%02u   ",
                 hour, minute, second);
    }

    TimeToBLE(GetTime(TIME_FULL,time));
    TimeToLCD(0,1,line1);
    TimeToLCD(1,2,line2);
}
// 创建双向箭头自定义字符
// 创建5×8像素的双向箭头字符
void CreateArrowChars(void)
{
    // 先创建上箭头（确保索引0是上箭头）
    uint8_t up_arrow[] = {
        0b00100,  //   *
        0b01110,  //  ***
        0b11111,  // *****
        0b11111,  // *****
        0b01110,  //  ***
        0b01110,  //  ***
        0b01110,  //  ***
        0b01110   //  ***
    };

    // 粗下箭头 ↓ (5x8像素)
    uint8_t down_arrow[] = {
        0b01110,  //  ***
        0b01110,  //  ***
        0b01110,  //  ***
        0b01110,  //  ***
        0b11111,  // *****
        0b11111,  // *****
        0b01110,  //  ***
        0b00100   //   *
    };

    // 对称双向箭头 ↕ (5x8像素)
    uint8_t bidir_arrow[] = {
        0b00100,  //   *
        0b01110,  //  ***
        0b10101,  // * * *
        0b00100,  //   *
        0b00100,  //   *
        0b10101,  // * * *
        0b01110,  //  ***
        0b00100   //   *
    };

    // 计时器图标
    uint8_t stopwatch_icon[] = {
        0b01110,  //  ***
        0b10001,  // *   *
        0b10001,  // *   *
        0b10101,  // * * *
        0b10101,  // * * *
        0b10001,  // *   *
        0b10001,  // *   *
        0b01110   //  ***
    };
    uint8_t alarm_icon[] = {
		0b01110,  //  ***
		0b10001,  // *   *
		0b10001,  // *   *
		0b11111,  // *****
		0b10101,  // * * *
		0b10101,  // * * *
		0b01110,  //  ***
		0b00100   //   *
	};

    // 重新创建所有自定义字符，确保顺序正确
    LCD_CreateChar(0, up_arrow);       // 自定义字符0 = 粗上箭头
    LCD_CreateChar(1, down_arrow);     // 自定义字符1 = 粗下箭头
    LCD_CreateChar(2, bidir_arrow);    // 自定义字符2 = 双向箭头
    LCD_CreateChar(3, stopwatch_icon); // 自定义字符3 = 计时图标
    LCD_CreateChar(4, alarm_icon);     // 自定义字符4 = 闹钟图标

    // 可选：添加延迟确保字符创建完成
    HAL_Delay(100);
}

/**
  * @brief  保存时间到RTC
  * @retval 成功返回1，失败返回0
  */
uint8_t SaveTimeToRTC(void)
{
    if(ADDD_RTC_SetTime(&settingTime) == HAL_OK) {
        return 1;  // 成功
    } else {
        return 0;  // 失败
    }
    return 0;
}

void SaveTimeFromLine1(char *line1)
{
    // 解析月份 (位置0-1)
    settingTime.tm_mon = (line1[0] - '0') * 10 + (line1[1] - '0') - 1;  // tm_mon是0-11

    // 解析日期 (位置3-4)
    settingTime.tm_mday = (line1[3] - '0') * 10 + (line1[4] - '0');

    // 解析小时 (位置6-7)
    settingTime.tm_hour = (line1[6] - '0') * 10 + (line1[7] - '0');

    // 解析分钟 (位置9-10)
    settingTime.tm_min = (line1[9] - '0') * 10 + (line1[10] - '0');

    // 解析秒钟 (位置12-13)
    settingTime.tm_sec = (line1[12] - '0') * 10 + (line1[13] - '0');

    // 验证数据的有效性
    ValidateSettingTime();

//    SaveTimeToRTC();
}


uint8_t GetDaysInMonth(uint8_t month)
{
    switch(month) {
        case 1:  // 一月
        case 3:  // 三月
        case 5:  // 五月
        case 7:  // 七月
        case 8:  // 八月
        case 10: // 十月
        case 12: // 十二月
            return 31;

        case 4:  // 四月
        case 6:  // 六月
        case 9:  // 九月
        case 11: // 十一月
            return 30;

        case 2:  // 二月
            return 28;

        default:
            return 31;
    }
}

// 日期校验函数
void ValidateDay(char *line1)
{
	// 既检查月份又检查日期
	uint8_t month = (line1[0] - '0') * 10 + (line1[1] - '0');
	uint8_t day = (line1[3] - '0') * 10 + (line1[4] - '0');

	// 1. 先检查月份有效性
	if(month < 1 || month > 12) {
		line1[0] = '0';
		line1[1] = '1';  // 重置为1月
		month = 1;
	}

	// 2. 再检查日期有效性
	uint8_t max_days = GetDaysInMonth(month);
	if(day > max_days || day < 1) {
		line1[3] = (max_days / 10) + '0';
		line1[4] = (max_days % 10) + '0';
	}
}

// 获取指定月份的天数（不考虑闰年，因为新格式没有年份）

void AdjustCurrentDigit(char *line1, int8_t increment)
{
    // 新的位置映射："01-15 14:30:25"
    // 位置: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
    // 字符: 0 1 - 1 5   1 4 : 3 0 : 2 5

    switch(current_pos) {
        // 月份部分: "01"
        case 0: // 月十位 '0' (位置0)
            line1[0] = ((line1[0] - '0' + increment + 10) % 10) + '0';
            if(line1[0] > '1') line1[0] = '1'; // 限制0-1
            if(line1[0] < '0') line1[0] = '0';
            break;

        case 1: // 月个位 '1' (位置1)
            line1[1] = ((line1[1] - '0' + increment + 10) % 10) + '0';
            // 与月十位配合检查
            if(line1[0] == '1' && line1[1] > '2') line1[1] = '2'; // 10-12月
            if(line1[0] == '0' && line1[1] < '1') line1[1] = '1'; // 1-9月
            break;

        // 日期部分: "15" (跳过位置2的'-')
        case 2: // 日十位 '1' (位置3)
            line1[3] = ((line1[3] - '0' + increment + 10) % 10) + '0';
            if(line1[3] > '3') line1[3] = '3'; // 限制0-3
            if(line1[3] < '0') line1[3] = '0';
            break;

        case 3: // 日个位 '5' (位置4)
            line1[4] = ((line1[4] - '0' + increment + 10) % 10) + '0';
            ValidateDay(line1); // 需要实现日期验证
            break;

        // 时间部分: "14:30:25" (跳过位置5的空格)
        case 4: // 小时十位 '1' (位置6)
            line1[6] = ((line1[6] - '0' + increment + 10) % 10) + '0';
            if(line1[6] > '2') line1[6] = '2'; // 限制0-2
            if(line1[6] < '0') line1[6] = '0';
            break;

        case 5: // 小时个位 '4' (位置7)
            line1[7] = ((line1[7] - '0' + increment + 10) % 10) + '0';
            // 配合小时十位检查(0-23)
            if(line1[6] == '2' && line1[7] > '3') line1[7] = '3';
            if(line1[6] == '2' && line1[7] < '0') line1[7] = '0';
            break;

        case 6: // 分钟十位 '3' (位置9，跳过8的':')
            line1[9] = ((line1[9] - '0' + increment + 10) % 10) + '0';
            if(line1[9] > '5') line1[9] = '5'; // 限制0-5
            if(line1[9] < '0') line1[9] = '0';
            break;

        case 7: // 分钟个位 '0' (位置10)
            line1[10] = ((line1[10] - '0' + increment + 10) % 10) + '0';
            break;

        case 8: // 秒十位 '2' (位置12，跳过11的':')
            line1[12] = ((line1[12] - '0' + increment + 10) % 10) + '0';
            if(line1[12] > '5') line1[12] = '5'; // 限制0-5
            if(line1[12] < '0') line1[12] = '0';
            break;

        case 9: // 秒个位 '5' (位置13)
            line1[13] = ((line1[13] - '0' + increment + 10) % 10) + '0';
            break;
    }

    LCD_SetCursor(0, 1);
    LCD_WriteString(line1);
    SaveTimeFromLine1(line1);
    has_unsaved_changes = true;
}

void ValidateSettingTime(void)
{
    // 月份验证 (0-11)
    if(settingTime.tm_mon < 0) settingTime.tm_mon = 0;
    if(settingTime.tm_mon > 11) settingTime.tm_mon = 11;

    // 日期验证 (1-31)
    if(settingTime.tm_mday < 1) settingTime.tm_mday = 1;
    if(settingTime.tm_mday > 31) settingTime.tm_mday = 31;

    // 小时验证 (0-23)
    if(settingTime.tm_hour < 0) settingTime.tm_hour = 0;
    if(settingTime.tm_hour > 23) settingTime.tm_hour = 23;

    // 分钟验证 (0-59)
    if(settingTime.tm_min < 0) settingTime.tm_min = 0;
    if(settingTime.tm_min > 59) settingTime.tm_min = 59;

    // 秒钟验证 (0-59)
    if(settingTime.tm_sec < 0) settingTime.tm_sec = 0;
    if(settingTime.tm_sec > 59) settingTime.tm_sec = 59;

    // 更精确的日期验证（考虑月份天数）
    uint8_t max_days = GetDaysInMonth(settingTime.tm_mon + 1);
    if(settingTime.tm_mday > max_days) {
        settingTime.tm_mday = max_days;
    }
}
/**
  * @brief  处理直接数字输入
  * @param  line1: 时间字符串
  * @param  num: 输入的数字 (0-9)
  */
/**
  * @brief  处理直接数字输入（带实时验证）
  */
static void ProcessDirectInput(char *line1, uint8_t num)
{
    char original_line1[21];
    strcpy(original_line1, line1);  // 保存原始值

    // 根据当前位置设置对应的数字
    switch(current_pos) {
        case 0: // 月十位
            line1[0] = '0' + num;
            // 实时验证：月十位只能是0或1
            if(num > 1) {
                line1[0] = '1';  // 强制修正为1
            }
            current_pos = 1;
            break;

        case 1: // 月个位
            line1[1] = '0' + num;
            // 实时验证：如果月十位是1，月个位只能是0-2
            if(line1[0] == '1' && num > 2) {
                line1[1] = '2';  // 强制修正为2
            }
            // 实时验证：如果月十位是0，月个位不能是0
            if(line1[0] == '0' && num == 0) {
                line1[1] = '1';  // 强制修正为1
            }
            ValidateDay(line1);  // 验证日期
            current_pos = 2;
            break;

        case 2: // 日十位
            line1[3] = '0' + num;
            // 实时验证：日十位只能是0-3
            if(num > 3) {
                line1[3] = '3';  // 强制修正为3
            }
            current_pos = 3;
            break;

        case 3: // 日个位
            line1[4] = '0' + num;
            ValidateDay(line1);  // 验证日期
            current_pos = 4;
            break;

        case 4: // 小时十位
            line1[6] = '0' + num;
            // 实时验证：小时十位只能是0-2
            if(num > 2) {
                line1[6] = '2';  // 强制修正为2
            }
            current_pos = 5;
            break;

        case 5: // 小时个位
            line1[7] = '0' + num;
            // 实时验证：如果小时十位是2，小时个位只能是0-3
            if(line1[6] == '2' && num > 3) {
                line1[7] = '3';  // 强制修正为3
            }
            // 实时验证：如果小时十位是0-1，小时个位可以是0-9
            current_pos = 6;
            break;

        case 6: // 分钟十位
            line1[9] = '0' + num;
            // 实时验证：分钟十位只能是0-5
            if(num > 5) {
                line1[9] = '5';  // 强制修正为5
            }
            current_pos = 7;
            break;

        case 7: // 分钟个位
            line1[10] = '0' + num;
            current_pos = 8;
            break;

        case 8: // 秒十位
            line1[12] = '0' + num;
            // 实时验证：秒十位只能是0-5
            if(num > 5) {
                line1[12] = '5';  // 强制修正为5
            }
            current_pos = 9;
            break;

        case 9: // 秒个位
            line1[13] = '0' + num;
            // 循环回到月十位
            current_pos = 0;
            break;
    }

    // 更新显示并保存
    LCD_SetCursor(0, 1);
    LCD_WriteString(line1);
    SaveTimeFromLine1(line1);
    has_unsaved_changes = true;
}
void DateTimeEditModeCompact(SystemMode_t *current_status)
{
    static char line1[21];

    if (!is_editing) {
        settingTime = *ADDD_RTC_GetTime();
        originalTime = settingTime;
        is_editing = true;
        has_unsaved_changes = false;
        direct_input_mode = false;  // 进入编辑时重置为正常模式

        int month = settingTime.tm_mon + 1;
        int day = settingTime.tm_mday;
        int hour = settingTime.tm_hour;
        int minute = settingTime.tm_min;
        int second = settingTime.tm_sec;

        snprintf(line1, sizeof(line1), "%02u-%02u %02u:%02u:%02u",
                    (unsigned int)month, (unsigned int)day,
                    (unsigned int)hour, (unsigned int)minute, (unsigned int)second);
    }

    static uint8_t cursor_positions[] = {0,1,3,4,6,7,9,10,12,13};
    static uint8_t arrow_state = 0;
    static uint32_t last_key_time = 0;

    // 按键处理
    ButtonAction_t key = GetButtonAction(SYS_MODE_SETTINGS);
    uint32_t current_time = HAL_GetTick();

    if(key && key != BTN_NONE) {
        last_key_time = current_time;

        switch(key) {
            case BTN_UP:    // 2键 - 上/数字2
                if (!direct_input_mode) {
                    // 正常模式：方向键上
                    arrow_state = 1;
                    AdjustCurrentDigit(line1, 1);
                } else {
                    // 直接输入模式：数字2
                    ProcessDirectInput(line1, 2);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_DOWN:  // 8键 - 下/数字8
                if (!direct_input_mode) {
                    // 正常模式：方向键下
                    arrow_state = 2;
                    AdjustCurrentDigit(line1, -1);
                } else {
                    // 直接输入模式：数字8
                    ProcessDirectInput(line1, 8);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_LEFT:  // 4键 - 左/数字4
                if (!direct_input_mode) {
                    // 正常模式：方向键左
                    arrow_state = 0;
                    current_pos = (current_pos + 9) % 10;
                } else {
                    // 直接输入模式：数字4
                    ProcessDirectInput(line1, 4);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_RIGHT: // 6键 - 右/数字6
                if (!direct_input_mode) {
                    // 正常模式：方向键右
                    arrow_state = 0;
                    current_pos = (current_pos + 1) % 10;
                } else {
                    // 直接输入模式：数字6
                    ProcessDirectInput(line1, 6);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_NUM_0: // 0键 - 数字0（只在直接模式有效）
                if (direct_input_mode) {
                    ProcessDirectInput(line1, 0);
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                }
                // 正常模式下0键无功能，忽略
                break;

            case BTN_SETTING_SAVE:  // A键 - 保存（两种模式都有效）
                if(has_unsaved_changes){
                    if(SaveTimeToRTC()) {
                        is_editing = false;
                        has_unsaved_changes = false;
                        LCD_SetCursor(1, 0);
                        buzz(BUZZ_ON);
                        HAL_Delay(20);
                        buzz(BUZZ_OFF);
                        LCD_WriteString("Save Success!   ");
                        HAL_Delay(1000);
                        LCD_Clear();
                        *current_status = SYS_MODE_CLOCK;
                        LCD_DisplayControl(1, 0, 0);
                        return;
                    } else {
                        LCD_SetCursor(1, 0);
                        buzz(BUZZ_ON);
                        HAL_Delay(20);
                        buzz(BUZZ_OFF);
                        LCD_WriteString("Save Failed!    ");
                        HAL_Delay(500);
                        LCD_Clear();
                    }
                } else {
                    LCD_SetCursor(1, 0);
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                    LCD_WriteString("No Changes       ");
                    HAL_Delay(500);
                    is_editing = false;
                    *current_status = SYS_MODE_CLOCK;
                    LCD_DisplayControl(1, 0, 0);
                    return;
                }
                arrow_state = 3;
                break;

            case BTN_DIRECT_INPUT:  // C键 - 切换模式
                direct_input_mode = !direct_input_mode;
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_MOVE_LEFT:    // *键 - 左移（只在直接模式）
                if (direct_input_mode) {
                    arrow_state = 0;
                    current_pos = (current_pos + 9) % 10;
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                }
                break;

            case BTN_MOVE_RIGHT:   // #键 - 右移（只在直接模式）
                if (direct_input_mode) {
                    arrow_state = 0;
                    current_pos = (current_pos + 1) % 10;
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                }
                break;

            case BTN_NUM_1:        // 1键 - 数字1（只在直接模式）
            case BTN_NUM_3:        // 3键 - 数字3（只在直接模式）
            case BTN_NUM_5:        // 5键 - 数字5（只在直接模式）
            case BTN_NUM_7:        // 7键 - 数字7（只在直接模式）
            case BTN_NUM_9:        // 9键 - 数字9（只在直接模式）
                if (direct_input_mode) {
                    uint8_t num = key - BTN_NUM_0;
                    ProcessDirectInput(line1, num);
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                }
                break;

            case BTN_BACK:         // D键 - 返回
                settingTime = originalTime;
                LCD_SetCursor(1, 0);
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                LCD_WriteString("Edit Canceled   ");
                HAL_Delay(500);
                is_editing = false;
                has_unsaved_changes = false;
                direct_input_mode = false;
                LCD_Clear();
                *current_status = SYS_MODE_CLOCK;
                LCD_DisplayControl(1, 0, 0);
                return;

            default:
                // 忽略其他按键
                break;
        }
    }

    // 显示逻辑 - 简洁版本
    if (direct_input_mode) {
        // 直接输入模式：显示">"提示符
        LCD_SetCursor(1, 0);
        LCD_WriteString(">               ");
    } else {
        // 正常模式：完全清空第二行
        LCD_SetCursor(1, 0);
        LCD_WriteString("                ");
    }

    // 计算箭头位置并显示箭头（只在正常模式下显示）
    uint8_t arrow_pos = cursor_positions[current_pos] + 1;
    if(!direct_input_mode && arrow_pos < 16 && arrow_state != 3) {
        LCD_SetCursor(1, arrow_pos);
        switch(arrow_state) {
            case 0: LCD_WriteChar(2); break;  // 双向箭头
            case 1: LCD_WriteChar(0); break;  // 上箭头
            case 2: LCD_WriteChar(1); break;  // 下箭头
        }
    }

    // 显示第一行（时间）
    LCD_SetCursor(0, 1);
    LCD_WriteString(line1);

    // 设置光标到当前编辑位置
    LCD_SetCursor(0, cursor_positions[current_pos] + 1);
    LCD_DisplayControl(1, 1, 1);

    // 500ms后恢复双向箭头
    if((arrow_state == 1 || arrow_state == 2) && (current_time - last_key_time > 500)) {
        arrow_state = 0;
    }
}

void ChangeTime(SystemMode_t *current_status)
{
    DateTimeEditModeCompact(current_status);
}



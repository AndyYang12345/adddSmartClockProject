#include "alarm.h"

// 闹钟数据结构
static struct tm alarmTime = {0};
static uint8_t alarmEnabled = 0;
static uint8_t current_alarm_pos = 0;
static bool is_edit = false;
static bool direct_input_mode = false;

/**
  * @brief  验证闹钟时间的有效性
  */
void ValidateAlarmTime(void)
{
    // 小时验证 (0-23)
    if(alarmTime.tm_hour < 0) alarmTime.tm_hour = 0;
    if(alarmTime.tm_hour > 23) alarmTime.tm_hour = 23;

    // 分钟验证 (0-59)
    if(alarmTime.tm_min < 0) alarmTime.tm_min = 0;
    if(alarmTime.tm_min > 59) alarmTime.tm_min = 59;

    // 秒钟验证 (0-59)
    if(alarmTime.tm_sec < 0) alarmTime.tm_sec = 0;
    if(alarmTime.tm_sec > 59) alarmTime.tm_sec = 59;
}


/**
  * @brief  从line1字符串解析并更新alarmTime
  * @param  line1: 时间字符串 "  HH:MM:SS  "
  */

void SaveAlarmFromLine1(char *line1)
{
    // 解析小时 (位置4-5)
    alarmTime.tm_hour = (line1[4] - '0') * 10 + (line1[5] - '0');

    // 解析分钟 (位置7-8)
    alarmTime.tm_min = (line1[7] - '0') * 10 + (line1[8] - '0');

    // 解析秒钟 (位置10-11)
    alarmTime.tm_sec = (line1[10] - '0') * 10 + (line1[11] - '0');

    // 验证数据的有效性
    ValidateAlarmTime();
}
/**
  * @brief  调整闹钟数字
  * @param  line1: 时间字符串 "  HH:MM:SS  "
  * @param  increment: 增减值
  */
void AdjustAlarmDigit(char *line1, int8_t increment)
{
    // 位置映射："  HH:MM:SS  " (实际从第4个字符开始是时间)
    // 索引: 0 1 2 3 4 5 6 7 8 9 10 11
    // 字符:     H H : M M : S S
    // 可编辑位置: 4,5,7,8,10,11

    switch(current_alarm_pos) {
        // 小时部分: "HH"
        case 0: // 小时十位 (位置4)
            line1[4] = ((line1[4] - '0' + increment + 10) % 10) + '0';
            if(line1[4] > '2') line1[4] = '2'; // 限制0-2
            if(line1[4] < '0') line1[4] = '0';
            break;

        case 1: // 小时个位 (位置5)
            line1[5] = ((line1[5] - '0' + increment + 10) % 10) + '0';
            // 配合小时十位检查(0-23)
            if(line1[4] == '2' && line1[5] > '3') line1[5] = '3';
            if(line1[4] == '2' && line1[5] < '0') line1[5] = '0';
            break;

        // 分钟部分: "MM" (跳过位置6的':')
        case 2: // 分钟十位 (位置7)
            line1[7] = ((line1[7] - '0' + increment + 10) % 10) + '0';
            if(line1[7] > '5') line1[7] = '5'; // 限制0-5
            if(line1[7] < '0') line1[7] = '0';
            break;

        case 3: // 分钟个位 (位置8)
            line1[8] = ((line1[8] - '0' + increment + 10) % 10) + '0';
            break;

        // 秒钟部分: "SS" (跳过位置9的':')
        case 4: // 秒十位 (位置10)
            line1[10] = ((line1[10] - '0' + increment + 10) % 10) + '0';
            if(line1[10] > '5') line1[10] = '5'; // 限制0-5
            if(line1[10] < '0') line1[10] = '0';
            break;

        case 5: // 秒个位 (位置11)
            line1[11] = ((line1[11] - '0' + increment + 10) % 10) + '0';
            break;
    }

    // 立即更新显示
    LCD_SetCursor(0, 0);
    LCD_WriteString(line1);
}

// 简单的校验和计算函数
uint16_t CalculateAlarmChecksum_16bit(uint16_t data1, uint16_t data2)
{
    // 使用简单的异或校验
    uint16_t checksum = 0;
    checksum ^= (data1 >> 8) & 0xFF;    // 小时
    checksum ^= data1 & 0xFF;           // 分钟
    checksum ^= (data2 >> 8) & 0xFF;    // 秒钟
    checksum ^= data2 & 0xFF;           // 使能状态
    checksum ^= 0xAA;                   // 固定魔数

    return checksum;
}


/**
  * @brief  保存闹钟到存储
  * @retval 成功返回1，失败返回0
  */
uint8_t SaveAlarmToStorage(void)
{
    // BKP寄存器分配方案：
    // DR1: 已用于RTC_INIT_FLAG（保持不变）
    // DR2: 闹钟时间数据（小时+分钟）
    // DR3: 闹钟时间数据（秒钟+使能状态）
    // DR4: 数据校验和（确保数据完整性）

    // 确保解锁BKP
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    // 确保使用无符号类型
    uint8_t hour = (uint8_t)alarmTime.tm_hour;
    uint8_t minute = (uint8_t)alarmTime.tm_min;
    uint8_t second = (uint8_t)alarmTime.tm_sec;
    uint8_t enabled = alarmEnabled ? 0x01 : 0x00;

    // 使用两个16位寄存器存储数据
    // DR2: [15:8]小时 [7:0]分钟
    uint16_t alarm_data1 = ((uint16_t)hour << 8) | minute;

    // DR3: [15:8]秒钟 [7:0]使能状态
    uint16_t alarm_data2 = ((uint16_t)second << 8) | enabled;

    // 计算校验和（使用两个数据部分）
    uint16_t checksum = CalculateAlarmChecksum_16bit(alarm_data1, alarm_data2);

    for(int retry = 0; retry < 3; retry++) {
        // 写入三个16位寄存器
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, alarm_data1);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, alarm_data2);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, checksum);

        // 添加延迟确保写入完成
        HAL_Delay(20);

        // 读取验证
        uint16_t read_back_data1 = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
        uint16_t read_back_data2 = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
        uint16_t read_back_checksum = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);

        // 重新计算校验和进行验证
        uint16_t calculated_checksum = CalculateAlarmChecksum_16bit(read_back_data1, read_back_data2);

        if ((read_back_data1 == alarm_data1) &&
            (read_back_data2 == alarm_data2) &&
            (read_back_checksum == calculated_checksum)) {
            return 1;  // 保存成功
        }

        HAL_Delay(10);  // 重试前等待
    }

    return 0;  // 所有重试都失败
}
/**
  * @brief  处理闹钟直接数字输入（带实时验证）
  */
static void ProcessAlarmDirectInput(char *line1, uint8_t num)
{
    // 根据当前位置设置对应的数字
    switch(current_alarm_pos) {
        case 0: // 小时十位
            line1[4] = '0' + num;
            if(num > 2) line1[4] = '2';  // 限制0-2
            current_alarm_pos = 1;
            break;

        case 1: // 小时个位
            line1[5] = '0' + num;
            if(line1[4] == '2' && num > 3) line1[5] = '3';
            current_alarm_pos = 2;
            break;

        case 2: // 分钟十位
            line1[7] = '0' + num;
            if(num > 5) line1[7] = '5';  // 限制0-5
            current_alarm_pos = 3;
            break;

        case 3: // 分钟个位
            line1[8] = '0' + num;
            current_alarm_pos = 4;
            break;

        case 4: // 秒钟十位
            line1[10] = '0' + num;
            if(num > 5) line1[10] = '5';  // 限制0-5
            current_alarm_pos = 5;
            break;

        case 5: // 秒钟个位
            line1[11] = '0' + num;
            current_alarm_pos = 0;  // 循环回到小时十位
            break;
    }

    // 更新显示并保存
    LCD_SetCursor(0, 0);
    LCD_WriteString(line1);
    SaveAlarmFromLine1(line1);
}
/**
  * @brief  闹钟设置模式 - 紧凑版本
  * @note   显示格式：HH:MM:SS 居中显示，只编辑时分秒
  */
/**
  * @brief  闹钟设置模式 - 紧凑版本
  * @note   显示格式：HH:MM:SS 居中显示，只编辑时分秒
  */
/**
  * @brief  闹钟设置模式 - 紧凑版本
  * @note   显示格式：HH:MM:SS 居中显示，只编辑时分秒
  */
/**
  * @brief  闹钟设置模式 - 紧凑版本
  * @note   显示格式：HH:MM:SS 居中显示，只编辑时分秒
  */
/**
  * @brief  闹钟设置模式 - 紧凑版本
  * @note   显示格式：HH:MM:SS 居中显示，只编辑时分秒
  */
/**
  * @brief  闹钟设置模式 - 支持直接输入
  */
/**
  * @brief  闹钟设置模式 - 支持直接输入
  */
void AlarmEditModeCompact(SystemMode_t *current_status)
{
    static char line1[17];
    static bool first_enter = true;
    static uint8_t arrow_state = 0;
    static uint32_t last_key_time = 0;

    // 只有第一次进入时才从 alarmTime 初始化显示
    if(first_enter) {
        snprintf(line1, sizeof(line1), "    %02u:%02u:%02u    ",
                    (unsigned int)alarmTime.tm_hour,
                    (unsigned int)alarmTime.tm_min,
                    (unsigned int)alarmTime.tm_sec);
        first_enter = false;
        direct_input_mode = false;  // 重置直接输入模式
    }

    // 闹钟格式：HH:MM:SS (6个可编辑位置)
    static uint8_t cursor_positions[] = {0,1,3,4,6,7}; // 对应 "HH:MM:SS" 中的位置

    // 按键处理
    ButtonAction_t key = GetButtonAction(SYS_MODE_ALARM_EDIT);
    uint32_t current_time = HAL_GetTick();

    if(key && key != BTN_NONE) {
        last_key_time = current_time;

        switch(key) {
            case BTN_UP:    // 2键 - 上/数字2
                if (!direct_input_mode) {
                    arrow_state = 1;
                    AdjustAlarmDigit(line1, 1);
                } else {
                    ProcessAlarmDirectInput(line1, 2);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_DOWN:  // 8键 - 下/数字8
                if (!direct_input_mode) {
                    arrow_state = 2;
                    AdjustAlarmDigit(line1, -1);
                } else {
                    ProcessAlarmDirectInput(line1, 8);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_LEFT:  // 4键 - 左/数字4
                if (!direct_input_mode) {
                    arrow_state = 0;
                    current_alarm_pos = (current_alarm_pos + 5) % 6;
                } else {
                    ProcessAlarmDirectInput(line1, 4);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_RIGHT: // 6键 - 右/数字6
                if (!direct_input_mode) {
                    arrow_state = 0;
                    current_alarm_pos = (current_alarm_pos + 1) % 6;
                } else {
                    ProcessAlarmDirectInput(line1, 6);
                }
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_NUM_0: // 0键 - 数字0（只在直接模式）
                if (direct_input_mode) {
                    ProcessAlarmDirectInput(line1, 0);
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                }
                break;

            case BTN_SETTING_SAVE:  // A键 - 保存
                SaveAlarmFromLine1(line1);
                ValidateAlarmTime();
                alarmEnabled = 1;

                if(SaveAlarmToStorage()) {
                    LCD_SetCursor(1, 0);
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                    LCD_WriteString("Save Success!   ");
                    HAL_Delay(1000);
                    is_edit = false;
                    first_enter = true;
                    LCD_DisplayControl(1, 0, 0);
                    return;
                } else {
                    LCD_SetCursor(1, 0);
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                    LCD_WriteString("Save Failed!    ");
                    HAL_Delay(1000);
                }
                arrow_state = 3;
                break;

            case BTN_DIRECT_INPUT:  // C键 - 切换模式
                direct_input_mode = !direct_input_mode;  // 使用 direct_input_mode
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                break;

            case BTN_MOVE_LEFT:    // *键 - 左移（只在直接模式）
                if (direct_input_mode) {
                    arrow_state = 0;
                    current_alarm_pos = (current_alarm_pos + 5) % 6;
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                }
                break;

            case BTN_MOVE_RIGHT:   // #键 - 右移（只在直接模式）
                if (direct_input_mode) {
                    arrow_state = 0;
                    current_alarm_pos = (current_alarm_pos + 1) % 6;
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
                    ProcessAlarmDirectInput(line1, num);
                    buzz(BUZZ_ON);
                    HAL_Delay(20);
                    buzz(BUZZ_OFF);
                }
                break;

            case BTN_BACK:         // D键 - 返回
                LCD_SetCursor(1, 0);
                buzz(BUZZ_ON);
                HAL_Delay(20);
                buzz(BUZZ_OFF);
                LCD_WriteString("Edit Canceled   ");
                HAL_Delay(500);
                is_edit = false;
                first_enter = true;
                direct_input_mode = false;  // 退出时重置模式
                LCD_DisplayControl(1, 0, 0);
                return;

            default:
                break;
        }
    }

    // 显示逻辑
    if (direct_input_mode) {
        // 直接输入模式：显示">"提示符
        LCD_SetCursor(1, 0);
        LCD_WriteString(">               ");
    } else {
        // 正常模式：清空第二行
        LCD_SetCursor(1, 0);
        LCD_WriteString("                ");
    }

    // 计算箭头位置
    uint8_t arrow_pos = cursor_positions[current_alarm_pos] + 4; // +4因为前面有4空格

    // 设置箭头字符（只在正常模式下显示）
    if(!direct_input_mode && arrow_pos < 16 && arrow_state != 3) {
        LCD_SetCursor(1, arrow_pos);
        switch(arrow_state) {
            case 0: // 双向箭头
                LCD_WriteChar(2);
                break;
            case 1: // 上箭头
                LCD_WriteChar(0);
                break;
            case 2: // 下箭头
                LCD_WriteChar(1);
                break;
        }
    }

    // 显示第一行（时间）
    LCD_SetCursor(0, 0);
    LCD_WriteString(line1);

    // 设置光标到当前编辑位置
    LCD_SetCursor(0, cursor_positions[current_alarm_pos] + 4);
    LCD_DisplayControl(1, 1, 1);

    // 500ms后恢复双向箭头
    if((arrow_state == 1 || arrow_state == 2) && (current_time - last_key_time > 500)) {
        arrow_state = 0;
    }
}

void AlarmViewMode(SystemMode_t *current_status)
{
	if(is_edit){
		AlarmEditModeCompact(current_status);  // 添加参数
		return;
	}
    char line1[17], line2[17];

    // 显示闹钟时间
    snprintf(line1, sizeof(line1), "    %02u:%02u:%02u    ",
             alarmTime.tm_hour, alarmTime.tm_min, alarmTime.tm_sec);

    // 显示状态和操作提示
    if(alarmEnabled) {
        strcpy(line2, "  ON    EDIT:A  ");
    } else {
        strcpy(line2, "  OFF   EDIT:A  ");
    }

    LCD_SetCursor(0, 0);
    LCD_WriteString(line1);
    LCD_SetCursor(1, 0);
    LCD_WriteString(line2);

    // 按键处理
    ButtonAction_t key = GetButtonAction(SYS_MODE_ALARM_VIEW);
    if(key) {
        switch(key) {
            case BTN_ENTER:
            	is_edit = true;
            	buzz(BUZZ_ON);
				HAL_Delay(20);
				buzz(BUZZ_OFF);
            	AlarmEditModeCompact(current_status);
                break;

            case BTN_ALARM_TOGGLE:
                alarmEnabled = !alarmEnabled;
                buzz(BUZZ_ON);
				HAL_Delay(20);
				buzz(BUZZ_OFF);
                SaveAlarmToStorage();
                break;

            case BTN_BACK:
            	buzz(BUZZ_ON);
				HAL_Delay(20);
				buzz(BUZZ_OFF);
                *current_status = SYS_MODE_CLOCK;
                break;

            case BTN_UP:
            case BTN_DOWN:
            case BTN_LEFT:
            case BTN_RIGHT:
                // 在查看模式下这些按键无效，但可以给提示
                LCD_SetCursor(1, 0);
                LCD_WriteString("Press B to Edit.");
                HAL_Delay(300);
                break;

            default:
                // 静默忽略其他按键
                break;
        }
    }
}


// 配套的读取函数（供闹钟检查使用）
uint8_t LoadAlarmFromStorage(void)
{
    // 确保解锁BKP
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    // 读取16位数据
    uint16_t alarm_data1 = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    uint16_t alarm_data2 = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
    uint16_t stored_checksum = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);

    // 计算校验和验证数据完整性
    uint16_t calculated_checksum = CalculateAlarmChecksum_16bit(alarm_data1, alarm_data2);

    if (stored_checksum == calculated_checksum) {
        // 数据完整，解码
        alarmTime.tm_hour = (alarm_data1 >> 8) & 0xFF;
        alarmTime.tm_min = alarm_data1 & 0xFF;
        alarmTime.tm_sec = (alarm_data2 >> 8) & 0xFF;
        alarmEnabled = (alarm_data2 & 0xFF) ? 1 : 0;
        return 1;  // 加载成功
    } else {
        // 数据损坏，使用默认值
        alarmTime.tm_hour = 8;
        alarmTime.tm_min = 0;
        alarmTime.tm_sec = 0;
        alarmEnabled = 0;
        return 0;  // 加载失败
    }
}

/**
  * @brief  检查闹钟是否触发
  * @retval 触发返回1，否则返回0
  */
uint8_t AlarmCheck(void)
{
    if(!alarmEnabled) return 0;

    struct tm *now = ADDD_RTC_GetTime();

    // 简单的时间比较（精确到秒）
    if(now->tm_hour == alarmTime.tm_hour &&
       now->tm_min == alarmTime.tm_min &&
       now->tm_sec == alarmTime.tm_sec) {
        return 1;
    }

    return 0;
}

/**
  * @brief  闹钟提醒处理
  */
void AlarmAlert(void)
{
    // 状态机变量
    static enum {
        ALARM_BUZZ_ON,
        ALARM_BUZZ_OFF
    } alarm_state = ALARM_BUZZ_OFF;

    static uint32_t last_state_time = 0;
    static uint32_t last_key_check = 0;

    // 显示提醒（保留你的原始提示字符）
    LCD_SetCursor(0, 0);
    LCD_WriteString("----ALARM!!!----");
    LCD_SetCursor(1, 0);
    LCD_WriteString("Press any key...");

    // 初始化状态机
    alarm_state = ALARM_BUZZ_OFF;
    last_state_time = HAL_GetTick();
    last_key_check = HAL_GetTick();

    while(1) {
        uint32_t current_time = HAL_GetTick();

        // 状态机：蜂鸣器节奏控制（500ms切换）
        switch(alarm_state) {
            case ALARM_BUZZ_OFF:
                if(current_time - last_state_time > 500) {
                    buzz(BUZZ_ON);  // 开启蜂鸣器
                    Led(LED_ON);
                    alarm_state = ALARM_BUZZ_ON;
                    last_state_time = current_time;
                }
                break;

            case ALARM_BUZZ_ON:
                if(current_time - last_state_time > 500) {
                    buzz(BUZZ_OFF); // 关闭蜂鸣器
                    Led(LED_OFF);
                    alarm_state = ALARM_BUZZ_OFF;
                    last_state_time = current_time;
                }
                break;
        }

        // 频繁按键检测（每50ms）
        if(current_time - last_key_check > 50) {
            last_key_check = current_time;

            ButtonAction_t key = GetButtonAction(SYS_MODE_ALARM_VIEW);
            if(key) {
                // 停止提醒
                buzz(BUZZ_OFF); // 确保蜂鸣器关闭
                alarmEnabled = 0; // 单次闹钟，触发后禁用
                SaveAlarmToStorage(); // 保存状态

                // 可选：显示关闭反馈
                LCD_SetCursor(1, 0);
                Led(LED_OFF);
                LCD_WriteString("Alarm Closed!   ");
                HAL_Delay(500);
                LCD_Clear();
                return;
            }
        }

        // 短延时避免占用全部CPU
        HAL_Delay(10);
    }
}
/**
  * @brief  检查闹钟是否启用
  * @retval 1=启用, 0=禁用
  */
uint8_t IsAlarmEnabled(void)
{
    return alarmEnabled;
}

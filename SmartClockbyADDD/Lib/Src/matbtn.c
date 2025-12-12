#include "matbtn.h"

// 全局变量
uint8_t KeyPre = 0;
uint8_t KeyCur = 0;

/**
  * @brief  初始化矩阵键盘行线
  * @param  row: 行号 (1-4)
  * @retval None
  */
void RowInit(int row)
{
    switch(row){
    case 1:
        HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(KEY2_GPIO_Port, KEY2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY3_GPIO_Port, KEY3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY4_GPIO_Port, KEY4_Pin, GPIO_PIN_SET);
        break;
    case 2:
        HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY2_GPIO_Port, KEY2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(KEY3_GPIO_Port, KEY3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY4_GPIO_Port, KEY4_Pin, GPIO_PIN_SET);
        break;
    case 3:
        HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY2_GPIO_Port, KEY2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY3_GPIO_Port, KEY3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(KEY4_GPIO_Port, KEY4_Pin, GPIO_PIN_SET);
        break;
    case 4:
        HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY2_GPIO_Port, KEY2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY3_GPIO_Port, KEY3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(KEY4_GPIO_Port, KEY4_Pin, GPIO_PIN_RESET);
        break;
    }
}

/**
  * @brief  扫描矩阵键盘获取按键值
  * @retval 按键值 (1-16)，0表示无按键
  */
uint8_t GetKeyVal(void)
{
    uint8_t KeyVal = 0;

    for(int i = 0; i < 4; i++){
        RowInit(i+1);  // 设置当前行

        // 扫描各列
        if(HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_RESET){
            KeyVal = 4*i + 1;  // 第1列
        }
        else if(HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_RESET){
            KeyVal = 4*i + 2;  // 第2列
        }
        else if(HAL_GPIO_ReadPin(KEY7_GPIO_Port, KEY7_Pin) == GPIO_PIN_RESET){
            KeyVal = 4*i + 3;  // 第3列
        }
        else if(HAL_GPIO_ReadPin(KEY8_GPIO_Port, KEY8_Pin) == GPIO_PIN_RESET){
            KeyVal = 4*i + 4;  // 第4列 - 修复了重复扫描KEY7的问题
        }
    }
    return KeyVal;
}

/**
  * @brief  按键扫描函数
  * @param  GetType: 扫描类型 (DOWNSTREAM=按下, UPSTREAM=松开)
  * @retval 按键值 (1-16)，0表示无按键
  */
uint8_t KeyScan(int GetType)
{
    uint8_t KeyDown = 0;
    uint8_t KeyUp = 0;

    KeyPre = KeyCur;
    KeyCur = GetKeyVal();

    if(KeyCur != KeyPre){
        HAL_Delay(10);  // 软件消抖

        if(KeyCur && (GetType == DOWNSTREAM)){
            KeyDown = KeyCur;
            return KeyDown;
        }
        else if(GetType == UPSTREAM){
            KeyUp = KeyPre;
            return KeyUp;
        }
    }
    return 0;
}

// 按键映射表保持不变
static const ButtonAction_t key_mapping[][16] = {
	// 时钟模式 SYS_MODE_CLOCK
		// ┌───────┬───────┬───────┬───────┐
		// │       │       │       │设置模式│
		// │   1   │   2   │   3   │  (A)  │
		// ├───────┼───────┼───────┼───────┤
		// │       │       │       │闹钟模式│
		// │   4   │   5   │   6   │  (B)  │
		// ├───────┼───────┼───────┼───────┤
		// │       │       │       │计时模式│
		// │   7   │   8   │   9   │  (C)  │
		// ├───────┼───────┼───────┼───────┤
		// │   *   │   0   │   #   │       │
		// │       │       │       │  (D)  │
		// └───────┴───────┴───────┴───────┘
    [SYS_MODE_CLOCK] = {
        [KEY_1]    = BTN_NONE,
        [KEY_2]    = BTN_NONE,
        [KEY_3]    = BTN_NONE,
        [KEY_A]    = BTN_MODE_SETTINGS,
        [KEY_4]    = BTN_NONE,
        [KEY_5]    = BTN_NONE,
        [KEY_6]    = BTN_NONE,
        [KEY_B]    = BTN_MODE_ALARM,
        [KEY_7]    = BTN_NONE,
        [KEY_8]    = BTN_NONE,
        [KEY_9]    = BTN_NONE,
        [KEY_C]    = BTN_MODE_TIMER,
        [KEY_STAR] = BTN_NONE,
        [KEY_0]    = BTN_NONE,
        [KEY_HASH] = BTN_NONE,
        [KEY_D]    = BTN_NONE
    },


	// 时钟模式 SYS_MODE_CLOCK
	// ┌───────┬───────┬───────┬───────┐
	// │       │       │       │设置模式│
	// │   1   │   2   │   3   │  (A)  │
	// ├───────┼───────┼───────┼───────┤
	// │       │       │       │闹钟模式│
	// │   4   │   5   │   6   │  (B)  │
	// ├───────┼───────┼───────┼───────┤
	// │       │       │       │计时模式│
	// │   7   │   8   │   9   │  (C)  │
	// ├───────┼───────┼───────┼───────┤
	// │   *   │   0   │   #   │       │
	// │       │       │       │  (D)  │
	// └───────┴───────┴───────┴───────┘
    [SYS_MODE_ALARM_VIEW] = {
        [KEY_A]    = BTN_ENTER,
        [KEY_B]    = BTN_ALARM_TOGGLE,
        [KEY_D]    = BTN_BACK,
    },

	// 闹钟编辑模式 SYS_MODE_ALARM_EDIT
	// ┌───────┬───────┬───────┬───────┐
	// │       │   上   │       │  保存  │
	// │       │  (2)  │       │  (A)  │
	// ├───────┼───────┼───────┼───────┤
	// │   左   │       │   右   │      │
	// │   (4) │       │   (6) │       │
	// ├───────┼───────┼───────┼───────┤
	// │       │   下   │       │       │
	// │       │  (8)  │       │       │
	// ├───────┼───────┼───────┼───────┤
	// │       │       │       │  返回  │
	// │       │       │       │  (D)  │
	// └───────┴───────┴───────┴───────┘
	[SYS_MODE_ALARM_EDIT] = {
	    [KEY_2]    = BTN_UP,
	    [KEY_8]    = BTN_DOWN,
	    [KEY_A]    = BTN_SETTING_SAVE,
	    [KEY_D]    = BTN_BACK,
	    [KEY_4]    = BTN_LEFT,
	    [KEY_6]    = BTN_RIGHT,
	    [KEY_C]    = BTN_DIRECT_INPUT,  // 新增：C键切换模式
	    [KEY_STAR] = BTN_MOVE_LEFT,     // 新增：*键左移
	    [KEY_HASH] = BTN_MOVE_RIGHT,    // 新增：#键右移
	    [KEY_0]    = BTN_NUM_0,         // 新增：0键数字0
	    [KEY_1]    = BTN_NUM_1,         // 新增：1键数字1
	    [KEY_3]    = BTN_NUM_3,         // 新增：3键数字3
	    [KEY_5]    = BTN_NUM_5,         // 新增：5键数字5
	    [KEY_7]    = BTN_NUM_7,         // 新增：7键数字7
	    [KEY_9]    = BTN_NUM_9,         // 新增：9键数字9
	},

	// 秒表模式 SYS_MODE_TIMER
	// ┌───────┬───────┬───────┬───────┐
	// │       │       │       │  开始  │
	// │       │       │       │  (A)  │
	// ├───────┼───────┼───────┼───────┤
	// │   下   │       │   上  │  计次   │
	// │   (4) │       │   (6) │  (B)  │
	// ├───────┼───────┼───────┼───────┤
	// │       │       │       │  重置  │
	// │       │       │       │  (C)  │
	// ├───────┼───────┼───────┼───────┤
	// │       │       │       │  返回  │
	// │       │       │       │  (D)  │
	// └───────┴───────┴───────┴───────┘
    [SYS_MODE_TIMER] = {
        [KEY_A]    = BTN_TIMER_START,
        [KEY_B]    = BTN_TIMER_LAP,
        [KEY_D]    = BTN_BACK,
        [KEY_C]    = BTN_TIMER_RESET,
        [KEY_4]    = BTN_DOWN,
        [KEY_6]    = BTN_UP,
    },

	// 时间设置模式 SYS_MODE_SETTINGS - 24568布局
	// ┌───────┬───────┬───────┬───────┐
	// │       │  上   │        │ 保存  │
	// │       │  (2)  │       │  (A)  │
	// ├───────┼───────┼───────┼───────┤
	// │  左   │       │  右    │       │
	// │  (4)  │       │  (6)  │       │
	// ├───────┼───────┼───────┼───────┤
	// │       │  下   │       │       │
	// │       │  (8)  │       │       │
	// ├───────┼───────┼───────┼───────┤
	// │       │  保存  │       │   返回  │
	// │       │  (0)  │       │  (D)  │
	// └───────┴───────┴───────┴───────┘
	[SYS_MODE_SETTINGS] = {
	    [KEY_2]    = BTN_UP,
	    [KEY_8]    = BTN_DOWN,
	    [KEY_4]    = BTN_LEFT,
	    [KEY_6]    = BTN_RIGHT,
	    [KEY_0]    = BTN_SETTING_SAVE,
	    [KEY_A]    = BTN_SETTING_SAVE,
	    [KEY_D]    = BTN_BACK,
	    [KEY_5]    = BTN_DIRECT_INPUT,  // 新增：5键进入直接输入模式
	    // 其他按键保持 BTN_NONE
	},

    // 时间直接输入模式 SYS_MODE_TIME_DIRECT_INPUT
    // ┌───────┬───────┬───────┬───────┐
    // │   1   │   2   │   3   │  确认  │
    // │ 数字1   │ 数字2  │ 数字3  │  (A)  │
    // ├───────┼───────┼───────┼───────┤
    // │   4   │   5   │   6   │       │
    // │ 数字4   │ 数字5  │ 数字6  │       │
    // ├───────┼───────┼───────┼───────┤
    // │   7   │   8   │   9   │  删除  │
    // │ 数字7  │  数字8  │ 数字9  │  (C)  │
    // ├───────┼───────┼───────┼───────┤
    // │       │   0   │       │  取消  │
    // │       │ 数字0  │       │  (D)  │
    // └───────┴───────┴───────┴───────┘
	[SYS_MODE_SETTINGS] = {
	    // 2468双重功能
	    [KEY_2]    = BTN_UP,           // 2键 = 上/数字2
	    [KEY_8]    = BTN_DOWN,         // 8键 = 下/数字8
	    [KEY_4]    = BTN_LEFT,         // 4键 = 左/数字4
	    [KEY_6]    = BTN_RIGHT,        // 6键 = 右/数字6

	    // 功能键
	    [KEY_0]    = BTN_NUM_0,        // 0键 = 数字0（直接模式）/ 无功能（正常模式）
	    [KEY_A]    = BTN_SETTING_SAVE, // A键 = 保存（始终保存）
	    [KEY_D]    = BTN_BACK,         // D键 = 返回
	    [KEY_C]    = BTN_DIRECT_INPUT, // C键 = 切换模式

	    // 移动键
	    [KEY_STAR] = BTN_MOVE_LEFT,    // *键 = 左移
	    [KEY_HASH] = BTN_MOVE_RIGHT,   // #键 = 右移

	    // 数字键
	    [KEY_1]    = BTN_NUM_1,
	    [KEY_3]    = BTN_NUM_3,
	    [KEY_5]    = BTN_NUM_5,
	    [KEY_7]    = BTN_NUM_7,
	    [KEY_9]    = BTN_NUM_9,

	    // 其他按键设为无功能
	    [KEY_B]    = BTN_NONE,
	},
};

// 蓝牙命令处理变量
static volatile uint8_t bt_cmd_available = 0;
static volatile ButtonAction_t bt_cmd_value = BTN_NONE;

/**
  * @brief  设置蓝牙命令
  * @param  cmd: 蓝牙命令
  * @retval None
  */
void SetBluetoothCommand(ButtonAction_t cmd)
{
    if(cmd != BTN_NONE && cmd < BTN_COUNT) {
        bt_cmd_value = cmd;
        bt_cmd_available = 1;
    }
}

/**
  * @brief  获取按键动作
  * @param  current_mode: 当前系统模式
  * @retval 按键动作
  */
void TestKeyMapping(void)
{
    char buffer[17];
    LCD_Clear();

    while(1) {
        uint8_t physical_key = GetKeyVal();
        if(physical_key != 0) {
            snprintf(buffer, sizeof(buffer), "PhysKey: %2d    ", physical_key);
            LCD_SetCursor(0, 0);
            LCD_WriteString(buffer);

            // 显示对应的逻辑按键
            const char* key_names[] = {
                "KEY_1", "KEY_2", "KEY_3", "KEY_A",
                "KEY_4", "KEY_5", "KEY_6", "KEY_B",
                "KEY_7", "KEY_8", "KEY_9", "KEY_C",
                "KEY_STAR", "KEY_0", "KEY_HASH", "KEY_D"
            };

            if(physical_key <= 16) {
                snprintf(buffer, sizeof(buffer), "Logic: %-8s", key_names[physical_key-1]);
            } else {
                snprintf(buffer, sizeof(buffer), "Logic: INVALID ");
            }
            LCD_SetCursor(1, 0);
            LCD_WriteString(buffer);

            HAL_Delay(300); // 防重复
        }
        HAL_Delay(50);
    }
}
ButtonAction_t GetButtonAction(SystemMode_t current_mode)
{
    // 优先处理蓝牙命令（现在蓝牙命令是物理按键值）
    if(bt_cmd_available) {
        bt_cmd_available = 0;
        uint8_t physical_key = (uint8_t)bt_cmd_value;  // 蓝牙发送的物理按键值
        bt_cmd_value = BTN_NONE;

        // 使用物理按键值通过映射表获取逻辑按键
        if(physical_key >= 1 && physical_key <= 16) {
            uint8_t key_index = physical_key - 1;
            uint8_t max_mode = sizeof(key_mapping) / sizeof(key_mapping[0]);
            if(current_mode < max_mode) {
                return key_mapping[current_mode][key_index];
            }
        }
        return BTN_NONE;
    }

    // 扫描物理按键（原有逻辑不变）
    uint8_t raw_key = KeyScan(DOWNSTREAM);
    if(raw_key == 0) {
        return BTN_NONE;
    }

    // 边界检查
    if(raw_key < 1 || raw_key > 16) {
        return BTN_NONE;
    }

    uint8_t max_mode = sizeof(key_mapping) / sizeof(key_mapping[0]);
    if(current_mode >= max_mode) {
        return BTN_NONE;
    }

    // 将物理按键值转换为数组索引
    uint8_t key_index = raw_key - 1;
    return key_mapping[current_mode][key_index];
}

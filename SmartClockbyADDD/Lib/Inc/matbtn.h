#ifndef INC_MATBTN_H_
#define INC_MATBTN_H_
#define UPSTREAM 0
#define DOWNSTREAM 1

// 按键值定义
#define KEY_1       0
#define KEY_2       1
#define KEY_3       2
#define KEY_A       3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_B       7
#define KEY_7       8
#define KEY_8       9
#define KEY_9       10
#define KEY_C       11
#define KEY_STAR    12
#define KEY_0       13
#define KEY_HASH    14
#define KEY_D       15

typedef enum {
    BTN_NONE = 0,
    BTN_ENTER,
    BTN_BACK,
    BTN_UP,
    BTN_DOWN,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_MODE_CLOCK,
    BTN_MODE_SETTINGS,
    BTN_MODE_ALARM,
    BTN_MODE_TIMER,
    BTN_ALARM_TOGGLE,
    BTN_TIMER_START,
    BTN_TIMER_LAP,
    BTN_TIMER_RESET,
    BTN_SETTING_SAVE,
    BTN_SETTING_NEXT,
    BTN_SETTING_PREV,
    // 新增数字按键和移动按键
    BTN_NUM_0,
    BTN_NUM_1,
    BTN_NUM_2,
    BTN_NUM_3,
    BTN_NUM_4,
    BTN_NUM_5,
    BTN_NUM_6,
    BTN_NUM_7,
    BTN_NUM_8,
    BTN_NUM_9,
    BTN_CONFIRM,
    BTN_DELETE,
    BTN_DIRECT_INPUT,
    BTN_MOVE_LEFT,    // 新增：*键左移
    BTN_MOVE_RIGHT,   // 新增：#键右移
    BTN_COUNT
} ButtonAction_t;

typedef enum {
    SYS_MODE_CLOCK = 0,
    SYS_MODE_SETTINGS,
    SYS_MODE_ALARM_VIEW,
    SYS_MODE_ALARM_EDIT,
    SYS_MODE_TIMER,
    SYS_MODE_ALERT,
    SYS_MODE_TIME_DIRECT_INPUT,  // 新增直接输入模式
    SYS_MODE_COUNT
} SystemMode_t;

#include "gpio.h"
#include "blt.h"
#include "task_lcd.h"
uint8_t GetKeyVal(void);
uint8_t KeyScan(int);
void TestKeyMapping(void);
void RowInit(int row);
void SetBluetoothCommand(ButtonAction_t cmd);
ButtonAction_t GetButtonAction(SystemMode_t current_mode);

#endif /* INC_MATBTN_H_ */

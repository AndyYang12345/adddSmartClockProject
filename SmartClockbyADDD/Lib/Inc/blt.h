#ifndef INC_BLT_H_
#define INC_BLT_H_

#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "matbtn.h"

void TimeToBLE(char *time_str);

// 蓝牙命令缓冲区

// 蓝牙命令枚举（与ButtonAction_t对应）
typedef enum {
    BT_CMD_ENTER = 1,
    BT_CMD_BACK,
    BT_CMD_UP,
    BT_CMD_DOWN,
    BT_CMD_LEFT,
    BT_CMD_RIGHT,
    BT_CMD_MODE_CLOCK,
    BT_CMD_MODE_SETTINGS,
    BT_CMD_MODE_ALARM,
    BT_CMD_MODE_TIMER,
    BT_CMD_ALARM_TOGGLE,
    BT_CMD_TIMER_START,
    BT_CMD_TIMER_LAP,
    BT_CMD_TIMER_RESET
} BluetoothCommand_t;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Bluetooth_Init(void);
#endif /* INC_BLT_H_ */

#include "blt.h"
#include <stdio.h>

void TimeToBLE(char *time_str){	//没有延迟！！
	HAL_UART_Transmit(&huart1, (uint8_t*)time_str, strlen(time_str), HAL_MAX_DELAY);
}

volatile BluetoothCommand_t bt_command = 0;
volatile uint8_t bt_command_ready = 0;

// 新增：蓝牙数据到物理按键的映射
static uint8_t MapBluetoothToPhysicalKey(uint8_t bt_data)
{
    // 蓝牙数据到物理按键的映射表
    // 假设蓝牙发送 1-16 对应物理按键 1-16
    // 你可以根据需要调整这个映射关系
    if(bt_data >= 1 && bt_data <= 16) {
        return bt_data;  // 直接映射到对应的物理按键
    }
    return 0;  // 无效数据
}
// 修改串口接收中断回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1) {
        static uint8_t received_data;
        HAL_UART_Receive_IT(&huart1, &received_data, 1);

        // 将蓝牙数据映射到物理按键
        uint8_t physical_key = MapBluetoothToPhysicalKey(received_data);
        if(physical_key != 0) {
            // 设置蓝牙命令为物理按键值
            SetBluetoothCommand((ButtonAction_t)physical_key);
        }
    }
}


void Bluetooth_Init(void)
{
    uint8_t dummy;
    HAL_UART_Receive_IT(&huart1, &dummy, 1);
}

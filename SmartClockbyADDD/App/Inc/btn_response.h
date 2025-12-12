#ifndef INC_BTN_RESPONSE_H_
#define INC_BTN_RESPONSE_H_

#include "matbtn.h"//处理按键底层操作 包含键值映射数据体以及进程名称
#include "addd_rtc.h"//处理RTC计时 已包含BKP读取及储存
#include "usart.h"//初始化串口 用于蓝牙通信
#include "blt.h"//蓝牙业务逻辑（待编写）
#include "task_lcd.h"//LCD业务逻辑
#include "get_time.h"//计时业务逻辑
#include "buzzer.h"//蜂鸣器
#include "alarm.h"//闹钟功能业务逻辑
#include "timer.h"//秒表业务逻辑
#include "led.h"

void interface(void);//用户界面

#endif /* INC_BTN_RESPONSE_H_ */

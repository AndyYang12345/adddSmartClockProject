#include "task_main.h"

void MainTaskInit(){
	HAL_Delay(50);
	ADDD_RTC_Init();//RTC初始化
	/*
	 * 为了防止系统RTC驱动在每次上电时自检导致的计时错误以及自定义初始化时的时间，
	 * 自己编写的RTC驱动函数，在判断备份寄存器1中没有数据时，即VBAT第一次上电则
	 * 向其中写入flag以避免每次启动都进行初始化。
	 */
	LCD_Init();//LCD初始化
	/*
	 * AI为我根据数据手册生成的驱动，能够提供最基本的写入函数，为我包装成即拆即用的样子
	 * D老师简直是我的神
	 */
	LCD_Clear();//LCD清屏
	CreateArrowChars();//自定义字符初始化
	/*
	 * 包含我们自定义的若干个字符，占据了字模转换芯片的几个自定义字符位置
	 */
	LoadAlarmFromStorage();//从BKP寄存器读取闹钟数据
	/*
	 * 在保存闹钟至备份寄存器通道2、3时，还会生成其异或校验和并存在DR4中，
	 * 从备份寄存器中读取闹钟时，也会将读取到的闹钟数据与校验和进行比对，以防出现数据损失的情况
	 */
	Bluetooth_Init();//蓝牙模块开始串口监听
	TimInit();//计时器初始化
	Timer_Init();//秒表初始化
	/*
	 * 为秒表对象初始化成员变量数据
	 */
	Led(LED_OFF);//关闭LED
	/*
	 * 推挽输出高电平反而是关闭LED，图是这样画的。。
	 */
}

void MainTask(){
	interface();//主界面函数
	HAL_Delay(100);
	return;
}

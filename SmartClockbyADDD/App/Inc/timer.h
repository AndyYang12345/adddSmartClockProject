#ifndef __TIMER_H
#define __TIMER_H

#include "main.h"
#include "tim.h"
#include "btn_response.h"


// 秒表状态定义
typedef enum {
    TIMER_IDLE = 0,     // 初始/重置状态
    TIMER_RUNNING,      // 运行中
    TIMER_PAUSED        // 暂停
} TimerState_t;

// 函数声明
void TimerMode(SystemMode_t *current_status);
void TimInit();
void Timer_Init(void);
void ResetTimer(void);

void TIM4_10ms_Callback(void);
uint8_t IsStopwatchRunningInBackground(void);
uint32_t GetStopwatchCurrentTime(void);

#endif /* __TIMER_H */

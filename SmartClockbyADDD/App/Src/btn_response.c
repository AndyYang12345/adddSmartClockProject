#include "btn_response.h"

void ChangeMode(SystemMode_t *mode){
    ButtonAction_t key = GetButtonAction(SYS_MODE_CLOCK);

    if(key == BTN_BACK){
        // 返回时钟模式
        *mode = SYS_MODE_CLOCK;
        buzz(BUZZ_ON);
        HAL_Delay(20);
        buzz(BUZZ_OFF);
        LCD_Clear();  // 返回时钟模式时清屏
        return;
    }

    SystemMode_t new_mode = *mode;  // 默认保持当前模式

    switch(key){
    case BTN_MODE_SETTINGS:
        new_mode = SYS_MODE_SETTINGS;
        break;
    case BTN_MODE_ALARM:
        new_mode = SYS_MODE_ALARM_VIEW;
        break;
    case BTN_MODE_TIMER:
        new_mode = SYS_MODE_TIMER;
        break;
    default:
        return;  // 没有模式切换，直接返回
    }

    // 只有真正切换模式时才清屏和蜂鸣
    if(new_mode != *mode) {
        LCD_Clear();
        *mode = new_mode;
        buzz(BUZZ_ON);
        HAL_Delay(20);
        buzz(BUZZ_OFF);
    }
}

void interface(void){
    static SystemMode_t CurStatus = SYS_MODE_CLOCK;
    static SystemMode_t PrevStatus = SYS_MODE_CLOCK;

    // 闹钟检查
    if(AlarmCheck() == 1){
        AlarmAlert();
    }

    // 检查模式是否变化，如果变化则清屏
    if(CurStatus != PrevStatus) {
        LCD_Clear();
        PrevStatus = CurStatus;
    }

    switch(CurStatus){
    case SYS_MODE_CLOCK://时钟模式
        struct tm *now = ADDD_RTC_GetTime();
        ShowTime(now);
        ChangeMode(&CurStatus);
        break;

    case SYS_MODE_SETTINGS://时间设置模式
        ChangeTime(&CurStatus);  // 这里包含了直接输入功能
        ChangeMode(&CurStatus);
        break;

    case SYS_MODE_ALARM_VIEW://闹钟模式
        AlarmViewMode(&CurStatus);
        ChangeMode(&CurStatus);
        break;

    case SYS_MODE_TIMER://秒表模式
        TimerMode(&CurStatus);
        ChangeMode(&CurStatus);
        break;

    default://默认时钟模式
        CurStatus = SYS_MODE_CLOCK;
        LCD_Clear();
        break;
    }
}

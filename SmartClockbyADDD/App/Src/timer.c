#include "timer.h"

void TimInit(void){
    // 先设置优先级
    HAL_NVIC_SetPriority(TIM4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    // 开启定时器和中断
    HAL_TIM_Base_Start_IT(&htim4);
}

// 秒表数据结构
typedef struct {
    uint32_t total_10ms;        // 总时间（10ms为单位）
    uint32_t lap_times[10];     // 圈时记录（最多10圈）
    uint32_t last_lap_10ms;     // 上一圈开始时间
    uint8_t lap_count;          // 当前圈数
    uint8_t display_lap_idx;    // 当前显示的圈数索引
    TimerState_t state;         // 当前状态
    uint8_t update_display;     // 显示更新标志
    uint8_t running_in_background; // 添加这行：后台运行标志
} Stopwatch_t;

static Stopwatch_t stopwatch = {0};

/**
  * @brief  秒表初始化
  */
void Timer_Init(void)
{
    stopwatch.total_10ms = 0;
    stopwatch.lap_count = 0;
    stopwatch.last_lap_10ms = 0;
    stopwatch.display_lap_idx = 0;
    stopwatch.state = TIMER_IDLE;
    stopwatch.update_display = 1;
    stopwatch.running_in_background = 0;  // 添加这行
}


/**
  * @brief  格式化时间显示
  * @param  total_10ms: 总时间（10ms单位）
  * @param  buffer: 输出缓冲区
  */
static void FormatTimerTime(uint32_t total_10ms, char *buffer)
{
    uint32_t total_ms = total_10ms * 10;
    uint16_t minutes = total_ms / 60000;
    uint8_t seconds = (total_ms % 60000) / 1000;
    uint8_t centiseconds = (total_ms % 1000) / 10;

    sprintf(buffer, "%02d:%02d.%02d", minutes, seconds, centiseconds);
}


/**
  * @brief  格式化圈时显示（紧凑版本）
  * @param  lap_time_10ms: 圈时（10ms单位）
  * @param  buffer: 输出缓冲区（至少6字节）
  */
static void FormatLapTime(uint32_t lap_time_10ms, char *buffer)
{
    uint32_t lap_ms = lap_time_10ms * 10;
    uint8_t seconds = (lap_ms % 60000) / 1000;
    uint8_t centiseconds = (lap_ms % 1000) / 10;

    // 紧凑格式：00.00 (5字符)
    snprintf(buffer, 6, "%02d.%02d", seconds, centiseconds);
}

/**
  * @brief  更新秒表显示
  */
/**
  * @brief  更新秒表显示
  */
static void UpdateTimerDisplay(void)
{
    char line1[19], line2[19];

    // 第一行：状态图标 + 总时间
    switch(stopwatch.state) {
        case TIMER_RUNNING:
            snprintf(line1, sizeof(line1), ">");
            break;
        case TIMER_PAUSED:
            snprintf(line1, sizeof(line1), "||");
            break;
        case TIMER_IDLE:
        default:
            snprintf(line1, sizeof(line1), "[]");
            break;
    }

    // 添加总时间（确保总长度不超过16字符）
    char time_str[12];
    FormatTimerTime(stopwatch.total_10ms, time_str);

    // 计算剩余空间并安全拼接
    int remaining_space = sizeof(line1) - strlen(line1) - 1;
    if(remaining_space > 0) {
        strncat(line1, time_str, remaining_space);
    }

    // 第二行：根据状态显示不同的操作提示
    switch(stopwatch.state) {
        case TIMER_IDLE:
            // "START LAP BACK"
            strncpy(line2, "START           ", sizeof(line2));
            break;

        case TIMER_RUNNING:
            // "STOP LAP:XX"
            snprintf(line2, sizeof(line2), "STOP LAP:%02d     ", stopwatch.lap_count);
            break;

        case TIMER_PAUSED:
            if(stopwatch.lap_count > 0) {
                // 显示圈时信息 "LAPX 00.00 RST"
                char lap_str[6]; // 00.00
                FormatLapTime(stopwatch.lap_times[stopwatch.display_lap_idx], lap_str);
                snprintf(line2, sizeof(line2), "L%d %s RST    ",
                         stopwatch.display_lap_idx + 1, lap_str);
            } else {
                // 无圈时记录 "START --- RST"
                strncpy(line2, " START --- RST  ", sizeof(line2));
            }
            break;
    }

    // 确保字符串以null结尾
    line1[sizeof(line1) - 1] = '\0';
    line2[sizeof(line2) - 1] = '\0';

    // 显示到LCD
    LCD_SetCursor(0, 0);
    LCD_WriteString(line1);
    LCD_SetCursor(1, 0);
    LCD_WriteString(line2);
}
/**
  * @brief  记录圈时
  */
static void RecordLap(void)
{
    if(stopwatch.lap_count < 10) {
        // 计算当前圈用时
        uint32_t current_lap = stopwatch.total_10ms - stopwatch.last_lap_10ms;
        stopwatch.lap_times[stopwatch.lap_count] = current_lap;
        stopwatch.lap_count++;
        stopwatch.last_lap_10ms = stopwatch.total_10ms;
        stopwatch.display_lap_idx = stopwatch.lap_count - 1;
    }
}

/**
  * @brief  重置秒表
  */
/**
  * @brief  重置秒表
  */
void ResetTimer(void)
{
    stopwatch.total_10ms = 0;
    stopwatch.lap_count = 0;
    stopwatch.last_lap_10ms = 0;
    stopwatch.display_lap_idx = 0;
    stopwatch.state = TIMER_IDLE;
    stopwatch.running_in_background = 0;
    stopwatch.update_display = 1;
}

/**
  * @brief  处理圈时显示导航
  */
static void HandleLapNavigation(ButtonAction_t key)
{
    if(stopwatch.lap_count == 0) return;

    // 不再重新获取按键，使用传入的按键值
    if(key == BTN_UP) {
        stopwatch.display_lap_idx = (stopwatch.display_lap_idx + 1) % stopwatch.lap_count;
    } else if(key == BTN_DOWN) {
        stopwatch.display_lap_idx = (stopwatch.display_lap_idx + stopwatch.lap_count - 1) % stopwatch.lap_count;
    }
}

/**
  * @brief  秒表主模式
  * @param  current_status: 当前系统状态指针
  */
void TimerMode(SystemMode_t *current_status)
{
    // 每次进入秒表模式都强制更新显示
    UpdateTimerDisplay();

    // 按键处理
    ButtonAction_t key = GetButtonAction(SYS_MODE_TIMER);
    if(key != BTN_NONE) {
        // 调试：记录可疑的模式切换按键
        if(key == BTN_MODE_SETTINGS || key == BTN_MODE_ALARM || key == BTN_MODE_TIMER) {
            // 这些按键在秒表模式下不应该出现
            char debug[17];
            snprintf(debug, sizeof(debug), "ERR: ModeKey:%d", key);
            LCD_SetCursor(1, 0);
            LCD_WriteString(debug);
            HAL_Delay(1000);
            // 忽略这些按键，直接返回
            return;
        }

        uint8_t need_update = 0;

        switch(key) {
            case BTN_UP:
            case BTN_DOWN:
                // 上下键专门用于圈时导航（只在暂停状态下有效）
                if(stopwatch.state == TIMER_PAUSED && stopwatch.lap_count > 0) {
                    HandleLapNavigation(key);
                    need_update = 1;
                }
                break;

            case BTN_TIMER_START:
                if(stopwatch.state == TIMER_IDLE || stopwatch.state == TIMER_PAUSED) {
                    stopwatch.state = TIMER_RUNNING;
                    stopwatch.running_in_background = 1;
                    need_update = 1;
                } else if(stopwatch.state == TIMER_RUNNING) {
                    stopwatch.state = TIMER_PAUSED;
                    stopwatch.running_in_background = 0;
                    need_update = 1;
                }
                break;

            case BTN_TIMER_LAP:
                if(stopwatch.state == TIMER_RUNNING) {
                    RecordLap();
                    need_update = 1;
                }
                break;

            case BTN_TIMER_RESET:
                if(stopwatch.state == TIMER_PAUSED || stopwatch.state == TIMER_IDLE) {
                    ResetTimer();
                    stopwatch.running_in_background = 0;
                    need_update = 1;
                }
                break;

            case BTN_BACK:
                *current_status = SYS_MODE_CLOCK;
                buzz(BUZZ_ON);
				HAL_Delay(20);
				buzz(BUZZ_OFF);
                LCD_DisplayControl(1, 0, 0);
                return;

            default:
                // 忽略所有其他按键
                break;
        }

        // 如果需要更新，立即更新显示
        if(need_update) {
            UpdateTimerDisplay();
        }

        // 按键反馈
        buzz(BUZZ_ON);
        HAL_Delay(20);
        buzz(BUZZ_OFF);
    }

    // 检查是否有定时器触发的显示更新
    if(stopwatch.update_display) {
        stopwatch.update_display = 0;
        UpdateTimerDisplay();
    }
}

/**
  * @brief  TIM4 10ms中断回调函数 - 修改为始终计时
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM4) {
        // 只要秒表状态是RUNNING，就在后台持续计时
        if(stopwatch.state == TIMER_RUNNING) {
            stopwatch.total_10ms++;
            stopwatch.update_display = 1;  // 即使不在秒表模式也设置标志
        }
    }
}

/**
  * @brief  检查秒表是否在后台运行
  * @retval 1=运行中, 0=停止
  */
uint8_t IsStopwatchRunningInBackground(void)
{
    return stopwatch.running_in_background;
}
/**
  * @brief  获取秒表当前时间（用于在其他模式显示）
  */
uint32_t GetStopwatchCurrentTime(void)
{
    return stopwatch.total_10ms;
}


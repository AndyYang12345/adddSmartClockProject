#ifndef INC_ALARM_H_
#define INC_ALARM_H_

#include "get_time.h"
#include "buzzer.h"
#include "lcd1602.h"
#include "btn_response.h"
#include "led.h"
#include "stdbool.h"

// 函数声明
void ValidateAlarmTime(void);
void SaveAlarmFromLine1(char *line1);
void AdjustAlarmDigit(char *line1, int8_t increment);
uint8_t SaveAlarmToStorage(void);
void AlarmEditModeCompact(SystemMode_t *current_status);
void AlarmViewMode(SystemMode_t *current_status);
uint8_t LoadAlarmFromStorage(void);
uint8_t AlarmCheck(void);
void AlarmAlert(void);
uint8_t IsAlarmEnabled(void);


#endif /* INC_ALARM_H_ */

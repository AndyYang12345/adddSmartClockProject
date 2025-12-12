#ifndef INC_GET_TIME_H_
#define INC_GET_TIME_H_
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "blt.h"
#include "task_lcd.h"
#include "matbtn.h"
#include "addd_rtc.h"
#include "timer.h"
#include "buzzer.h"

char *GetTime(int mode, struct tm* time);
void ShowTime(struct tm* time);
void CreateArrowChars(void);
void ChangeTime(SystemMode_t *current_status);
void ValidateSettingTime(void);
void TimeDirectInputMode(SystemMode_t *current_status);

#endif /* INC_GET_TIME_H_ */

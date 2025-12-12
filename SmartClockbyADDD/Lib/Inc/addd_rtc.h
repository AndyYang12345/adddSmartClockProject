#ifndef INC_ADDD_RTC_H_
#define INC_ADDD_RTC_H_

#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "time.h"

HAL_StatusTypeDef ADDD_RTC_SetTime(struct tm *time);


struct tm* ADDD_RTC_GetTime();


void ADDD_RTC_Init();

#endif /* INC_ADDD_RTC_H_ */

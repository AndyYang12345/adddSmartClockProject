#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_
#ifndef BUZZ_ON
#define BUZZ_ON 1
#define BUZZ_OFF 0
#endif

#include "gpio.h"

void buzz(int state);

#endif /* INC_BUZZER_H_ */

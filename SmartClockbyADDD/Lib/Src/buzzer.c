#include "buzzer.h"

void buzz(int state){
	switch(state){
	case BUZZ_ON:
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		break;
	case BUZZ_OFF:
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		break;
	}
}

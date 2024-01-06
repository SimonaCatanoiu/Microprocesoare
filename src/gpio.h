#include "MKL25Z4.h"

#define RED_LED_PIN (18) //PORT B PIN 18
#define GREEN_LED_PIN (19) //PORT B PIN 19
#define BLUE_LED_PIN (1) //PORT D PIN 1

void GPIO_Init(void);
void GPIO_change_color(void);
void GPIO_change_seq(void);
void GPIO_start_stop(void);
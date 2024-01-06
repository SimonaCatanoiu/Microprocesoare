#include "MKL25Z4.h"
#define OSR 32
#define GPIO_CHANGE_FLAG 17 //POT SA PUN 17 deoarece nu exista caracter ascii cu aceasta valoare
#define ADC_START_END_TRANSMISSION 25 //la fel

void UART0_Initialize(uint32_t baud_rate);
void UART0_Transmit(uint8_t data);
void UART0_Transmit_ADC_val(uint16_t data);

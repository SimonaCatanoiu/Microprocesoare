#include "MKL25Z4.h"
#define OSR 32
#define GPIO_CHANGE_FLAG 17 //POT SA PUN 17 deoarece nu exista caracter ascii cu aceasta valoare

void UART0_Transmit(uint8_t data);
uint8_t UART0_Receive(void);
void UART0_Initialize(uint32_t baud_rate);
void UART0_IRQHandler(void);

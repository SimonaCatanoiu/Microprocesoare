#include "uart.h"
#include "gpio.h"
#include "pit.h"

#define BAUD_RATE 9600

int main(void) {
	rgb_led_init();
	//PIT_Init();
	UART0_Initialize(BAUD_RATE);
	for(;;){
	}
}

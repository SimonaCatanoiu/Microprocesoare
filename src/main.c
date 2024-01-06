#include "uart.h"
#include "gpio.h"
#include "pit.h"
#include "Adc.h"
#include "ClockSettings.h"
#include "Pwm.h"

#define BAUD_RATE 9600

int main(void) {
	GPIO_Init();
	PIT_Init();
	SystemClockTick_Configure();
	UART0_Initialize(BAUD_RATE);
	ADC0_Init();
	TPM2_Init();
	
	for(;;){
		if(flag_1s){
			uint16_t val = ADC_get_value();
			UART0_Transmit_ADC_val(val);
			TPM2_Set_DutyCycle(val);
			flag_1s = 0U;
		}
	}
}


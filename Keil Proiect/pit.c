#include "pit.h"
#include "Uart.h"
#include "gpio.h"

void PIT_Init(void) {
	
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Setarea valoarea numaratorului de pe canalul 0 la o perioada de 275 ms
	// 0.275*10485760 -1 = 2883583  // pentru test: 0x9FFFFF
	PIT->CHANNEL[0].LDVAL = 0x9FFFFF;

	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,1);
	NVIC_EnableIRQ(PIT_IRQn);
}


void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		GPIO_change_color();
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
	}
}
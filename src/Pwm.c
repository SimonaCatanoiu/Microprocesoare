#include "Pwm.h"

#define SERVO_PIN (2) // PORT B , PIN 2

void TPM2_Init(){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	PORTB->PCR[SERVO_PIN] |= PORT_PCR_MUX(3);
	
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	SIM->SCGC6 |= SIM_SCGC6_TPM2(1);
	
	TPM2->SC |= TPM_SC_PS(7);
	TPM2->SC |= TPM_SC_CPWMS(0);
	TPM2->SC |= TPM_SC_CMOD(1);
	
	TPM2->CONTROLS[0].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
}

void TPM2_Set_DutyCycle(uint16_t val)
{
	    uint8_t duty_cycle = 0;
			if (val > 3000) {
					duty_cycle = 2;
			} else if (val > 1000) {
					duty_cycle = 1;
			} else {
					duty_cycle = 0;
			}
			TPM2->CNT = 0x0000;
			TPM2->MOD = 375 * 20;
			TPM2->CONTROLS[0].CnV =100+duty_cycle*150;  //valori aproximativ calculate: 375 pt 0, 563 pentru 90 si 750 pentru 180 -> merge la aproximativ jumatate din valorile calculate?
}

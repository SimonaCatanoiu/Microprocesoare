#include "Pwm.h"

#define OSCILLOSCOPE_PIN (1) // PORT A , PIN 1

void TPM2_Init(){
	
	
	// Activarea semnalului de ceas pentru utilizarea LED-ului de culoare rosie
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	// Utilizarea alternativei de functionare pentru perifericul TMP
	// TMP2_CH0
	PORTA->PCR[OSCILLOSCOPE_PIN] |= PORT_PCR_MUX(3);
	
	// Selects the clock source for the TPM counter clock (MCGFLLCLK) - PG. 196
	// MCGFLLCLK Freq. - 48 MHz
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	// Activarea semnalului de ceas pentru modulul TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM2(1);
	
	// Divizor de frecventa pentru ceasul de intrare
	// PWM CLK -> 48MHz / 128 = 48.000.000 / 128 [Hz] = 375.000 [Hz] = 375 kHz
	TPM2->SC |= TPM_SC_PS(7);
	
		// LPTPM counter operates in up counting mode. - PG. 553
	// Selects edge aligned PWM
	TPM2->SC |= TPM_SC_CPWMS(0);
	
	// LPTPM counter increments on every LPTPM counter clock
	TPM2->SC |= TPM_SC_CMOD(1);
	
	
	// LPTPM counter operates in up-down counting mode. - PG. 553
	// Selects center aligned PWM
	//TPM2->SC |= TPM_SC_CPWMS(1);
	
	
	// Edge-Aligned Pulse Width Modulation
	// TPM->SC[CPWMS] = 0
	
	// ===== Mode selection ====
	// Configured for EPWM
	// TPM->CnSC[MnSB] = 1
	// TPM->CnSC[MnSB] = 0
	
	// ==== Edge selection ====
	// Set output on counter overflow, clear output on match
	// Counter overflow = LPTPM counter reaches the value configured in the MOD register, and then resets
	// Match = LPTPM counter reaches the value configured in the CnV register
	
	// TPM->CnSC[ELSnB] = 1
	// TPM->CnSC[ELSnA] = 0 
	
	// Regiter associated to the control of channel 0
	// Why channel 0?
	TPM2->CONTROLS[0].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
	
	
}
void Signal_Control(void){
	static uint8_t duty_cycle = 5;
    
	// Resetarea valorii numaratorului asociat LPTPM Counter
	TPM2->CNT = 0x0000;
	
	// Setarea perioadei semnalului PWM generat
  	TPM2->MOD = 375 * 5;
	
	// Setarea duty cycle-ului asociat semnalului PWM generat
  	TPM2->CONTROLS[0].CnV = (375 * 10 * duty_cycle) / 100;
	
	duty_cycle += 5;
  if (duty_cycle > 50)
  {   
		duty_cycle = 5;
  }
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
			TPM2->CONTROLS[0].CnV =100+duty_cycle*150;  //valori aproximativ calculate: 375 pt 0, 563 pentru 90 si 750 pentru 180 -> merge la jumatate din cauza ceasului de la UART cred
}

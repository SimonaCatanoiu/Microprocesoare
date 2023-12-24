#include "gpio.h"

uint8_t state=0;
uint8_t flag=0;
const uint8_t seq1[] = {0, 1, 2, 3, 4};
const uint8_t seq2[] = {4, 3, 2, 1, 0};

void GPIO_change_seq(void)
{
	if(flag==0)
	{
		flag=1;
		state=3;
		return;
	}
	flag=0;
	state=1;
}

void GPIO_change_color(void)
{
	//  state=1-> white on   state=2 -> green on    state=3  -> blue on  state=4 -> purple on
	//flag =1 -> reverse state
	if((state ==1 && flag==0)||(state==4 && flag==1))
	{
		GPIOB->PCOR|=(1<<RED_LED_PIN);
	  GPIOB->PCOR|=(1<<GREEN_LED_PIN);
	  GPIOD->PCOR|=(1<<BLUE_LED_PIN);
	}
	if((state==2 && flag==0)||(state==1 && flag==1))
	{
		GPIOB->PSOR|=(1<<RED_LED_PIN);
	  GPIOB->PCOR|=(1<<GREEN_LED_PIN);
	  GPIOD->PSOR|=(1<<BLUE_LED_PIN);
	}
	if((state ==3 && flag==0)||(state==2 && flag==1))
	{
		GPIOB->PSOR|=(1<<RED_LED_PIN);
	  GPIOB->PSOR|=(1<<GREEN_LED_PIN);
	  GPIOD->PCOR|=(1<<BLUE_LED_PIN);
	}
	if((state ==4 && flag==0)||(state==3 && flag==1))
	{
		GPIOB->PCOR|=(1<<RED_LED_PIN);
	  GPIOB->PSOR|=(1<<GREEN_LED_PIN);
	  GPIOD->PCOR|=(1<<BLUE_LED_PIN);
	}
	if(flag==0)
	{
		state=(state%4)+1;
	}
	else
	{
		if(state!=1)
			state=state-1;
		else
			state=4;
	}
}

void rgb_led_init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_PIN] |=PORT_PCR_MUX(1); 
	
	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] |=PORT_PCR_MUX(1);
	
	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] |=PORT_PCR_MUX(1);

	GPIOB->PDDR|= (1<<RED_LED_PIN);
	GPIOB->PDDR|= (1<<GREEN_LED_PIN);
	GPIOD->PDDR|= (1<<BLUE_LED_PIN);
	
	//dau psor la toate ca sa le sting
  GPIOB->PSOR|=(1<<RED_LED_PIN);
	GPIOB->PSOR|=(1<<GREEN_LED_PIN);
	GPIOD->PSOR|=(1<<BLUE_LED_PIN);
	
	state=1;
}
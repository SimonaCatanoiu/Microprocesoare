#include "uart.h"
#include "utils.h"
#include "gpio.h"

//uint8_t buffer[128];
//uint8_t write_index=0;
//uint8_t read_index=0;
uint16_t osr = OSR;

uint8_t gpio_message[]="GPIO SEQUENCE CHANGED";
uint8_t gpio_message_size=21;
uint8_t gpio_message_flag=0;

void UART0_Transmit(uint8_t data) {
	while(!(UART0->S1 & UART_S1_TDRE_MASK)) {}
	if(!gpio_message_flag)
		UART0->D = lsb_to_msb(data);
	if(gpio_message_flag)
	{
		uint8_t gpio_message_index=0;
		while(gpio_message_index<gpio_message_size)
		{
			while(!(UART0->S1 & UART_S1_TDRE_MASK)) {}
			UART0->D = lsb_to_msb(gpio_message[gpio_message_index]);
			gpio_message_index++;
		}
		gpio_message_flag=0;
	}
}

void UART0_Transmit_ADC_val(uint16_t data)
{
	  uint8_t letters[5];
		int index=0;
		while(data)
		{
			letters[index] = data%10+0x30;
			data/=10;
			index++;
		}
		index--;
		
		UART0_Transmit(ADC_START_END_TRANSMISSION);
		while(index>=0)
		{
			UART0_Transmit(letters[index--]);	
		}
		
    UART0_Transmit(ADC_START_END_TRANSMISSION);
}


void UART0_Initialize(uint32_t baud_rate) {

	uint16_t sbr;
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	//oprim re si te ca sa configuram
	UART0->C2 &= ~((UART0_C2_RE_MASK) | (UART0_C2_TE_MASK)); 
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(01);
	
	PORTA->PCR[1] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2);
	PORTA->PCR[2] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2);
	
	sbr = (uint16_t)((DEFAULT_SYSTEM_CLOCK)/(baud_rate * (osr)));
	UART0->BDH &= UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(osr - 1);
			
	UART0->C1 = UART0_C1_M(0) | UART0_C1_PE(0);
	
	//MSB first
	UART0->S2 = UART0_S2_MSBF(1);
	//TX Inverted
	//UART0->C3 |= UART0_C3_TXINV_MASK;
	
	//UART0->C2 |= UART0_C2_TCIE(0);
	UART0->C2 |= UART0_C2_RIE_MASK;;  //deschidem intreruperi pentru receive
	
	//deschidem re si te
	UART0->C2 |= ((UART_C2_RE_MASK) | (UART_C2_TE_MASK));
	
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_SetPriority(UART0_IRQn,2);
	NVIC_EnableIRQ(UART0_IRQn);
	__enable_irq();
}


void UART0_IRQHandler(void) {
		if(UART0->S1 & UART0_S1_RDRF_MASK)
		{
			uint8_t data=UART0->D;
			//Reverse LSB to MSB
			uint8_t reverse_data=msb_to_lsb(data);
			
			//ENTER HANDLER
			if(reverse_data==0x0D)
			{
				UART0_Transmit(0x0D);
				UART0_Transmit(0x0A);
			}
			
			//GPIO FLAG HANDLER
			if(reverse_data==GPIO_CHANGE_FLAG)
			{
				GPIO_change_seq();
				gpio_message_flag=1;
				UART0_Transmit(0);
				return;
			}
			//GPIO CHANGE COLOR
			if(reverse_data==18)
			{
				GPIO_change_color();
				return;
			}
			else
			{
				UART0_Transmit(reverse_data);
			}
		}
}

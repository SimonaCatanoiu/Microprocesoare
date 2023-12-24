#include "uart.h"
#include "utils.h"
#include "gpio.h"

uint8_t buffer[128];
uint8_t write_index=0;
uint8_t read_index=0;
uint16_t osr = OSR;

uint8_t gpio_message[]="GPIO SEQUENCE CHANGED";
uint8_t gpio_message_size=21;
uint8_t gpio_message_index=0;
uint8_t gpio_message_flag=0;

void UART0_Transmit(uint8_t data)
{
	//PIT Transmit logic
//	while (!(UART0->S1 & UART0_S1_TDRE_MASK)) {
//	}
//	UART0->D=lsb_to_msb(data);
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
	
	
	UART0->C2 |= UART0_C2_TCIE(0);
	UART0->C2 |= UART0_C2_RIE_MASK |  UART0_C2_TIE_MASK;
	
	UART0->C2 |= ((UART_C2_RE_MASK) | (UART_C2_TE_MASK));
	
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_SetPriority(UART0_IRQn,2);
	NVIC_EnableIRQ(UART0_IRQn);
	__enable_irq();
	
}


void UART0_IRQHandler(void) {
		if(UART0->S1 & UART0_S1_RDRF_MASK)//-> UART0_S1_RDRF_MASK spune ca bufferul de transimisie e full
		{
			uint8_t data=UART0->D;
			//Reverse LSB to MSB
			uint8_t reverse_data=msb_to_lsb(data);
			
			//ENTER HANDLER
			if(reverse_data==0x0D)
			{
				buffer[write_index++]=lsb_to_msb(0x0D);
				buffer[write_index++]=lsb_to_msb(0x0A);
			}
			
			//GPIO FLAG HANDLER
			if(reverse_data==GPIO_CHANGE_FLAG)
			{
				GPIO_change_seq();
				gpio_message_flag=1;
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
				buffer[write_index++]=data;
			}
		}
		
		//UART transmit data
	  if((UART0->S1 & UART0_S1_TDRE_MASK) && (read_index<write_index))//-> UART0_S1_TDRE_MASK spune ca bufferul de transimisie e empty
		{
			UART0->D = buffer[read_index++];
		} 
		
		//GPIO info message transmit 
		if((UART0->S1 & UART0_S1_TDRE_MASK) && (gpio_message_flag==1))
		{
			UART0->D = lsb_to_msb(gpio_message[gpio_message_index]);
			gpio_message_index++;
			if(gpio_message_index>gpio_message_size)
			{
				gpio_message_flag=0;
				gpio_message_index=0;
			}
		}
		
}

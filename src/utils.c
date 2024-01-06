#include "utils.h"
#include <stdint.h>

uint8_t msb_to_lsb(uint8_t data)
{
	uint8_t reverse_data=0;
	uint8_t i;
	for(i=0;i<8;i++)
	{
		reverse_data<<=1;
		reverse_data|=(data&1);
		data>>=1;
	}
	return reverse_data;
}

//aceeasi functie dar scrisa din nou ca sa ma ajute sa citesc
//mai bine codul (sa vad unde fac schimbarea)
uint8_t lsb_to_msb(uint8_t data)
{
	uint8_t reverse_data=0;
	uint8_t i;
	for(i=0;i<8;i++)
	{
		reverse_data<<=1;
		reverse_data|=(data&1);
		data>>=1;
	}
	return reverse_data;
}

uint8_t invert_data(uint8_t data)
{
	return ~data;
}
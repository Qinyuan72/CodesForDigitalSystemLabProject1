/*
 * Cylon.c
 *
 * Created: 01/06/2016 15:35:47
 * Author : ciaran.macnamee
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define DELAY_QTR_SEC 50000
#define DELAY_ONE_SEC 1000000

void move_leds(int polar, int delay)
{
	uint8_t i = 0;
	
	for (i=1; i < 128; i = i<<1)
	{
		if (polar == 1)
			PORTD = i;
		else
			PORTD = ~i;
			
		if (delay == 1)
			_delay_us(DELAY_QTR_SEC);
		else
			_delay_us(DELAY_ONE_SEC);
	}
	
	for (i=128; i > 1; i = i>>1)
	{
		if (polar == 1)
			PORTD = i;
		else
			PORTD = ~i;
            
		if (delay == 1)
			_delay_us(DELAY_QTR_SEC);
		else
			_delay_us(DELAY_ONE_SEC);
	}
}

int main(void)
{
	uint8_t pol = 0, del = 0;
	
	DDRD = 0b11111111;		/* set PORTD so all its pins are outputs  */
	DDRB = 0b00000000;		/* Set up Port B as all inputs */
	PORTB = 0b00110000;		/* Enable programmable pull ups on Portb Bits 5 & 4 */
	
	while(1)
	{
		if ((PINB & 0b0010000) == 0b0010000)
			pol = 0;
		else
			pol = 1;

		if ((PINB & 0b0010000) == 0b0010000)
			del = 0;
		else
			del = 1;
		move_leds(pol,del);
	}
		
	return 1;
}


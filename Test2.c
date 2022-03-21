#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t timecount0;
uint16_t Delay = 20;
uint8_t Pin5, Pin4;
uint8_t cylon = 1;
uint8_t direction = 1;
uint16_t adc_reading_1;
uint16_t adc_reading_2;
uint8_t cylon_mini = 16;
uint8_t volt_mini;
uint8_t new_adc;

void pin_instruction();
void cylon_eyes();
void voltage_thermo();
void voltage_thermo_mini();
void cylon_eyes_mini();

int main(void)
{
    DDRD = 0xff;
	DDRB = 0xff;
	PORTB = 0b00110000;
	
	timecount0 = 0;
	TCCR0B = 0b00000101;
	TCCR0A = 0b00000000;
	TCNT0 = 61;
	TIMSK0 = (1<<TOIE0);
	
	ADMUX = 0b01000000;
	ADCSRA = 0b11111111;
	
	sei();
	
    while (1) 
    {
		if(new_adc)
		{
			new_adc = 0;	
			pin_instruction();
			if(Pin4 == 0)
			{
				voltage_thermo();
			}
			if(Pin5 == 0)
			{
				voltage_thermo_mini();
				PORTD = cylon_mini|volt_mini;
			}
		}
    }
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 61;
	++timecount0;
	if(timecount0 >= Delay)
	{
		timecount0 = 0;
		if(Pin5 == 1)
		{
			PORTD = cylon;
			cylon_eyes();//full
		}
			
		if(Pin5 == 0){
			cylon_eyes_mini();//half
		}
	}
}

ISR(ADC_vect)
{	
	adc_reading_1 = ADC;
	adc_reading_2 = ADC;
	new_adc = 1;
	if (adc_reading_1 < 512)
	{
		Delay = 20; // 0.25s Delay
	}
	else
	{
		Delay = 80; // 1s Delay
	}
}

void pin_instruction()
{
	if(( PINB&0b00010000) == 0b00010000)
	{
		Pin4 = 1;
	}
	else
	{
		Pin4 = 0;
	}
	if(( PINB&0b00100000) == 0b00100000)
	{
		Pin5 = 1;
	}
	else
	{
		Pin5 = 0;
	}	
}

void cylon_eyes()
{
	if(direction == 0)
	{
		cylon = cylon << 1;
		if(cylon >= 128)
		{
			direction = 1;
			cylon = 128;
		}
	}
	else
	{
		cylon = cylon >> 1;
		if(cylon <= 1)
		{
			direction = 0;
			cylon = 1;
		}
	}
}

void voltage_thermo()
{
	if(adc_reading_2 < 128)
		PORTD = 0b00;
	if((adc_reading_2 >= 128) & (adc_reading_2 < 256))
		PORTD = 0b01;
	if((adc_reading_2 >= 256) & (adc_reading_2 < 384))
		PORTD = 0b011;
	if((adc_reading_2 >= 384) & (adc_reading_2 < 512))
		PORTD = 0b0111;
	if((adc_reading_2 >= 512) & (adc_reading_2 < 640))
		PORTD = 0b01111;
	if((adc_reading_2 >= 640) & (adc_reading_2 < 768))
		PORTD = 0b011111;
	if((adc_reading_2 >= 768) & (adc_reading_2 < 896))
		PORTD = 0b0111111;
	if((adc_reading_2 >= 896) & (adc_reading_2 < 1023))
		PORTD = 0b01111111;
	if(adc_reading_2 >= 1023)
		PORTD = 0b11111111;
}

void voltage_thermo_mini()
{
	if(adc_reading_2 < 256)
	volt_mini = 0b00000000;
	if((adc_reading_2 >= 256) & (adc_reading_2 < 512))
	volt_mini = 0b00000001;
	if((adc_reading_2 >= 512) & (adc_reading_2 < 768))
	volt_mini = 0b00000011;
	if((adc_reading_2 >= 768) & (adc_reading_2 < 1023))
	volt_mini = 0b00000111;
	if(adc_reading_2 >= 1023)
	volt_mini = 0b00001111;
}

void cylon_eyes_mini()
{
	if(direction == 0)
	{
		cylon_mini = cylon_mini << 1;
		if(cylon_mini >= 128)
		{
			direction = 1;
			cylon_mini = 128;
		}
	}
	else
	{
		cylon_mini = cylon_mini >> 1;
		if(cylon_mini <= 16)
		{
			direction = 0;
			cylon_mini = 16;
		}
	}
}
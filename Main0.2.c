/*
 * ADC_FR_EX2.c
 *
 * ADC Example using Free Running Mode
 * VREF = AVCC - simplest setup for us to use
 *
 * Created: 3/11/2020 16:38:00, this program uses the background to display ADC,
 * 14/02/2022, int8_t used for flag
 * Author : ciaran.macnamee
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#define THRESHOLD_VOLTAGE 512 /* This is the reading for 2.5V  */

#define ONE_EIGHTH_VOLTAGE 128
#define ONE_QUARTER_VOLTAGE 256
#define THREE_EIGHTH_VOLTAGE 384
#define HALF_VOLTAGE 512
#define FIVE_EIGHTH_VOLTAGE 640
#define THREE_QUARTER_VOLTAGE 768
#define SEVEN_EIGHTH_VOLTAGE 896

#define DLY_2_ms 200
#define COUNT_FOR_10ms 50
uint16_t timecount0;


volatile int8_t new_adc_data_flag;
volatile uint16_t adc_reading; /* Defined as a global here, because it's shared with main  */

int move_leds_counter = 0;
uint8_t PORTV = 0x01;

int main(void)
{

    new_adc_data_flag = 0;

    DDRD = 0b11111111;
    PORTD = 1; /* Initialise PORTD and Clear Bit 7  */

    DDRB = 0b00000000;  /* Set up Port B as all inputs */
    PORTB = 0b00110000; /* Enable programmable pull ups on Portb Bits 5 & 4 */

    timecount0 = 0;
    TCCR0B = (5 << CS00);
    TCCR0A = 0;
    TCNT0 = DLY_2_ms;
    TIMSK0 = (1 << TOIE0);

    ADMUX = ((1 << REFS0) | (0 << ADLAR) | (0 << MUX0));                              /* AVCC selected for VREF, ADC0 as ADC input  */
    ADCSRA = ((1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (7 << ADPS0)); // Enable ADC, Start Conversion, Auto Trigger enabled, Interrupt enabled, Prescale = 128
    ADCSRB = (0 << ADTS0);                                                            /* Select AutoTrigger Source to Free Running Mode
                                                                                         Strictly speaking - this is already 0, so we could omit the write to
                                                                                         ADCSRB, but this is included here so the intent is clear */

    sei(); // global interrupt enable

    while (1)
    {
        if ((PINB & 0x20) == 0x20)
        {
            if (new_adc_data_flag)
            {
                if (adc_reading < ONE_EIGHTH_VOLTAGE)
                {
              //      PORTD = 0x00;
                    PORTD = PORTD & 0x0F;
                }
                else if (adc_reading < ONE_QUARTER_VOLTAGE)
                {
               //     PORTD = 0x00;
                    PORTD = PORTD & 0x1F;
                }
                else if (adc_reading < THREE_EIGHTH_VOLTAGE)
                {
                //    PORTD = 0x00;
                    PORTD = PORTD & 0x3F;
                }
                else if (adc_reading < HALF_VOLTAGE)
                {
                   // PORTD = 0x00;
                    PORTD = PORTD & 0x7F;
                }
                else
                {
                    //PORTD = 0x00;
                    PORTD = PORTD & 0xFF;
                }

                new_adc_data_flag = 0;
            }
        }
        else
        {
            if ((PINB & 0x10) == 0x10)
            {
                if (new_adc_data_flag)
                {
                    if (adc_reading < ONE_EIGHTH_VOLTAGE)

                        PORTD = PORTD | 0b00000000;
                    else if (adc_reading < ONE_QUARTER_VOLTAGE)
                        PORTD = PORTD | 0b00000001;
                    else if (adc_reading < THREE_EIGHTH_VOLTAGE)
                        PORTD = PORTD | 0b00000011;
                    else if (adc_reading < HALF_VOLTAGE)
                        PORTD = PORTD | 0b00000111;
                    else
                        PORTD = 0b11111111;

                    new_adc_data_flag = 0;
                }
            }
            else
            {
                PORTD = PORTD | 0x00;
            }
        }
    }
}

ISR(TIMER0_OVF_vect)
{
    TCNT0 = DLY_2_ms;     /*	TCNT0 needs to be set to the start point each time				*/
    ++timecount0;         /* count the number of times the interrupt has been reached			*/
    if (timecount0 >= 20) /* 5 * 2ms = 10ms									*/
    {
        if ((PINB & 0x20) == 0x20)
        {
            ++move_leds_counter;
            // PORTD = i;
            if (move_leds_counter < 4)
            {
                if（direction == 0)

                    //Shift left
                else
                    //Shift RIght

                /*PORTV = PORTV << 1;
                PORTD = PORTD ^ PORTV;*/
            }
            else if (move_leds_counter =  3)
            {
                PORTV = 0x01;
                PORTD = PORTD ^ PORTV;


                //Shift left
                //set move_leds_contor = 0
                //toggle direction!
                move_leds_counter = 0;
                /
            }
            else
        }
        else
        {
            if ((PINB & 0x10) == 0x10)
            {
                PORTD = 0;
            }
            else
            {
                ++move_leds_counter;
                // PORTD = i;
                if (move_leds_counter < 8)
                {
                    PORTD = PORTD << 1;
                }
                else if (move_leds_counter > 14)
                {
                    PORTD = 0x01;
                    move_leds_counter = 0;
                }
                else
                {
                    PORTD = PORTD >> 1;
                }
            }
        }
        timecount0 = 0;
    }
}

ISR(ADC_vect) /* handles ADC interrupts  */
{
    adc_reading = ADC; /* ADC is in Free Running Mode - you don't have to set up anything for
                          the next conversion */
    new_adc_data_flag = 1;
    
}

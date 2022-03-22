/*
 * Main.c
 *
 * Code for assinment 1 of Digital system
 *
 * Created: 3/8/2022 16:38:00
 * 14/02/2022, int8_t used for flag
 * Author : ciaran.macnamee
 * Author : Qinyuan.Liu 20137095, Mandisi.Sibanda 20146817
 */

#include <math.h>
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
#define FULL_SCALE 1022
#define DLY_2_ms 61
#define COUNT_FOR_10ms 50
uint16_t timecount0;
uint16_t time_rowover = 40;

volatile int8_t new_adc_data_flag;
volatile uint16_t adc_reading; /* Defined as a global here, because it's shared with main  */

int move_leds_counter = 0;
uint8_t PORTV = 0x01;
int derection = 0;

/*
    @param int    limit The upper limit for the cylon.
    @param int twoPower    twoPower = 2^(limit-1), can't find a way to incroprate this equation in to the code, need help.
    this function is to achive cylon efficet, called at both the full portD cylon, and the split cylon.
*/
void move_bit(int limit, int twoPower)
{
    ++move_leds_counter;
    if (derection == 0)
    {
        PORTV = PORTV << 1;
        PORTD = PORTV;
    }

    else
    {
        // move the bit right
        PORTV = PORTV >> 1;
        PORTD = PORTV;
    }

    if (move_leds_counter >= limit - 1)
    {
        move_leds_counter = 0;
        if (derection == 0)
            PORTV = twoPower; // 2^(limit-1)
        derection = ~derection;
    }
}

/*
    Display ADC value on o-7 bit of portD. should just directily set PortD?
*/
void ADC_reading_display_full_ADC()
{
    if (adc_reading < ONE_EIGHTH_VOLTAGE)
        PORTD = 0b00000000;
    else if (adc_reading < ONE_QUARTER_VOLTAGE)
        PORTD = 0b00000001;
    else if (adc_reading < THREE_EIGHTH_VOLTAGE)
        PORTD = 0b00000011;
    else if (adc_reading < HALF_VOLTAGE)
        PORTD = 0b00000111;
    else if (adc_reading < FIVE_EIGHTH_VOLTAGE)
        PORTD = 0b00001111;
    else if (adc_reading < FIVE_EIGHTH_VOLTAGE)
        PORTD = 0b00001111;
    else if (adc_reading < THREE_QUARTER_VOLTAGE)
        PORTD = 0b00011111;
    else if (adc_reading < SEVEN_EIGHTH_VOLTAGE)
        PORTD = 0b00111111;
    else if (adc_reading < FULL_SCALE)
        PORTD = 0b01111111;
    else
        PORTD = 0b11111111;
}

/*
    Conbin (bitwise or "|") half cylon (portD bit 0-3) and half of the ADC reading(portD bit 4-7)
*/
void ADC_reading_display_half_cylon_half_ADC()
{
    if (adc_reading < ONE_QUARTER_VOLTAGE)
    {
        PORTD = PORTD | 0x00;
    }
    else if (adc_reading < HALF_VOLTAGE)
    {
        PORTD = PORTD | 0x10;
    }
    else if (adc_reading < THREE_QUARTER_VOLTAGE)
    {
        PORTD = PORTD | 0x30;
    }
    else if (adc_reading < FULL_SCALE)
    {
        PORTD = PORTD | 0x70;
    }
    else
    {

        PORTD = PORTD | 0xF0;
    }
}

/*
    Evalueate the ADC value and change the overflow imiter for timecount0.
    To be finished.
*/
void ADC_cylon_speed_control()
{
    if (adc_reading < HALF_VOLTAGE)
        time_rowover = 20;
    else
        time_rowover = 80;
}

/*
    Desear and configure some registers for ADC
*/
void ADC_Declaration()
{
    new_adc_data_flag = 0;
    ADMUX = ((1 << REFS0) | (0 << ADLAR) | (0 << MUX0));                              /* AVCC selected for VREF, ADC0 as ADC input  */
    ADCSRA = ((1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (7 << ADPS0)); // Enable ADC, Start Conversion, Auto Trigger enabled, Interrupt enabled, Prescale = 128
    ADCSRB = (0 << ADTS0);
}

/*
    Decear and configure some registers for Time_ISR
*/
void Time_ISR_Declaration()
{
    timecount0 = 0;
    TCCR0B = (5 << CS00);
    TCCR0A = 0;
    TCNT0 = DLY_2_ms;
    TIMSK0 = (1 << TOIE0);
}

/*
    Decear and configure some registers for PortD & ProtB
*/
void PortB_D_Declaration()
{
    DDRD = 0b11111111;
    PORTD = 1; /* Initialise PORTD and Clear Bit 7  */

    DDRB = 0b00000000;  /* Set up Port B as all inputs */
    PORTB = 0b00110000; /* Enable programmable pull ups on Portb Bits 5 & 4 */
}

int main(void)
{
    PortB_D_Declaration();
    Time_ISR_Declaration();
    ADC_Declaration();
    sei(); // global interrupt enable

    while (1)
    {
        if (new_adc_data_flag)
        {
            if (PINB == 0b00110000)
            { // do nothing
            }
            if (PINB == 0b00010000)
                ADC_reading_display_half_cylon_half_ADC();
            if (PINB == 0b00100000)
                ADC_reading_display_full_ADC();
            new_adc_data_flag = 0;
        }
    }
}

ISR(TIMER0_OVF_vect)
{
    TCNT0 = DLY_2_ms;               /*  TCNT0 needs to be set to the start point each time        */
    ++timecount0;                   /* count the number of times the interrupt has been reached      */
    if (timecount0 >= time_rowover) /* 5 * 2ms = 10ms                  */
    {
        if (PINB == 0b00110000)
            move_bit(8, 128);
        if (PINB == 0b00010000)
            move_bit(4, 8);
        if (PINB == 0b00100000)
        {
        }
        new_adc_data_flag = 0;
        timecount0 = 0;
    }
}

ISR(ADC_vect) /* handles ADC interrupts  */
{
    adc_reading = ADC; /* ADC is in Free Running Mode - you don't have to set up anything for the next conversion */
    ADC_cylon_speed_control();
    new_adc_data_flag = 1;
}
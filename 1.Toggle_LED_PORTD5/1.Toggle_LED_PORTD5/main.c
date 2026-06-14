/*
 * 1.Toggle_LED_PORTD5.c
 *
 * Created: 12-06-2026 13:36:39
 * Author : kiran
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /*Configure PD5 as output*/
	DDRD|=(1<<PD5);
	
    while (1) 
    {
		PORTD |= (1<<PD5); // LED On
		_delay_ms(3000);
		
		PORTD &=~(1<<PD5);
		_delay_ms(5000);
    }
	return 0;
}


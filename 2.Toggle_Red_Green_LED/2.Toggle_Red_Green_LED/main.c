/*
 * 2.Toggle_Red_Green_LED.c
 *
 * Created: 12-06-2026 13:58:16
 * Author : kiran
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    // Set PB0(RED) and PB1(Green) as outputs
	DDRB |= (1<<PB0) | (1<<PB1);
	
	// Set PD2(Button) as input and enable its internal pull-up resistor
	DDRD &=~(1<<PD2);
	PORTD |=(1<<PD2);
	
	// Initial state: RED ON and GREEN OFF
	PORTB |=(1<<PB0);
	PORTB &=~(1<<PB1);
	
    while (1) 
    {
		// Wait here till the button is not pressed
		while(PIND & (1<<PD2)); // Button has been pressed
		
		PORTB ^=(1<<PB0) | (1<<PB1); // Toggle both pin simultaneously
		_delay_ms(250);	
    }
}


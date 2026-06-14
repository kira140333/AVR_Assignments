#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// Traffic light state definitions
#define STATE_GREEN    0
#define STATE_ORANGE   1
#define STATE_RED      2

volatile uint8_t seconds_elapsed = 0;

//Interrupt Service Routine for Timer1.
ISR(TIMER1_COMPA_vect) {
	seconds_elapsed++;
}

int main(void) {
	// Configure PB0,PB1,PB2 as outputs
	DDRB|=(1<<PB0) | (1<<PB1) | (1<<PB2);
	
	// Configure Timer1 for Clear Timer on Compare Match
	TCCR1B|=(1<<WGM12); // Turn on CTC mode
	TCCR1B|=(1<<CS12)|(1<<CS10); // Set prescalar to 1024
	
	// Calculate Compare match value: OCR1A = (16000000/(1024*1))-1 =15624
	OCR1A=15624;
	
	TIMSK1|=(1<<OCIE1A); // Enable Timer1 Compare match A interrupt
	sei();
	
	// Initial state setup
	uint8_t current_state=STATE_GREEN;
	PORTB=(1<<PB0); // Turn GREEN ON, others OFF
	seconds_elapsed=0;
	
	// Main loop
	while(1) {
		switch(current_state) {
			
			case STATE_GREEN:
			if(seconds_elapsed>=10) {
				PORTB=(1<<PB1); // Green OFF, Orange ON
				seconds_elapsed=0; // Reset second tracker
				current_state=STATE_ORANGE; // Switch state
			}
			break;
			
			case STATE_ORANGE:
			if(seconds_elapsed>=3) {
				PORTB=(1<<PB2);     // Orange OFF, Red ON
				seconds_elapsed=0;  // Reset second tracker
				current_state=STATE_RED; // Switch state
			}
			break;
			
			case STATE_RED:
			if(seconds_elapsed>=10) {
				PORTB=(1<<PB0);   //RED OFF, Green ON
				seconds_elapsed=0; // Reset second tracker
				current_state=STATE_GREEN; // Loop back to green
			}
			break;
		}
	}
	
	return 0;
}
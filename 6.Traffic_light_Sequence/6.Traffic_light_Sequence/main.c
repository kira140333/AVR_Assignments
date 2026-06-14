#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// State definitions
#define STATE_GREEN    0
#define STATE_ORANGE   1
#define STATE_RED      2

// volatile flags for ISR communication
volatile uint8_t seconds_elapsed=0;
volatile uint8_t emergency_flag=0;

// Timer1 ISR
ISR(TIMER1_COMPA_vect) {
	seconds_elapsed++;
}

// External Interrupt 0 ISR: Launches immediately when the emergency button is pressed
ISR(INT0_vect) {
	emergency_flag=1; // Raise flag, do not interrupt the activity
}

int main(void) {
	// Configure LEDs PB0-PB2 as outputs
	DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2);
	
	// Configure Emergency button (PD2/INT0) as input with internal pull-up
	DDRD&=~(1<<PD2);
	PORTD|=(1<<PD2);
	
	// Configure Hardware Timer1
	TCCR1B|=(1<<WGM12);   // CTC mode
	TCCR1B|=(1<<CS12)|(1<<CS10); // Prescalar 1024
	OCR1A=15624; // Target count for 1 second
	TIMSK1|=(1<<OCIE1A);  // Enable Timer1 interrupt
	
	// Configure External Interrupt 0 (INT0)
	EICRA|=(1<<ISC01);  // Trigger INT0 on falling edge
	EIMSK|=(1<<INT0);  // Enable INT0 external interrupt
	
	// Enable all interrupts globally
	sei();
	
	// Initial Traffic State
	uint8_t current_state=STATE_GREEN;
	PORTB=(1<<PB0); // Green ON, others OFF
	seconds_elapsed=0;
	
	while(1) {
		switch(current_state) {
			case STATE_GREEN:
			if(seconds_elapsed>=10) {
				seconds_elapsed=0; // Reset timer
				
				// Safe duration finished. Check for emergency overide
				if(emergency_flag) {
					emergency_flag=0;  // Clear Emergency request
					PORTB=(1<<PB0);  //Keep Green ON
					current_state=STATE_GREEN; // Reset to start of green cycle
				}else {
					PORTB=(1<<PB1);
					current_state=STATE_ORANGE;
				}
			}
			break;
			
		case STATE_ORANGE:
		    if(seconds_elapsed>=3) {
				seconds_elapsed=0; //Reset Timer
				
				if(emergency_flag) {
					emergency_flag=0;
					PORTB=(1<<PB0);  // Turn Green
					current_state=STATE_GREEN;
				}else {
					PORTB=(1<<PB2);  //Normal Cycle: Orange OFF, Red ON
					current_state=STATE_RED;
				}
			}
			break;
			
			case STATE_RED:
			if(seconds_elapsed>=10) {
				seconds_elapsed=0; //Reset Timer
				
				// Whether normal loop or emergency, the next state is GREEN
				emergency_flag=0;
				PORTB=(1<<PB0);
				current_state=STATE_GREEN;
			}	
			break;
		}
	}
	return 0;
	
}
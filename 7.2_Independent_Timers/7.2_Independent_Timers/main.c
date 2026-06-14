/*
 * 7.2_Independent_Timers.c
 *
 * Created: 12-06-2026 20:36:35
 * Author : kiran
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// Software counter for Timer2 to achieve 500ms
volatile uint8_t timer2_ticks=0;

// Timer1 ISR (Status LED): Configured to trigger every 2 seconds
ISR(TIMER1_COMPA_vect) {
	PORTB^=(1<<PB1); // Toggle Status LED on Pin 1
}

//Timer2 ISR(Heartbeat LED): Configured to trigger every 10 milliseconds
ISR(TIMER2_COMPA_vect) {
	timer2_ticks++;
	
	// 10ms * 50 =500ms
	if(timer2_ticks>=50) {
		PORTB^=(1<<PB2); // Toggle Heartbeat LED on Pin 2
		timer2_ticks=0; // Reset software counter
	}
}


int main(void) {
	// Configure PB1 and PB2 as outputs
	DDRB|=(1<<PB1)|(1<<PB2);
	
	// Configure Timer1 (16-bit) for 2-Second Interval
	// Mode: CTC(Clear Timer on Compare Match)
	TCCR1B|=(1<<WGM12);
	//Prescaler: 1024
	TCCR1B|=(1<<CS12) | (1<<CS10);
	//Calculation: OCR1A = 16000000Hz/(1024*0.5Hz))-1= 31249
	OCR1A=31249;
	// Enable Timer1 Compare Match A Interrupt
	TIMSK1|=(1<<OCIE1A);
	
	// Configure Timer2 (8-bit) for a 10-Millisecond interval
	// Mode:CTC
	TCCR2A|=(1<<WGM21);
	// Prescaler: 1024
	TCCR2B|=(1<<CS22) | (1<<CS21) | (1<CS20);
	// Calculation: OCR2A=(16000000Hz/(1024*100Hz))-1 =155.25
	OCR2A=155;
	// Enable Timer2 Compare Match A Interrupt
	TIMSK2|=(1<<OCIE2A);
	
	// Enable global interrupts
	sei();
	
	// Main loop empty since both tasks run in background via hardware interrupts
	while(1) {
		
	}
	return 0;
}


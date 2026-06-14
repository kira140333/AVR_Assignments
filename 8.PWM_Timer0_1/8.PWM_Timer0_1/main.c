#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// Array mapping duty cycles (25%,50%,75%,0%) to 8-bit OCR0A values
// Max value = 255, 25% = 64, 50% = 128, 75% = 191, 0% = 0
const uint8_t duty_cycles[]= {64, 128, 191, 0};
uint8_t current_step=0; // Tracks active index (0 to 3)

// Dedicated functions to update the PWM timer register
void set_pwm_duty(uint8_t register_val) {
	OCR0A=register_val;
} 

int main(void) {
	// Configure Pin D6 (OC0A) as anooutput for PWM signal
	DDRD|=(1<<DDD6);
	// Configure Pin D2 (Button) as an input and enable internal pull-up
	DDRD &= ~(1 << DDD2);
	PORTD |= (1 << PIND2);

	// Initialize Timer0 in Fast PWM Mode
	// COM0A1: Clear OC0A on Compare Match, set OC0A at BOTTOM (Non-inverting mode)
	// WGM01 & WGM00: Select Fast PWM Mode (Mode 3, Top = 0xFF)
	TCCR0A |= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
	
	// CS01 & CS00: Set Prescaler to 64 
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// Initialize at the starting state (25% Duty Cycle)
	set_pwm_duty(duty_cycles[current_step]);

	while (1) {
		// Wait here while the button is NOT pressed (reads HIGH due to pull-up)
		while (PIND & (1 << PIND2)); // Button pressed
		
		// Advance to the next speed profile step (0 -> 1 -> 2 -> 3 -> 0)
		current_step++;
		if (current_step >= 4) {
			current_step = 0;
		}

		// Call our dedicated function to update the hardware timer register
		set_pwm_duty(duty_cycles[current_step]);

		_delay_ms(250);
	}

	return 0;
}

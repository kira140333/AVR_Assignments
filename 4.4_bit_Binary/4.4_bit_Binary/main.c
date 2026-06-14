#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

uint8_t counter = 0; // variable to keep track of count

//function to update the LED states based on counts
void update_leds(uint8_t count_val) {
	// Clear pins 0,1,2 and 3 on PORTB first(0b11110000 mask)
	PORTB &= 0xF0;
	
	// Combine the lower 4 bits of counter with PORTB
	PORTB |= (count_val & 0x0F);
}

int main(void) {
	// Configure lowest 4 pins of PORTB (PB0-PB3) as outputs
	DDRB |= 0x0F;
	
	// Configure PD2(Button) as input and enable internal pull-up resistor
	DDRD &=~(1<<PD2);
	PORTD |=(1<<PD2);
	
	update_leds(counter); // Initialize LEDs to show 0 at start
	
	while(1) {
		// wait till button is pressed
		while(PIND & (1<<PD2));
		
		counter++; // increment the counter
		
		// If counter exceeds 15 bits(4-bits max:1111), reset it back to 0
		if(counter>15) {
			counter = 0;
		}
		// Call the dedicated function to change the LED's
		update_leds(counter);
		_delay_ms(250);
	}
}


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	// Configure all 8 pins of PORTB as outputs
	DDRB=0xFF;
	
	while(1) {
		// Loop through pins 0 to 7
		for(int i=0;i<8;i++) {
			
			// Shift the number 1 to the left by 'i' positions
			PORTB = (1<<i);
			
			_delay_ms(200); 
		}
	}
}
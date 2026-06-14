#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h> 

void lcd_init(void);
void lcd_clear(void);
void lcd_print(char *text);

int main(void) {
	// 1. Setup pins: Set Pin D2 as Input for the button
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);  // Turn on internal pull-up resistor
	
	// 2. Turn on the LCD screen
	lcd_init();
	lcd_clear();

	unsigned long counter = 0; // counter variable (can hold large numbers)
	char text_buffer[16];      // A temporary text box to hold 9 digits

	while (1) {
		
		// --- PRINT THE CURRENT NUMBER ---
		// "%09lu" turns number into text and forces it to be exactly 9 digits long (adds zeros)
		sprintf(text_buffer, "%09lu", counter);
		lcd_clear();
		lcd_print(text_buffer);

		// --- MONITOR THE BUTTON ---
		// If Pin D2 reads 0, the button is being pressed
		if ((PIND & (1 << PD2)) == 0) {
			
			_delay_ms(40); // Debounce: ignore button static vibration
			
			int hold_timer = 0; // Variable to count how long the button is held

			// While the button is physically held down...
			while ((PIND & (1 << PD2)) == 0) {
				_delay_ms(10);  // Wait 10 milliseconds
				hold_timer += 10; // Add 10 to our timer

				// If held for 2000ms (2 seconds), trigger a LONG PRESS reset
				if (hold_timer >= 2000) {
					counter = 0; // Reset counter back to zero
					
					// Freeze here until the button is let go
					while ((PIND & (1 << PD2)) == 0);
					break; // Exit this loop immediately
				}
			}

			// If let go BEFORE 2 seconds passed, it's a SHORT PRESS
			if (hold_timer < 2000) {
				counter++; // Add 1 to the counter
				
				if (counter > 999999999) { // If it rolls past 9 digits, reset
					counter = 0;
				}
			}
			
			_delay_ms(100); // Small delay after button release
		}
	}
	return 0;
}



void lcd_command(unsigned char cmd) {
	PORTB = (PORTB & 0xF0) | ((cmd >> 4) & 0x0F);
	PORTD &= ~(1 << PD4); PORTD |= (1 << PD5); _delay_us(1); PORTD &= ~(1 << PD5); _delay_us(200);
	PORTB = (PORTB & 0xF0) | (cmd & 0x0F);
	PORTD |= (1 << PD5); _delay_us(1); PORTD &= ~(1 << PD5); _delay_ms(2);
}
void lcd_char(unsigned char data) {
	PORTB = (PORTB & 0xF0) | ((data >> 4) & 0x0F);
	PORTD |= (1 << PD4); PORTD |= (1 << PD5); _delay_us(1); PORTD &= ~(1 << PD5); _delay_us(200);
	PORTB = (PORTB & 0xF0) | (data & 0x0F);
	PORTD |= (1 << PD5); _delay_us(1); PORTD &= ~(1 << PD5); _delay_ms(2);
}
void lcd_init(void) {
	DDRD |= (1 << PD4) | (1 << PD5); DDRB |= 0x0F; _delay_ms(50);
	lcd_command(0x02); lcd_command(0x28); lcd_command(0x0C); lcd_command(0x01); _delay_ms(2);
}
void lcd_clear(void) { lcd_command(0x01); lcd_command(0x80); }
void lcd_print(char *text) { while(*text) lcd_char(*text++); }
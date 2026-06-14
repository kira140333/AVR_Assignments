#define F_CPU 16000000UL
#define BAUD 9600
#define MY_UBRR (F_CPU/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

void setup_hardware(void);
void send_text(char *message);
void read_line(char *buffer, int max_size);

int main(void) {
	setup_hardware(); // Run setup configurations once
	
	char input[16];   // hold the incoming text command
	
	send_text("System Ready! Type ON, OFF, or 0-100:\r\n");
	
	while(1) {
		// 1. Wait here until a whole command is typed out
		read_line(input, 15);
		
		// 2. Check if the user typed "ON"
		if (input[0] == 'O' && input[1] == 'N' && input[2] == '\0') {
			OCR0A = 255; // Max speed/brightness
			send_text("LED is now ON\r\n");
		}
		// 3. Check if the user typed "OFF"
		else if (input[0] == 'O' && input[1] == 'F' && input[2] == 'F' && input[3] == '\0') {
			OCR0A = 0;   // Fully off
			send_text("LED is now OFF\r\n");
		}
		// 4. Otherwise, assume they typed a number
		else {
			int percentage = atoi(input); // Convert the text string into a normal number
			
			// Check if the number is legal (between 1 and 100)
			if (percentage > 0 && percentage <= 100) {
				
				// Convert 0-100% to a 0-255 scale for the hardware register
				OCR0A = (percentage * 255) / 100;
				
				send_text("Brightness changed!\r\n");
			}
			else {
				send_text("Invalid Command! Try again.\r\n");
			}
		}
	}
	return 0;
}

// HARDWARE INITIALIZATION

void setup_hardware(void) {
	// 1. Setup PWM Pin PD6 (OC0A) as Output
	DDRD |= (1 << PD6);
	
	// Timer0 in Phase Correct PWM Mode (Mode 1), Prescaler = 64
	TCCR0A = (1 << COM0A1) | (1 << WGM00);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 0; // Start at 0%
	
	// 2. Setup UART Hardware for 9600 Baud
	UBRR0H = (unsigned char)(MY_UBRR >> 8);
	UBRR0L = (unsigned char)MY_UBRR;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit characters
}

// TEXT TRANSMITTING TOOL

void send_text(char *message) {
	// Loop through every single character in the string until it hits the end
	while (*message != '\0') {
		while (!(UCSR0A & (1 << UDRE0))); // Wait for internal serial channel to empty
		UDR0 = *message;                  // Send the current character out
		message++;                        // Move to the next letter
	}
}

// TEXT RECEIVING TOOL

void read_line(char *buffer, int max_size) {
	int i = 0;
	
	while (i < max_size) {
		while (!(UCSR0A & (1 << RXC0))); // Wait for a keyboard key press
		char incoming_char = UDR0;       // Grab the key pressed
		
		// If the user hit 'Enter' (Newline or Carriage Return), stop reading
		if (incoming_char == '\r' || incoming_char == '\n') {
			break;
		}
		
		buffer[i] = incoming_char; // Store the letter in our box
		i++;
	}
	buffer[i] = '\0'; // Add a clean End-of-Text marker to the string
}
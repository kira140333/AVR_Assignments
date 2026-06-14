#include <avr/io.h>

// Function Prototypes
void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);
void PWM_Init(void);

int main(void) {
	uint16_t adc_value;
	uint8_t pwm_value;

	// Initialize the ADC and PWM peripherals
	ADC_Init();
	PWM_Init();
	
	while (1) {
		// 1. Read the 10-bit value from the potentiometer on ADC Channel 0
		adc_value = ADC_Read(0);

		// 2. Scale the 10-bit value (0-1023) to an 8-bit value (0-255)
		// Shifting right by 2 bits is a fast way to divide by 4 (1023 / 4 = 255)
		pwm_value = (uint8_t)(adc_value >> 2);

		// 3. Update the PWM duty cycle to change the motor speed
		OCR0A = pwm_value;
	}

	return 0; // Standard but never reached
}

// Initialize ADC Peripherals

void ADC_Init(void) {
	// Set reference voltage to AVcc (5V)
	ADMUX = (1 << REFS0);
	
	// Enable ADC and set the clock prescaler to 128
	// (16MHz / 128 = 125kHz, which is perfect for accurate ADC conversion)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// Read Analog Value from a Specific Channel
uint16_t ADC_Read(uint8_t channel) {
	// Clear old channel bits and select the new channel (0-7)
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for the conversion to finish (ADSC bit returns to 0)
	while (ADCSRA & (1 << ADSC));

	// Return the 10-bit result
	return ADC;
}

// Initialize Timer0 for fast PWM on OC0A (PORTD Pin 6)
void PWM_Init(void) {
	// Set PD6(OC0A) as an output pin
	DDRD|=(1<<DDD6);
	
	// Configure Timer0:
	// 1. COM0A1 = 1, COM0A0 = 0 -> Clear OC0A on Compare Match, set at BOTTOM (Non-inverting mode)
	// 2. WGM01 = 1, WGM00 = 1   -> Select Fast PWM Mode
	TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);

	// Set Timer0 Prescaler to 64 and start the timer
	// PWM Frequency = 16MHz / (64 * 256) = ~976 Hz
	TCCR0B = (1 << CS01) | (1 << CS00);

	// Start with the motor turned off
	OCR0A = 0;
}
	
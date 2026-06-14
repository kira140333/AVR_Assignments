/*
 * 10.LM35_Cooling_System.c
 *
 * Created: 13-06-2026 19:43:00
 * Author : kiran
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// Initialize hardware
void setup_hardware(void) {
	// Set Pin D6 as Output for the motor
	DDRD=DDRD|(1<<PD6);
	
	// Set Pin D2 as input for the Push-Button
	DDRD=DDRD&~(1<<PD2);
	PORTD=PORTD|(1<<PD2); // Turn on internal pull-up resistor
	
	// Configure Timer0 for fast PWM (Motor Speed Control) on pin D6
	TCCR0A=(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B=(1<<CS01); // Set prescaler to 8 to turn on the timer
	OCR0A=0; // Start with the motor turned completely OFF
	
	// Configure the ADC for the LM 35 sensor on Pin A0
	ADMUX=(1<<REFS0); // 5V AS MEASUREMENT GUIDE
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // Turn ADC ON
}

// READ TEMPERATURE FUNCTION
int get_temperature(void) {
	ADCSRA=ADCSRA|(1<<ADSC); // Tell Pin A0 to read the sensor
	
	while(ADCSRA&(1<<ADSC)) {
		// Wait here till chip finishes reading sensor
	}
	
	int raw_value=ADC;  // Get the raw number (0 to 1023)
	int celsius=raw_value/2; // Raw number into degree celsius
	
	return celsius; 
}



int main(void)
{
   setup_hardware(); // Run the setup code once
   
   int system_is_on=0; // Tracks if system is ON(1) or OFF(0)
   
   while(1) {
	   // If Pin D2 reads 0, the button is being held down
	   if((PIND&(1<<PD2))==0) {
		   
		   _delay_ms(50); // Wait 50ms to ignore button static/vibration(Debounce)
		   
		   if((PIND&(1<<PD2))==0) { // If it is still presses...
			   
			   if(system_is_on==0) {
				   system_is_on=1; // Turn system ON
			   } else {
				   system_is_on=0; // Turn system OFF
				   OCR0A=0; // Turns OFF the monitor..
			   }
			   
			   // Wait right here until finger lifted completely off the button
			   while((PIND&(1<<PD2))==0) {
				   // Do nothing, just wait..
			   }
			   _delay_ms(50); // Wait 50 ms after release to ensure clean stop
			   
	}
}

// Automated Fan Logic
// Only run the cooling logic if system turned ON
if(system_is_on==1) {
	
	int current_temp=get_temperature(); // Ask function for the temperature
	
	if(current_temp<25) {
		OCR0A=0;  // Under 25 degree: Turn motor completely OFF
	}
	else if(current_temp>=25 && current_temp<35) {
		OCR0A=100;  // Between 25 and 34 degree celsius: Run motor at medium speed
	}
	else {
		OCR0A=255;  // 35 degree or hotter: Run motor at maximum speed
	}
}

_delay_ms(100); // Wait 1/10th of second before repeating the loop
	  
	   }
	   return 0;
   }



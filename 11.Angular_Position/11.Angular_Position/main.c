  #define F_CPU 16000000UL
  #include <avr/io.h>
  #include <util/delay.h>
  
  void setup(void) {
	  // Set Pin D9(PB1) as output for the servo signal
	  DDRB|=(1<<PB1);
	  
	  // Setup ADC for potentiometer on Pin A0
	  ADMUX=(1<<REFS0); // Using 5V as reference
	  ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // Turn ADPS on
	  
	  // Setup Timer1(16-bit) for Servo PWM
	  // Mode 10: Phase Correct PWM where ICR1 sets the frequency
	  TCCR1A=(1<<COM1A1)|(1<<WGM11);
	  TCCR1B=(1<<WGM13)|(1<<CS11); // CS11 sets prescaler to 8
	  
	  // Set the frequency to 50Hz(20ms period)
	  ICR1=20000;
  }
  
 int main(void) {
	 setup();
	 
	 while(1) {
		 // Read the Potentiometer
		 ADCSRA|=(1<<ADSC);  // Start reading Pin A0
		 while(ADCSRA&(1<<ADSC)); // Wait until finished
		 
		 int pot_reading=ADC; // Number from 0 to 1023
		 
		 // Convert Reading to Servo Position
		 // Servo needs a pulse between 1 ms(0*) and 2 ms(180*)
		 // 2000=1ms pulse(0 degrees)
		 // 4000=2ms pulse(180 degrees)
		 
		 // We calculate (Pot Value*roughly 2)+2000
		 // This scales our 0-1023 input to roughly 2000-4000 output.
		 unsigned int pulse_width=(pot_reading*2)+2000;
		 
		 // Move the servo
		 OCR1A=pulse_width;  // Update hardware pin
		 
		 _delay_ms(15);
	 }
	 
	 return 0;
		 
 }
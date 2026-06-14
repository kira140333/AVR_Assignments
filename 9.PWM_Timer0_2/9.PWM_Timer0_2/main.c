/*
 * 9.PWM_Timer0_2.c
 *
 * Created: 13-06-2026 19:09:57
 * Author : kiran
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
   // Set pin PD6 as Output for the LED/Motor
   DDRD=DDRD|(1<<PD6);
   
   // Set Pin PD2 as an Input for the push-button
   DDRD=DDRD&~(1<<PD2);
   
   // Turn ON internal pull-up resistor for Pin PD2
   PORTD=PORTD|(1<<PD2);
   
   // Setup TIMER0 for Hardware PWM
   // Set clear-on-compare mode(Non-Inverting PWM), Turn on Fast PWM mode
   TCCR0A=(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
   
   // Set clock prescaler to 8.
   TCCR0B=(1<<CS01);
   
   // Initial state: 25%
   // 25% of 255 is 64
   OCR0A=64;
   
   // 1=25%, 2=50%, 3=75%, 4=0%(OFF)
   int current_mode=1;
   
   while(1) {
	   // Check whether the button is pressed, because of the pull-up resistor, pressing the button drops the pin to 0
	   if((PIND&(1<<PD2))==0) {
		   
		   _delay_ms(50); // DEBOUNCE: Wait 50ms to let the metal contacts stop vibrating
		   
		   // Check again to make sure it was not accidental
		   if((PIND&(1<<PD2))==0) {
			   
			   // Move to next mode
			   current_mode=current_mode+1;
			   
			   // If we go past mode 4, loop back to mode 1
			   if(current_mode>4) {
				   current_mode=1;
		   }
		   
		   // Update the PWM output depending on what mode
		   if(current_mode==1) {
			   OCR0A=64;  // 25% Duty Cycle
		   }
		   else if(current_mode==2) {
			   OCR0A=127; // 50% Duty Cycle
		   }
		   else if(current_mode==3) {
			   OCR0A=191; // 75% D.C
		   }
		   else if(current_mode==4) {
			   OCR0A=0; // 0% D.C
		   }
		   
		   // Freeze the program until released
		   while((PIND&(1<<PD2))==0) {
			   // Do nothing..
		   }
		   
		   _delay_ms(50); // Small delay to prevent debounce
		   }
		   
   }
   
}
return 0;
}


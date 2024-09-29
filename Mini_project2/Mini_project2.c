#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//initialise global variables (seconds,minutes,hours)
unsigned char sec=0;
unsigned char mins=0;
unsigned char hr=0;

ISR(TIMER1_COMPA_vect){   // increment time after every second
	sec++;
}
ISR(INT0_vect){     // reset stop watch
	sec=0;
	mins=0;
	hr=0;
}
ISR(INT1_vect){     // pause stop watch
	TCCR1B &=~(1 << CS12) & ~(1 << CS10); //disable timer 1
}
ISR(INT2_vect){     // resume stop watch
	TCCR1B |=(1 << CS12) | (1 << CS10);  //enable timer 1
}
void initTimer1() {
	// system clock 1Mhz and prescaler 1024
	TCNT0 = 0;   //set initial count to zero
    TIMSK |=(1 << OCIE1A); //Enable Timer1 Compare A Interrupt
    TCCR1B |=(1 << WGM12);  //CTC mode (mode number 4)
    OCR1A =976;    // compare value after rounding value =976
    TCCR1B |=(1 << CS12) | (1 << CS10);  //prescalar 1024 => CS12=1 , CS10=1
}
void INT0_Init() {
	DDRD &= ~(1 << PD2);   //  Configure PD2 as input pin
    MCUCR |= (1 << ISC01);  // trigger INT0 with falling edge
	MCUCR &= ~(1 << ISC00);
    GICR |= (1 << INT0);    // Enable external interrupt pin INT0
}
void INT1_Init(){
	DDRD  &= ~(1<<PD3);  //  Configure PD3 as input pin
	MCUCR |= (1 << ISC11) | (1 << ISC10);  //trigger INT1 with raising edge
	GICR  |= (1<<INT1);    // Enable external interrupt pin INT1
}
void INT2_Init(){
	DDRB &= ~(1 << PB2);  //  Configure PB2 as input pin
	MCUCSR &= ~(1 << ISC2);  //trigger INT2 with falling edge
	GICR |= (1 << INT2);   // Enable external interrupt pin INT2
}
int main(void){
	SREG |= (1<<7);  // enable global interrupts in MC
	initTimer1();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	DDRC = 0x0F;    // Configure first 4 pins in PORTC as output pins
	DDRA = 0x3F;    //// Configure first 6 pins in PORTC as output pins

    while(1){
		if (sec == 60) {
		    sec = 0;
		    mins++;
		}
		if (mins == 60) {
		    mins = 0;
		    hr++;
		}
		if(hr ==24){
			hr=0;
		}
		//display time
        PORTA=0x01;       //enable first seven segment only
		PORTC=(sec%10);   //first digit of seconds
	    _delay_ms(4);    // a small delay for number to be visible
	    PORTA=0x02;      //enable second seven segment only
	    PORTC=(sec/10);   // second digit of seconds
	    _delay_ms(4);
	    PORTA=0x04;      //enable third seven segment only
	    PORTC=(mins%10);  // first digit of minutes
	    _delay_ms(4);
	    PORTA=0x08;      //enable fourth seven segment only
	    PORTC=(mins/10);  //second digit of minutes
	    _delay_ms(4);
	    PORTA=0x10;      //enable fifth seven segment only
	    PORTC=(hr&10);  //first digit of hours
	    _delay_ms(4);
	    PORTA=0x20;      //enable sixth seven segment only
	    PORTC=(hr/10);  //second digit of hours
	    _delay_ms(4);
}
  return 0;
}

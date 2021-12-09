/*
 * main.c
 *
 *  Created on: Sep 21, 2021
 *      Author: LAPTOP
 */


#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


unsigned char SEC1 = 0; 		//First Digit Of Seconds
unsigned char SEC2 = 0;			//Seconds Digit Of Seconds
unsigned char MIN1 = 0;			//First Digit Of Minutes
unsigned char MIN2 = 0;			//Second Digit Of Minutes
unsigned char HOURS1 = 0;		//First Digit Of Hours
unsigned char HOURS2 = 0;		//Second Digit Of Hours

unsigned char Timer1_ISR_FLAG = 0; // Flag For Timer1 ISR , it's set to 1 if Timer1 ISR code Executed




void Timer1_Init()
{
		// set up timer with prescaler = 1024 and CTC mode
	    TCCR1B |= (1 << WGM12)|(1 << CS12)|(1 << CS10);

	    // initialize counter
	    TCNT1 = 0;

	    // initialize compare value
	    OCR1A = 999;

	    // enable compare interrupt
	    TIMSK |= (1 << OCIE1A);

	    // enable global interrupts
	    SREG |=(1<<7);

}

// Timer1 ISR

ISR(TIMER1_COMPA_vect)
{
	Timer1_ISR_FLAG = 1;
}


// This Function is Called When TIMER1_ISR_FLAG =1

void ISR_Execution()
{
	SEC1++;

	if(SEC1 == 10)
	{
		SEC2++;
		SEC1 = 0;
	}


	if(SEC2 == 6)
	{
		SEC2 =0;
		SEC1 = 0;
		MIN1++;
	}

	if(MIN1 == 10)
	{
		MIN2++;
		MIN1 =0;
	}

	if(MIN2 == 6)
	{
		HOURS1++;
		MIN2 = 0;
	}

	if(HOURS1 == 9)
	{
		HOURS2++;
		HOURS1 = 0;
	}

	if (HOURS1 == 4 && HOURS2 == 2)
	{
		SEC1 =0;
		SEC2 =0;
		MIN1 =0;
		MIN2 =0;
		HOURS1 =0;
		HOURS2 =0;
	}
}


//*************************************** Reset Interrupt ***********************************************
void INT0_Init()
{
	// Activate Interrupt 0 With Falling Edge
	MCUCR |= (1<<ISC01);

	// Enable Interrupt 0
	GICR |=(1<<INT0);

	// Enable General Interrupt
	SREG |= (1<<7);

}

// Interrupt 0 ISR
ISR(INT0_vect)
{
	SEC1 =0;
	SEC2 =0;
	MIN1 =0;
	MIN2 =0;
	HOURS1 =0;
	HOURS2 =0;
}


//******************************************** Pause Interrupt ************************************
void INT1_Init()
{
	// Activate Interrupt 1 With Rising Edge
	MCUCR |= (1<<ISC10) | (1<<ISC11);

	// Enable Interrupt 1
	GICR |=(1<<INT1);

	// Enable General Interrupt
	SREG |= (1<<7);

}


// Interrupt 1 ISR
ISR(INT1_vect)
{
	 TCCR1B &= ~(1 <<CS12 ) & ~(1 << CS11) & ~(1 << CS10);
}



//**************************************** Resume Interrupt *************************************
void INT2_Init()
{

	// Activate Interrupt 1 With Falling Edge
	MCUCSR &= ~(1<<ISC2);

	// Enable Interrupt 2
	GICR |=(1<<INT2);

	// Enable General Interrupt
	SREG |= (1<<7);
}


// Interrupt 2 ISR
ISR(INT2_vect)
{
	TCCR1B |= (1 << CS12)|(1 << CS10);
}

//**************************************** Main Fn ****************************************

int main(void)
{
	/********** Initialization Code **********/
	DDRA = 0XFF;  // Configure PORTA as Output
	DDRC = 0X0F; // 1ST 4 Pins In PORTC as Output
	PORTA = 0X00; // Initially Disable All 7-Segments

	DDRD &= ~(1<<PD2); // Configure PD2 AS input (INT0)
	PORTD |= (1<<PD2); // Activate Internal Pull-Up

	DDRD &= ~(1<<PD3); // Configure PD3 As input (INT1)

	DDRB &= ~ (1<<PB2); // Configure PB2 AS input (INT2)
	PORTB |= (1<<PB2);  // Activate Internal Pull-Up

	Timer1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();

	/************************* This Loop Displays The Numbers on 7-Segment  ************************/
	while(1)
	{
		if(Timer1_ISR_FLAG)
		{
			ISR_Execution();
			Timer1_ISR_FLAG = 0;
		}

		PORTC = (PORTC & 0XF0) | (SEC1 & 0X0F);
		PORTA =0X41;
		_delay_ms(5);
		PORTA = 0X00;

		PORTC = (PORTC & 0XF0) | (SEC2 & 0X0F);
		PORTA =0X42;
		_delay_ms(5);
		PORTA = 0X00;

		PORTC = (PORTC & 0XF0) | (MIN1 & 0X0F);
		PORTA =0X04;
		_delay_ms(5);
		PORTA = 0X00;

		PORTC = (PORTC & 0XF0) | (MIN2 & 0X0F);
		PORTA =0X48;
		_delay_ms(5);
		PORTA = 0X00;

		PORTC = (PORTC & 0XF0) | (HOURS1 & 0X0F);
		PORTA =0X10;
		_delay_ms(5);
		PORTA = 0X00;

		PORTC = (PORTC & 0XF0) | (HOURS2 & 0X0F);
		PORTA =0X60;
		_delay_ms(5);
		PORTA = 0X00;
	}
}



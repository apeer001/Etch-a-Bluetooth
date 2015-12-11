/*
 * apeer001_CS122A_custom_lab_micro1.c
 *
 *	Partner 1 Name & E-mail: Aaron Peery (apeer001@ucr.edu)
 *	Lab Section: 021
 *	Assignment: Custom Lab Lab # 10
 *	Exercise Description:
			Etch-A-Sketch Micro controller #1 code
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>
#include <ADC.h>
#include <usart_ATmega1284.h>
#include <seven_seg.h>
#include <scheduler.h>

// -------- shared variables ---------------------------------------------------
unsigned char C6 = 0;  // 3 button inputs from PORTC
unsigned char C5 = 0; 
unsigned char C4 = 0; 

unsigned char A2 = 0;		// select button input from Joystick 
unsigned char A3 = 0;
unsigned char A5 = 0;		// Write/Hover mode for cursor
unsigned char A6 = 0;		// Save button
unsigned char A7 = 0;		// Menu button
unsigned char start = 0;	// When High will allow for all other task to operate


unsigned char directionVal = 0;
unsigned char prevDir = 0;
const unsigned char Seven_segChar[] = {SS_0, SS_1, SS_2, SS_3, SS_4, SS_5, SS_6, SS_7, SS_8, SS_9,
									   SS_A, SS_B, SS_C, SS_D, SS_E, SS_F, SS_DP,SS_G, SS_H, SS_h,
									   SS_I, SS_J, SS_L, SS_m, SS_N, SS_n, SS_P, SS_R, SS_r, SS_t,
									   SS_U, SS_u, SS_V, SS_v, SS_X, SS_Y, SS_Z, SS_lambda,SS_char1,
									   SS_char2, SS_char3, SS_char4, SS_char5, SS_char6, SS_char7,
									   SS_char8, SS_char9, SS_char10, SS_char11, SS_char12, SS_char13};
static unsigned char size = sizeof(Seven_segChar) / sizeof(*Seven_segChar);


// --------End shared variables -------------------------------------------------


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks


// ------------------- Timer -----------------------------------------------------

/*
// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B 	= 0x0B;	// bit3 = 1: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: prescaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A 	= 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register

	TIMSK1 	= 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1 = 0;

	// TimerISR will be called every _avr_timer_cntcurr milliseconds
	_avr_timer_cntcurr = _avr_timer_M;

	//Enable global interrupts
	SREG |= 0x80;	// 0x80: 1000000
}

void TimerOff() {
	TCCR1B 	= 0x00; // bit3bit2bit1bit0=0000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
	// CPU automatically calls when TCNT0 == OCR0 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; 			// Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { 	// results in a more efficient compare
		TimerISR(); 				// Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
*/

// ------------------- Timer End--------------------------------------------------


// ------------------- Tasks -----------------------------------------------------

int StartScreen(int state) {
	enum Start_State {init,on, Wait,Begin};
	static unsigned char sent;
	static unsigned char ack;
	switch(state) {			// State transitions
		case -1:
			state = init;
			break;
		case init:
			state = Wait;
			break;
		case Wait:
			A2 = (~PINA) & 0x04;
			if(A2)
			{
				state = on;
			}
			else  
			{
				state = Wait;
			}
			break;
		case on:
			if(!sent)
			{
				state = on;
			}
			else
			{
				state = Begin;
			}
			
			break;
		case Begin:
			state = Begin;
			break;
		default:
			state = init;
			break;
	}
	switch(state) {			// State actions
		case init:
			sent = 0;
			ack = 0;
			start = 0;
			break;
		case Wait:
			if(USART_IsSendReady(0))
			{
				USART_Send(valOf7Seg(Seven_segChar[5]),0);  // Sends char '5'
			}
			break;
		case on:
			if (USART_IsSendReady(1)) {
				USART_Send(0x80,1);
				sent = 1;
			}
			break;
		case Begin:
			if(!start)
			{
				if(USART_IsSendReady(0))
				{
					USART_Send(valOf7Seg(Seven_segChar[23]),0); // Sends char 'm'
				}
				start = 1;
			}
			break;
		default:
			break;
	}
	return state;
}

//Test USART
int Leader_Tick(int state) {
	enum Leader_States {init,on,off};
    switch(state) {
	    case -1:
			state = init;
			break;
	    case init:
			state = on;
			break;
	    case on:
			state = off;
			break;
	    case off:
			state = on;
			break;
	    default:
			state = init;
			break;
    }

    switch(state) {
	    case init:
			break;
	    case on:
			if( USART_IsSendReady(0) ) {
				USART_Send(0x02,0);
				PORTB = 0x02;
			}
			/*if( USART_HasTransmitted(0) ) {
			USART_Flush(0);
			}*/
			break;
	    case off:
			if( USART_IsSendReady(0) ) {
				USART_Send(0x01,0);
				PORTB = 0x01;
			}
			/*if( USART_HasTransmitted(0) ) {
			USART_Flush(0);
			}*/
			break;
	    default:
			break;
	}
	return state;
}
// Will display all characters in array to seven seg sending through USART 0
int Seven_seg(int state) {
	enum seg_States {init,on,off,buttonRelease, increment, decrement, wait};
	static unsigned char prev;
	static unsigned char i, sent;
	C6 = (~PINA) & 0x80;
	C5 = (~PINA) & 0x40;
    C4 = (~PINA) & 0x20;
	switch(state) {						// State transistions
	    case -1:
			state = init;
			break;
	    case init:
			state = on;
			break;
	    case on:
			if(C6)
			{
				state = buttonRelease;
				prev = on;
			}
			else if(C5)
			{
				state = increment;
			}
			else if(C4)
			{
				state = decrement;
			}
			else 
			{
				state = on;
			}
			break;
	    case off:
			if(C6)
			{
				state = buttonRelease;
				prev = off;
			}
			else
			{
				state = off;
			}
			break;
		case buttonRelease:
			if(prev == off && !C6)
			{
				state = on;
			}
			else if(prev == on && !C6)
			{
				state = off; 
			}
			break;
		case increment:
			state = wait;
			break;
		case decrement:
			state = wait;
			break;
		case wait:
			if(!C5 && !C4)
			{
				state = on;
			}
			else
			{
				state = wait;
			}
			break;
	    default:
			state = init;
			break;
    }

    switch(state) {					// State actions
	    case init:
			i = 0;
			sent = 0;
			break;
	    case on:
			if( USART_IsSendReady(0) && !sent) {
				USART_Send(valOf7Seg(Seven_segChar[i]),0);
				PORTB = 0x01;
				sent = 1;
			}
			break;
	    case off:
			i = 0;
			if( USART_IsSendReady(0) ) {
				USART_Send(0x00,0);
				PORTB = 0x02;
			}
			sent = 0;
			break;
		case buttonRelease:
			break;
		case increment:
			if(i < size){
				i++;
				sent = 0;
			}
			break;
		case decrement:
			if(i > 0){
				i--;
				sent = 0;
			}
			break;
		case wait:
			break;
	    default:
			break;
	}
	return state;
}
//Will Slideshow all saved images until button or input response is found
int IdleSlideShowSM(int state)
{
	enum changeCursorLocState{Init, left, right, up, down};
	switch(state)		// State transitions
	{
		case -1:
			break;
		default:
			break;
	}
	switch (state)		// State actions
	{
		case -1:
			break;
		default:
			break;
	}
	return state;
}
// Will get information from joystick and send through USART 1
int InputSM(int state)
{
		enum JoystickState{Init, getXY};
		static unsigned char LEFT, RIGHT;
		static unsigned char UP, DOWN;
		static unsigned short ADCVal;
		static unsigned char sent;
		switch(state)		// State transitions
		{
			case -1:
				state = Init;
				break;
			case Init:
				state = getXY;
				break;
			case getXY:
				state = Init;
				break;
			default:
				state = Init;
				break;
		}
		switch (state)		// State actions
		{
			case -1:
				break;
			case Init:
				RIGHT = 0;
				LEFT = 0;
				UP = 0;
				DOWN = 0;
				sent = 0;
				break;
			case getXY:
				if(start)
				{
					A2 = (~PINA) & 0x04;
					A3 = (~PINA) & 0x08;
					A5 = (~PINA) & 0x20;
					A6 = (~PINA) & 0x40;
					A7 = (~PINA) & 0x80;
					
					// Will Get Joystick X axis values 
					ADC_Select(0);
					ADCVal = ADC;
					if(ADCVal > 700)
					{
						RIGHT = 1;
					}
					else if(ADCVal < 50)
					{
						LEFT = 1;
					}
					// Will Get Joystick Y axis values
					ADC_Select(1);
					ADCVal = ADC;
					if(ADCVal > 600)
					{
						DOWN = 1;
					}
					else if(ADCVal < 300)
					{
						UP = 1;	
					}
				
					// Constructs directional 8-bit value to be sent through USART
					directionVal = 0;
					if(RIGHT == 1)    {directionVal |= (1 << 3);}
					else      		  {directionVal &= ~(1 << 3);}
					if(LEFT == 1)     {directionVal |= (1 << 2);}
					else      		  {directionVal &= ~(1 << 2);}
					if(DOWN == 1)	  {directionVal |= (1 << 1);}
					else      		  {directionVal &= ~(1 << 1);}
					if(UP == 1)		  {directionVal |= 0x01;}
					else			  {directionVal &= ~0x01;}
					if(A2)			  {directionVal |= (1 << 4);}		// Select button was pressed
					else              {directionVal &= ~(1 << 4);}
					if(A5)			  {directionVal |= (1 << 5);}		// Write button was pressed
					else              {directionVal &= ~(1 << 5);}
					if(A6)			  {directionVal |= (1 << 6);}		// Save button was pressed
					else              {directionVal &= ~(1 << 6);}
					if(A7)			  {directionVal |= (1 << 7);}		// Menu button was pressed
					else              {directionVal &= ~(1 << 7);}
					// Send Data through USART 1 as directional bits and button press signals
					PORTC = directionVal ;
				
					if(USART_IsSendReady(1) && !A3)
					{
						USART_Send(directionVal,1);
						sent = 1;
					}
					else if(A3)
					{
						if(USART_IsSendReady(0))
						{
							USART_Send(valOf7Seg(Seven_segChar[0]),0);
						}
						if(USART_IsSendReady(1))
						{
							USART_Send(0x0F,1);
						}
					}
					
					sent = 0;
				}
				break;
			default:
				break;
		}
		return state;
}
//// ------------------ End Tasks -----------------------------------------------------

int main(void)
{
	// Set DDR for ports to be used
	DDRA = 0x10; PORTA = 0xEF;					// Inputs from Joystick
	DDRB = 0xFF; PORTB = 0x00;					// Outputs for test LEDs
	DDRC = 0xFF; PORTC = 0x00;					// Inputs from Buttons
	DDRD = 0x7E; PORTD = 0x81;					// I/O from TX & RX pins for USART

	//Set up list of tasks into task 
	task task_array[2];
	tasksNum = 2;
	tasks = task_array;
	
	//Set up Period for each task
	unsigned short direction_Task_period		 = 50;
	unsigned short detectVibrate_Task_period	 = 50;
	unsigned short drawPixel_Task_period		 = 400;
	unsigned short changeCursorLoc_Task_period   = 100;
	unsigned short Leader_Tick_period		     = 50;
	unsigned short Seven_seg_period			     = 50;
	unsigned short InputSM_period				 = 50;
	unsigned short StartScreen_period			 = 200;
	
	//Calculate Greatest common divisor
	unsigned tmpGCD = GCD(direction_Task_period, detectVibrate_Task_period);
	tmpGCD = GCD(tmpGCD, drawPixel_Task_period);
	tmpGCD = GCD(tmpGCD, changeCursorLoc_Task_period);
	tmpGCD = GCD(tmpGCD, StartScreen_period);
	
	//divide period by newly found GCD for shorter iterations
	//direction_Task_period		= direction_Task_period/tmpGCD;
	//detectVibrate_Task_period	= detectVibrate_Task_period/tmpGCD;
	//drawPixel_Task_period		= drawPixel_Task_period/tmpGCD;
	//changeCursorLoc_Task_period = changeCursorLoc_Task_period/tmpGCD;
	//Leader_Tick_period			= Leader_Tick_period/ tmpGCD;
	//Seven_seg_period            = Seven_seg_period/ tmpGCD;
	//InputSM_period				= InputSM_period/ tmpGCD;
	//StartScreen_period          = StartScreen_period/ tmpGCD;
	
	//Set up each task for first ticks of each function
	tasks[0].state		 = -1;
	tasks[0].period		 = StartScreen_period;
	tasks[0].elapsedTime = StartScreen_period;
	tasks[0].TickFct	 = &StartScreen;
	
	tasks[1].state		 = -1;
	tasks[1].period		 = InputSM_period;
	tasks[1].elapsedTime = InputSM_period;
	tasks[1].TickFct     = &InputSM;
	
	// End Task Set up -----------------------------------
	
	// Set up USART UsartNum -> 0 for ATmega 1284 (1:0)
	unsigned char usartNum = 0;
	initUSART(usartNum);
	USART_Flush(usartNum);
	
	initUSART(1);
	USART_Flush(1);
	
	//Set up ADC
	ADC_init();
	
	// Set up Timer
	TimerSet(1);
	TimerOn();
	
	// Wait forever
	while (1) {}
}
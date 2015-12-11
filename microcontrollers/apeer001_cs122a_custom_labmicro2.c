/*
 * apeer001_cs122a_custom_labmicro2.c
 *
 *	Partner 1 Name & E-mail: Aaron Peery (apeer001@ucr.edu)
 *	Lab Section: 021
 *	Assignment: Custom Lab Lab # 10
 *	Exercise Description:
			Etch-A-Sketch Micro controller #2 code
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */



#include <avr/io.h>
#include "AdaFruit_LCD.c"
#include <usart_ATmega1284.h>
#include <scheduler.h>
#include <util/delay.h>
#include <seven_seg.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>



// ------------------- Shared Variables ------------------------------------------------
// LCD required variables
GFX g;
LCD l;

// etc
volatile unsigned char inputButtonBits = 0;
uint8_t Save[504];									// Byte array with N_bytes	=> (g.HEIGHT*g.WIDTH/8)s

unsigned char start = 0;
unsigned char MenuMode = 0;
unsigned char DrawMode = 0;
unsigned char Slideshow_Mode = 0;
unsigned char Saving = 0;

unsigned char cleared = 0;
unsigned char reset = 0;
unsigned char A2 = 0;		 
unsigned char A6 = 0;								// Save button temp variable
unsigned char A7 = 0;								// Menu button temp variable
unsigned char SelectButton;

// ------------------ Shared Variables End ---------------------------------------------


// ------------------- Tasks -----------------------------------------------------

//Inputs from USART 1 (first Microcontroller)
int GetInputs(int state)
{
	enum inputStates{init, PollInputs};
	switch(state)			// State transistions
	{
		case -1:
			state = init;
			break;
		case init:
			state = PollInputs;
			break;
		case PollInputs:
			state = PollInputs;
			break;
		default:
			state = init;
			break;
	}
	switch(state)			// State actions
	{
		case init:
			break;
			
		case PollInputs:
			inputButtonBits = 0x00;
			if( USART_HasReceived(1) ) {
				inputButtonBits = USART_Receive(1);
				USART_Flush(1);
			}
			if(inputButtonBits == 0x80 )
			{
				SelectButton = 1;
			} 
			reset = inputButtonBits & 0xE0;
			A2 = inputButtonBits & 0x10;
			A6 = inputButtonBits & 0x40;
			A7 = inputButtonBits & 0x80;
			break;
		default:
			break;
	}
	return state;
}
// Start screen will be displayed
int StartScreen(int state) {
	enum Start_State {init,on, Wait, clearScreen} ;
	static unsigned char page;
	switch(state) {			// State transitions
		case -1:
			state = init;
			break;
		case init:
			state = Wait;
			break;
		case Wait:
			if(SelectButton == 0)
			{
				state = Wait;
			}
			else if(SelectButton)
			{
				state = clearScreen;
			}
			break;
		case clearScreen:
			state = on;
			break;
		case on:
			if(!start)
			{
				state = Wait;
			}
			else
			{
				state = on;
			}
			break;
		default:
			state = init;
			break;
	}
	switch(state) {			// State actions
		case init:
			page = 0;
			start = 0;
			clearDisplay(&g);
			display(&l);
			break;
		case Wait:
		    if(page == 0)
			{
				displayBitmap(StartScreen1, &l);
			}
			else
			{
				displayBitmap(StartScreen2, &l);
			}
			page = ~page;
			if (A2) {
				start = 1; 
			}
			break;
		case clearScreen:	
			clearDisplay(&g);
			display(&l);
			break;
		case on:
			if(reset == 0xE0)
			{
				PORTC = valOf7Seg(SS_E);
				start = 0;
				MenuMode = 0;
				DrawMode = 0;
				Slideshow_Mode = 0;
				reset = 0;
			}
			if (!start) {
				start = 1;
				MenuMode = 1;
			} 
			break;
		default:
			break;
	}
	return state;
}
// Menu screen will be displayed
int MenuModeSM(int state)
{
	enum menuStates{init, WaitforMovement,Choose};
	static unsigned char UP, DOWN;
	static unsigned char i, imageState;
	switch(state)			// State transistions
	{
		case -1:
			state = init;
			break;
		case init:
			state = WaitforMovement;
			break;
		case WaitforMovement:
			if(start && MenuMode && !A2)
			{
				state = WaitforMovement;
			}
			else if(start && MenuMode && A2)
			{
				state = Choose;
			}
			else if (start && !MenuMode)
			{
				state = init;
			}
			break;
		case Choose:
			if(!A2)
			{
				state = WaitforMovement;
			}
			else 
			{
				state = Choose;
			}
			break;
		default:
			state = init;
			break;
	}
	switch(state)			// State actions
	{
		case init:
			UP = 0;
			DOWN = 0;
			i = 0;
			imageState = 0;
			DrawMode = 0;
			break;
		case WaitforMovement:
			if(start && !MenuMode && A7)
			{
				MenuMode = 1;
				DrawMode = 0;
				Slideshow_Mode = 0;
				Saving = 0;
			}
		    if(start && MenuMode && !Saving && !DrawMode && !Slideshow_Mode)
			{
				// get directional motion for menu screen
				UP  = inputButtonBits & 0x01;
				DOWN = inputButtonBits & 0x02;
				
				// Determine direction to move on Menu
				if(!UP && DOWN)
				{
					imageState = 0;
					i = 1;
				}
				else if((UP && !DOWN))
				{
					imageState = 0;
					i = 0;
				}
				
				// Display Menu Screen 
				if(i == 0)
				{
					if(imageState == 0)
					{
						displayBitmap(draw_BlinkOn, &l);
					}
					else if(imageState)
					{
						displayBitmap(draw_BlinkOff,&l);
					}
				}
				else if(i == 1)
				{
					if(imageState == 0)
					{
						displayBitmap(slideshow_BlinkOn, &l);
					}
					else if(imageState)
					{
						displayBitmap(slideshow_BlinkOff,&l);
					}
				}
				imageState = ~imageState;
			}
			break;
		case Choose:
			MenuMode = 0;
			if(i == 0 && MenuMode == 0)
			{
				DrawMode = 1;
				Slideshow_Mode = 0;
				MenuMode = 0;
				Saving = 0;
				display(&l);
			}
			else if(i == 1 && MenuMode == 0)
			{
				Slideshow_Mode = 1;
				DrawMode = 0;
				MenuMode = 0;
				Saving = 0;
			}
			break;
		default:
			break;
	}
	return state;
}
// Output seven seg error/State message
int Follower_Tick(int state) {
	enum Follower_States {init,on} ;
	switch(state) {
		case -1:
			state = init;
			break;
		case init:
			state = on;
			break;
		case on:
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
			if(start && Slideshow_Mode && !MenuMode && !DrawMode)
			{
				PORTC = valOf7Seg(SS_2);
			}
			else if( USART_HasReceived(0) && !cleared) {
				PORTC = USART_Receive(0);
				USART_Flush(0);
			}
			else if (start && MenuMode && !DrawMode && !Slideshow_Mode)
			{
				PORTC = valOf7Seg(SS_A);
			}
			else if(start && DrawMode && !Slideshow_Mode &&!MenuMode)
			{
				PORTC = valOf7Seg(SS_D);
			}
			break;
		default:
			break;
	}
	return state;
}
// Take joystick input from usart 1 and move overing cursor on screen / Draw
int DrawModeSM(int state)
{
	enum Cursor_State{Init, WaitMove,HoverCursor, WritePixel};
	static unsigned char posY, negY, negX, posX;
	static unsigned char PixelOn;
	static unsigned char prevX, prevY;
	static unsigned char Write;
	switch(state)			// State Transitions
	{
		case -1:
		state = Init;
		break;
		case Init:
		state = WaitMove;
		break;
		case WaitMove:
		if(!Write)
		{
			state = HoverCursor;
		}
		else
		{
			state = WritePixel;
		}
		break;
		case HoverCursor:
		if(!Write)
		{
			state = WaitMove;
		}
		else
		{
			state = WritePixel;
		}
		break;
		case WritePixel:
		state = WaitMove;
		break;
		default:
		state = WaitMove;
		break;
	}
	switch(state)			// State actions
	{
		case Init:
			PixelOn = 0;
			prevX = g.cursor_x;
			prevY = g.cursor_y;
			posX = 0;
			posY = 0;
			negX = 0;
			negY = 0;
			break;
		case WaitMove:
			Write = inputButtonBits & 0x20;
			if(start && !MenuMode && !Slideshow_Mode && !Saving)
			{
				PixelOn = 0;
				// Capture directional bits
				posY = inputButtonBits & 0x01;
				negY = inputButtonBits & 0x02;
				posX = inputButtonBits & 0x08;
				negX = inputButtonBits & 0x04;
			
				// Change direction by one every tick of this state
				if(posY && !negY)
				{
					if(g.cursor_y - 1 >= 0)
					{
						g.cursor_y--;
					}
				}
				else if(negY && !posY)
				{
					if(g.cursor_y + 1 < g.HEIGHT)
					{
						g.cursor_y++;
					}
				}
				if(posX && !negX)
				{
					if(g.cursor_x + 1 < g.WIDTH)
					{
						g.cursor_x++;
					}
				}
				else if(negX && !posX)
				{
					if(g.cursor_x - 1 >= 0)
					{
						g.cursor_x--;
					}
				}
			}
			break;
		case HoverCursor:
			if(start && !MenuMode && !Slideshow_Mode && !Saving)
			{
				// Check all cases that a pixel may be off or on
				if(prevX != g.cursor_x && prevY == g.cursor_y && !PixelOn)
				{
					drawPixel(prevX,prevY, WHITE);
				}
				else if(prevY != g.cursor_y && prevX == g.cursor_x && !PixelOn)
				{
					drawPixel(prevX,prevY, WHITE);
				}
				else if(prevX != g.cursor_x && prevY != g.cursor_y && !PixelOn)
				{
					drawPixel(prevX,prevY, WHITE);
				}
				else if(PixelOn)
				{
					drawPixel(prevX,prevY, BLACK);
				}
			
				// Store New cursor location and if that pixel was on/off
				prevX = g.cursor_x;
				prevY = g.cursor_y;
				if(getPixel(prevX,prevY))
				{
					PixelOn = 1;
				}
				else
				{
					PixelOn = 0;
				}
			
				// Alternate between on and Off for cursor effect
				if(!getPixel(g.cursor_x, g.cursor_y))
				{
					drawPixel(g.cursor_x,g.cursor_y, BLACK);
				}
				else
				{
					drawPixel(g.cursor_x,g.cursor_y, WHITE);
				}
				// Display New buffer
				display(&l);
			}
			break;
		case WritePixel:
			if(start && !MenuMode && !Slideshow_Mode && !Saving)
			{
				// Color previous pixel
				drawPixel(prevX,prevY, BLACK);
			
				// Store New cursor location and if that pixel was on/off
				prevX = g.cursor_x;
				prevY = g.cursor_y;
			
				// Alternate between on and Off for cursor effect
				if(!getPixel(g.cursor_x, g.cursor_y))
				{
					drawPixel(g.cursor_x,g.cursor_y, BLACK);
				}
				else
				{
					drawPixel(g.cursor_x,g.cursor_y, WHITE);
				}
				// Display New buffer
				display(&l);
			}
			break;
		default:
			break;
	}
	return state;
}
// Will display saved pictures in the EEPROM to the LCD 
int Slideshow_ModeSM(int state)
{
	enum show_state {Init, getPic, DispPic};
	static unsigned char i = 0;
	static uint16_t N_bytes = 0;				// Number of Bytes need for LCD
	static uint16_t eeprom_LOC = 0;
	switch(state)
	{
		case -1:
			state = Init;
			break;
		case Init:
			state = getPic;	
			break;
		case getPic:
			if(Slideshow_Mode)
			{
				state = DispPic;
			}
			else
			{
				state = getPic;
			}
			break;
		case DispPic:
			state = getPic;
			break;
		default:
			state = Init;
			break;
	}
	switch (state)
	{
		case Init:
			i = 0;
			N_bytes = 504;
			break;
		case getPic:
			if(Slideshow_Mode)
			{
				// Reset to 0 to cycle through images
				if(i >= 8)
				{
					i = 0;
				}
				// Determine Location of where to read image from the EEPROM
				if(i == 0)
				{
					eeprom_LOC = 0;
				}
				else
				{
					eeprom_LOC = (i*N_bytes);
				}
				// Read from EEPROM and store into SavedImage array
				if(eeprom_is_ready())
				{
					eeprom_read_block((void*)Save,(const void *)eeprom_LOC,504);
				}
				i++;
			}
			if(!Slideshow_Mode && i != 0)
			{
				i = 0;
			}
			break;
		case DispPic:
			if(Slideshow_Mode)
			{
				displayBitmap(Save, &l);
			}
			break;
		default:
			state = Init;
			break;
	}
	return state;
}
// Will Save image during DrawMode feature into EEPROM
int SaveImageSM(int state)
{
	enum SaveState{Init, Wait, saveImage};
	static unsigned char LOC_index = 0;
	static unsigned char dispI = 0;
	static unsigned short eeprom_addr = 0;
	switch (state)		// State transitions
	{
		case -1:
			state = Init;
			break;
		case Init:
			state = Wait;
			break;
		case Wait:
			if(A6 && !MenuMode && !Slideshow_Mode)
			{
				state = saveImage;
			}
			else
			{
				state = Wait;
			}
			break;
		case saveImage:
			state = Wait;
			break;
		default:
			state = Init;
			break;
	}
	switch (state)		// State actions
	{
		case Init:
			LOC_index = 1;
			break;
		case Wait:
			if(LOC_index >= 8)
			{
				LOC_index = 2;
			}
			if (dispI >= 3)
			{
				Saving = 0;
				dispI++;	
			}
			if(Saving)
			{
				dispI++;
			}
			break;
		case saveImage:
			eeprom_addr = LOC_index*(g.WIDTH*g.HEIGHT/8);
			if(eeprom_is_ready())
			{
				eeprom_write_block((const void*)pcd8544_buffer, (void*)eeprom_addr, 504);
			}
			// Display Saved image bitmap to let user know it was saved
			displayBitmap(ImageSaved, &l);
			// Will allow for other tasks to wait for saved image picture to display for 500 ms
			Saving = 1;
			// Increase to new Location in EEPROM
			LOC_index++;
			break;
		default:
			break;
	}
	return state;
}
// Will continously check if there is Tilt or shake of the device to clear Buffer/Screen
int clearBuf(int state)
{
	enum clear_states{init, wait,clear};
	switch (state)		// State Transitions
	{
		case -1:
			state = init;
			break;
		case init:
			state = wait;
			break;
		case wait:
			if((inputButtonBits == 0x0F) && start)
			{
				state = clear;
			}
			else 
			{
				state = wait;
			}
			break;
		case clear:
			state = wait;
			break;
		default:
			state = init;
			break;
	}
	switch (state)		// State actions
	{
		case init:
			cleared = 0;
			break;
		case wait:
			break;
		case clear:
			clearBuffer(pcd8544_buffer, 0,&g);
			cleared = 1;
			break;
		default:
			break;
	}
	return state;
}

// ------------------ End Tasks --------------------------------------------------


int main(void)
{	
	// initialize ports
	DDRA = 0xFF; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0x7A; PORTD = 0x85;
	
	// Setup Sleep feature
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	cli();

	//Set up USART UsartNum -> 0
	initUSART(0);
	initUSART(1);
	
	//Set up LCD
	fillLCD2(0, 1, 2, 3, &g, &l);
	begin(42,&l);
	
	// Set up RTOS (Task Scheduler)
	task Tasks[8];
	unsigned short size = sizeof(Tasks)/sizeof(*Tasks);
	tasksNum = size;
	tasks = Tasks;
	
	// Give initial periods
	unsigned short period1 = 100;
	unsigned short period2 = 500;
	unsigned short period3 = 400;
	unsigned short period4 = 50;
	unsigned short period5 = 100;
	unsigned short period6 = 400;
	unsigned short period7 = 1000;
	unsigned short period8 = 500;
	
	// Find GCD of all Periods
	unsigned short tempGCD = GCD(period1, period2);
	tempGCD = GCD(tempGCD,period3);
	tempGCD = GCD(tempGCD,period4);
	tempGCD = GCD(tempGCD,period5);
	tempGCD = GCD(tempGCD, period6);
	tempGCD = GCD(tempGCD, period7);
	tempGCD = GCD(tempGCD, period8);
	
	// Calulate how many times GCD can evenly divide periods
	//period1 = period1/tempGCD;
	//period2 = period2/tempGCD;
	//period3 = period3/tempGCD;
	//period4 = period4/tempGCD;
	//period5 = period5/tempGCD;
	//period6 = period6/tempGCD;
	//period7 = period7/tempGCD;
	//period8 = period8/tempGCD;
	
	
	// Initialize all values for scheduler
	tasks[0].state = -1;
	tasks[0].elapsedTime = period1;
	tasks[0].period = period1;
	tasks[0].TickFct = &GetInputs;
	
	tasks[1].state = -1;
	tasks[1].elapsedTime = period2;
	tasks[1].period = period2;
	tasks[1].TickFct = &StartScreen;
	
	tasks[2].state = -1;
	tasks[2].elapsedTime = period6;
	tasks[2].period = period6;
	tasks[2].TickFct = &MenuModeSM;
	
	tasks[3].state = -1;
	tasks[3].elapsedTime = period5;
	tasks[3].period = period5;
	tasks[3].TickFct = &clearBuf;
	
	tasks[4].state = -1;
	tasks[4].elapsedTime = period3;
	tasks[4].period = period3;
	tasks[4].TickFct = &Follower_Tick;
	
	tasks[5].state = -1;
	tasks[5].elapsedTime = period4;
	tasks[5].period = period4;
	tasks[5].TickFct = &DrawModeSM;
	
	tasks[6].state = -1;
	tasks[6].elapsedTime = period7;
	tasks[6].period = period7;
	tasks[6].TickFct = &Slideshow_ModeSM;
	
	tasks[7].state = -1;
	tasks[7].elapsedTime = period8;
	tasks[7].period = period8;
	tasks[7].TickFct = &SaveImageSM;
	
	
	// Set up Timer
	TimerSet(1);
	TimerOn();
	
	while (1) {}
}
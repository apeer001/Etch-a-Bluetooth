/*
SoftwareSerial.h (formerly NewSoftSerial.h) - 
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and 
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
The latest version of this library can always be found at
http://arduiniana.org.
*/

#ifndef SoftwareSerial_h
#define SoftwareSerial_h

#include <stdbool.h>
#include <inttypes.h>
#include <iostream>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// Includes

//#include <SoftwareSerial.h>
//#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/

#define _DEBUG 0
#define _DEBUG_PIN1 11
#define _DEBUG_PIN2 13


#define _SS_MAX_RX_BUFF 64 // RX buffer size
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#define OUTPUT 0
#define INPUT 1

typedef struct SoftwareSerial {
  // per object data
  uint8_t _receivePin;
  uint8_t _receiveBitMask;
  volatile uint8_t *_receivePortRegister;
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;

  uint16_t _rx_delay_centering;
  uint16_t _rx_delay_intrabit;
  uint16_t _rx_delay_stopbit;
  uint16_t _tx_delay;

  uint16_t _buffer_overflow:1;
  uint16_t _inverse_logic:1;

  // static data
  static char _receive_buffer[_SS_MAX_RX_BUFF]; 
  static volatile uint8_t _receive_buffer_tail;
  static volatile uint8_t _receive_buffer_head;
  static SoftwareSerial *active_object;
  
};

SoftwareSerial softwareSerialObj;

// private methods
void recv();
uint8_t rx_pin_read();
void tx_pin_write(uint8_t pin_state);
void setTX(uint8_t transmitPin);
void setRX(uint8_t receivePin);

// private static method for timing
static inline void tunedDelay(uint16_t delay);

// public methods
initSoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false);

void begin(long speed);
bool listen();
void end();
bool isListening() { return this == active_object; }
bool overflow() { bool ret = _buffer_overflow; _buffer_overflow = false; return ret; }
int peek();

size_t write(uint8_t byte);
int read();
int available();
void flush();
  
//using Print::write;

// public only for easy access by interrupt handlers
static inline void handle_interrupt();


// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round



//
// Lookup table
//
typedef struct _DELAY_TABLE
{
  long baud;
  unsigned short rx_delay_centering;
  unsigned short rx_delay_intrabit;
  unsigned short rx_delay_stopbit;
  unsigned short tx_delay;
} DELAY_TABLE;

#if F_CPU == 16000000

static const DELAY_TABLE PROGMEM table[] = 
{
  //  baud    rxcenter   rxintra    rxstop    tx
  { 115200,   1,         17,        17,       12,    },
  { 57600,    10,        37,        37,       33,    },
  { 38400,    25,        57,        57,       54,    },
  { 31250,    31,        70,        70,       68,    },
  { 28800,    34,        77,        77,       74,    },
  { 19200,    54,        117,       117,      114,   },
  { 14400,    74,        156,       156,      153,   },
  { 9600,     114,       236,       236,      233,   },
  { 4800,     233,       474,       474,      471,   },
  { 2400,     471,       950,       950,      947,   },
  { 1200,     947,       1902,      1902,     1899,  },
  { 600,      1902,      3804,      3804,     3800,  },
  { 300,      3804,      7617,      7617,     7614,  },
};

const int XMIT_START_ADJUSTMENT = 5;

#elif F_CPU == 8000000

static const DELAY_TABLE table[] PROGMEM = 
{
  //  baud    rxcenter    rxintra    rxstop  tx
  { 115200,   1,          5,         5,      3,      },
  { 57600,    1,          15,        15,     13,     },
  { 38400,    2,          25,        26,     23,     },
  { 31250,    7,          32,        33,     29,     },
  { 28800,    11,         35,        35,     32,     },
  { 19200,    20,         55,        55,     52,     },
  { 14400,    30,         75,        75,     72,     },
  { 9600,     50,         114,       114,    112,    },
  { 4800,     110,        233,       233,    230,    },
  { 2400,     229,        472,       472,    469,    },
  { 1200,     467,        948,       948,    945,    },
  { 600,      948,        1895,      1895,   1890,   },
  { 300,      1895,       3805,      3805,   3802,   },
};

const int XMIT_START_ADJUSTMENT = 4;

#elif F_CPU == 20000000

// 20MHz support courtesy of the good people at macegr.com.
// Thanks, Garrett!

static const DELAY_TABLE PROGMEM table[] =
{
  //  baud    rxcenter    rxintra    rxstop  tx
  { 115200,   3,          21,        21,     18,     },
  { 57600,    20,         43,        43,     41,     },
  { 38400,    37,         73,        73,     70,     },
  { 31250,    45,         89,        89,     88,     },
  { 28800,    46,         98,        98,     95,     },
  { 19200,    71,         148,       148,    145,    },
  { 14400,    96,         197,       197,    194,    },
  { 9600,     146,        297,       297,    294,    },
  { 4800,     296,        595,       595,    592,    },
  { 2400,     592,        1189,      1189,   1186,   },
  { 1200,     1187,       2379,      2379,   2376,   },
  { 600,      2379,       4759,      4759,   4755,   },
  { 300,      4759,       9523,      9523,   9520,   },
};

const int XMIT_START_ADJUSTMENT = 6;

#else

#error This version of SoftwareSerial supports only 20, 16 and 8MHz processors

#endif

//
// Statics
//
SoftwareSerial *active_object = 0;
char _receive_buffer[_SS_MAX_RX_BUFF]; 
volatile uint8_t _receive_buffer_tail = 0;
volatile uint8_t_receive_buffer_head = 0;

//
// Debugging
//
// This function generates a brief pulse
// for debugging or measuring on an oscilloscope.
inline void DebugPulse(uint8_t pin, uint8_t count)
{
#if _DEBUG
  //volatile uint8_t *pport = portOutputRegister(digitalPinToPort(pin));
  volatile uint8_t *pport = DDRA;

  uint8_t val = *pport;
  while (count--)
  {
    *pport = val | (1 << pin);
    *pport = val;
  }
#endif
}

//
// Private methods
//

/* static */ 
inline void tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+r" (delay), "+a" (tmp)
    : "0" (delay)
    );
}

// This function sets the current object as the "listening"
// one and returns true if it replaces another 
bool listen()
{
  if (active_object != this)
  {
    _buffer_overflow = false;
    uint8_t oldSREG = SREG;
    cli();
    _receive_buffer_head = _receive_buffer_tail = 0;
    active_object = this;
    SREG = oldSREG;
    return true;
  }

  return false;
}

//
// The receive routine called by the interrupt handler
//
void recv()
{

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Preserve the registers that the compiler misses
// (courtesy of Arduino forum user *etracer*)
  asm volatile(
    "push r18 \n\t"
    "push r19 \n\t"
    "push r20 \n\t"
    "push r21 \n\t"
    "push r22 \n\t"
    "push r23 \n\t"
    "push r26 \n\t"
    "push r27 \n\t"
    ::);
#endif  

  uint8_t d = 0;

  // If RX line is high, then we don't see any start bit
  // so interrupt is probably not for us
  if (_inverse_logic ? rx_pin_read() : !rx_pin_read())
  {
    // Wait approximately 1/2 of a bit width to "center" the sample
    tunedDelay(softwareSerialObj->_rx_delay_centering);
    DebugPulse(_DEBUG_PIN2, 1);

    // Read each of the 8 bits
    for (uint8_t i=0x1; i; i <<= 1)
    {
      tunedDelay(softwareSerialObj->_rx_delay_intrabit);
      DebugPulse(_DEBUG_PIN2, 1);
      uint8_t noti = ~i;
      if (rx_pin_read())
        d |= i;
      else // else clause added to ensure function timing is ~balanced
        d &= noti;
    }

    // skip the stop bit
    tunedDelay(softwareSerialObj->_rx_delay_stopbit);
    DebugPulse(_DEBUG_PIN2, 1);

    if (_inverse_logic)
      d = ~d;

    // if buffer full, set the overflow flag and return
    if ((softwareSerialObj->_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF != softwareSerialObj->_receive_buffer_head) 
    {
      // save new data in buffer: tail points to where byte goes
      softwareSerialObj->_receive_buffer[_receive_buffer_tail] = d; // save new byte
      softwareSerialObj->_receive_buffer_tail = (softwareSerialObj->_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
    } 
    else 
    {
#if _DEBUG // for scope: pulse pin as overflow indictator
      DebugPulse(_DEBUG_PIN1, 1);
#endif
      softwareSerialObj->_buffer_overflow = true;
    }
  }

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Restore the registers that the compiler misses
  asm volatile(
    "pop r27 \n\t"
    "pop r26 \n\t"
    "pop r23 \n\t"
    "pop r22 \n\t"
    "pop r21 \n\t"
    "pop r20 \n\t"
    "pop r19 \n\t"
    "pop r18 \n\t"
    ::);
#endif
}

void tx_pin_write(uint8_t pin_state)
{
  if (pin_state == LOW)
    *softwareSerialObj->_transmitPortRegister &= ~softwareSerialObj->_transmitBitMask;
  else
    *softwareSerialObj->_transmitPortRegister |= softwareSerialObj->_transmitBitMask;
}

uint8_t rx_pin_read()
{
  return *softwareSerialObj->_receivePortRegister & softwareSerialObj->_receiveBitMask;
}

//
// Interrupt handling
//

/* static */
inline void handle_interrupt()
{
  if (softwareSerialObj->active_object)
  {
    softwareSerialObj->active_object->recv();
  }
}

#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
  handle_interrupt();
}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect)
{
  handle_interrupt();
}
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect)
{
  handle_interrupt();
}
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect)
{
  handle_interrupt();
}
#endif

//
// Constructor
//
void initSoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic /* = false */) : 
  
{
	softwareSerialObj->_rx_delay_centering = 0;
	softwareSerialObj->_rx_delay_intrabit = 0;
	softwareSerialObj->_rx_delay_stopbit = 0;
	softwareSerialObj->_tx_delay = 0;
	softwareSerialObj->_buffer_overflow = false;
	softwareSerialObj->_inverse_logic = inverse_logic;
	
	setTX(transmitPin);
	setRX(receivePin);
}


void digitalWritePortA(int8_t pin, int8_t state)
{
	if(state == 0)
	{
		PORTA &= ~(0x01 << pin);
	}
	else
	{
		PORTA |= (0x01 << pin);
	}
}

//
// Destructor
//
/*
~SoftwareSerial()
{
  end();
}
*/

void setTX(uint8_t tx)
{
	// set pin directions
	PORTA = PORTA & 0xFD; DDRB |= 0x02;
    //pinMode(tx, OUTPUT);
	
	digitalWritePortA(tx, HIGH);
	
	//_transmitBitMask = digitalPinToBitMask(tx);
	softwareSerialObj->_transmitBitMask = (1 << tx);
	uint8_t port = PORTA;
	softwareSerialObj->_transmitPortRegister = DDRA;
}

void setRX(uint8_t rx)
{
	// set pin directions
	PORTA = PORTA & 0xFE; DDRB |= 0x01;
    //pinMode(rx, INPUT);
	if (!_inverse_logic) {
		digitalWritePortA(rx, HIGH);  // pullup for normal logic!
	}
	softwareSerialObj->_receivePin = rx;
	softwareSerialObj->_receiveBitMask = digitalPinToBitMask(rx);
	uint8_t port = PORTA;
	softwareSerialObj->_receivePortRegister = DDRA;
}

//
// Public methods
//

void begin(long speed)
{
  softwareSerialObj->_rx_delay_centering = softwareSerialObj->_rx_delay_intrabit = softwareSerialObj->_rx_delay_stopbit = softwareSerialObj->_tx_delay = 0;

  for (unsigned i=0; i<sizeof(table)/sizeof(table[0]); ++i)
  {
    long baud = pgm_read_dword(&table[i].baud);
    if (baud == speed)
    {
      softwareSerialObj->_rx_delay_centering = pgm_read_word(&table[i].rx_delay_centering);
      softwareSerialObj->_rx_delay_intrabit = pgm_read_word(&table[i].rx_delay_intrabit);
      softwareSerialObj->_rx_delay_stopbit = pgm_read_word(&table[i].rx_delay_stopbit);
      softwareSerialObj->_tx_delay = pgm_read_word(&table[i].tx_delay);
      break;
    }
  }

  // Set up RX interrupts, but only if we have a valid RX baud rate
  if (_rx_delay_stopbit)
  {
    if (digitalPinToPCICR(_receivePin))
    {
      *digitalPinToPCICR(_receivePin) |= _BV(digitalPinToPCICRbit(_receivePin));
      *digitalPinToPCMSK(_receivePin) |= _BV(digitalPinToPCMSKbit(_receivePin));
    }
    tunedDelay(softwareSerialObj->_tx_delay); // if we were low this establishes the end
  }

#if _DEBUG
  //pinMode_DEBUG_PIN1, OUTPUT);
  //pinMode(_DEBUG_PIN2, OUTPUT);
#endif

  listen();
}

void end()
{
  if (digitalPinToPCMSK(softwareSerialObj->_receivePin))
    *digitalPinToPCMSK(softwareSerialObj->_receivePin) &= ~_BV(digitalPinToPCMSKbit(softwareSerialObj->_receivePin));
}


// Read data from buffer
int read()
{
  if (!isListening())
    return -1;

  // Empty buffer?
  if (softwareSerialObj->_receive_buffer_head == softwareSerialObj->_receive_buffer_tail)
    return -1;

  // Read from "head"
  uint8_t d = softwareSerialObj->_receive_buffer[softwareSerialObj->_receive_buffer_head]; // grab next byte
  softwareSerialObj->_receive_buffer_head = (softwareSerialObj->_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
  return d;
}

int available()
{
  if (!isListening())
    return 0;

  return (softwareSerialObj->_receive_buffer_tail + _SS_MAX_RX_BUFF - softwareSerialObj->_receive_buffer_head) % _SS_MAX_RX_BUFF;
}

size_t write(uint8_t b)
{
  if (softwareSerialObj->_tx_delay == 0) {
    //setWriteError();
    return 0;
  }

  uint8_t oldSREG = SREG;
  cli();  // turn off interrupts for a clean txmit

  // Write the start bit
  tx_pin_write(softwareSerialObj->_inverse_logic ? HIGH : LOW);
  tunedDelay(softwareSerialObj->_tx_delay + XMIT_START_ADJUSTMENT);

  // Write each of the 8 bits
  if (softwareSerialObj->_inverse_logic)
  {
    for (byte mask = 0x01; mask; mask <<= 1)
    {
      if (b & mask) // choose bit
        tx_pin_write(LOW); // send 1
      else
        tx_pin_write(HIGH); // send 0
    
      tunedDelay(softwareSerialObj->_tx_delay);
    }

    tx_pin_write(LOW); // restore pin to natural state
  }
  else
  {
    for (byte mask = 0x01; mask; mask <<= 1)
    {
      if (b & mask) // choose bit
        tx_pin_write(HIGH); // send 1
      else
        tx_pin_write(LOW); // send 0
    
      tunedDelay(softwareSerialObj->_tx_delay);
    }

    tx_pin_write(HIGH); // restore pin to natural state
  }

  SREG = oldSREG; // turn interrupts back on
  tunedDelay(softwareSerialObj->_tx_delay);
  
  return 1;
}

void flush()
{
  if (!isListening())
    return;

  uint8_t oldSREG = SREG;
  cli();
  softwareSerialObj->_receive_buffer_head = softwareSerialObj->_receive_buffer_tail = 0;
  SREG = oldSREG;
}

int peek()
{
  if (!isListening())
    return -1;

  // Empty buffer?
  if (softwareSerialObj->_receive_buffer_head == softwareSerialObj->_receive_buffer_tail)
    return -1;

  // Read from "head"
  return softwareSerialObj->_receive_buffer[softwareSerialObj->_receive_buffer_head];
}



#endif
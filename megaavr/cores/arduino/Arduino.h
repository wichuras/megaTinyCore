/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

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
*/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#undef F
#define F(str) (str)

#ifdef __cplusplus
extern "C"{
#endif

/* Analog reference options */

/* Change in mega4809: two places to define analog reference
 - VREF peripheral defines internal reference
 - analog peripherals define internal/Vdd/external
*/

 // internal from VREF

 /* Values shifted to avoid clashing with ADC REFSEL defines
	Will shift back in analog_reference function
  */
#define INTERNAL0V55 (VREF_ADC0REFSEL_0V55_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL1V1 (VREF_ADC0REFSEL_1V1_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL2V5 (VREF_ADC0REFSEL_2V5_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL4V3 (VREF_ADC0REFSEL_4V34_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL1V5 (VREF_ADC0REFSEL_1V5_gc >> VREF_ADC0REFSEL_gp)

#define DEFAULT     INTERNAL0V55
#define INTERNAL    ADC_REFSEL_INTREF_gc
#define VDD         ADC_REFSEL_VDDREF_gc
#ifdef ADC_REFSEL_VREFA_gc
#define EXTERNAL    ADC_REFSEL_VREFA_gc
#endif

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()


// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

/* Allows performing a correction on the CPU value using the signature row
	values indicating oscillator error provided from the device manufacturer */
#define PERFORM_SIGROW_CORRECTION_F_CPU 0

uint16_t clockCyclesPerMicrosecondComp(uint32_t clk);
uint16_t clockCyclesPerMicrosecond();
unsigned long clockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToClockCycles(unsigned long microseconds);

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

extern const uint8_t PROGMEM digital_pin_to_port[];
extern const uint8_t PROGMEM digital_pin_to_bit_mask[];
extern const uint8_t PROGMEM digital_pin_to_bit_position[];
extern const uint8_t PROGMEM digital_pin_to_timer[];

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
// These perform slightly better as macros compared to inline functions
//
#define NOT_A_PIN 255
#define NOT_A_PORT 255
#define NOT_AN_INTERRUPT 255

#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5
#define NUM_TOTAL_PORTS 6

// These are used for two things: identifying the timer on a pin
// and for the MILLIS_TIMER define that the uses can test which timer
// actually being used for millis is actually being used
// Reasoning these constants are what they are:
// Low 3 bits are the number of that peripheral
// other bits specify the type of timer
// TCA=0x08, TCB=0x10, TCD=0x10 (leaving room in case Microchip ever decides to release a TCC)
// Things that aren't hardware timers with output compare are after that
// DAC output isn't a timer, but has to be treated as such by PINMODE
// RTC timer is a tiner, but certainly not that kind of timer
#define NOT_ON_TIMER 0x00
#define TIMERA0 0x10
#define TIMERB0 0x20
#define TIMERB1 0x21
#define TIMERB2 0x22
#define TIMERB3 0x23
#define TIMERD0 0x40
#define TIMERRTC 0x90
#define DACOUT 0x80

void setup_timers();

#define digitalPinToPort(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_port + pin) : NOT_A_PIN )
#define digitalPinToBitPosition(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_bit_position + pin) : NOT_A_PIN )
#define analogPinToBitPosition(pin) ( (digitalPinToAnalogInput(pin)!=NOT_A_PIN) ? pgm_read_byte(digital_pin_to_bit_position + pin + 0) : NOT_A_PIN )
#define digitalPinToBitMask(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_bit_mask + pin) : NOT_A_PIN )
#define analogPinToBitMask(pin) ( (digitalPinToAnalogInput(pin)!=NOT_A_PIN) ? pgm_read_byte(digital_pin_to_bit_mask + pin + 0) : NOT_A_PIN )
#define digitalPinToTimer(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_timer + pin) : NOT_ON_TIMER )

#define portToPortStruct(port) ( (port < NUM_TOTAL_PORTS) ? ((PORT_t *)&PORTA + port) : NULL)
#define digitalPinToPortStruct(pin) ( (pin < NUM_TOTAL_PINS) ? ((PORT_t *)&PORTA + digitalPinToPort(pin)) : NULL)
#define getPINnCTRLregister(port, bit_pos) ( ((port != NULL) && (bit_pos < NOT_A_PIN)) ? ((volatile uint8_t *)&(port->PIN0CTRL) + bit_pos) : NULL )
#define digitalPinToInterrupt(P) (P)

#define portOutputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->OUT ) )
#define portInputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->IN ) )
#define portModeRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->DIR ) )

//#defines to identify part families
#if defined(__AVR_ATtiny3217__)
#define MEGATINYCORE_MCU 3217
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1617__)
#define MEGATINYCORE_MCU 1617
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny817__)
#define MEGATINYCORE_MCU 817
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny417__)
#define MEGATINYCORE_MCU 417
#define __AVR_ATtinyx17__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny3207__)
#define MEGATINYCORE_MCU 3207
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1607__)
#define MEGATINYCORE_MCU 1607
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny807__)
#define MEGATINYCORE_MCU 807
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny407__)
#define MEGATINYCORE_MCU 407
#define __AVR_ATtinyx07__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny3216__)
#define MEGATINYCORE_MCU 3216
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1616__)
#define MEGATINYCORE_MCU 1616
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny816__)
#define MEGATINYCORE_MCU 816
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny416__)
#define MEGATINYCORE_MCU 416
#define __AVR_ATtinyx16__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1606__)
#define MEGATINYCORE_MCU 1606
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny806__)
#define MEGATINYCORE_MCU 806
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny406__)
#define MEGATINYCORE_MCU 406
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny214__)
#define MEGATINYCORE_MCU 214
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1614__)
#define MEGATINYCORE_MCU 1614
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny814__)
#define MEGATINYCORE_MCU 814
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny414__)
#define MEGATINYCORE_MCU 414
#define __AVR_ATtinyx14__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny204__)
#define MEGATINYCORE_MCU 204
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny804__)
#define MEGATINYCORE_MCU 804
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny404__)
#define MEGATINYCORE_MCU 404
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1604__)
#define MEGATINYCORE_MCU 1604
#define __AVR_ATtinyx04__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny212__)
#define MEGATINYCORE_MCU 212
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny412__)
#define MEGATINYCORE_MCU 412
#define __AVR_ATtinyx12__
#define __AVR_ATtinyxy2__
#elif defined(__AVR_ATtiny202__)
#define MEGATINYCORE_MCU 202
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny402__)
#define MEGATINYCORE_MCU 402
#define __AVR_ATtinyx02__
#define __AVR_ATtinyxy2__
#else
#error "Can't-happen: unknown chip somehow being used"
#endif

#define MEGATINYCORE "1.1.19"
#define MEGATINYCORE_MAJOR 01
#define MEGATINYCORE_MINOR 01
#define MEGATINYCORE_PATCH 09
#define MEGATINYCORE_RELEASED 01
#define MEGATINYCORE_NUM 0x01010900



#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include "UART.h"

#endif

#include "pins_arduino.h"
#endif

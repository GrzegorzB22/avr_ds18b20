
#ifndef PIN_DEFINES_H_
#define PIN_DEFINES_H_

#include <avr/io.h>
#include <avr/eeprom.h>

/*----------------------*/
#define LED_DDR  DDRB
#define LED_PIN  PINB
#define LED_PORT PORTB

#define SEG_A PB0
#define SEG_C PB3
#define SEG_D PB4
#define SEG_E PB5
#define SEG_F PB1
#define SEG_G PB2
/*-----------------------*/


/*------------------------*/
#define BUT_DDR DDRC
#define BUT_PIN PINC
#define BUT_PORT PORTC

#define BUT_START PC2
#define BUT_UP    PC1
#define BUT_DOWN  PC0

#define DS_DDR DDRC
#define DS_PIN PINC
#define DS_PORT PORTC

#define DS PC5
/*------------------------*/

/*------------------------*/
#define T_DDR DDRD
#define T_PORT PORTD
#define T_PIN PIND

#define T0 PD2
#define T1 PD3
#define T2 PD4

#define LED_DP_DDR DDRD
#define LED_DP_PORT PORTD
#define LED_DP_PIN PIND

#define LED_DP PD0

#define BUZ_DDR DDRD
#define BUZ_PORT PORTD
#define BUZ_PIN PIND

#define BUZ PD1

#define SEG_B_DDR DDRD
#define SEG_B_PORT PORTD
#define SEG_B_PIN PIND

#define SEG_B PD5

#define COMP_P PD6
#define COMP_M PD7

#endif


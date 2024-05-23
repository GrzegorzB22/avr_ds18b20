
#ifndef AVR_COMP_H_
#define AVR_COMP_H_

#include <avr/io.h>
#include <avr/eeprom.h>

void comp_init(void);

extern volatile uint8_t therm_data[3];
extern uint8_t therm_data_E[3] EEMEM;

#endif
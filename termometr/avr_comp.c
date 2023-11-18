
#include "avr_comp.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

volatile uint8_t therm_data[3];
uint8_t therm_data_E[3] EEMEM;

void comp_init(void)
{
	ACSR |= _BV(ACIE) | _BV(ACIS1);
}

ISR(ANA_COMP_vect, ISR_NAKED)
{
	for(uint8_t c = 0; c < 3; c++){
		while(eeprom_is_ready() == 0);
		eeprom_update_byte(&therm_data_E[c], therm_data[c]);
	}
	while(1);
}

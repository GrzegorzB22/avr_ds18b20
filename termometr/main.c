
#include <avr/io.h>
#include "pin_defines.h"
#include "display.h"
#include <avr/interrupt.h>
#include "ds18b20.h"
#include <util/delay.h>
#include <avr/eeprom.h>
#include "avr_comp.h"

extern volatile uint8_t therm_data[3];
extern uint8_t therm_data_E[3] EEMEM;

int main(void)
{

	for(uint8_t c = 0; c < 3; c++){
		while(eeprom_is_ready() == 0);
		therm_data[c] = eeprom_read_byte(&therm_data_E[c]);
	}

	uint8_t mode = 0;
	int8_t tempH, tempL;
	tempL = tempH = 0;
	uint8_t enter_settings = 0;
	int16_t temp = 0;
	uint16_t temp_fra = 0;
	uint8_t temp_dec = 0;
	uint8_t but = 0;

	tempH = therm_data[0];
	tempL = therm_data[1];
	mode = therm_data[2];
	
	if(mode > 2) mode = 0;

	pins_init();
	timer_init();
	comp_init();
	
	sei();

	

	while (1)
	{

		but = 0;
		if(but_holdCounter[0] == BUTTON_HOLD_MS){
			but = 20;
			but_holdCounter[0] = 0;
		}
		else if(but_holdCounter[0] == BUTTON_RELEASED){
			but = 2;
			but_holdCounter[0] = 0;
		}

		if(but_holdCounter[1] == BUTTON_HOLD_MS){
			but = 30;
			but_holdCounter[1] = 0;
		}
		else if(but_holdCounter[1] == BUTTON_RELEASED){
			but = 3;
			but_holdCounter[1] = 0;
		}

		switch(but_counter){
			case 1: enter_settings = 1;
			start_blink = 0;
			mode = therm_setMode(mode, but);
			break;
			case 2: if(mode == 2) tempL = therm_setTemp(tempL, but);
			else tempH = therm_setTemp(tempH, but);
			break;
			case 3: if(mode == 0) tempL = therm_setTemp(tempL, but);
			else enter_settings = but_counter = 0;
			break;
			case 4: enter_settings = but_counter = 0;
			break;
		}

		if(enter_settings == 0){
			if(ds18b20_startConversion() == 0){
				TEMP_DIGITS[2] = TEMP_DIGITS[1] = TEMP_DIGITS[0] = 0;
				LED_DP_PORT |= _BV(LED_DP);
				continue;
			}
			temp = ds18b20_getTemp();
			temp_dec = ABS(temp) / 16;
			temp_fra = (temp & 0b1111) * 625;

			if(((temp_fra/100) % 10) > 5)
			temp_fra += 1000;
			temp_fra /= 1000;

			start_blink = therm_tempExceed(mode, tempH, tempL, temp_dec, temp_fra);

			
			switch(mode){
				case 0: if(tempH > 0){
					if((temp_dec > tempH) || ((temp_dec == tempH) && (temp_fra > 0)))
					start_blink = 1;
					else
					start_blink = 0;
				}
				else{
					if(temp_dec < ABS(tempH))
					start_blink = 1;
					else
					start_blink = 0;
				}
				
				if(tempL > 0){
					if(temp_dec < tempL)
					start_blink = 1;
					else
					start_blink = 0;
				}
				else{
					if((temp_dec > ABS(tempL)) || ((temp_dec == ABS(tempL)) && (temp_fra > 0)))
					start_blink = 1;
					else
					start_blink = 0;
				}
				break;
				case 1: if(tempH > 0){
					if((temp_dec > tempH) || ((temp_dec == tempH) && (temp_fra > 0)))
					start_blink = 1;
					else
					start_blink = 0;
				}
				else{
					if(temp_dec < ABS(tempH))
					start_blink = 1;
					else
					start_blink = 0;
				}
				break;
				case 2: if(tempL > 0){
					if(temp_dec < tempL)
					start_blink = 1;
					else
					start_blink = 0;
				}
				else{
					if((temp_dec > ABS(tempL)) || ((temp_dec == ABS(tempL)) && (temp_fra > 0)))
					start_blink = 1;
					else
					start_blink = 0;
				}
				break;
			}
			
			therm_tempDisplay((temp > 0) ? 1 : 0, temp_dec, temp_fra);
		}
		therm_data[0] = tempH;
		therm_data[1] = tempL;
		therm_data[2] = mode;
	}
}


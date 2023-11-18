
#include <avr/interrupt.h>
#include <avr/io.h>
#include "display.h"
#include "pin_defines.h"
#include <util/delay.h>

volatile const uint8_t __flash LED_DIGITS[ LED_DIGITS_SIZE ] =
{
	_BV( SEG_A ) | _BV( SEG_F ) | _BV( SEG_E ) | _BV( SEG_C ) | _BV( SEG_D ),                // 0 B
	_BV( SEG_C ),                                                                            // 1 B
	_BV( SEG_A ) | _BV( SEG_G ) | _BV( SEG_E ) | _BV( SEG_D ),                               // 2 B
	_BV( SEG_A ) | _BV( SEG_G ) | _BV( SEG_C ) | _BV( SEG_D ),                               // 3 B
	_BV( SEG_F ) | _BV( SEG_G ) | _BV( SEG_C ),                                              // 4 B
	_BV( SEG_A ) | _BV( SEG_F ) | _BV( SEG_G ) | _BV( SEG_C ) | _BV( SEG_D ),                // 5
	_BV( SEG_A ) | _BV( SEG_F ) | _BV( SEG_G ) | _BV( SEG_E ) | _BV( SEG_D ) | _BV( SEG_C ), // 6
	_BV( SEG_A ) | _BV( SEG_C ),                                                             // 7 B
	_BV( SEG_A ) | _BV( SEG_F ) | _BV( SEG_G ) | _BV( SEG_E ) | _BV( SEG_D ) | _BV( SEG_C ), // 8 B
	_BV( SEG_A ) | _BV( SEG_F ) | _BV( SEG_G ) | _BV( SEG_C ) | _BV( SEG_D ),                // 9 B
	_BV( SEG_G )										                                     // -
};

volatile uint8_t TEMP_DIGITS[TEMP_DIGITS_SIZE];
volatile uint8_t but_counter;
volatile uint8_t but_holdCounter[2];
volatile uint8_t start_blink;


void timer_init(void)
{
	TCCR0 = _BV(CS02);
	TCNT0 = 131;
	TCCR2 = _BV(CS21) | _BV(CS20);
	TCNT2 = 6;
	TIMSK = _BV(TOIE0) | _BV(TOIE2);
}

void pins_init( void )
{
	LED_DDR |= _BV( SEG_A ) | _BV( SEG_C ) | _BV( SEG_D ) | _BV( SEG_E ) | _BV( SEG_F ) | _BV( SEG_G );
	LED_PORT &= ~(_BV( SEG_A ) | _BV( SEG_C ) | _BV( SEG_D ) | _BV( SEG_E ) | _BV( SEG_F ) | _BV( SEG_G ));

	BUT_DDR &= ~(_BV( BUT_START ) | _BV( BUT_UP ) | _BV( BUT_DOWN ));
	BUT_PORT |= _BV( BUT_START) | _BV( BUT_UP ) | _BV( BUT_DOWN );

	DS_DDR &= ~_BV( DS );
	DS_PORT &= ~_BV( DS );

	T_DDR |= _BV( T0 ) | _BV( T1 ) | _BV( T2 );
	T_PORT &= ~(_BV( T0 ) | _BV( T1 ) | _BV( T2 ));

	LED_DP_DDR |= _BV( LED_DP );
	LED_DP_PORT |= _BV( LED_DP );

	BUZ_DDR |= _BV( BUZ );
	BUZ_PORT &= ~_BV( BUZ );

	SEG_B_DDR |= _BV( SEG_B );
	SEG_B_PORT &= ~_BV( SEG_B );
}

uint8_t therm_setMode(uint8_t mode, uint8_t but)
{
	TEMP_DIGITS[2] = TEMP_DIGITS[1] = LED_OFF;
	LED_DP_PORT &= ~_BV(LED_DP);

	if(but == 2){
		if(mode < 2) mode++;
	}
	else if(but == 3){
		if(mode > 0) mode--;
	}

	TEMP_DIGITS[0] = mode;
	return mode;
}

static inline void therm_showTemp(int8_t temp)
{
	if(temp >= 100){
		TEMP_DIGITS[0] = temp % 10;
		TEMP_DIGITS[1] = (temp/10) % 10;
		TEMP_DIGITS[2] = temp / 100;
	}
	else if(temp >= 10){
		TEMP_DIGITS[0] = temp % 10;
		TEMP_DIGITS[1] = temp / 10;
		TEMP_DIGITS[2] = LED_OFF;
	}
	else if(temp >= 0){
		TEMP_DIGITS[0] = temp;
		TEMP_DIGITS[1] = LED_OFF;
		TEMP_DIGITS[2] = LED_OFF;
	}
	else if(temp > (-10)){
		TEMP_DIGITS[0] = ABS(temp);
		TEMP_DIGITS[1] = LED_MINUS;
		TEMP_DIGITS[2] = LED_OFF;
	}
	else if(temp >= (TEMP_MIN)){
		TEMP_DIGITS[0] = ABS(temp) % 10;
		TEMP_DIGITS[1] = ABS(temp) / 10;
		TEMP_DIGITS[2] = LED_MINUS;
	}
}

void therm_tempDisplay(uint8_t sign, uint8_t temp_dec, uint8_t temp_fra)
{
	if(sign){
		LED_DP_PORT |= _BV(LED_DP);
		if(temp_dec >= 100){
			TEMP_DIGITS[0] = temp_dec % 10;
			TEMP_DIGITS[1] = (temp_dec/10) % 10;
			TEMP_DIGITS[2] = temp_dec / 100;
			LED_DP_PORT &= ~_BV(LED_DP);
		}
		else if(temp_dec >= 10){
			TEMP_DIGITS[0] = temp_fra;
			TEMP_DIGITS[1] = temp_dec % 10;
			TEMP_DIGITS[2] = temp_dec / 10;
		}
		else{
			TEMP_DIGITS[0] = temp_fra;
			TEMP_DIGITS[1] = temp_dec;
			TEMP_DIGITS[2] = LED_OFF;
		}
	}
	else{
		LED_DP_PORT &= ~_BV(LED_DP);
		if((temp_dec > 0) && (temp_dec < 10)){
			TEMP_DIGITS[0] = temp_dec;
			TEMP_DIGITS[1] = LED_MINUS;
			TEMP_DIGITS[2] = LED_OFF;
		}
		else{
			TEMP_DIGITS[0] = temp_dec % 10;
			TEMP_DIGITS[1] = temp_dec / 10;
			TEMP_DIGITS[2] = LED_MINUS;
		}
	}
}

int8_t therm_setTemp(int8_t temp, uint8_t but)
{
	switch(but){
		case 2: if(temp < TEMP_MAX) temp++;
		break;
		case 3: if(temp > (TEMP_MIN)) temp--;
		break;
		case 20: if((temp+10) < TEMP_MAX) temp += 10;
		break;
		case 30: if((temp-10) > (TEMP_MIN)) temp -= 10;
		break;
	}

	therm_showTemp(temp);
	
	return temp;
}

void led_display( uint8_t number )
{
	if(number == LED_OFF){
		LED_PORT &= 0xC0;
		SEG_B_PORT &= ~_BV(SEG_B);
		return;
	}

	if(number < 11)
	LED_PORT = (LED_PORT & 0xC0) | (LED_DIGITS[number] & 0x3F);

	if((number == 5) || (number == 6) || (number == LED_MINUS))
	SEG_B_PORT &= ~_BV( SEG_B );
	else
	SEG_B_PORT |= _BV( SEG_B );
}

static inline void scan_buttons(void)
{
	static uint8_t oldkey = 0xFF;
	static uint8_t counter[5];
	static uint8_t but_clicked[2];
	static uint8_t but_hold[2];
	uint8_t k = BUT_PIN;

	if(counter[0] == 0){
		if(((k ^ oldkey) & _BV(BUT_START)) && ((k & _BV(BUT_START)) == 0)){
			but_counter++;
			counter[0] = 80;
		}
		oldkey &= ~_BV(BUT_START);
		oldkey |= (k & _BV(BUT_START));
	}
	else counter[0]--;

	if(counter[1] == 0){
		if(((k ^ oldkey) & _BV(BUT_UP)) && ((k & _BV(BUT_UP)) == 0)){
			counter[1] = 80;
			but_clicked[0] = 1;
		}
		oldkey &= ~_BV(BUT_UP);
		oldkey |= (k & _BV(BUT_UP));
	}
	else counter[1]--;

	if(but_clicked[0] == 1){
		if(counter[3] == 0){
			if((k & _BV(BUT_UP)) == 0){
				but_holdCounter[0]++;
				if(but_holdCounter[0] == BUTTON_HOLD_MS) but_hold[0] = 1;
				counter[3] = 1;
			}
			else{
				if(but_hold[0] == 1) but_hold[0] = but_holdCounter[0] = 0;
				else but_holdCounter[0] = BUTTON_RELEASED;
				counter[3] = 0;
				but_clicked[0] = 0;
			}
		}
		else counter[3]--;
	}

	if(counter[2] == 0){
		if(((k ^ oldkey) & _BV(BUT_DOWN)) && ((k & _BV(BUT_DOWN)) == 0)){
			counter[2] = 80;
			but_clicked[1] = 1;
		}
		oldkey &= ~_BV(BUT_DOWN);
		oldkey |= (k & _BV(BUT_DOWN));
	}
	else counter[2]--;

	if(but_clicked[1] == 1){
		if(counter[4] == 0){
			if((k & _BV(BUT_DOWN)) == 0){
				but_holdCounter[1]++;
				if(but_holdCounter[1] == BUTTON_HOLD_MS) but_hold[1] = 1;
				counter[4] = 1;
			}
			else{
				if(but_hold[1] == 1) but_hold[1] = but_holdCounter[1] = 0;
				else but_holdCounter[1] = BUTTON_RELEASED;
				counter[4] = 0;
				but_clicked[1] = 0;
			}
		}
		else counter[4]--;
	}
}

uint8_t therm_tempExceed(uint8_t mode, int8_t tempH, int8_t tempL, uint8_t temp_dec, uint8_t temp_fra)
{
	
	uint8_t tH = ABS(tempH);
	uint8_t tL = ABS(tempL);
	
	switch(mode){
		case 0: if(tempH >= 0){
			if(((temp_dec == tH) && (temp_fra > 0)) || (temp_dec > tH)) return 1;
		}
		else if(temp_dec < tH) return 1;
		
		if(tempL < 0){
			if(((temp_dec == tL) && (temp_fra > 0)) || (temp_dec > tL)) return 1;
		}
		else if(temp_dec < tL) return 1;

		break;
		case 1: if(tempH > 0){
			if(((temp_dec == tH) && (temp_fra > 0)) || (temp_dec > tH)) return 1;
		}
		else if(temp_dec < tH) return 1;

		break;
		case 2: if(tempL < 0){
			if(((temp_dec == tL) && (temp_fra > 0)) || (temp_dec > tL)) return 1;
		}
		else if(temp_dec < tL) return 1;

		break;
	}

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 131;

	static uint8_t z = 0;
	static uint8_t t_counter = 2;

	if(start_blink == 1){
		if((z > 0) && (z < 125)){
			T_PORT = (T_PORT & 0xE3) | (_BV(t_counter) & (~0xE3));
			led_display(TEMP_DIGITS[(t_counter++)-2]);
			if(t_counter == 5) t_counter = 2;
		}
		else if((z > 125) && (z < 250)) T_PORT &= 0xE3;
		z = (z+1) % 250;
	}
	else{
		T_PORT = (T_PORT & 0xE3) | (_BV(t_counter) & (~0xE3));
		led_display(TEMP_DIGITS[(t_counter++)-2]);
		if(t_counter == 5) t_counter = 2;
	}
}

ISR(TIMER2_OVF_vect)
{
	TCNT2 = 6;
	scan_buttons();
}
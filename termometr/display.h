
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>

#define LED_DIGITS_SIZE 11
#define TEMP_DIGITS_SIZE 3
#define T_MASK 0xE3
#define LED_OFF 0xFF
#define LED_MINUS 10
#define TEMP_MAX 125
#define TEMP_MIN -55
#define BUTTON_HOLD_MS	100
#define BUTTON_RELEASED 250

#define ABS(X) (((X) < 0) ? -(X) : (X))

extern volatile uint8_t TEMP_DIGITS[TEMP_DIGITS_SIZE];
extern volatile const uint8_t __flash LED_DIGITS[LED_DIGITS_SIZE];
extern volatile uint8_t but_counter;
extern volatile uint8_t but_holdCounter[2];
extern volatile uint8_t start_blink;

void timer_init(void);
void pins_init(void);
void led_display(uint8_t number);
void therm_tempDisplay(uint8_t sign, uint8_t temp_dec, uint8_t temp_fra);
uint8_t therm_setMode(uint8_t mode, uint8_t but);
int8_t therm_setTemp(int8_t temp, uint8_t but);
uint8_t therm_tempExceed(uint8_t mode, int8_t tempH, int8_t tempL, uint8_t temp_dec, uint8_t temp_fra);


#endif
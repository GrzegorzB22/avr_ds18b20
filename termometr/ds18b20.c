
#include <avr/io.h>
#include "ds18b20.h"
#include "pin_defines.h"
#include <util/delay.h>
#include <util/atomic.h>

uint8_t ds18b20_reset(void)
{
	uint8_t counter = 0;

	DS_DDR |= _BV( DS );
	_delay_us( 480 );
	DS_DDR &= ~_BV( DS );
	_delay_us( 60 );

	counter = DS_PIN & _BV(DS);
	_delay_us(420);

	return !counter;
}

void ds18b20_writeBit(uint8_t bit)
{
	DS_DDR |= _BV(DS);
	_delay_us(1);
	
	if(bit) DS_DDR &= ~_BV(DS);
	_delay_us(60);
	
	DS_DDR &= ~_BV(DS);
}

uint8_t ds18b20_readBit(void)
{
	uint8_t bit;
	
	DS_DDR |= _BV(DS);
	_delay_us(1);
	DS_DDR &= ~_BV(DS);
	_delay_us(15);

	bit = DS_PIN & _BV(DS);
	
	_delay_us(45);
	return bit;
}

void ds18b20_writeByte(uint8_t byte)
{
	for(uint8_t i = 0; i < 8; i++){
		ds18b20_writeBit(byte & 0x01);
		byte >>= 1;
		_delay_us(1);
	}
}

uint8_t ds18b20_readByte(void)
{
	uint8_t byte = 0;

	for(uint8_t i = 0; i < 8; i++){
		byte >>= 1;
		if(ds18b20_readBit()) byte |= 0x80;
		_delay_us(1);
	}
	
	return byte;
}

uint8_t ds18b20_startConversion(void)
{
	if(ds18b20_reset() == 0)
	return 0;

	ds18b20_writeByte(DS_SKIP_ROM);
	ds18b20_writeByte(DS_CONVERT);
	while(!ds18b20_readBit());
	
	return 1;
}

int16_t ds18b20_getTemp(void)
{
	int16_t temp;

	if(ds18b20_reset() == 0)
	return 0xFFFF;
	
	ds18b20_writeByte(DS_SKIP_ROM);
	ds18b20_writeByte(DS_READ_SCRATCHPAD);
	
	temp = ds18b20_readByte();
	temp |= (ds18b20_readByte() << 8);
	
	return temp;
}

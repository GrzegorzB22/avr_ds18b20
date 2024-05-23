
#ifndef DS18B20_H_
#define DS18B20_H_

#include <avr/io.h>

#define DS_SKIP_ROM         0xCC
#define DS_CONVERT          0x44
#define DS_READ_SCRATCHPAD  0xBE

uint8_t ds18b20_reset( void );
void ds18b20_writeBit( uint8_t bit );
uint8_t ds18b20_readBit( void );
void ds18b20_writeByte( uint8_t byte );
uint8_t ds18b20_readByte( void );
uint8_t ds18b20_startConversion(void);
int16_t ds18b20_getTemp(void);

#endif

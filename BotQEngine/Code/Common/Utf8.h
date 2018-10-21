#ifndef __XUTF8_H__
#define __XUTF8_H__

#include <stdint.h>
#include <stddef.h>

/*
Парсер utf8. писал его по правилам описанным здесь https://ru.wikipedia.org/wiki/UTF-8
*/

namespace utf8 
{
	const uint16_t ByteOrderMask = 0xfeff;

	inline size_t char_length_in_utf8(uint32_t c)
	{
		if (c >> 7 == 0)
			return 1;
		if (c >> 11 == 0)
			return 2;
		if (c >> 16 == 0)
			return 3;
		if (c >> 21 == 0)
			return 4;
		return 1;
	}

	inline size_t write_utf8_char(uint8_t* buffer, uint32_t c)
	{
		size_t sz = char_length_in_utf8(c);
		switch (sz)
		{
		case 1:
			buffer[0] = c;
			break;
		case 2:
			buffer[0] = 0b11000000 | uint8_t(c >> 6) & 0b00011111;
			buffer[1] = 0b10000000 | uint8_t(c) & 0b00111111;
			break;
		case 3:
			buffer[0] = 0b11100000 | uint8_t(c >> 12) & 0b00001111;
			buffer[1] = 0b10000000 | uint8_t(c >> 6) & 0b00111111;
			buffer[2] = 0b10000000 | uint8_t(c) & 0b00111111;
			break;
		case 4:
			buffer[0] = 0b11110000 | uint8_t(c >> 16) & 0b00000111;
			buffer[1] = 0b10000000 | uint8_t(c >> 12) & 0b00111111;
			buffer[2] = 0b10000000 | uint8_t(c >> 6) & 0b00111111;
			buffer[3] = 0b10000000 | uint8_t(c) & 0b00111111;
			break;
		}
		return sz;
	}

	inline size_t utf8_char_length(const uint8_t sz)
	{
		if (sz >> 7 == 0)
			return 1;
		if (sz >> 5 == 0b110)
			return 2;
		if (sz >> 4 == 0b1110)
			return 3;
		if (sz >> 3 == 0b11110)
			return 4;
		return 1;
	}

	inline uint32_t utf8_char_iterator(const uint8_t* &iter)
	{
		char part0 = 0, part1 = 0, part2 = 0, part3 = 0;
		uint32_t c = 0;
		size_t lngth = utf8_char_length(*iter);
		switch (lngth)
		{
		case 1:
			c = *iter & 0b01111111;
			iter++;
			break;
		case 2:
			part0 = *iter & 0b00011111;
			iter++;
			part1 = *iter & 0b00111111;
			iter++;
			c = (uint32_t(part0) << 6) | uint32_t(part1);
			break;
		case 3:
			part0 = *iter & 0b00001111;
			iter++;
			part1 = *iter & 0b00111111;
			iter++;
			part2 = *iter & 0b00111111;
			iter++;
			c = (uint32_t(part0) << 12) | (uint32_t(part1) << 6) | uint32_t(part2);
			break;
		case 4:
			part0 = *iter & 0b00000111;
			iter++;
			part1 = *iter & 0b00111111;
			iter++;
			part2 = *iter & 0b00111111;
			iter++;
			part3 = *iter & 0b00111111;
			iter++;
			c = (uint32_t(part0) << 16) | (uint32_t(part1) << 12) + (uint32_t(part2) << 6) | uint32_t(part3);
			break;
		}
		return c;
	}

};

#endif
#include "bits.h"
#include <stdio.h>


uint32_t get_field(uint32_t word, int pos, int width)
{
	uint32_t mask;
	
	if (width < 1 || width > 32 || pos < 0 || pos > 32 || pos + width > 32) {
		return 0;
	}

	if (width == 32) {
		mask = 0xFFFFFFFFU;
	} else {
		mask = (1U << width) - 1U;
	}

	return (word >> pos) & mask;
}

uint32_t set_field(uint32_t word, int pos, int width, uint32_t value)
{
	uint32_t mask;

	if (width < 1 || width > 32 || pos < 0 || pos > 32 || pos + width > 32) {
		return word;
	}
	
	if (width == 32) {
		mask = 0xFFFFFFFFU;
	} else {
		mask = (1U << width) - 1U;
	}

	word = word & ~(mask << pos);
	word = word | ((value & mask) << pos);

	return word;
}

int32_t sign_extend(uint32_t x, int width)
{

	uint32_t mask;
	uint32_t sign_bit;

	if (width < 1 || width > 32) {
		return 0;
	}

	if (width == 32) {
		mask = 0xFFFFFFFFU;
	} else {
		mask = (1U << width) - 1U;
	}
	
	x = x & mask;

	sign_bit = 1U << (width -1);

	if (x & sign_bit) {
		x = x | ~mask;
	}
	
	return (int32_t)x;
}

void print_binary(uint32_t x, int width)
{
	int count = 0;

	if (width < 1 || width > 32) {
		return;
	}

	for (int i = width - 1; i >=0; i--) {
		if (x & (1U << i)) {
			putchar('1');
		} else {
			putchar('0');
		}

		count++;

		if (count % 4 == 0 && i != 0) {
			putchar(' ');
		}
	}
}	

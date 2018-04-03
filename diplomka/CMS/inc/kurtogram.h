
#include <stdint.h>
#include <string.h>

#define sizeOfArray			 2000

union {
	uint16_t adc_values[sizeOfArray];
	float flt[sizeOfArray];
	uint32_t u[sizeOfArray];
} samples;

void kurtogram();

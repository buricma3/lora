
#include <stdint.h>
#include <string.h>

#define sizeOfArray			 2048

union {
	uint16_t adc_values[sizeOfArray];
	float test[sizeOfArray];
	uint32_t data[sizeOfArray];
} samples;

void kurtogram();





/*
 * kurtogram.h
 *
 *
 *  Created on: 20. 3. 2018
 *      Author: Martina Buricova
 */


#include <stdint.h>
#include <string.h>

#define sizeOfArray			 2048


int index_i, index_j;
float index_m;
float rms;
float crest;
float kr;


//union for sampled data
union {
	uint16_t adc_values[2050];
	uint32_t data[sizeOfArray+2];
} samples;


void kurtogram();
void compute_crest();
void kurtosis_ratio();
void normalization();
uint32_t FloatToUint(float n);
float UintToFloat(uint32_t n);






#include "arm_math.h"
//#include "stm32l0xx_hal_flash.h"
#include "vcom.h"
#include "math.h"
#include <stdint.h>
#include <string.h>


#define nlevel 3
#define mocnina_dvou 8 		//2 na nlevel

extern int sizeOfArray;

union {
	uint16_t adc_values[2000];
	float flt[2000];
} samples;

float K[nlevel+1][mocnina_dvou];


void kurtogram()
{
	PRINTF("kurtogram\n\r");



	//float samples_imag[sizeOfArray];
	//odecteni prumeru
	/*for (int i=sizeOfArray-1; i >= 0; i--) {
		samples.flt[i] = (float)samples.adc_values[i];
	}

	float mean;
	arm_mean_f32(samples.flt, sizeOfArray, &mean);


	for (int i = 0; i < sizeOfArray; i++) {
		samples.flt[i] -= mean;
		//samples_imag[i] = 0;
	}*/

}

/*
void compute_crest()
{
	PRINTF("crest\n\r");
	for (int i=sizeOfArray-1; i >= 0; i--) {
		samples.flt[i] = (float)samples.adc_values[i];
	}

	float mean;
	arm_mean_f32(samples.flt, sizeOfArray, &mean);

	char buf[1000];
	sprintf(buf, "mean: %d\n\r", (int)mean);
	PRINTF(buf);


	for (int i = 0; i < sizeOfArray; i++) {
		samples.flt[i] -= mean;
	}

	char buf[1000];
	sprintf(buf, "hodnota: %u, mean: %d\n\r", samples.adc_values[1],  (int)mean);
	PRINTF(buf);
	//PRINTF("mean: %.6f\n\r", mean);


	//rms - efektivni hodnota

	float rms;
	arm_rms_f32(samples.flt, sizeOfArray, &rms);


	// absolute value of each element
	arm_abs_f32(samples.flt, samples.absolute, sizeOfArray);

	// peak
	float max;
	uint32_t indexMax;
	arm_max_f32(samples.absolute, sizeOfArray, &max, &indexMax);

	// crest
	float crest = max/rms;


}*/

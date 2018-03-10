
#include "arm_math.h"
#include "vcom.h"
#include "math.h"
//#include "timeServer.h"
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


float h_real[17] = {-0.0019,	 0.0022, 	0, 		 0, 	0.0409, 	0.0316, 	-0, 		 0.2067, 	 0.4003, 	0.2067, 	0, 			 0.0316, 	 0.0409, 	0, 	-0, 		0.0022, 	-0.0019};
float h_imag[17] = {0,			 0.0022, 	0.0108, -0, 	-0, 		0.0316,		-0.0810, 	-0.2067, 	-0, 		0.2067, 	0.0810, 	-0.0316,	-0,			0, 	-0.0108,	-0.0022, 	0};

float g_real[16] = {-0.0022, 	-0,	 		-0,		 0.0409, 	-0.0316,	0,			-0.2067,		 0.4003,	-0.2067,		-0,			-0.0316,	 0.0409,	0,	0,		-0.0022,	-0.0019};
float g_imag[16] = {0.0022,		-0.0108,	-0, 	-0,			 0.0136,	0.0810,		-0.2067,		-0,			 0.2067,		-0.0810,	-0.0316,	-0,		 	0,	0.0108,	-0.0022,	 0};


void DBFB(float *res_real, float *res_imag, float *x_real, float *x_imag, float *f_real, float *f_imag, int size);
void kurt_local(float *x_real, float *x_imag , int size, int level, int begin);





void kurtogram()
{
	PRINTF("kurtogram\n\r");

	//float samples_imag[sizeOfArray];
	//odecteni prumeru
	for (int i=sizeOfArray-1; i >= 0; i--) {
		samples.flt[i] = (float)samples.adc_values[i];
	}

	float mean;
	arm_mean_f32(samples.flt, sizeOfArray, &mean);


	for (int i = 0; i < sizeOfArray; i++) {
		samples.flt[i] -= mean;
		//samples_imag[i] = 0;
	}


	//K_wpq_local
	//kurt_local(samples.flt, samples_imag, sizeOfArray, nlevel, 0);

}
/*
int emptyArray(float *x, int size)
{
	for(int i=0;i<size;i++)
	{
		if(x[i] != 0)
		{
			return 0;
		}
	}
	return 1;
}


int kurtosis(float *x_real, float *x_imag, int size)
{
	int kurtosis_value = 0;
	if(emptyArray(x_real, size) == 1 && emptyArray(x_imag, size) == 1)
	{
		return 0;
	}

	float mean_real, mean_imag;
	arm_mean_f32(x_real, size, &mean_real);
	arm_mean_f32(x_imag, size, &mean_imag);

	for (int i = 0; i < size; i++) {
		x_real[i] -= mean_real;
	}
	for (int i = 0; i < size; i++) {
		x_imag[i] -= mean_imag;
	}
	int E = 0;
	for (int i = 0; i < size; i++)
	{
		E += (x_real[i]  *x_real[i] ) + (x_imag[i]  *x_imag[i] );
	}
	E = E/size;
	if (E < 0.00000001)
	{
		return 0;
	}
	for (int i = 0; i < size; i++)
	{
		kurtosis_value += ((x_real[i]  *x_real[i] ) + (x_imag[i]  *x_imag[i] )) * ((x_real[i]  *x_real[i] ) + (x_imag[i]  *x_imag[i] ));
	}
	kurtosis_value = (kurtosis_value/size)/(E*E);
	if (emptyArray(x_imag, size) == 1)
	{
		kurtosis_value = kurtosis_value - 3;
	}
	else
	{
		kurtosis_value = kurtosis_value - 2;
	}
	return kurtosis_value;
}

int row_index_for_first_level = 0;
void kurt_local(float *x_real, float *x_imag , int size, int level, int begin)
{

	float a_real[size/2];
	float a_imag[size/2];

	float d_real[size/2];
	float d_imag[size/2];

	float a_real_small[(size/2)-17];
	float a_imag_small[(size/2)-17];

	float d_real_small[(size/2)-16];
	float d_imag_small[(size/2)-16];

	DBFB(a_real, a_imag, x_real, x_imag, h_real, h_imag, size);
	DBFB(d_real, d_imag, x_real, x_imag, g_real, g_imag, size);

	for(int i=17;i<size/2;i++)
	{
		a_real_small[i] = a_real[i];
		a_imag_small[i] = a_imag[i];
	}

	for(int i=16;i<size/2;i++)
	{
		d_real_small[i] = d_real[i];
		d_imag_small[i] = d_imag[i];
	}


	float K1 = kurtosis(a_real_small, a_imag_small, (size/2)-17);
	float K2 = kurtosis(d_real_small, d_imag_small, (size/2)-16);

	if (level == 1)
	{
		K[nlevel][row_index_for_first_level] = K1;
		K[nlevel][row_index_for_first_level+1] = K2;
		row_index_for_first_level += 2;
	}

	if(level>1)
	{
		int copy = 1;
		for(int i = 0;i<level-1;i++)
		{
			copy = copy*2;
		}

		kurt_local(a_real, a_imag , size/2, level-1, begin);
		kurt_local(d_real, d_imag , size/2, level-1, begin+copy);

		for(int i=begin; i<copy; i++)
		{
			K[nlevel-level][i] = K1;
		}
		for(int i=begin+copy; i<copy; i++)
		{
			K[nlevel-level][i] = K2;
		}
	}

	if(level == nlevel)
	{
		K1 = kurtosis(x_real, x_imag, size);
		for(int i=0;i<mocnina_dvou;i++)
		{
			K[0][i] = K1;
		}

	}
}


void DBFB(float *res_real, float *res_imag, float *x_real, float *x_imag, float *f_real, float *f_imag, int size)
{
	float filtr_real[size];
	float filtr_imag[size];
	int index_start = -16, index_end = 0, i_s;
	int index_coef = 0;
	float result_real;
	float result_imag;
	while(index_end<size)
	{
		result_real = 0;
		result_imag = 0;
		if(index_start<0)
		{
			i_s = 0;
		}
		else
		{
			i_s = index_start;
		}
		index_coef = 0;
		for(int i=i_s;i<=index_end;i++)
		{
			result_real += f_real[index_coef]*x_real[i] - f_imag[index_coef]*x_imag[i];
			result_imag += f_real[index_coef]*x_imag[i] + f_imag[index_coef]*x_real[i];
			index_coef++;
		}
		filtr_real[index_end] = result_real;
		filtr_imag[index_end] = result_imag;
		index_start++;
		index_end++;
	}
	for(int i=0;i<size/2;i++)
	{
		res_real[i] = filtr_real[i*2-1];
		res_imag[i] = filtr_imag[i*2-1];
	}

}

int index_i, index_j;
float m;
void maxK()
{
	int maxI, maxJ;
	float maxM = 0;
	for(int i = 0; i < nlevel+1; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(K[i][j] > maxM)
			{
				maxM = K[i][j];
				maxI = i;
				maxJ = j;
			}
		}
	}
	index_i = maxI;
	index_j = maxJ;
	m = maxM;
}*/

/*
 * kurtogram.c
 *
 *  Created on: 20. 3. 2018
 *      Author: matula
 */

#include "stm32l0xx.h"
#include "stm32l0xx_nucleo.h"
#include "hw.h"
#include "arm_math.h"
#include "vcom.h"
#include "math.h"
#include "kurtogram.h"
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#define nlevel 3
#define mocnina_dvou 8 		//2 na nlevel

float K[nlevel+1][mocnina_dvou];
int row_index_for_first_level = 0;


float h_real[17] = {-0.001873, 0.002176, 0.000000, 0.000000, 0.040902, 0.031603, -0.000000, 0.206738, 0.400330, 0.206738, 0.000000, 0.031603, 0.040902, 0.000000, -0.000000, 0.002176, -0.001873};
float h_imag[17] = { 0.000000, 0.002176, 0.010843, -0.000000, -0.000000, 0.031603, -0.081012, -0.206738, -0.000000, 0.206738, 0.081012, -0.031603, -0.000000, 0.000000, -0.010843, -0.002176, 0.000000};

float g_real[16] = {-0.002176, -0.000000, -0.000000, 0.040902, -0.031603, 0.000000, -0.206738, 0.400330, -0.206738, -0.000000, -0.031603, 0.040902, -0.000000, 0.000000, -0.002176, -0.001873};
float g_imag[16] = {0.002176, -0.010843, -0.000000, -0.000000, 0.031603, 0.081012, -0.206738, -0.000000, 0.206738, -0.081012, -0.031603, -0.000000, 0.000000, 0.010843, -0.002176, 0.000000};


void DBFB(uint32_t *res_real, uint32_t *res_imag, uint32_t *x_real, uint32_t *x_imag, float *f_real, float *f_imag, int size, int size_filter);
void kurt_local(uint32_t *x_real, uint32_t *x_imag , int size, int level, int begin);



#define FLASH_START_imag_signal   		(FLASH_BASE + FLASH_PAGE_SIZE * 1024)
#define FLASH_END_imag_signal     		(FLASH_START_imag_signal + 8000)
#define FLASH_END_imag_signal_page      (FLASH_START_imag_signal + FLASH_PAGE_SIZE * 63)







uint32_t Address = 0, PAGEError = 0;
__IO uint32_t data32 = 0, MemoryProgramStatus = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

#define FLASH_USER_START_ADDR   (FLASH_BASE + FLASH_PAGE_SIZE * 1024)             /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     (FLASH_USER_START_ADDR + FLASH_PAGE_SIZE)   /* End @ of user Flash area */

float x = -88.123456 ;
uint32_t y;

uint32_t FloatToUint(float n)
{
   return (uint32_t)(*(uint32_t*)&n);
}

float UintToFloat(uint32_t n)
{
   return (float)(*(float*)&n);
}

void flash_test()
{
	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		PRINTF("flash error\n\r");
	}
	Address = FLASH_USER_START_ADDR;

	y = FloatToUint(x);
	while (Address < FLASH_USER_END_ADDR)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, y) == HAL_OK)
	    {
			Address = Address + 4;
	    }
		else
		{
			PRINTF("flash error\n\r");
		}
	}

	HAL_FLASH_Lock();

	uint32_t *p = FLASH_USER_START_ADDR;
	//PRINTF("pointer: %" PRIu32 "\n\r", p[0]);
	float p_float = UintToFloat(p[0]);
	PRINTF("pointer: %.6f \n\r", p_float);

	/*PRINTF("pointer: %" PRIu32 "\n\r", p[0]);
	PRINTF("pointer: %" PRIu32 "\n\r", p[1]);
	PRINTF("pointer: %" PRIu32 "\n\r", p[2]);*
	/*Address = FLASH_USER_START_ADDR;

	while (Address < FLASH_USER_END_ADDR)
	{
		data32 = *(__IO uint32_t *)Address;

		if (data32 != DATA_32)
		{
		   MemoryProgramStatus++;
		}
	    Address = Address + 4;
	}

	if (MemoryProgramStatus == 0)
	{
		PRINTF("No Error detected\n\r");
	}
	else
	{
		PRINTF("flash error\n\r");
	}*/


}




void kurtogram()
{
	PRINTF("kurtogram\n\r");

	//flash_test();
	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_START_imag_signal;
	EraseInitStruct.NbPages     = (FLASH_END_imag_signal_page - FLASH_START_imag_signal) / FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		PRINTF("flash error\n\r");
	}
	Address = FLASH_START_imag_signal;

	while (Address < FLASH_END_imag_signal)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, (uint32_t)0) == HAL_OK)
	    {
			Address = Address + 4;
	    }
		else
		{
			PRINTF("flash error\n\r");
		}
	}

	HAL_FLASH_Lock();



	uint32_t *samples_imag = FLASH_START_imag_signal;
	//odecteni prumeru
	for (int i=sizeOfArray-1; i >= 0; i--) {
		samples.flt[i] = (float)samples.adc_values[i];
	}

	float mean = 0;
	arm_mean_f32(samples.flt, sizeOfArray, &mean);

	for (int i = 0; i < sizeOfArray; i++) {
		samples.flt[i] -= mean;
	}

	for (int i=sizeOfArray-1; i >= 0; i--) {
		samples.u[i] = FloatToUint(samples.flt[i]);
	}


	//K_wpq_local
	kurt_local(samples.u, samples_imag, sizeOfArray, nlevel, 0);


}


float mean(float *x, int size)
{
	float result = 0;
	for (int i=0; i < size; i++) {
			result += x[i];
	}
	return result/size;
}


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


float kurtosis(float *x_real, float *x_imag, int size)
{
	float kurtosis_value = 0;

	if(emptyArray(x_real, size) == 1 && emptyArray(x_imag, size) == 1)
	{
		return 0;
	}
	float mean_real;
	float mean_imag;
	arm_mean_f32(x_real, size, &mean_real);
	arm_mean_f32(x_imag, size, &mean_imag);

	for (int i = 0; i < size; i++) {
		x_real[i] -= mean_real;
	}
	for (int i = 0; i < size; i++) {
		x_imag[i] -= mean_imag;
	}

	float E = 0;
	for (int i = 0; i < size; i++)
	{
		E += (x_real[i]*x_real[i] ) + (x_imag[i]*x_imag[i] );
	}
	E = E/size;
	//printf("E = %f\n", E);
	if (E < 0.00000001)
	{
		return 0;
	}

	for (int i = 0; i < size; i++)
	{
		kurtosis_value += ((x_real[i]*x_real[i] ) + (x_imag[i]*x_imag[i] )) * ((x_real[i]*x_real[i] ) + (x_imag[i]*x_imag[i] ));
	}
	kurtosis_value = (kurtosis_value/size)/(E*E);
	//printf("K = %f\n", kurtosis_value);
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



void kurt_local(uint32_t *x_real, uint32_t *x_imag , int size, int level, int begin)
{

	uint32_t *a_real;
	uint32_t *a_imag;

	uint32_t *d_real;
	uint32_t *d_imag;


	DBFB(a_real, a_imag, x_real, x_imag, h_real, h_imag, size, 17);
	DBFB(d_real, d_imag, x_real, x_imag, g_real, g_imag, size, 16);

	for(int i=0;i<size/2;i=i+2)
	{
		d_real[i] = -d_real[i];
		d_imag[i] = -d_imag[i];
	}

	/*float K1 = kurtosis(&a_real[16], &a_imag[16], (size/2)-16);
	float K2 = kurtosis(&d_real[15], &d_imag[15], (size/2)-15);
	//printf("k1 = %f\n", K1);
	//printf("k2 = %f\n", K2);


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

		for(int i=begin; i<begin+copy; i++)
		{
			K[nlevel-level+1][i] = K1;
		}
		for(int i=begin+copy; i<begin+2*copy; i++)
		{
			K[nlevel-level+1][i] = K2;
		}
	}

	if(level == nlevel)
	{
		K1 = kurtosis(x_real, x_imag, size);
		for(int i=0;i<mocnina_dvou;i++)
		{
			K[0][i] = K1;
		}

	}*/
}


void DBFB(uint32_t *res_real, uint32_t *res_imag, uint32_t *x_real, uint32_t *x_imag, float *f_real, float *f_imag, int size, int size_filter)
{
	int index_start = -size_filter+1, index_end = 0, i_s;
	int index_coef = 0;
	int index_array = 0;
	int save = 0;
	float result_real;
	float result_imag;
	while(index_end<size)
	{
		if(save == 1)
		{
			result_real = 0;
			result_imag = 0;
			index_coef = 0;

			if(index_start<0)
			{
				i_s = 0;
			}
			else
			{
				i_s = index_start;
			}
			for(int i=index_end;i>=i_s;i--)
			{
				result_real += f_real[index_coef]*x_real[i] - f_imag[index_coef]*x_imag[i];
				result_imag += f_real[index_coef]*x_imag[i] + f_imag[index_coef]*x_real[i];
				index_coef++;
			}
			res_real[index_array] = result_real;
			res_imag[index_array] = result_imag;
			index_array++;
			save = 0;
		}
		else if(save == 0)
		{
			save = 1;
		}

		index_start++;
		index_end++;
	}
}

int index_i, index_j;
float index_m;
void maxK()
{
	int maxI, maxJ;
	float maxM = 0;
	for(int i = 0; i < nlevel+1; i++)
	{
		for(int j = 0; j < mocnina_dvou; j++)
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
	index_m = maxM;
}


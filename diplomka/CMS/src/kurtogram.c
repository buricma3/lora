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
#include <stdint.h>
#include <string.h>


#define nlevel 3
#define mocnina_dvou 8 		//2 na nlevel


float K[nlevel+1][mocnina_dvou];

#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 128 bytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000080) /* Base @ of Page 1, 128 bytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08000100) /* Base @ of Page 2, 128 bytes */
/* ... */
#define ADDR_FLASH_PAGE_1533  ((uint32_t)0x0802FE80) /* Base @ of Page 1533, 128 bytes */
#define ADDR_FLASH_PAGE_1534  ((uint32_t)0x0802FF00) /* Base @ of Page 1534, 128 bytes */
#define ADDR_FLASH_PAGE_1535  ((uint32_t)0x0802FF80) /* Base @ of Page 1535, 128 bytes */
/*
#define FLASH_USER_START_ADDR   (FLASH_BASE + FLASH_PAGE_SIZE * 256)
#define FLASH_USER_END_ADDR     (FLASH_USER_START_ADDR + FLASH_PAGE_SIZE * 10)

#define DATA_32                 ((uint32_t)0x12345678)


uint32_t Address = 0, PAGEError = 0;
__IO uint32_t data32 = 0, MemoryProgramStatus = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;*/
void kurtogram()
{
	PRINTF("kurtogram\n\r");


	/*HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		Error_Handler();
	}
	Address = FLASH_USER_START_ADDR;

	while (Address < FLASH_USER_END_ADDR)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
	    {
			Address = Address + 4;
	    }
		else
		{
			Error_Handler();
		}
	}

	HAL_FLASH_Lock();*/

		//Address = FLASH_USER_START_ADDR;

		/*while (Address < FLASH_USER_END_ADDR)
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
		   // BSP_LED_On(LED2);
			PRINTF("No Error detected");
		  }
		  else
		  {
			  Error_Handler();
		  }*/

		/*char buf[1000];
		sprintf(buf, "flash: %d\n\r", (int)data32);
		PRINTF(buf);*/


	//float samples_imag[sizeOfArray];
	//odecteni prumeru
	/*for (int i=sizeOfArray-1; i >= 0; i--) {
		samples.flt[i] = (float)samples.adc_values[i];
	}

	float mean = 0;
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


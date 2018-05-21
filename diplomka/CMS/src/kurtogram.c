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
#include <stdio.h>

#define nlevel 4
#define mocnina_dvou 16 		//2 na nlevel

float K[nlevel+1][mocnina_dvou];
int row_index_for_first_level = 0;


const float h_real[17] = {-0.001873, 0.002176, 0.000000, 0.000000, 0.040902, 0.031603, -0.000000, 0.206738, 0.400330, 0.206738, 0.000000, 0.031603, 0.040902, 0.000000, -0.000000, 0.002176, -0.001873};
const float h_imag[17] = { 0.000000, 0.002176, 0.010843, -0.000000, -0.000000, 0.031603, -0.081012, -0.206738, -0.000000, 0.206738, 0.081012, -0.031603, -0.000000, 0.000000, -0.010843, -0.002176, 0.000000};

const float g_real[16] = {-0.002176, -0.000000, -0.000000, 0.040902, -0.031603, 0.000000, -0.206738, 0.400330, -0.206738, -0.000000, -0.031603, 0.040902, -0.000000, 0.000000, -0.002176, -0.001873};
const float g_imag[16] = {0.002176, -0.010843, -0.000000, -0.000000, 0.031603, 0.081012, -0.206738, -0.000000, 0.206738, -0.081012, -0.031603, -0.000000, 0.000000, 0.010843, -0.002176, 0.000000};


void DBFB(uint32_t *res_real, uint32_t *res_imag, uint32_t *x_real, uint32_t *x_imag, const float *f_real, const float *f_imag, int size, int size_filter, int first);
void kurt_local(uint32_t *x_real, uint32_t *x_imag , int size, int level, int begin, int first);
void insertSort(uint32_t *array, int left, int right);
void quicksort(uint32_t *A, int l, int h);
void shakerSort(uint32_t * array, int size);
float kurtosis(uint32_t *x_real, uint32_t *x_imag, int size, int first);
void maxK();

const uint16_t testovaciDATA [] = {2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2074, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2075, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2074, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2071, 2074, 2073, 2072, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2072, 2072, 2073, 2071, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2071, 2072, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2074, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2071, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2072, 2072, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2071, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2071, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2074, 2073, 2072, 2073, 2073, 2073, 2075, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2074, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2071, 2073, 2073, 2073, 2073, 2074, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2074, 2072, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2071, 2073, 2073, 2072, 2071, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2072, 2072, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2075, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2072, 2074, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2074, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2074, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2074, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2072, 2072, 2074, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2072, 2071, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2072, 2072, 2072, 2073, 2073, 2071, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2072, 2072, 2072, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2075, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2072, 2072, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2071, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2071, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2073, 2072, 2072, 2073, 2074, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2074, 2072, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2074, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2071, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2072, 2073, 2072, 2072, 2072, 2072, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2074, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2070, 2072, 2072, 2074, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2074, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2072, 2073, 2072, 2072, 2072, 2073, 2072, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2074, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2072, 2072, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2071, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2072, 2072, 2073, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2073, 2074, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2072, 2073, 2072, 2073, 2073, 2073, 2072, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2071, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2072, 2072, 2073, 2072, 2072, 2072, 2072, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2072, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2072, 2071, 2072, 2073, 2073, 2072, 2073, 2073, 2073, 2073, 2072, 2072, 2073, 2072, 2073, 2073, 2073, 2073, 2073, 2073, 2073, 2072, 2073, 2073, 2073, 2072, 2072};

float kurtosisOfSignal;


uint32_t flash_address(int level, int begin, int what)
{
	uint32_t start = 1040 + (4-level)*128;

	int count = 1;
	for(int i=0;i<level+1;i++)
	{
		count = count * 2;
	}

	if(level > 1)
	{
		begin = (begin/2)*(4-level);
	}
	else
	{
		begin = (begin*2);
	}

	start += begin*count + count*what;

	//preteceni pole
	if(start > 1516)
	{
		begin = begin - 24;
		start = 1040 + count*what + begin*count;
	}

	return FLASH_BASE + FLASH_PAGE_SIZE * start;
}


uint32_t FloatToUint(float n)
{
   return (uint32_t)(*(uint32_t*)&n);
}

float UintToFloat(uint32_t n)
{
   return (float)(*(float*)&n);
}

/*void testData()
{
	int i =0;

	while( i<sizeOfArray )
	{
		samples.test[i] = testovaciDATA[i];
		i++;
	}

}*/


float mean(uint32_t *x, int size)
{
	float result = 0;
	float value;
	for (int i=0; i < size; i++)
	{
		value = UintToFloat(x[i]);
		result += value;
	}
	return result/size;
}

void normalization()
{
	/*for(int i=0;i<sizeOfArray+2;i++)
	{
		PRINTF("%"PRIu16", ", samples.adc_values[i]);
	}*/

	//testData();
	for (int i=sizeOfArray+1; i >= 0; i--) {
		samples.data[i] = FloatToUint(samples.adc_values[i]);//FloatToUint(samples.test[i]);//;
	}

	for(int i=0;i<sizeOfArray+2;i++)
	{
		samples.data[i] = samples.data[i+2];
	}

	/*for(int i=0;i<sizeOfArray;i++)
	{
		PRINTF("%f, ", UintToFloat(samples.data[i]));
	}*/

	float m = mean(samples.data, sizeOfArray);
	PRINTF("mean: %.6f \n\r", m);

	//normalizace
	for (int i = 0; i < sizeOfArray; i++) {
		samples.data[i] = FloatToUint(UintToFloat(samples.data[i]) - m);
	}
}

void kurtogram()
{
	//PRINTF("kurtogram\n\r");

	//inicializace
	row_index_for_first_level = 0;

	for(int i=0;i<nlevel+1;i++)
	{
		for(int j=0;j<mocnina_dvou;j++)
		{
			K[i][j] = 0;
		}
	}


	uint32_t *samples_imag = 0;

	//K_wpq_local
	kurt_local(samples.data, samples_imag, sizeOfArray, nlevel, 0, 1);

	//printf
	/*for(int i=0;i<nlevel+1;i++)
	{
		for(int j=0;j<mocnina_dvou;j++)
		{
			PRINTF("%.6f ", K[i][j]);
		}
		PRINTF("\n\r");
	}*/

	maxK();
	//PRINTF("\n\r");
	PRINTF("%d \n\r", index_i);
	PRINTF("%d \n\r", index_j);
	PRINTF("%.6f \n\r", index_m);

}



void compute_crest()
{
	arm_rms_f32(samples.data, sizeOfArray, &rms);

	PRINTF("rms: %.6f \n\r", rms);

	// peak
	float max;
	uint32_t indexMax;

	float min;
	uint32_t indexMin;
	arm_max_f32(samples.data, sizeOfArray, &max, &indexMax);
	arm_min_f32(samples.data, sizeOfArray, &min, &indexMin);
	float peak;

	if(max < 0){
		max = -max;
	}

	if(min < 0){
		min = -min;
	}

	if(max>min)
	{
		peak = max;
	}
	else
	{
		peak = min;
	}

	PRINTF("peak: %.6f \n\r", peak);
	// crest
	crest = peak/rms;
	PRINTF("crest: %.6f \n\r", crest);

}


void kurtosis_ratio()
{
	shakerSort(samples.data, sizeOfArray);
	uint32_t *samples_imag = 0;
	float kurtosisOfTrimmedSignal = kurtosis(&samples.data[102], samples_imag, 1843, 1) +3;
	PRINTF("trimmed: %.6f \n\r", kurtosisOfTrimmedSignal);
	kurtosisOfSignal = kurtosis(&samples.data[0], samples_imag, sizeOfArray, 1) +3;
	PRINTF("raw: %.6f \n\r", kurtosisOfSignal);
	kr = kurtosisOfSignal/kurtosisOfTrimmedSignal;
	PRINTF("kr: %.6f \n\r", kr);
}

void shakerSort(uint32_t * array, int size) {
    for (int i = 0; i < 103; i++) {
        bool swapped = false;
        for (int j = i; j < size - i - 1; j++) {
            if (UintToFloat(array[j]) < UintToFloat(array[j+1])) {
            	uint32_t tmp = array[j];
                array[j] = array[j+1];
                array[j+1] = tmp;
                swapped = true;
            }
        }
        for (int j = size - 2 - i; j > i; j--) {
            if (UintToFloat(array[j]) > UintToFloat(array[j-1])) {
            	uint32_t tmp = array[j];
                array[j] = array[j-1];
                array[j-1] = tmp;
                swapped = true;
            }
        }
        if(!swapped) break;
    }
}


int emptyArray(uint32_t *x, int size)
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


float kurtosis(uint32_t *x_real, uint32_t *x_imag, int size, int first)
{
	float kurtosis_value = 0;
	float x_imag_temp;
	float x_real_temp;

	if(first == 0)
	{
		if(emptyArray(x_real, size) == 1 &&  emptyArray(x_imag, size) == 1)
		{
			return 0;
		}
	}
	float mean_real = mean(x_real, size);
	float mean_imag = 0;

	if(first == 0)
	{
		mean_imag = mean(x_imag, size);
	}


	float E = 0;
	for (int i = 0; i < size; i++)
	{
		if(first)
		{
			x_imag_temp = 0;
		}
		else
		{
			x_imag_temp = UintToFloat(x_imag[i]) - mean_imag;
		}
		x_real_temp = UintToFloat(x_real[i]) - mean_real;
		E += (x_real_temp*x_real_temp ) + (x_imag_temp*x_imag_temp );
	}
	E = E/size;
	//PRINTF("E = %f  \n\r", E);
	if (E < 0.00000001)
	{
		return 0;
	}

	for (int i = 0; i < size; i++)
	{
		if(first)
		{
			x_imag_temp = 0;
		}
		else
		{
			x_imag_temp = UintToFloat(x_imag[i]) - mean_imag;
		}
		x_real_temp = UintToFloat(x_real[i]) - mean_real;
		kurtosis_value += ((x_real_temp*x_real_temp ) + (x_imag_temp*x_imag_temp )) * ((x_real_temp*x_real_temp ) + (x_imag_temp*x_imag_temp ));
	}

	kurtosis_value = (kurtosis_value/size)/(E*E);
	//PRINTF("K = %f  \n\r", kurtosis_value);

	if (first == 1 || emptyArray(x_imag, size) == 1)
	{
		kurtosis_value = kurtosis_value - 3;
	}
	else
	{
		kurtosis_value = kurtosis_value - 2;
	}
	return kurtosis_value;
}



void kurt_local(uint32_t *x_real, uint32_t *x_imag , int size, int level, int begin, int first)
{
	uint32_t *a_real;
	uint32_t *a_imag;

	uint32_t *d_real;
	uint32_t *d_imag;

	a_real = flash_address(level, begin, 0);
	a_imag = flash_address(level, begin, 1);
	d_real = flash_address(level, begin, 2);
	d_imag = flash_address(level, begin, 3);


	//PRINTF("----------------------------level = %d\n\r", level);
	DBFB(a_real, a_imag, x_real, x_imag, h_real, h_imag, size, 17, first);
	DBFB(d_real, d_imag, x_real, x_imag, g_real, g_imag, size, 16, first);



	float K1 = kurtosis(&a_real[16], &a_imag[16], (size/2)-16, 0);
	float K2 = kurtosis(&d_real[15], &d_imag[15], (size/2)-15, 0);
	//PRINTF("k1 = %f\n\r", K1);
	//PRINTF("k2 = %f\n\r", K2);

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

		kurt_local(a_real, a_imag , size/2, level-1, begin, 0);
		kurt_local(d_real, d_imag , size/2, level-1, begin+copy, 0);

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
		K1 = kurtosis(x_real, x_imag, size, 1);
		kurtosisOfSignal = K1;

		for(int i=0;i<mocnina_dvou;i++)
		{
			K[0][i] = K1;
		}

	}
}


static FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t Address_real = 0, PAGEError = 0, Address_imag = 0;

void DBFB(uint32_t *res_real, uint32_t *res_imag, uint32_t *x_real, uint32_t *x_imag, const float *f_real, const float *f_imag, int size, int size_filter, int first)
{
	int index_start = -size_filter+1, index_end = 0, i_s;
	int index_coef = 0;
	int index_array = 0;
	int save = 0;
	float result_real;
	float result_imag;
	float x_imag_temp, x_real_temp;

	//erase flash memory
	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = res_real;
	EraseInitStruct.NbPages     = ((size*2) / FLASH_PAGE_SIZE)+1; // (size/2)*4 byte

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		PRINTF("flash error Erase real\n\r");
	}
	Address_real = res_real;

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = res_imag;
	EraseInitStruct.NbPages     = ((size*2) / FLASH_PAGE_SIZE)+1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		PRINTF("flash error Erase imag\n\r");
	}
	Address_imag = res_imag;

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
				if(first == 1)
				{
					x_imag_temp = 0;
				}
				else
				{
					x_imag_temp = UintToFloat(x_imag[i]);
				}
				x_real_temp = UintToFloat(x_real[i]);

				result_real += f_real[index_coef]*x_real_temp - f_imag[index_coef]*x_imag_temp;
				result_imag += f_real[index_coef]*x_imag_temp + f_imag[index_coef]*x_real_temp;
				index_coef++;
			}
			if(size_filter == 16 && ((index_array == 0)||index_array % 2 == 0))
			{
				result_real = -result_real;
				result_imag = -result_imag;
			}

			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_real, FloatToUint(result_real)) == HAL_OK)
			{
				Address_real = Address_real + 4;
			}
			else
			{
				PRINTF("flash error program real\n\r");
			}

			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_imag, FloatToUint(result_imag)) == HAL_OK)
			{
				Address_imag = Address_imag + 4;
			}
			else
			{
				PRINTF("flash error program imag\n\r");
			}

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

	HAL_FLASH_Lock();
}




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





#include <stdio.h>
#include <stdint.h>
#include "math.h"
//#include "timeServer.h"
#include <stdint.h>
#include <string.h>



#define nlevel 3
#define mocnina_dvou 8		//2 na nlevel

#define sizeOfArray  3423

union {
	double adc_values[sizeOfArray];
	double flt[sizeOfArray];
} samples;

double K[nlevel+1][mocnina_dvou];


//double h_real[17] = {-0.0019,	 0.0022, 	0, 		 0, 	0.0409, 	0.0316, 	-0, 		 0.2067, 	 0.4003, 	0.2067, 	0, 			 0.0316, 	 0.0409, 	0, 	-0, 		0.0022, 	-0.0019};
//double h_imag[17] = {0,			 0.0022, 	0.0108, -0, 	-0, 		0.0316,		-0.0810, 	-0.2067, 	-0, 		0.2067, 	0.0810, 	-0.0316,	-0,			0, 	-0.0108,	-0.0022, 	0};
double h_real[17] = {-0.001873, 0.002176, 0.000000, 0.000000, 0.040902, 0.031603, -0.000000, 0.206738, 0.400330, 0.206738, 0.000000, 0.031603, 0.040902, 0.000000, -0.000000, 0.002176, -0.001873};
double h_imag[17] = { 0.000000, 0.002176, 0.010843, -0.000000, -0.000000, 0.031603, -0.081012, -0.206738, -0.000000, 0.206738, 0.081012, -0.031603, -0.000000, 0.000000, -0.010843, -0.002176, 0.000000};

//double g_real[16] = {-0.0022, 	-0,	 		0,		 0.0409, 	-0.0316,	0,			-0.2067,		 0.4003,	-0.2067,		-0,			-0.0316,	 0.0409,	0,	0,		-0.0022,	-0.0019};
//double g_imag[16] = {0.0022,		-0.0108,	0, 		-0,			 0.0316,	0.0810,		-0.2067,		-0,			 0.2067,		-0.0810,	-0.0316,	-0,		 	0,	0.0108,	-0.0022,	 0};

double g_real[16] = {-0.002176, -0.000000, -0.000000, 0.040902, -0.031603, 0.000000, -0.206738, 0.400330, -0.206738, -0.000000, -0.031603, 0.040902, -0.000000, 0.000000, -0.002176, -0.001873};
double g_imag[16] = {0.002176, -0.010843, -0.000000, -0.000000, 0.031603, 0.081012, -0.206738, -0.000000, 0.206738, -0.081012, -0.031603, -0.000000, 0.000000, 0.010843, -0.002176, 0.000000};


void DBFB(double *res_real, double *res_imag, double *x_real, double *x_imag, double *f_real, double *f_imag, int size, int size_filter);
void kurt_local(double *x_real, double *x_imag , int size, int level, int begin);


double mean(double *x, int size)
{
	double result = 0;
	for (int i=0; i < size; i++) {
			result += x[i];
	}
	return result/sizeOfArray;
}


void kurtogram()
{

	double samples_imag[sizeOfArray];
	//odecteni prumeru
	for (int i=sizeOfArray-1; i >= 0; i--) {
		samples.flt[i] = (double)samples.adc_values[i];
	}

	double m = mean(samples.flt, sizeOfArray);

	//arm_mean_f32(samples.flt, sizeOfArray, &mean);
	//printf("mean = %f\n", m);

	for (int i = 0; i < sizeOfArray; i++) {
		samples.flt[i] -= m;
		samples_imag[i] = 0;
	}


	//K_wpq_local
	kurt_local(samples.flt, samples_imag, sizeOfArray, nlevel, 0);

	for(int i=0;i<nlevel+1;i++)
	{
		for(int j=0;j<mocnina_dvou;j++)
		{
			printf("%f ", K[i][j]);
		}
		printf("\n ");
	}

}

int emptyArray(double *x, int size)
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


double kurtosis(double *x_real, double *x_imag, int size)
{
	double kurtosis_value = 0;
	if(emptyArray(x_real, size) == 1 && emptyArray(x_imag, size) == 1)
	{
		return 0;
	}

	//double mean_real, mean_imag;
	double mean_real = mean(x_real, size);
	double mean_imag = mean(x_imag, size);
	//arm_mean_f32(x_real, size, &mean_real);
	//arm_mean_f32(x_imag, size, &mean_imag);

	for (int i = 0; i < size; i++) {
		x_real[i] -= mean_real;
	}
	for (int i = 0; i < size; i++) {
		x_imag[i] -= mean_imag;
	}


	double E = 0;
	for (int i = 0; i < size; i++)
	{
		E += (x_real[i]*x_real[i] ) + (x_imag[i]*x_imag[i] );
	}
	E = E/size;
	printf("E = %f\n", E);
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

int row_index_for_first_level = 0;
void kurt_local(double *x_real, double *x_imag , int size, int level, int begin)
{

	double a_real[size/2];
	double a_imag[size/2];

	double d_real[size/2];
	double d_imag[size/2];

	double a_real_small[(size/2)-17];
	double a_imag_small[(size/2)-17];

	double d_real_small[(size/2)-16];
	double d_imag_small[(size/2)-16];


	DBFB(a_real, a_imag,x_real, x_imag, h_real, h_imag, size, 17);
	DBFB(d_real, d_imag, x_real, x_imag, g_real, g_imag, size, 16);



	for(int i=0;i<size/2;i=i+2)
	{
		d_real[i] = -d_real[i];
		d_imag[i] = -d_imag[i];
	}


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



	double K1 = kurtosis(a_real_small, a_imag_small, (size/2)-17);
	double K2 = kurtosis(d_real_small, d_imag_small, (size/2)-16);
	printf("k1 = %f\n", K1);
	printf("k2 = %f\n", K2);


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

	}
}


void DBFB(double *res_real, double *res_imag, double *x_real, double *x_imag, double *f_real, double *f_imag, int size, int size_filter)
{
	double filtr_real[size];
	double filtr_imag[size];
	int index_start = -size_filter+1, index_end = 0, i_s;
	int index_coef = 0;
	double result_real;
	double result_imag;
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
		for(int i=index_end;i>=i_s;i--)
		{
			//result_real += f_real[index_coef]*x_real[i];
			result_real += f_real[index_coef]*x_real[i] - f_imag[index_coef]*x_imag[i];
			result_imag += f_real[index_coef]*x_imag[i] + f_imag[index_coef]*x_real[i];
			//printf("real res = %f filter = %f x = %f \n", result_real, f_real[index_coef], x_real[i]);
			//printf("imag res = %f filter = %f x = %f \n", result_imag, f_imag[index_coef], x_imag[i]);
			index_coef++;
		}

		//printf(" \n\n");
		filtr_real[index_end] = result_real;
		filtr_imag[index_end] = result_imag;
		index_start++;
		index_end++;
	}
	/*for(int i=0;i<20;i++)
	{
		printf("real = %f ", filtr_real[i]);
		printf("imag = %f \n", filtr_imag[i]);
	}*/
	for(int i=1;i<=size/2;i++)
	{
		res_real[i-1] = filtr_real[i*2-1];
		res_imag[i-1] = filtr_imag[i*2-1];
	}

	/*for(int i=0;i<20;i++)
	{
		printf("real = %f ", res_real[i]);
		printf("imag = %f \n", res_imag[i]);
	}*/
}

int index_i, index_j;
double index_m;
void maxK()
{
	int maxI, maxJ;
	double maxM = 0;
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


int main()
{
   printf("kurtogram start\n");

   FILE* f = fopen("data3.txt", "r");
   double number = 0;
   int i =0;

   while( fscanf(f, "%lf", &number) > 0 ) // parse %d followed by ','
   {
	   samples.adc_values[i] = number;
	   i++;
   }
   fclose(f);
   kurtogram();
   maxK();
   printf("i = %d\n", index_i);
   printf("j = %d\n", index_j);
   printf("k = %f\n", index_m);

   return 0;
}

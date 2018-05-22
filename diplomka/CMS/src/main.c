/*
 * kurtogram.c
 *
 * Main file for Condition monitoring system.
 *
 *
 *  Created on: 17. 1. 2018
 *      Author: Martina Buricova
 */


#include "stm32l0xx.h"
#include "stm32l0xx_nucleo.h"
#include "hw.h"
#include "low_power.h"
#include "lora.h"
#include "timeServer.h"
#include "vcom.h"
#include "arm_math.h"
#include "kurtogram.h"
#include <stdint.h>
#include <inttypes.h>
#include <string.h>


static TimerEvent_t              MeasurementStartTimer;
ADC_HandleTypeDef 				 hadc;
extern DMA_HandleTypeDef         DmaHandle;
extern TIM_HandleTypeDef 		 htim2;

volatile bool dma_done = false;

#define MEAS_INTERVAL_MS							( 1*10*1000) // 10 seconds

// constants for LoRa interface

#define LPP_APP_PORT 99
/*!
 * Defines the application data transmission duty cycle. 5 min, value in [ms].
 */
#define APP_TX_DUTYCYCLE                           5 * 60 * 1000
/*!
 * LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                              1
/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG                    DISABLE
/*!
 * LoRaWAN application port
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_APP_PORT                            2
/*!
 * Number of trials for the join request.
 */
#define JOINREQ_NBTRIALS                            3


static void MX_ADC_Init(void);
/* call back when LoRa will transmit a frame*/
static void LoraTxData( lora_AppData_t *AppData, FunctionalState* IsTxConfirmed);
/* call back when LoRa has received a frame*/
static void LoraRxData( lora_AppData_t *AppData);

/* load call backs*/
static LoRaMainCallback_t LoRaMainCallbacks ={ HW_GetBatteryLevel,
                                               HW_GetUniqueId,
                                               HW_GetRandomSeed,
                                               LoraTxData,
                                               LoraRxData};


/*
 *  Initialises the Lora Parameters
 */
static  LoRaParam_t LoRaParamInit= {TX_ON_TIMER,
									APP_TX_DUTYCYCLE,
                                    CLASS_A,
                                    LORAWAN_ADR_ON,
                                    DR_0,
                                    LORAWAN_PUBLIC_NETWORK,
                                    JOINREQ_NBTRIALS};



/*
 *  This function is called when DMA transmission is complete
 *
 *  inputs:
 *          dh: DMA handler
 *
 */
static void done_cb(DMA_HandleTypeDef* dh)
{
	dma_done = true;
}


/*
 *  This function is called on timer interrupt.
 *  It starts measuring and calculates sign
 *
 */
static void MeasurementStartTimerIrq(void)
{
	GPIOC->BSRR = 1<<7;
	dma_done = false;
	PRINTF("measure\n\r");
	HAL_Delay(200);

	// start DMA transfer
	if (HAL_ADC_Start_DMA(&hadc, (void*)samples.adc_values, 2050) != HAL_OK)
	{
		Error_Handler();
		PRINTF("Error DMA\n\r");
	}
	DmaHandle.XferCpltCallback = done_cb;


	while(!dma_done) {
		HAL_Delay(100); // waiting until DMA transmission is complete
	}

	// signal processing
	normalization();
	kurtogram();
	compute_crest();
	kurtosis_ratio();

	PRINTF("measure done \n\r\n\r");
	GPIOC->BRR = 1<<7;
}


int main(void)
{
    // inisialiyation of monitoring system
	HAL_Init( );
	SystemClock_Config( );
	HW_Init( );
	MX_ADC_Init();
	lora_Init( &LoRaMainCallbacks, &LoRaParamInit); // Configure the Lora Stack

	// first measuring at start
	MeasurementStartTimerIrq();

    // set timer for measure
	TimerInit( &MeasurementStartTimer, MeasurementStartTimerIrq );
	TimerSetValue( &MeasurementStartTimer, MEAS_INTERVAL_MS );
	TimerStart( &MeasurementStartTimer );


	PRINTF("START\n\r");

	// infinite loop
	while( 1 )
	{
	    // LoRa class A state machine
	    lora_fsm( );

	    DISABLE_IRQ( );
	    // if an interrupt has occurred after DISABLE_IRQ, it is kept pending
	    // and cortex will not enter low power anyway
	    if ( lora_getDeviceState( ) == DEVICE_STATE_SLEEP )
	    {
	#ifndef LOW_POWER_DISABLE
	      LowPower_Handler( );
	#endif
	    }
	    ENABLE_IRQ();
	  }
}


/*
 *  Configuration of Analog to digital converter.
 */
static void MX_ADC_Init(void)
{

	hadc.Instance = ADC1;
	hadc.Init.OversamplingMode      = DISABLE;

	hadc.Init.LowPowerAutoPowerOff  = DISABLE;
	hadc.Init.LowPowerFrequencyMode = DISABLE;
	hadc.Init.LowPowerAutoWait      = DISABLE;

	hadc.Init.Resolution            = ADC_RESOLUTION_12B;
	hadc.Init.SamplingTime    		= ADC_SAMPLETIME_79CYCLES_5;
	hadc.Init.ClockPrescaler   		= ADC_CLOCK_SYNC_PCLK_DIV1;
	hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;
	hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	hadc.Init.ContinuousConvMode    = ENABLE; //ENABLE
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
	hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
	hadc.Init.DMAContinuousRequests = DISABLE; //ENABLE;
	hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
	hadc.Init.OversamplingMode      = DISABLE;


	// Initialize ADC peripheral according to the passed parameters
	if (HAL_ADC_Init(&hadc) != HAL_OK)
	{
		Error_Handler();
	}


	// Start calibration
	if (HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) !=  HAL_OK)
	{
	    Error_Handler();
	}

	// Channel configuration
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel      = ADC_CHANNEL_1;               // Channel to be converted
	sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	{
	    Error_Handler();
	}
}


/*
 *  Prepare data frame for transfer over LoRa interface
 *
 *  inputs:
 *           AppData: structure for sending
 *           IsTxConfirmed:
 */
static void LoraTxData( lora_AppData_t *AppData, FunctionalState* IsTxConfirmed)
{
	uint32_t i = 0;

	// reset timer
    TimerSetValue( &MeasurementStartTimer, MEAS_INTERVAL_MS );
    TimerReset(&MeasurementStartTimer);

    AppData->Port = LORAWAN_APP_PORT;

    *IsTxConfirmed =  LORAWAN_CONFIRMED_MSG;

    // change type computes value from float to uint32
    uint32_t maxKurt = FloatToUint(index_m);
    uint32_t rms_send = FloatToUint(rms);
    uint32_t crest_send = FloatToUint(crest);
    uint32_t kr_send = FloatToUint(kr);

    // set bytes in buffer
    AppData->Buff[i++] = index_i;
    AppData->Buff[i++] = index_j;
    AppData->Buff[i++] = maxKurt;
    AppData->Buff[i++] = maxKurt>>8;
    AppData->Buff[i++] = maxKurt>>16;
    AppData->Buff[i++] = maxKurt>>24;
    AppData->Buff[i++] = rms_send;
    AppData->Buff[i++] = rms_send>>8;
    AppData->Buff[i++] = rms_send>>16;
    AppData->Buff[i++] = rms_send>>24;
    AppData->Buff[i++] = crest_send;
    AppData->Buff[i++] = crest_send>>8;
    AppData->Buff[i++] = crest_send>>16;
    AppData->Buff[i++] = crest_send>>24;
    AppData->Buff[i++] = kr_send;
    AppData->Buff[i++] = kr_send>>8;
    AppData->Buff[i++] = kr_send>>16;
    AppData->Buff[i++] = kr_send>>24;

    AppData->BuffSize = i;

}

static void LoraRxData( lora_AppData_t *AppData )
{
	PRINTF("RX data\n\r");
}

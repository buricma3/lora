

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "low_power.h"
#include "lora.h"
#include "timeServer.h"
#include "vcom.h"
#include "arm_math.h"
//#include "kurtogram.h"
#include <stdint.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
static TimerEvent_t MeasurementStartTimer;
ADC_HandleTypeDef 				 hadc;
extern DMA_HandleTypeDef         DmaHandle;
extern TIM_HandleTypeDef 		  htim2;
int sizeOfArray = 2000;


extern union {
	uint16_t *adc_values;
	float *flt;
} samples;
/*
union {
	uint16_t adc_values[2000];
	float flt[2000];
} samples;*/

#define MEAS_INTERVAL_MS							( 1*10*1000) //every 3*60 second
#define LPP_APP_PORT 99

/*!
 * Defines the application data transmission duty cycle. 4 min, value in [ms].
 */
#define APP_TX_DUTYCYCLE                           4 * 60 * 1000
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

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void MX_ADC_Init(void);
/* call back when LoRa will transmit a frame*/
static void LoraTxData( lora_AppData_t *AppData, FunctionalState* IsTxConfirmed);

/* call back when LoRa has received a frame*/
static void LoraRxData( lora_AppData_t *AppData);

/* Private variables ---------------------------------------------------------*/
/* load call backs*/
static LoRaMainCallback_t LoRaMainCallbacks ={ HW_GetBatteryLevel,
                                               HW_GetUniqueId,
                                               HW_GetRandomSeed,
                                               LoraTxData,
                                               LoraRxData};


/* !
 *Initialises the Lora Parameters
 */
static  LoRaParam_t LoRaParamInit= {TX_ON_TIMER,
									APP_TX_DUTYCYCLE,
                                    CLASS_A,
                                    LORAWAN_ADR_ON,
                                    DR_0,
                                    LORAWAN_PUBLIC_NETWORK,
                                    JOINREQ_NBTRIALS};

/* Private functions ---------------------------------------------------------*/

volatile bool dma_done = false;

#define FLASH_USER_START_ADDR   (FLASH_BASE + FLASH_PAGE_SIZE * 256)             /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     (FLASH_USER_START_ADDR + FLASH_PAGE_SIZE * 10)   /* End @ of user Flash area */

#define DATA_32                 ((uint32_t)0x12345678)

static FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t Address = 0;
__IO uint32_t data32 = 0;

static void done_cb(DMA_HandleTypeDef* dh)
{
	//DISABLE_IRQ( );
	//dma_done = true;
	PRINTF("dma handler\n\r");
	//kurtogram();
	//compute_crest();
	//ENABLE_IRQ();

	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, 0) != HAL_OK)
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

	}

	HAL_FLASH_Lock();

	Address = FLASH_USER_START_ADDR;

	while (Address < FLASH_USER_END_ADDR)
	{
		data32 = *(__IO uint32_t *)Address;
	    Address = Address + 4;
	}

	char buf[1000];
	sprintf(buf, "flash: %d\n\r", (int)Address);
	PRINTF(buf);
}


static void MeasurementStartTimerIrq(void)
{
	GPIOC->BSRR = 1<<7;
	dma_done = false;

	PRINTF("measure\n\r");
	TimerSetValue( &MeasurementStartTimer, MEAS_INTERVAL_MS );
	TimerReset(&MeasurementStartTimer);

	HAL_Delay(200);
	if (HAL_ADC_Start_DMA(&hadc, (void*)samples.adc_values, sizeOfArray) != HAL_OK)
	{
		Error_Handler();
	}
	DmaHandle.XferCpltCallback = done_cb;


	HAL_TIM_Base_Start(&htim2);

	/*while(!dma_done) {
		HAL_Delay(100);
	}*/

	PRINTF("measure done \n\r");
	GPIOC->BRR = 1<<7;

}



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main( void )
{
  /* STM32 HAL library initialization*/
  HAL_Init( );
  
  /* Configure the system clock*/
  SystemClock_Config( );
  
  /* Configure the hardware*/
  HW_Init( );
  MX_ADC_Init();
  
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */
  
  /* Configure the Lora Stack*/
  lora_Init( &LoRaMainCallbacks, &LoRaParamInit);
  
  TimerInit( &MeasurementStartTimer, MeasurementStartTimerIrq );
  TimerSetValue( &MeasurementStartTimer, MEAS_INTERVAL_MS );
  TimerStart( &MeasurementStartTimer );

  PRINTF("START\n\r");
  /* main loop*/
  while( 1 )
  {
    /* run the LoRa class A state machine*/
    lora_fsm( );

    DISABLE_IRQ( );
    /* if an interrupt has occurred after DISABLE_IRQ, it is kept pending 
     * and cortex will not enter low power anyway  */
    if ( lora_getDeviceState( ) == DEVICE_STATE_SLEEP )
    {
#ifndef LOW_POWER_DISABLE
      //LowPower_Handler( );
#endif
    }
    ENABLE_IRQ();
    
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */
  }
}


/* ADC init function */

static void MX_ADC_Init(void)
{

	hadc.Instance = ADC1;
	hadc.Init.OversamplingMode      = DISABLE;

	hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc.Init.LowPowerAutoPowerOff  = DISABLE;
	hadc.Init.LowPowerFrequencyMode = DISABLE;
	hadc.Init.LowPowerAutoWait      = DISABLE;

	hadc.Init.Resolution            = ADC_RESOLUTION_12B;
	hadc.Init.SamplingTime          = ADC_SAMPLETIME_160CYCLES_5;
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


	  // ### - 2 - Start calibration ############################################
	  if (HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) !=  HAL_OK)
	  {
	    Error_Handler();
	  }

	  // ### - 3 - Channel configuration ########################################
	  ADC_ChannelConfTypeDef sConfig;
	  sConfig.Channel      = ADC_CHANNEL_1;               // Channel to be converted
	  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }


}

static void LoraTxData( lora_AppData_t *AppData, FunctionalState* IsTxConfirmed)
{
	uint32_t i = 0;

    TimerSetValue( &MeasurementStartTimer, MEAS_INTERVAL_MS );
    TimerReset(&MeasurementStartTimer);

    AppData->Port = LORAWAN_APP_PORT;
  
    *IsTxConfirmed =  LORAWAN_CONFIRMED_MSG;

    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;
    AppData->Buff[i++] = 0x00;

    AppData->BuffSize = i;
  
}

static void LoraRxData( lora_AppData_t *AppData )
{
	PRINTF("RX data\n\r");
}


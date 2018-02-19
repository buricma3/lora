

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "low_power.h"
#include "lora.h"
#include "timeServer.h"
#include "vcom.h"
//#include "arm_math.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
static TimerEvent_t MeasurementStartTimer;
ADC_HandleTypeDef 				 hadc;
extern DMA_HandleTypeDef         DmaHandle;
extern TIM_HandleTypeDef         htim2;
#define sizeOfArray  2000
static uint16_t adc_values[sizeOfArray];
static float adc_values_abs[sizeOfArray];
static float adc_values_float[sizeOfArray];
int counter = 0;

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
void measure();
void compute_crest();
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

/*!
 * Specifies the state of the application LED
 */
static uint8_t AppLedStateOn = RESET;

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

static volatile bool dma_done = false;
static void done_cb(DMA_HandleTypeDef* dh)
{
	DISABLE_IRQ( );
	dma_done = true;
	PRINTF("dma handler\n\r");
	counter++;
	ENABLE_IRQ();

	//compute_crest();

}

/*
void compute_crest()
{

	for (int i=0; i < sizeOfArray; i++) {
		adc_values_float[i] = (float)adc_values[i];
	}

	float mean;
	arm_mean_f32(adc_values_float, sizeOfArray, &mean);
	for (int i = 0; i < sizeOfArray; i++) {
		adc_values[i] -= mean;
	}

	PRINTF("mean: %.6f\n\r", mean);
	//rms - efektivni hodnota

	float rms;
	arm_rms_f32(adc_values_float, sizeOfArray, &rms);


	// absolute value of each element
	arm_abs_f32(adc_values_float, adc_values_abs, sizeOfArray);

	// peak
	float max;
	uint32_t indexMax;
	arm_max_f32(adc_values_abs, sizeOfArray, &max, &indexMax);

	// crest
	float crest = max/rms;


}*/

static void MeasurementStartTimerIrq(void)
{
	GPIOC->BSRR = 1<<7;

	PRINTF("measure\n\r");
	TimerSetValue( &MeasurementStartTimer, MEAS_INTERVAL_MS );
	TimerReset(&MeasurementStartTimer);

	measure();

	PRINTF("measure done \n\r");

	GPIOC->BRR = 1<<7;
}


void measure()
{
	dma_done = false;
	if (HAL_ADC_Start_DMA(&hadc, (uint32_t *)adc_values, sizeOfArray) != HAL_OK)
	{
		Error_Handler();
	}
	DmaHandle.XferCpltCallback = done_cb;


	//HAL_TIM_Base_Start(&htim2);

	/*while(!dma_done) {
		//HAL_Delay(100);
		PRINTF("waiting...\n\r ");
	}*/


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

	  // ### - 4 - Start conversion in DMA mode #################################
	 /* if (HAL_ADC_Start_DMA(&hadc, (uint32_t *)adc_values, sizeOfArray) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  DmaHandle.XferCpltCallback = done_cb;*/

}

static void LoraTxData( lora_AppData_t *AppData, FunctionalState* IsTxConfirmed)
{

	uint32_t i = 0;


    TimerSetValue( &MeasurementStartTimer, MEAS_INTERVAL_MS );
    TimerReset(&MeasurementStartTimer);

    AppData->Port = LORAWAN_APP_PORT;
  
    *IsTxConfirmed =  LORAWAN_CONFIRMED_MSG;

    AppData->Buff[i++] = counter & 0xFF;
    AppData->Buff[i++] = (counter & 0xFF00)<<8;
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
  switch (AppData->Port)
  {
  case LORAWAN_APP_PORT:
    if( AppData->BuffSize == 1 )
    {
      AppLedStateOn = AppData->Buff[0] & 0x01;
      if ( AppLedStateOn == RESET )
      {
        PRINTF("LED OFF\n\r");
        LED_Off( LED_BLUE ) ; 
        
      }
      else
      {
        PRINTF("LED ON\n\r");
        LED_On( LED_BLUE ) ; 
      }
      //GpioWrite( &Led3, ( ( AppLedStateOn & 0x01 ) != 0 ) ? 0 : 1 );
    }
    break;
  case LPP_APP_PORT:
  {
    AppLedStateOn= (AppData->Buff[2] == 100) ?  0x01 : 0x00;
      if ( AppLedStateOn == RESET )
      {
        PRINTF("LED OFF\n\r");
        LED_Off( LED_BLUE ) ; 
        
      }
      else
      {
        PRINTF("LED ON\n\r");
        LED_On( LED_BLUE ) ; 
      }
    break;
  }
  default:
    break;
  }
}


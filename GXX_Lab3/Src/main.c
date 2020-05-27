#include "main.h"
#include "stm32l4xx_hal.h"

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

//SysTick global flag
int systick_global_flag;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void AdcConfig(void);
int UART_Print_String(UART_HandleTypeDef *huart, char *data, int data_length);

int UART_Print_String(UART_HandleTypeDef *huart, char *data, int data_length){
	
		//if transmission over UART was successful, return 1;
		HAL_StatusTypeDef return_value = HAL_UART_Transmit(huart, (uint8_t *)&data[0], data_length, 30000);
		if(return_value == HAL_OK){
			return 1;
		}
		else{
			return 0;
		}
}
int main(void)
{
	char temp[19] = {'T','e','m','p','e','r','a','t','u','r','e',' ','=',' ','X','Y',' ','C','\n'};

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
	
	/*ADC Config*/
	AdcConfig();

  /* Infinite loop */
//  while (1)
 // {

		//UART_Print_String(&huart1, ch, 9);
		//HAL_Delay(100);

/*		char data;
		//we check what we recieve over the UART transmission
		HAL_UART_Receive(&huart1, (uint8_t *)&data, 1, 30000);
		
		//for comparison purposes we compare our value
		if(data == 'X' || data == 'x'){
			
			//if we receive an X we transmit a Y
			data = 'Y'; 
			
			//we delay for a bit
			HAL_Delay(100);
			
			//then transmit
			HAL_UART_Transmit(&huart1, (uint8_t *)&data, 1, 30000);
		}*/
//  }
	
	uint8_t data;
	uint8_t temperature;
	uint8_t tens;
	uint8_t units;
	char a;
	char b;
	
	while(1){
			if(systick_global_flag == 1){
				systick_global_flag = 0;
				//Calibrate the ADC
				if(HAL_ADCEx_Calibration_Start(&hadc1, ADC_DIFFERENTIAL_ENDED) == HAL_OK){
					
					//Activate the ADC peripheral and start conversion
					HAL_ADC_Start(&hadc1);
				
					//first we check for polling
					if(HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK){
				
						//Retrieve raw ADC data 
						data = HAL_ADC_GetValue(&hadc1);

						//we convert the data to degrees
						//The reference voltage is at 3.3V
						temperature = __HAL_ADC_CALC_TEMPERATURE(3300, data, hadc1.Init.Resolution);

						//extract the tens of temperature
						tens = temperature / 10;
						
						//extract the units of temperature
						units = temperature - (tens * 10);

						//https://stackoverflow.com/questions/46006861/converting-int-to-char-in-c/46006919
						//https://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
						//Convert to char
						a = (char)units+'0';
						b = (char)tens+'0';
						
						//replace into array
						temp[14] = b;
						temp[15] = a;

						//send the temperature data
						UART_Print_String(&huart1, &temp[0], 19);
						//HAL_UART_Transmit(&huart1, (uint8_t *)&temp[0], 19, 30000);
						
						//Stop conversion and disable the ADC peripheral
						HAL_ADC_Stop(&hadc1);
					}
				}

			}
		
	}

}

/*ADC Configuration*/
void AdcConfig(void){
	
	//Enable clock source and prescaler
	__HAL_RCC_ADC_CLK_ENABLE();
	
	//Enable ADC channel config
	ADC_ChannelConfTypeDef sConfig;

	//Configure the ADC peripherla
	hadc1.Instance = ADC1;
	
	hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.NbrOfDiscConversion = 0;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = DISABLE;
	
	if(HAL_ADC_Init(&hadc1) != HAL_OK){
		
		//Init error
		_Error_Handler(__FILE__, __LINE__);
	}
	
	//Configure the channel
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
	sConfig.Offset = 0;
	
	if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
		//Channel config error
		_Error_Handler(__FILE__, __LINE__);
	}
}

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 32;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config((HAL_RCC_GetHCLKFreq()/1000)*100);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
}


void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
}
#endif /* USE_FULL_ASSERT */

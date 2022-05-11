/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;//структуры, через которые мы будем управлять периферией
TIM_HandleTypeDef htim2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void); //настройка системы тактирования
static void MX_GPIO_Init(void);//настройка портов ввода-вывода (пинов)
static void MX_TIM1_Init(void);//настройка таймера 1
static void MX_TIM2_Init(void);//настройка таймера 2

/*------------делители для нот------------------------------------------------*/

#define CB  6421*2   
#define CdB 6061*2
#define DB  5720*2
#define DdB 5400*2
#define EB  5096*2
#define FB  4810*2
#define FdB 4540*2
#define GB  4286*2
#define GdB 4045*2
#define AB  1909*4
#define AdB 3604*2
#define BB  3401*2

#define CM  6421   
#define CdM 6061
#define DM  5720
#define DdM 5400
#define EM  5096
#define FM  4810
#define FdM 4540
#define GM  4286
#define GdM 4045
#define AM  3818
#define AdM 3604
#define BM  3401

#define C1  3210   
#define Cd1 3030
#define D1  2860
#define Dd1 2700
#define E1  2548
#define F1  2405
#define Fd1 2270
#define G1  2143
#define Gd1 2022
#define A1  1909
#define Ad1 1802
#define B1  1700

#define C2  1605   
#define Cd2 1515
#define D2  1430
#define Dd2 1350
#define E2  1274
#define F2  1202
#define Fd2 1135
#define G2  1071
#define Gd2 1011
#define A2  954
#define Ad2 901
#define B2  850

#define C3  802   
#define Cd3 757
#define D3  715
#define Dd3 675
#define E3  637
#define F3  601
#define Fd3 567
#define G3  535
#define Gd3 505
#define A3  477
#define Ad3 450
#define B3  425

#define PA  2  //пауза

// последовательность нот для мелодии, заданная таблично (две мелодии)
uint16_t sound[2][64]={ EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, PA, BB, PA,BB, PA,BB, PA, 
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, BB, BB, PA,PA, PA,PA, PA,
												E1, E1, PA, E1, B1, A1, PA, PA, E2, D2, PA, B1, PA, A1, PA, B1, B1, PA, PA, E1, B1, A1, PA, PA, E2, D2, PA, B1, PA, A1, PA, G1, G1, PA, PA,
												E1, B1, A1, PA, PA, E2, D2, PA, B1, PA, A1, PA, B1, B1, PA, PA, E1, B1, A1, PA, G1, G1, PA, PA, PA, PA, PA, PA, PA}; // сигнализация
// наши переменные
uint16_t S_ON=64; //счетчик нот, ноты играются пока он не досчитает до 32
uint8_t Delay=5; 	//*0.1 sec - длительность для ноты (0.5 сек)
uint8_t del;    	// счетчик задержки (сколько раз по 0,1 сек. прошло)
uint8_t melody;
	/*----------------------- макросы запуска мелодий --------------------------*/
#define PLAY_PRIV {melody=0; Delay=2; S_ON=0;} 		// мелодия №1, длительность ноты 0,3 сек,
#define PLAY_RELE {melody=1; Delay=5;  S_ON=0;}		//  запуск обнулением S_ON
#define MAKDAK_BASS {melody = 0; Delay = 1; S_ON = 0;}
#define MAKDAK_MELODY {melody = 1; Delay =1; S_ON = 0;}
	
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{   // первая секция, настройки.
  HAL_Init();                       //  Инициализация библиотеки  для работы с перефирийными устройствами
  SystemClock_Config();             //  Настройка системы тактирования
  MX_GPIO_Init();                   //  Настройка портов ввода-вывода
  MX_TIM1_Init();                   //  Настройка таймера 1, он будет осчитывать интервалы
  MX_TIM2_Init();                   //  Настройка таймера 2, его регистр захвата-сравнения будет генерировать нам звук
    // вторая секция, действия выполняемые однократно
  HAL_TIM_Base_Start_IT(&htim1);    //  Запуск интервального таймера, теперь он работает и каждый раз досчитывая до 
                                    //  заданного значения вызывает прерывание HAL_TIM_PeriodElapsedCallback и сбрасывается в 0
  HAL_TIM_Base_Start(&htim2);       //  Запуск таймера от которого будет брать частоту регистр захвата сравнения, частоту этого таймера мы будем менять
                                    //  частоту этого таймера мы будем менять
  //HAL_Delay(1000);                //  Задержка при включении (для отладки)
                           //  Макрос, выставляющий начальное состояние переменных для запуска автомата воспроизводящего мелодию
  while (1)
  {
   
  }
}
/*--------прерывание таймера 1-------------------------------------------*/
// после запуска таймера эта функция вызывается каждый раз, когда таймер отмеряет заданный промежуток
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)// функция вызывается при срабатывании любого таймера, нам нужно убедиться, что событие пришло от нужного нам
	{
		if (!(S_ON==64))       // Если счетчик нот в мелодии не стоит на последней ноте (мелодия не проиграна полностью) 
		{
			if (S_ON==0){HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_1);} // если счетчик стоит на первой ноте - включаем блок захвата-сравнения для вывода сигнала на динамик
			if (del<Delay)          // если длительность звучания ноты меньше заданной     
			{
				del++;              // прибавляем счетчик длительности
			}
			else                    // если нота сыграна по длительности полностью 
			{
                del=0;                                      // Сбрасываем счетчик длительности
                if (!(TIM2->PSC == sound[melody][S_ON]))    // Если предделитель не совпадает с заданным в таблице мелодии. (Здесь мы используем 
                {                                           //      прямое обращение к регистру, хранящему значение делителя частоты таймера)        
                    TIM2->PSC=sound[melody][S_ON];          // Перезаписываем новое значение делителя из таблицы
                }
                S_ON++;                                     // увеличиваем счетчик нот
			}
		}
		else                                                // Если ноты закончились, выключаем модуль захвата 
		{                                                   // перестаем вырабатывать сигнал звука
			//TIM2->PSC=2;
			HAL_TIM_OC_Stop(&htim2,TIM_CHANNEL_1);
		} 
	}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7200;                              
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 50;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 25;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
	HAL_TIM_OC_Init(&htim2);
  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


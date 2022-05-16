/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  *
  *
  ******************************************************************************
  */
#define STM32F103xB

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "ssd1306.h"
/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
/* Private variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef hUsbDeviceFS;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);  
void Error_Handler(void);
void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
void MX_ADC1_Init(void);
void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
extern uint8_t USBD_CUSTOM_HID_SendReport     (USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len);
/* Private function prototypes -----------------------------------------------*/
unsigned int ADC_T(void);
unsigned int ADC_V(void);
void RGB_POWER(unsigned int R,unsigned int G,unsigned int B);
unsigned char dev_connected;
// делители для нот
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

#define KICK 50000
#define SNARE 20000
#define PA  2  //пауза

// последовательность нот для мелодии, заданная таблично (две мелодии)
uint16_t mcbass[328] = {EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, PA, BB, PA,BB, PA,BB, PA, 
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, BB, BB, PA,PA, PA,PA, PA, 
												EB, EB, PA, EB, BB, AB, PA, PA, EM, DM, PA, BB, PA, AB, PA, BB, BB, PA, PA, EB, BB, AB, PA, PA, EM, DM, PA, BB, PA, AB, PA, GB, GB, PA, PA,
												EB, BB, AB, PA, PA, EM, DM, PA, BB, PA, AB, PA, BB, BB, PA, PA, EB, BB, AB, PA, GB, GB, PA, PA, PA, PA, PA, PA, PA,
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, PA, BB, PA,BB, PA,BB, PA, 
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, BB, BB, BB,PA, PA,PA, PA,
												PA,PA,PA,PA,PA,PA,PA,PA,
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, PA, BB, PA,BB, PA,BB, PA, 
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, BB, BB, PA,PA, PA,PA, PA,
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, PA, BB, PA,BB, PA,BB, PA, 
												EB, PA, EB, PA, EB, PA, EB, PA, CM, PA, CM, PA, CM, PA, CM, PA, GB, PA, GB, PA, GB, PA, GB, PA, BB, BB, BB, PA,PA, PA,PA, PA,};
uint16_t mc[328] = 	   {EM, EM, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, BM, BM, PA, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, GM, GM, PA, PA,
												EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, BM, BM, PA, PA, EM, BM, AM, PA, GM, GM, PA, PA, PA, PA, PA, PA, PA,
												PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, BM, AM, PA, GM, PA, AM, PA, GM,
												PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, AM, GM, PA, AM, AM, PA, GM, GM,
												EM, EM, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, BM, BM, PA, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, GM, GM, PA, PA,
												EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, BM, BM, PA, PA, EM, BM, AM, PA, GM, GM, PA, PA, PA, PA, PA, PA, PA,
												PA, PA, PA, PA, PA, PA, PA, PA, EM, EM, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, BM, BM, PA, PA, EM, BM, 
												AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, GM, GM, PA, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, BM, BM, PA, 
												PA, EM, BM, AM, PA, GM, GM, PA, PA, PA, PA, PA, PA, PA, EM, EM, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, 
												BM, BM, PA, PA, EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, GM, GM, PA, PA,
												EM, BM, AM, PA, PA, E1, D1, PA, BM, PA, AM, PA, BM, BM, PA, PA, EM, BM, AM, PA, GM, GM, PA, PA, PA, PA, PA, PA, PA,};
uint16_t mc_back[360] ={PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, 
												PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, 
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, 
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												A1, A1, G1, G1, E1, E1, D1, D1, A1, A1, G1, G1, B1, B1, Fd1, Fd1, A1, A1, G1, G1, E1, E1, D1, D1, A1, A1, G1, G1, B1, B1, Fd1, Fd1, 
												A1, A1, G1, G1, E1, E1, D1, D1, A1, A1, G1, G1, B1, B1, Fd1, Fd1, A1, A1, G1, G1, E1, E1, D1, D1, A1, A1, G1, G1, B1, B1, Fd1, Fd1,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK};
uint16_t mc_drums[360]={PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, 
												PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, PA, 
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, 
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, 
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, SNARE,
												KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK, KICK, PA, SNARE, PA, KICK, KICK, SNARE, KICK
};
uint16_t NeverMeant[320] = {CM, CM, E1, D1, C1, C1, GM, GM, AM, AM, C1, C1, E1,E1, C1,C1, BM, BM, C1, C1, E1,E1, C1,C1, D1,D1, C1, C1, E1,E1, C1,C1, E1,E1, C1, C1, E1,E1, F1,F1,G1,G1,C1, C1, E1,E1, C1,C1, 
														CM, CM, E1, D1, C1, C1, GM, GM, AM, AM, C1, C1, E1,E1, C1,C1, BM, BM, C1, C1, E1,E1, C1,C1, D1,D1, C1, C1, E1,E1, C1,C1, E1,E1, C1, C1, E1,E1, F1,F1,G1,G1,C1, C1, E1,E1, C1,C1, PA
};
uint16_t sound[1024];

uint16_t S_ON=64; //счетчик нот, ноты играются пока он не досчитает до 32
uint8_t Delay=5; 	//*0.1 sec - длительность для ноты (0.5 сек)
uint8_t del;    	// счетчик задержки (сколько раз по 0,1 сек. прошло)
uint8_t melody;
uint16_t length = 64;
	/*----------------------- макросы запуска мелодий --------------------------*/
#define MAKDAK_BASS {melody = 0; Delay = 1; S_ON = 0; length = 64;}
#define MAKDAK_MELODY {melody = 1; Delay =2; S_ON = 0; length = 64;}
void SetBPM(uint8_t bpm)
{
	htim3.Init.Period = 10000/bpm;
}
void PlayNeverMeant(uint8_t line)
{
	length = 320;
	S_ON = 0;
	Delay = 1;
	SetBPM(255);
	switch(line)
	{
		case 0x01:
			memcpy(sound, NeverMeant, 640);
			break;
	}
}
void PlayMCMelody(uint8_t line)
{
	length = 328;
	S_ON = 0;
	Delay = 2;
	SetBPM(120);
	switch(line)
	{
		case 0x01:
			memcpy(sound, mc, 656);
			break;
		case 0x02:
			memcpy(sound,mcbass,656);
			break;
		case 0x03:
			memcpy(sound,mc_back,720);
			break;
		case 0x04:
			memcpy(sound,mc_drums,720);
			length = 360;
			break;
	}

}
	void Stop()
	{
		S_ON = length;
	}
int main(void)
{
    HAL_Init();
    /* Configure the system clock */
    SystemClock_Config();               // настройка    системы тактирования ядра и перифирии
    MX_GPIO_Init();                     //              вводов/выводов
    MX_I2C1_Init();                     //              интерфейса связи с дисплеем
    MX_TIM1_Init();                     //              таймера для шим (светодиоды)
    MX_TIM3_Init();                  	  //              таймера периода ноты
		MX_TIM2_Init();                  	  //              таймера генератора звуковой частоты
		HAL_TIM_Base_Start(&htim1);        	// 	Запуск			таймера периода ШИМ светодиодов					
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);	//					канала светодиода 1
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);//					канала светодиода 2
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);//					канала светодиода 1
    HAL_TIM_Base_Start_IT(&htim3);						//				интервального таймера мелодии
	HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_1);			//				канала выработки звуковой частоты
		MX_ADC1_Init();                     //              аналого-цифрового преобразователя для измерения напряжения
    /* Initialize all configured peripherals */
		MX_USB_DEVICE_Init();               //   запуск           подсистемы USB
	RGB_POWER(50,50,50);									//   включаем светодиоды
    ssd1306_Init(hi2c1,0x78);           // процедура настройки дисплея
    ssd1306_Fill(Black) ;               // очистка буфера дисплея
    ssd1306_SetCursor(1,0);							// 
    ssd1306_WriteString("plug device to PC USB port", Font_7x9, White);
    ssd1306_UpdateScreen();             // обновление экрана
																// запускаем мелодию
  /* Infinite loop */

		
    while (1)
    {
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4))
				{
					PlayMCMelody(0x01);
				}
				if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5))
        {
					Stop();
				}
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (!(S_ON==length))
		{
			if (S_ON==0){HAL_TIM_Base_Start(&htim2);}
			if (del<Delay)
			{
				del++;
			}
			else 
			{
        del=0;
        if (!(htim2.Init.Prescaler == sound[S_ON]))
        {
					TIM2->PSC=sound[S_ON];
				}
        S_ON++; 
			}
		}
		else
		{
			TIM2->PSC=2;
			HAL_TIM_Base_Stop(&htim2);
		} 
	}
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

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

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void MX_I2C1_Init(void)
{

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 1200000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}
/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	//7-digit/seg`s
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	//7-digit/dig`s
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM1_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 64;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_TIM_MspPostInit(&htim1);

}
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7200;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 800;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
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

 void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
  /** Common config
  */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
/** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

unsigned int ADC_T (void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    unsigned int ADC;

    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start (&hadc1);
    HAL_ADC_PollForConversion (&hadc1, 100);
    ADC=HAL_ADC_GetValue (&hadc1);  /* Infinite loop */
    HAL_ADC_Stop(&hadc1);

    return ADC;
}

unsigned int ADC_V (void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    unsigned int ADC;

    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start (&hadc1);
    HAL_ADC_PollForConversion (&hadc1, 100);
    ADC=HAL_ADC_GetValue (&hadc1);  /* Infinite loop */
    HAL_ADC_Stop(&hadc1);

    return ADC;
}

void RGB_POWER(unsigned int R,unsigned int G,unsigned int B)
{
    TIM1->CCR1=B;
    TIM1->CCR2=R;
    TIM1->CCR3=G;
}
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
__NOP();
  }
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

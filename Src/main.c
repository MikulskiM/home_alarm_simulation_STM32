/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_i2c.h"
#include <stdbool.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum state_t
{
	OFF,				// ALARM WY£¥CZONY
	TIME10S,			// 10 SEKUND DO AKTYWOWANIA ALARMU
	LOUD,				// ALARM!!!
	READY,				// ALARM AKTYWNY
	READY_door_open		// STAN PRZECHODNI, GDY DRZWI ZOSTAN¥ OTWARTE I OSOBA MA 10 SEKUND NA WPISANIE KODU ALBO ZOSTANIE W£¥CZONY ALARM!!!
} state_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
struct lcd_disp disp;	// to struktura u¿ywana w bibliotece lcd_i2c.h

state_t state = OFF;
bool changed = false;

bool button_used = true;
uint16_t button_clicked;

char password_correct[4] = "0000";
char password[4] = "----";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BUTTON_ENTER_Pin && button_used == true)				// ENTER
	{
		button_used = false;
		button_clicked = BUTTON_ENTER_Pin;

		__HAL_TIM_CLEAR_IT(&htim1 ,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim1);
	}
	else if(GPIO_Pin == BUTTON_DOOR_Pin && button_used == true)			// DOOR
	{
		button_used = false;
		button_clicked = BUTTON_DOOR_Pin;

		__HAL_TIM_CLEAR_IT(&htim1 ,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim1);
	}
	else if(GPIO_Pin == BUTTON_WINDOW_Pin && button_used == true)		// WINDOW
	{
		button_used = false;
		button_clicked = BUTTON_WINDOW_Pin;

		__HAL_TIM_CLEAR_IT(&htim1 ,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim1);
	}
	else
	{
		__NOP();	// Generuje kod maszynowy specyficzny dla platformy, który nie wykonuje operacji. "No Operation Performed"
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_Base_Stop_IT(&htim1);
	if(state == OFF)	// -------------------------------- OFF
	{
		if(htim->Instance == TIM1)	// DEBAUNCING	ENTER
		{
			if(button_clicked == BUTTON_ENTER_Pin)
			{
				changed = false;

				/*
				 *  TU BEDZIE SPRAWDZANIE POPRAWONOSCI KODU DO ALARMU
				 */

				state = TIME10S;

				__HAL_TIM_CLEAR_IT(&htim5 ,TIM_IT_UPDATE);
				HAL_TIM_Base_Start_IT(&htim5);

				button_used = true;
				HAL_TIM_Base_Stop_IT(&htim1);
			}
		}
	}
	else if(state == TIME10S)	// ------------------------ TIME10S
	{
		if(htim->Instance == TIM5)	// TIMER 10 seconds
		{
			changed = false;
			state = READY;
			HAL_TIM_Base_Stop_IT(&htim5);
		}
	}
	else if(state == READY)		// ------------------------ READY
	{
		if(htim->Instance == TIM1)	// DEBAUNCING	DOOR / WINDOW
		{
			if(button_clicked == BUTTON_DOOR_Pin)
			{
				changed = false;
				state = READY_door_open;

				__HAL_TIM_CLEAR_IT(&htim5 ,TIM_IT_UPDATE);
				HAL_TIM_Base_Start_IT(&htim5);

				button_used = true;
				HAL_TIM_Base_Stop_IT(&htim1);
			}
			else if(button_clicked == BUTTON_WINDOW_Pin)
			{
				changed = false;
				state = LOUD;

				button_used = true;
				HAL_TIM_Base_Stop_IT(&htim1);
			}
		}
	}
	else if(state == READY_door_open)	// ---------------- READY_door_open
	{
		if(htim->Instance == TIM5)	// TIMER 10 seconds
		{
			changed = false;
			state = LOUD;
			HAL_TIM_Base_Stop_IT(&htim5);
		}
		else if(htim->Instance == TIM1)	// DEBAUNCING	ENTER
		{
			if(button_clicked == BUTTON_ENTER_Pin)	// jeœli w stanie Ready_door_open zostanie wykryte przerwanie do przycisku ENTER
			{
				HAL_TIM_Base_Stop_IT(&htim5);		// zatrzymaj timer 5 ¿eby nie pojawi³o siê te¿ przerwanie po 10 sekundach

				changed = false;
				state = OFF;						// zmieñ stan na OFF

				button_used = true;
				HAL_TIM_Base_Stop_IT(&htim1);		// zatrzymaj timer odpowiedzialny za debouncing
			}
		}
	}
	else if(state == LOUD)		// ------------------------ LOUD
	{
		if(htim->Instance == TIM1)	// DEBAUNCING	ENTER
		{
			if(button_clicked == BUTTON_ENTER_Pin)
			{
				changed = false;
				state = OFF;

				button_used = true;
				HAL_TIM_Base_Stop_IT(&htim1);
			}
		}
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM10_Init();
  MX_TIM1_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */

  state = OFF;

  disp.addr = (0x27 << 1);
  disp.bl = true;

  lcd_init(&disp);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if(state == OFF)
	  	  {
	  			  // htim2.Instance->CCR1 = 0;

	  			  sprintf((char*)disp.f_line, "OFF");
	  			  sprintf((char*)disp.s_line, "   ");
	  			  lcd_display(&disp);
	  	  }
	  	  else if(state == LOUD)
	  	  {
	  			  // htim2.Instance->CCR1 = 5;

	  			  sprintf((char*)disp.f_line, "LOUD");
	  			  sprintf((char*)disp.s_line, "   ");
	  			  lcd_display(&disp);
	  	  }
	  	  else if(state == TIME10S)
	  	  {
	  			  // htim2.Instance->CCR1 = 0;

	  			  sprintf((char*)disp.f_line, "TIME10s");
	  			  sprintf((char*)disp.s_line, "   ");
	  			  lcd_display(&disp);
	  	  }
	  	  else if(state == READY)
	  	  {
	  			  // htim2.Instance->CCR1 = 0;

	  			  sprintf((char*)disp.f_line, "READY");
	  			  sprintf((char*)disp.s_line, "   ");
	  			  lcd_display(&disp);
	  	  }
	  	  else if(state == READY_door_open)
	  	  {
	  			  // htim2.Instance->CCR1 = 0;

	  			  sprintf((char*)disp.f_line, "READY");
	  			  sprintf((char*)disp.s_line, "(door)");
	  			  lcd_display(&disp);
	  	  }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

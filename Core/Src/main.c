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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 3
#define SOF  0xAA
#define ACT1_EN_CMD (rx_buffer[1] & 0xF0)
#define ACT1_DIR_CMD (rx_buffer[1] & 0x0F)
#define ACT2_EN_CMD (rx_buffer[2] & 0xF0)
#define ACT2_DIR_CMD (rx_buffer[2] & 0x0F)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define HAL_UART_Transmit_new_line() HAL_UART_Transmit(&huart1,(uint8_t*) "\r\n", strlen("\r\n"), HAL_MAX_DELAY)
#define HAL_UART_Transmit_Error_IT(err) HAL_UART_Transmit_IT(&huart1, (uint8_t*) (err), strlen((err)))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

const char * info_device_name 	= "Platform Levelling Actuator Driver";
const char * info_fw_verison 	= "FW: 0.1.0";
const char * info_hw_version	= "HW: v0.0";
const char * info_build_date 	= __DATE__;
const char * info_build_time 	= __TIME__;
const char * info_company 		= "kelectronics.pl";
const char * info_serial_number = "001";

static uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
static volatile bool new_frame = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Transmit(&huart1,(uint8_t*) info_device_name, strlen(info_device_name), HAL_MAX_DELAY);
  HAL_UART_Transmit_new_line();
  HAL_UART_Transmit(&huart1,(uint8_t*) info_fw_verison, strlen(info_fw_verison), HAL_MAX_DELAY);
  HAL_UART_Transmit_new_line();
  HAL_UART_Transmit(&huart1,(uint8_t*) info_hw_version, strlen(info_hw_version), HAL_MAX_DELAY);
  HAL_UART_Transmit_new_line();
  HAL_UART_Transmit(&huart1,(uint8_t*) info_build_date, strlen(info_build_date), HAL_MAX_DELAY);
  HAL_UART_Transmit_new_line();
//  HAL_UART_Transmit(&huart1,(uint8_t*) info_build_time, strlen(info_build_time), HAL_MAX_DELAY);
//  HAL_UART_Transmit_new_line();
  HAL_UART_Transmit(&huart1,(uint8_t*) info_company, strlen(info_company), HAL_MAX_DELAY);
  HAL_UART_Transmit_new_line();
  HAL_UART_Transmit(&huart1,(uint8_t*) "SN: ", strlen("SN: "), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1,(uint8_t*) info_serial_number, strlen(info_serial_number), HAL_MAX_DELAY);
  HAL_UART_Transmit_new_line();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Receive_IT(&huart1, rx_buffer, 3);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(new_frame)
	  {
		  if(rx_buffer[0] == SOF)
		  {
			  if((rx_buffer[1] & 0xF0) <= 0x10)
			  {
				  if((rx_buffer[1] & 0x0F) <= 0x01)
				  {
					  if((rx_buffer[2] & 0xF0) <= 0x10)
					  {
						  if((rx_buffer[2] & 0x0F) <= 0x01)
						  {
							  HAL_GPIO_WritePin(ACT1_EN_GPIO_Port, ACT1_EN_Pin, ACT1_EN_CMD ? GPIO_PIN_SET: GPIO_PIN_RESET);
							  HAL_GPIO_WritePin(ACT1_DIR_GPIO_Port, ACT1_DIR_Pin, ACT1_DIR_CMD ? GPIO_PIN_SET: GPIO_PIN_RESET);
							  HAL_GPIO_WritePin(ACT2_EN_GPIO_Port, ACT2_EN_Pin, ACT2_EN_CMD ? GPIO_PIN_SET: GPIO_PIN_RESET);
							  HAL_GPIO_WritePin(ACT2_DIR_GPIO_Port, ACT2_DIR_Pin, ACT2_DIR_CMD ? GPIO_PIN_SET: GPIO_PIN_RESET);
						  }
						  else
						  {
							  HAL_UART_Transmit_Error_IT("Error value in position E - 0xAAxxxE should be 0 or 1\r\n");
						  }
					  }
					  else
					  {
						  HAL_UART_Transmit_Error_IT("Error value in position E - 0xAAxxEx should be 0 or 1\r\n");
					  }
				  }
				  else
				  {
					  HAL_UART_Transmit_Error_IT("Error value in position E - 0xAAxExx should be 0 or 1\r\n");
				  }
			  }
			  else
			  {
				  HAL_UART_Transmit_Error_IT("Error value in position E - 0xAAExxx should be 0 or 1\r\n");
			  }
		  }
		  else
		  {
			  HAL_UART_Transmit_Error_IT("Error - wrong start of frame - should be 0xAA\r\n");
		  }

		  memset(rx_buffer, 0x00, RX_BUFFER_SIZE);
		  new_frame = false;
		  HAL_UART_Receive_IT(&huart1, rx_buffer, 3);
	  }
	  else
	  {

	  }

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	new_frame = true;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
//	new_frame = true;
	asm("nop");
}

void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
	asm("nop");
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	asm("nop");
}
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
	asm("nop");
}
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
	asm("nop");
}

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

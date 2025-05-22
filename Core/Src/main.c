/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "SIGNAL.h"
#include "VL53L1_Handler.h"
#include "CAN_Handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
VL53L1X sensorVL53L1XLeft, sensorVL53L1XRight;
char buffer[100];
#define ADDRESS_I2C_SENSOR_LEFT 0x22
#define ADDRESS_I2C_SENSOR_RIGHT 0x26
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t state = 0, prevState = 0, forceChange = 0;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
	returnSignal = RxData[0];
}
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
  MX_USART1_UART_Init();
  MX_CAN_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	TOF_InitStruct(&sensorVL53L1XLeft, &hi2c1, ADDRESS_I2C_SENSOR_LEFT, XSHUT0_GPIO_Port, XSHUT0_Pin);
	TOF_InitStruct(&sensorVL53L1XRight, &hi2c1, ADDRESS_I2C_SENSOR_RIGHT, XSHUT1_GPIO_Port, XSHUT1_Pin);
	int status = VL53L1_BootDualSensors(&sensorVL53L1XLeft, &sensorVL53L1XRight);
	if (VL53L1_BootDualSensors(&sensorVL53L1XLeft, &sensorVL53L1XRight) != 0) {
		VL53L1_Error_Hanlder(status);
	}
	CAN_FilterInit_SingleFF0(&hcan, &canfilterconfig, ACTUATOR_ADDR);
	CAN_ComInit_Std(&TxHeader, &hcan, SENSOR_ADDR, 2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		if (forceChange != 0) {
			if (forceChange == BACKWARD_RIGHT_STATE) {
				state = LEFT_STATE;
			} else {
				state = RIGHT_STATE;
			}
			forceChange = 0;
		} else {
			VL53L1_GetDistance(&sensorVL53L1XLeft);
			VL53L1_GetDistance(&sensorVL53L1XRight);
			if (sensorVL53L1XRight.distance >= 55) {
				sensorVL53L1XRight.distance += 50;
			}

			state = Get_State(sensorVL53L1XLeft.distance, sensorVL53L1XRight.distance, 0, prevState);
		}


		switch (state) {
		case LEFT_STATE:
			forceChange = 0;
			prevState = state;

			HAL_Delay(50);
			while (returnSignal != LEFT_STATE) {
				CAN_Transmit(&hcan, &TxHeader, LEFT);
			}
			break;
		case RIGHT_STATE:
			forceChange = 0;
			prevState = state;
			CAN_Transmit(&hcan, &TxHeader, RIGHT);
			HAL_Delay(50);
			while (returnSignal != RIGHT_STATE) {
				CAN_Transmit(&hcan, &TxHeader, RIGHT);
			}
			break;
		case FORWARD_STATE:
			forceChange = 0;
			//prevState = state;
			CAN_Transmit(&hcan, &TxHeader, FORWARD);
			//while (returnSignal != FORWARD_STATE) {}
			break;
		case BACKWARD_LEFT_STATE:
			CAN_Transmit(&hcan, &TxHeader, BACKWARD_LEFT);
			while (state == BACKWARD_LEFT_STATE) {
				VL53L1_GetDistance(&sensorVL53L1XLeft);
				VL53L1_GetDistance(&sensorVL53L1XRight);
				if (sensorVL53L1XRight.distance >= 55) {
					sensorVL53L1XRight.distance += 50;
				}
				state = Get_State(sensorVL53L1XLeft.distance, sensorVL53L1XRight.distance, 0, prevState);
			}
			forceChange = BACKWARD_LEFT_STATE;
			break;
		case BACKWARD_RIGHT_STATE:
			CAN_Transmit(&hcan, &TxHeader, BACKWARD_RIGHT);
			while (state == BACKWARD_RIGHT_STATE) {
				VL53L1_GetDistance(&sensorVL53L1XLeft);
				VL53L1_GetDistance(&sensorVL53L1XRight);
				if (sensorVL53L1XRight.distance >= 55) {
					sensorVL53L1XRight.distance += 50;
				}
				state = Get_State(sensorVL53L1XLeft.distance, sensorVL53L1XRight.distance, 0, prevState);
			}
			forceChange = BACKWARD_RIGHT_STATE;
			break;
		case TEST_SENSORS:
			sprintf(buffer, "\n\rSensor Left\n\r  D1: %dmm\n\n\rSensor Right\n\r  D2: %dmm", sensorVL53L1XLeft.distance, sensorVL53L1XRight.distance);
			Print(buffer, 200);
			break;
		}
		//HAL_Delay(100);

    /* USER CODE END WHILE */

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

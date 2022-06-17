#include <stdint.h>
#include <stdlib.h>

#include "sfunc.h"

#include <stm32l011xx.h>
#include <stm32l0xx_hal.h>
#include <stm32l0xx_hal_rcc.h>
#include <stm32l0xx_hal_gpio.h>
#include <stm32l0xx_nucleo_32.h>
#include <stm32l0xx_hal_uart.h>

#include <string.h>

volatile __attribute__((used)) uint32_t var1 = 0x12345678U;
volatile __attribute__((used, section(".noinit"))) uint32_t var2;

#define HSI_VALUE    ((uint32_t)16000000U) /*!< Value of the Internal oscillator in Hz*/

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void Error_Handler(void)
{
	__disable_irq();

	while (1);
}

/* This variable is updated in three ways:
    1) by calling CMSIS function SystemCoreClockUpdate()
    2) by calling HAL API function HAL_RCC_GetHCLKFreq()
    3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
       Note: If you use this function to configure the system clock; then there
             is no need to call the 2 first functions listed above, since SystemCoreClock
             variable is updated automatically.
*/
uint32_t SystemCoreClock = 2097152U; /* 32.768 kHz * 2^6 */

   /*!< AHB prescalers table values */
const uint8_t AHBPrescTable[16] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U };

    /*!< APB prescalers table values */
const uint8_t APBPrescTable[8] = { 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U };

      /*!< PLL multipiers table values */
const uint8_t PLLMulTable[9] = { 3U, 4U, 6U, 8U, 12U, 16U, 24U, 32U, 48U };

void HAL_MspInit(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if (huart->Instance == USART2) {
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

int main(void)
{
	HAL_Init();

	SystemClock_Config();

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef port_cfg = {
		.Pin = LED3_PIN,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	};

	HAL_GPIO_Init(LED3_GPIO_PORT, &port_cfg);

	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_16);

	/* Configure GPIO MCO pin : PA8 */
	port_cfg.Pin = GPIO_PIN_8;
	port_cfg.Mode = GPIO_MODE_AF_PP;
	port_cfg.Pull = GPIO_NOPULL;
	port_cfg.Speed = GPIO_SPEED_FREQ_LOW;
	port_cfg.Alternate = GPIO_AF0_MCO;

	HAL_GPIO_Init(GPIOA, &port_cfg);

	UART_HandleTypeDef uart = {
		.Init = {
			.BaudRate = 115200U,
			.WordLength = UART_WORDLENGTH_8B,
			.StopBits = UART_STOPBITS_1,
			.Parity = UART_PARITY_NONE,
			.HwFlowCtl = UART_HWCONTROL_NONE,
			.Mode = UART_MODE_TX_RX,
			.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLED,
			.OverSampling = UART_OVERSAMPLING_16,
		},
		.Instance = USART2,
		.AdvancedInit = {
			.AdvFeatureInit = UART_ADVFEATURE_NO_INIT
		}
	};

	if (HAL_UART_Init(&uart) != HAL_OK) {
		Error_Handler();
	}

	char msg[] = "Hello from nucleo_l011k4 !\n";

	uint32_t i = 0U;

	for (;;) {
		HAL_Delay(500);
		HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);

		if (HAL_UART_Transmit(&uart, (uint8_t *)msg, strlen(msg), 1000U) != HAL_OK) {
			Error_Handler();
		}

		i = _sinc(i);
	}
}
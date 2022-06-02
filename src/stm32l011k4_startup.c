#include <stdint.h>

#include <stm32l010x4.h>

/* https://github.com/STMicroelectronics/cmsis_device_l0/blob/master/Source/Templates/gcc/startup_stm32l011xx.s */

#define SRAM_START 0X20000000U
#define SRAM_SIZE (0x2U * 1024U) // 2kB SRAM
#define SRAM_END ((SRAM_START) + (SRAM_SIZE))

#define STACK_START SRAM_END

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _la_data;

extern uint32_t _sbss;
extern uint32_t _ebss;

extern uint32_t _end_stack;

/* application entry point */
int main(void);

void Reset_Handler(void);
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void);
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_CRS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC_COMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPTIM1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART4_USART5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM21_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM22_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPUART1_AES_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

/*
 * check page 29/110 of PM0223 Rev 5
 * check page 266/905 of RM0377 Rev 10
 */
__attribute__((used, section(".isr_vector"))) uint32_t vectors[] = {
    (uint32_t)&_end_stack,
    (uint32_t)Reset_Handler,
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)SVC_Handler,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)PendSV_Handler,
    (uint32_t)SysTick_Handler,

    (uint32_t)WWDG_IRQHandler,
    (uint32_t)PVD_IRQHandler,
    (uint32_t)RTC_IRQHandler,
    (uint32_t)FLASH_IRQHandler,
    (uint32_t)RCC_CRS_IRQHandler,
    (uint32_t)EXTI1_0_IRQHandler,
    (uint32_t)EXTI3_2_IRQHandler,
    (uint32_t)EXTI15_5_IRQHandler,
    (uint32_t)0U,
    (uint32_t)DMA1_Channel1_IRQHandler,
    (uint32_t)DMA1_Channel3_2_IRQHandler,
    (uint32_t)DMA1_Channel7_4_IRQHandler,
    (uint32_t)ADC_COMP_IRQHandler,
    (uint32_t)LPTIM1_IRQHandler,
    (uint32_t)USART4_USART5_IRQHandler,
    (uint32_t)TIM2_IRQHandler,
    (uint32_t)TIM3_IRQHandler,
    (uint32_t)TIM6_IRQHandler,
    (uint32_t)TIM7_IRQHandler,
    (uint32_t)0U,
    (uint32_t)TIM21_IRQHandler,
    (uint32_t)I2C3_IRQHandler,
    (uint32_t)TIM22_IRQHandler,
    (uint32_t)I2C1_IRQHandler,
    (uint32_t)I2C2_IRQHandler,
    (uint32_t)SPI1_IRQHandler,
    (uint32_t)SPI2_IRQHandler,
    (uint32_t)USART1_IRQHandler,
    (uint32_t)USART2_IRQHandler,
    (uint32_t)LPUART1_AES_IRQHandler,
    (uint32_t)0U,
    (uint32_t)0U,
    (uint32_t)0U };

extern uint32_t __la_data;
extern uint32_t __end_text;
extern uint32_t __start_data;
extern uint32_t __end_data;
extern uint32_t __start_bss;
extern uint32_t __end_bss;

extern void __libc_init_array(void);

#define VECT_TAB_BASE_ADDRESS FLASH_BASE
#define VECT_TAB_OFFSET (0x00000000UL)

void Reset_Handler(void)
{
	/* set vector table location */
	SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;

	/* copy data to RAM */
	uint32_t *s = &__la_data;
	uint32_t *p = &__start_data;

	while (p < &__end_data) {
		*p++ = *s++;
	}

	/* initialize bss to 0 */
	p = &__start_bss;
	while (p < &__end_bss) {
		*p++ = 0U;
	}

	/* initialize libc library */
	__libc_init_array();

	/* call application entry function */
	main();

	/* trap main return */
	while (1);
}

void Default_Handler(void)
{
	while (1);
}

void HardFault_Handler(void)
{
	while (1);
}
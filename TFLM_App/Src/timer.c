#include "timer.h"

TIM_HandleTypeDef htim7;

void TimerInit(void) {
    __HAL_RCC_TIM7_CLK_ENABLE();

    htim7.Instance = TIM7;
    htim7.Init.Prescaler = (SystemCoreClock / 1000000) - 1;  // 1 us per count
    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim7.Init.Period = 0xFFFF;  // Maximum period

    HAL_TIM_Base_Init(&htim7);
    HAL_TIM_Base_Start(&htim7);
}

void delay_us(uint32_t us) {
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim7);
    while ((__HAL_TIM_GET_COUNTER(&htim7) - start) < us) {
        // Wait
    }
}

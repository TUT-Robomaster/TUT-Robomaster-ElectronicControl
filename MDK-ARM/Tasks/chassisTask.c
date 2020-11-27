#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
/* USER CODE BEGIN Header_chassisTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_chassisTask */
void chassisTaskEntry(void *argument)
{
extern TIM_HandleTypeDef htim2;
  /* USER CODE BEGIN 5 */
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	TIM2->CCR1=0;
	TIM2->CCR2=2499;
  /* Infinite loop */
  for(;;)
  {
		TIM2->CCR1++;
		TIM2->CCR2--;
		if(TIM2->CCR1==2499)
			TIM2->CCR1=0;
		if(TIM2->CCR2==0)
			TIM2->CCR2=2499;
		osDelay(1);
  }
  /* USER CODE END 5 */
}

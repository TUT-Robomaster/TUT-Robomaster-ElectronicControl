#include "cmsis_os.h"
#include "tim.h"
/* USER CODE BEGIN Header_shootTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_shootTask */
int shoottask = 0;
float poke_speed = 0;
int a = 1000;
void shootTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  /* Infinite loop */
  for(;;)
  {
		shoottask++;
		TIM2->CCR1 = a;
		TIM2->CCR2 = a;
		osDelay(1);
  }
  /* USER CODE END 5 */
}

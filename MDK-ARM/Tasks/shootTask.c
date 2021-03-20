#include "cmsis_os.h"
#include "tim.h"
/* USER CODE BEGIN Header_shootTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_shootTask */
//int a=1000;
void shootTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
//		TIM2->CCR1=a;
//		TIM2->CCR2=a;
  }
  /* USER CODE END 5 */
}

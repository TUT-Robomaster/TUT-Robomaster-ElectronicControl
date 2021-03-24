#include "cmsis_os.h"
/* USER CODE BEGIN Header_gimbalTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_gimbalTask */
int gimbaltask = 0;
void gimbalTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		gimbaltask++;
		osDelay(1);
  }
  /* USER CODE END 5 */
}

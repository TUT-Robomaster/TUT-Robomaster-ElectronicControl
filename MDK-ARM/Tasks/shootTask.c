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
void shootTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		shoottask++;
		osDelay(1);
  }
  /* USER CODE END 5 */
}

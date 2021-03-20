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
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_1);
		osDelay(1000);
  }
  /* USER CODE END 5 */
}

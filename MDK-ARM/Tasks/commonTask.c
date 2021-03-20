#include "cmsis_os.h"

extern void refereeDataUnpack(void);
/* USER CODE BEGIN Header_commonTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ccommonTask*/
void commonTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	refereeDataUnpack();
  }
  /* USER CODE END 5 */
}

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "bsp_uart.h"

extern rc_info_t rc;
int chassistask = 0;
extern void set_chassis_current(int16_t rf,int16_t lf,int16_t lr,int16_t rr);
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
		chassistask++;
		osDelay(1);
		set_chassis_current(rc.ch1*50,rc.ch1*50,rc.ch1*50,rc.ch1*50);
  }
  /* USER CODE END 5 */
}

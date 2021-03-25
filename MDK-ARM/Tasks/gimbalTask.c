#include "cmsis_os.h"
#include "bsp_uart.h"
/* USER CODE BEGIN Header_gimbalTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_gimbalTask */
int gimbaltask = 0;
extern rc_info_t rc;
extern re_info_t re;
extern void set_gimbal_voltage(int16_t yaw,int16_t pit,int16_t poke,int16_t zero);
void gimbalTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		gimbaltask++;
		
		set_gimbal_voltage(rc.ch1*50,rc.ch2*50,rc.ch3*50,rc.ch4*50);
		osDelay(1);
  }
  /* USER CODE END 5 */
}

void Gimbal_Control()
{
	if(rc.sw1==1&&rc.sw2==1)
		
}
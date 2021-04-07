#include "cmsis_os.h"
#include "tim.h"
#include "bsp_uart.h"
/* USER CODE BEGIN Header_shootTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_shootTask */
float poke_speed = 0;
int shoot = 0;
int shootspeed = 1165;
extern rc_info_t rc;
extern user_input_t input;
void shoot_RemoteControl_ready(void);
void shoot_RemoteControl(void);
void shoot_PC(void);
void shoot_off(void);
float shootfrequency(int frq);
void shootTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
	TIM2->CCR1 = 1000;
	TIM2->CCR2 = 1000;
	TIM2->CCR3 = 2300;
  /* Infinite loop */
  for(;;)
  {
		if(rc.sw1==1&&rc.sw2==1)
		{
			shoot_off();
		}
		else if(rc.sw1==1&&rc.sw2==2)
		{
			shoot_PC();
		}
		else if(rc.sw1==3&&rc.sw2==1)
		{
			shoot_RemoteControl_ready();
		}
		else if(rc.sw1==2&&rc.sw2==1)
		{
			shoot_RemoteControl();
		}
		else
		{
			shoot_off();
		}
		osDelay(1);
  }
  /* USER CODE END 5 */
}
void shoot_PC(void)
{
	if(input.key.F)
	{
		shoot = 1;
		HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
	}
	else if(input.key.G)
	{
		shoot = 0;
		HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_RESET);
	}
	else
	{
		
	}
	if(input.key.C)
	{
		shootspeed = 1180;
	}
	else if(input.key.Z)
	{
		shootspeed = 1165;
	}
	else
	{
	}
		if(shoot == 1)
		{
			TIM2->CCR1 = shootspeed;//1320
			TIM2->CCR2 = shootspeed;//1320
		}
		else
		{
			TIM2->CCR1 = 1000;//1000
			TIM2->CCR2 = 1000;//1000
		}
		if(input.mouse.press_l&&shoot == 1)
		{
			poke_speed = shootfrequency(7);
		}
		else if(input.mouse.press_r && shoot ==1)
		{
			poke_speed = shootfrequency(-5);
		}
		else
		{
			poke_speed = shootfrequency(0);
		}
}
float shootfrequency(int frq)
{
	return -308*frq;
}
void shoot_RemoteControl_ready(void)
{
	TIM2->CCR1 = 1100;//1175
	TIM2->CCR2 = 1100;//1175
	poke_speed = shootfrequency(0);
}
void shoot_RemoteControl(void)
{
	TIM2->CCR1 = 1175;//1175
	TIM2->CCR2 = 1175;//1175
	poke_speed = shootfrequency(5);
}
void shoot_off(void)
{
	TIM2->CCR1 = 1000;//1000
	TIM2->CCR2 = 1000;//1000
	poke_speed = shootfrequency(0);
}
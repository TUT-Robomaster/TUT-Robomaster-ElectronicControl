#include "cmsis_os.h"
#include "bsp_uart.h"
#include "refree_sys.h"
#include "bsp_can.h"
#include "pid.h"
/* USER CODE BEGIN Header_gimbalTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_gimbalTask */
int gimbaltask = 0;
extern float poke_speed;
extern rc_info_t rc;
extern re_info_t re;
extern struct pid pid_gimbal_yaw;
extern struct pid pid_gimbal_pit;
extern struct pid pid_gimbal_poke;
extern referee_data_t reRxData;
extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;
extern moto_measure_t moto_poke;	//拨单电机
int16_t mouse_x_angle;
int16_t mouse_y_angle;
extern void set_gimbal_voltage(int16_t yaw,int16_t pit,int16_t poke,int16_t zero);
void Gimbal_Control(void);
void Angle_transimit(void);
void gimbalTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		gimbaltask++;
		Angle_transimit();
		Gimbal_Control();
		osDelay(1);
  }
  /* USER CODE END 5 */
}
int16_t yaw_current;
int16_t pit_current;
void Gimbal_Calculate_RemoteControl(void)
{
	int16_t poke_current;
	yaw_current = pid_calculate(&pid_gimbal_yaw,moto_yaw.real_angle+moto_yaw.offset_angle,-rc.ch3*2);
	pit_current = pid_calculate(&pid_gimbal_pit,moto_pit.real_angle+moto_pit.offset_angle,rc.ch4*2);
	poke_current = pid_calculate(&pid_gimbal_poke,moto_poke.speed_rpm,poke_speed);
	set_gimbal_voltage(yaw_current,pit_current,poke_current,0);
}
void Gimbal_Calculate_PC(void)
{

	int16_t pit_current;
	int16_t poke_current;
	mouse_x_angle += reRxData.input.mouse_x;
	mouse_y_angle += reRxData.input.mouse_y;
				(mouse_x_angle> 700)?(mouse_x_angle= 700):(mouse_x_angle);
				(mouse_x_angle<-700)?(mouse_x_angle=-700):(mouse_x_angle);
				(mouse_y_angle> 700)?(mouse_y_angle= 700):(mouse_y_angle);
				(mouse_y_angle<-700)?(mouse_y_angle=-700):(mouse_y_angle);
	yaw_current = pid_calculate(&pid_gimbal_yaw,moto_yaw.real_angle+moto_yaw.offset_angle,mouse_x_angle);
	pit_current = pid_calculate(&pid_gimbal_pit,moto_pit.real_angle+moto_pit.offset_angle,mouse_y_angle);
	poke_current = pid_calculate(&pid_gimbal_poke,moto_poke.speed_rpm,poke_speed);
	set_gimbal_voltage(yaw_current,pit_current,poke_current,0);
}
void Angle_transimit(void){
	
	moto_pit.real_angle = (moto_pit.correcting_angle >= 4096)?
												(((moto_pit.angle <= 8192 && moto_pit.angle > moto_pit.correcting_angle-4096)?(moto_pit.angle):(moto_pit.angle+8192)) - moto_pit.correcting_angle):
												(((moto_pit.angle <= 8192 && moto_pit.angle > moto_pit.correcting_angle+4096)?(moto_pit.angle-8192):(moto_pit.angle)) - moto_pit.correcting_angle);
	
	
	
	
	moto_yaw.real_angle = (moto_yaw.correcting_angle >= 4096 )?
												(((moto_yaw.angle <= 8192 && moto_yaw.angle > moto_yaw.correcting_angle-4096)?(moto_yaw.angle):(moto_yaw.angle+8192)) - moto_yaw.correcting_angle):
												(((moto_yaw.angle <= 8192 && moto_yaw.angle > moto_yaw.correcting_angle+4096)?(moto_yaw.angle-8192):(moto_yaw.angle)) - moto_yaw.correcting_angle);
	
}

void Gimbal_Offline(void)
{
	set_gimbal_voltage(0,0,0,0);
}
void Gimbal_Control(void)
{
	if(rc.sw1==1&&rc.sw2==1)
	{
		Gimbal_Calculate_RemoteControl();
	}
	else if(rc.sw1==1&&rc.sw2==2)
	{
		Gimbal_Calculate_PC();
	}
	else if(rc.sw1==2&&rc.sw2==2)
	{
		Gimbal_Offline();
	}
	else
	{
		Gimbal_Offline();
	}
}
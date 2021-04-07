#include "cmsis_os.h"
#include "bsp_uart.h"
#include "refree_sys.h"
#include "bsp_can.h"
#include "pid.h"
#define PIT_OFFSET 2600
#define YAW_OFFSET 4030
/* USER CODE BEGIN Header_gimbalTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_gimbalTask */
extern float poke_speed;
extern rc_info_t rc;
extern user_input_t input;
extern struct pid pid_gimbal_yaw;
extern struct pid pid_gimbal_pit;
extern struct pid pid_gimbal_yaw_omg;
extern struct pid pid_gimbal_pit_omg;
extern struct pid pid_gimbal_poke;
extern referee_data_t reRxData;
extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;
extern moto_measure_t moto_poke;	//拨单电机
int16_t mouse_x_angle;
int16_t mouse_y_angle;
int16_t yaw_angle;
int16_t yaw_current;
int16_t pit_current;
extern void set_gimbal_voltage(int16_t yaw,int16_t pit,int16_t poke,int16_t zero);
void Gimbal_Calculate_RemoteControl(void);
void Gimbal_Calculate_PC(void);
void Gimbal_Offline(void);
void Angle_transimit(void);
void gimbalTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		Angle_transimit();
		if(rc.sw1==1&&rc.sw2==1)
		{
			Gimbal_Calculate_RemoteControl();
		}
		else if(rc.sw1==1&&rc.sw2==2)
		{
			Gimbal_Calculate_PC();
		}
		else if(rc.sw1==3&&rc.sw2==1)
		{
			Gimbal_Calculate_RemoteControl();
		}
		else if(rc.sw1==2&&rc.sw2==1)
		{
			Gimbal_Calculate_RemoteControl();
		}
		else if(rc.sw1==2&&rc.sw2==2)
		{
			Gimbal_Offline();
		}
		else
		{
			Gimbal_Offline();
		}
		osDelay(1);
  }
  /* USER CODE END 5 */
}
int16_t poke_current;
void Gimbal_Calculate_RemoteControl(void)
{
	int16_t pit_angle;
	
	float yaw_fdb;
	float pit_fdb;
	float poke_fdb;
	yaw_angle = -rc.ch3*2+YAW_OFFSET;
	pit_angle = rc.ch4*2+PIT_OFFSET;
	yaw_fdb = moto_yaw.angle;
	pit_fdb = moto_pit.angle;
	poke_fdb = moto_poke.speed_rpm;
	pid_calculate(&pid_gimbal_yaw,yaw_fdb,yaw_angle);
	pid_calculate(&pid_gimbal_pit,pit_fdb,pit_angle);
	yaw_current = pid_calculate(&pid_gimbal_yaw_omg,moto_yaw.real_current,pid_gimbal_yaw.out);
	pit_current = pid_calculate(&pid_gimbal_pit_omg,moto_pit.real_current,pid_gimbal_pit.out);
	poke_current = pid_calculate(&pid_gimbal_poke,poke_fdb,poke_speed);
	set_gimbal_voltage(yaw_current,pit_current,poke_current,poke_current);
}
void Gimbal_Calculate_PC(void)
{
	int16_t poke_current;
	float yaw_fdb;
	float pit_fdb;
	float poke_fdb;
	yaw_fdb = moto_yaw.angle;
	pit_fdb = moto_pit.angle;
	poke_fdb = moto_poke.speed_rpm;
	mouse_x_angle += 0.2*(-input.mouse.x);
	mouse_y_angle += 0.2*(input.mouse.y);
				(mouse_x_angle> 1700)?(mouse_x_angle= 1700):(mouse_x_angle);
				(mouse_x_angle<-1700)?(mouse_x_angle=-1700):(mouse_x_angle);
				(mouse_y_angle> 600)?(mouse_y_angle= 600):(mouse_y_angle);
				(mouse_y_angle<-700)?(mouse_y_angle=-700):(mouse_y_angle);
	pid_calculate(&pid_gimbal_yaw,yaw_fdb,mouse_x_angle+YAW_OFFSET);
	pid_calculate(&pid_gimbal_pit,pit_fdb,mouse_y_angle+PIT_OFFSET);
	yaw_current = pid_calculate(&pid_gimbal_yaw_omg,moto_yaw.real_current,pid_gimbal_yaw.out);
	pit_current = pid_calculate(&pid_gimbal_pit_omg,moto_pit.real_current,pid_gimbal_pit.out);
	poke_current = pid_calculate(&pid_gimbal_poke,poke_fdb,poke_speed);
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

}
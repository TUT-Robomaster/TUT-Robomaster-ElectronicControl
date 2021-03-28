#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "bsp_uart.h"
#include "bsp_can.h"
#include "pid.h"
#include "refree_sys.h"
#include <string.h>
#define MAX_CHASSIS_VX_SPEED			14000//3200//150
#define MAX_CHASSIS_VY_SPEED			14000//4000//150
#define MAX_CHASSIS_OMEGA_SPEED		10000//300
#define MAX_FOLLOW_SPEED          6000
#define MyAbs(x) 	( (x>0) ? (x) : (-x) )
#define CHASSIS_OLD
extern moto_measure_t moto_chassis_RF;
extern moto_measure_t moto_chassis_LF;
extern moto_measure_t moto_chassis_LR;
extern moto_measure_t moto_chassis_RR;
extern struct pid pid_chassis_LF; //Left Front
extern struct pid pid_chassis_RF; //Right Front
extern struct pid pid_chassis_LR; //Left Rare
extern struct pid pid_chassis_RR; //Right Rare
extern rc_info_t rc;
extern key_state_t keyboard;
extern int16_t mouse_x_angle;
int chassistask = 0;
extern void set_chassis_current(int16_t rf,int16_t lf,int16_t lr,int16_t rr);
void Chassis_Calculate_RemoteControl(void);
void Chassis_Control(void);
void Chassis_Offline(void);
void Chassis_Calculate_PC(void);
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
		Chassis_Control();
		
		osDelay(1);
//		set_chassis_current(rc.ch1*50,rc.ch1*50,rc.ch1*50,rc.ch1*50);
  }
  /* USER CODE END 5 */
}
void Chassis_Control(void)
{
	if(rc.sw1==1&&rc.sw2==1)
	{
		Chassis_Calculate_RemoteControl();
	}
	else if(rc.sw1==1&&rc.sw2==2)
	{
		Chassis_Calculate_PC();
	}
	else if(rc.sw1==2&&rc.sw2==2)
	{
		Chassis_Offline();
	}
	else
	{
		Chassis_Offline();
	}
}
void Mecanum_calc(float vx, float vy, float omega, const int each_max_spd, int16_t speed[]){
	int16_t buf[4];
	int i;
	float max=0, rate;
	
	vx = vx > MAX_CHASSIS_VX_SPEED ? MAX_CHASSIS_VX_SPEED : vx;
	vx = vx < -MAX_CHASSIS_VX_SPEED ? -MAX_CHASSIS_VX_SPEED : vx;	
	vy = vy > MAX_CHASSIS_VY_SPEED ? MAX_CHASSIS_VY_SPEED : vy;
	vy = vy < -MAX_CHASSIS_VY_SPEED ? -MAX_CHASSIS_VY_SPEED : vy;
	omega = omega > MAX_CHASSIS_OMEGA_SPEED ? MAX_CHASSIS_OMEGA_SPEED : omega;
	omega = omega < -MAX_CHASSIS_OMEGA_SPEED ? -MAX_CHASSIS_OMEGA_SPEED : omega;
	
#ifdef DEBUG_REVERSE
	buf[0] = ( -vx + vy + omega );
	buf[1] = ( -vx - vy + omega );
	buf[2] = ( +vx + vy + omega );
	buf[3] = ( +vx - vy + omega ); 
#endif	
	
	
#ifdef CHASSIS_OLD
	buf[0] = ( -vx + vy - omega );
	buf[1] = ( -vx - vy - omega );
	buf[2] = ( +vx - vy - omega );
	buf[3] = ( +vx + vy - omega );  
#else 
	buf[0] = ( vx + vy + omega );
	buf[1] = -( vx - vy - omega );
	buf[2] = ( vx - vy + omega );
	buf[3] = -( vx + vy - omega );  
#endif

	for(i=0; i<4; i++){
		if ( MyAbs(buf[i]) > max )
			max = MyAbs(buf[i]) ;
	}
	if (max > each_max_spd){
		rate = each_max_spd / max;
		for(i=0; i<4; i++)
			buf[i] *= rate;
	}
	//output
	memcpy(speed, buf, sizeof(int16_t)*4); 
}
void Chassis_Calculate_PC(void)
{
	int16_t vx;
	int16_t vy;
	int16_t omega;
	float spd_multi;
	int16_t speed[4];
	int16_t LF_Speed;
	int16_t RF_Speed;
	int16_t RR_Speed;
	int16_t LR_Speed;
	if(keyboard.CTRL ==1)
	{
		spd_multi = 0.5;
	}
	else if(keyboard.SHIFT ==1)
	{
		spd_multi = 2;
	}
	else
	{
		spd_multi = 1;
	}
	vy = (keyboard.W)*spd_multi*100-(keyboard.S)*spd_multi*100;
	vx = (keyboard.A)*spd_multi*100-(keyboard.D)*spd_multi*100;
	omega = (keyboard.E)*spd_multi*100-(keyboard.Q)*spd_multi*100;
	if(mouse_x_angle == 700)
	{omega += 100;}
	else if(mouse_x_angle == -700)
	{omega -= 100;}
	else
	{}
	Mecanum_calc(vx, vy, omega, MAX_CHASSIS_VX_SPEED, speed);
	LF_Speed = pid_calculate(&pid_chassis_LF,moto_chassis_LF.speed_rpm,speed[0]);
	RF_Speed = pid_calculate(&pid_chassis_RF,moto_chassis_RF.speed_rpm,speed[1]);
	RR_Speed = pid_calculate(&pid_chassis_RR,moto_chassis_RR.speed_rpm,speed[2]);
	LR_Speed = pid_calculate(&pid_chassis_LR,moto_chassis_LR.speed_rpm,speed[3]);
	set_chassis_current(RF_Speed,LF_Speed,LR_Speed,RR_Speed);
}
void Chassis_Offline(void)
{
	set_chassis_current(0,0,0,0);
}

void Chassis_Calculate_RemoteControl(void)
{
	int16_t speed[4];
	int16_t LF_Speed,RF_Speed,RR_Speed,LR_Speed;
	LF_Speed = moto_chassis_LF.speed_rpm;
	RF_Speed = moto_chassis_RF.speed_rpm;
	RR_Speed = moto_chassis_RR.speed_rpm;
	LR_Speed = moto_chassis_LR.speed_rpm;
	Mecanum_calc(-rc.ch1*9, rc.ch2*9, rc.sw*9, MAX_CHASSIS_VX_SPEED, speed);
	
	set_chassis_current(pid_calculate(&pid_chassis_LF,LF_Speed,speed[0]),
											pid_calculate(&pid_chassis_RF,RF_Speed,speed[1]),
											pid_calculate(&pid_chassis_RR,RR_Speed,speed[2]),
											pid_calculate(&pid_chassis_LR,LR_Speed,speed[3]));
}
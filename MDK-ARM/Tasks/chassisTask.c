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
#define 	PI 					(3.1415926535898f)
extern moto_measure_t moto_chassis_RF;
extern moto_measure_t moto_chassis_LF;
extern moto_measure_t moto_chassis_LR;
extern moto_measure_t moto_chassis_RR;
extern struct pid pid_chassis_LF; //Left Front
extern struct pid pid_chassis_RF; //Right Front
extern struct pid pid_chassis_LR; //Left Rare
extern struct pid pid_chassis_RR; //Right Rare
extern rc_info_t rc;
extern user_input_t input;
extern int16_t mouse_x_angle;
extern int16_t yaw_angle;
  double sindata,cosdata,rcdata,rcangle;
  float CHout,gimbalangle;
  uint16_t cca;
int16_t vx;
int16_t vy;
int16_t omega;
extern void set_chassis_current(int16_t rf,int16_t lf,int16_t lr,int16_t rr);
void Chassis_Calculate_RemoteControl(void);
void Chassis_Offline(void);
void Chassis_Calculate_PC(void);
void ALLtoward_Mode(uint16_t correctAngle);
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
		
		osDelay(1);
//		set_chassis_current(rc.ch1*50,rc.ch1*50,rc.ch1*50,rc.ch1*50);
  }
  /* USER CODE END 5 */
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

	float spd_multi;
	int16_t speed[4];
	int16_t LF_Speed;
	int16_t RF_Speed;
	int16_t RR_Speed;
	int16_t LR_Speed;
	if(input.key.CTRL ==1)
	{
		spd_multi = 0.5;
	}
	else if(input.key.SHIFT ==1)
	{
		spd_multi = 2;
	}
	else
	{
		spd_multi = 1;
	}
	vy = (input.key.W)*spd_multi*2400-(input.key.S)*spd_multi*2400;
	vx = (input.key.A)*spd_multi*2400-(input.key.D)*spd_multi*2400;
	omega = (input.key.Q)*spd_multi*2400-(input.key.E)*spd_multi*2400;
	if(mouse_x_angle == 1700)
	{omega += 2400;}
	else if(mouse_x_angle == -1700)
	{omega -= 2400;}
	else
	{}
	LF_Speed = moto_chassis_LF.speed_rpm;
	RF_Speed = moto_chassis_RF.speed_rpm;
	RR_Speed = moto_chassis_RR.speed_rpm;
	LR_Speed = moto_chassis_LR.speed_rpm;
	ALLtoward_Mode(4030); //
	Mecanum_calc(vx, vy, omega, MAX_CHASSIS_VX_SPEED, speed);
	
	set_chassis_current(pid_calculate(&pid_chassis_LF,LF_Speed,speed[0]),
											pid_calculate(&pid_chassis_RF,RF_Speed,speed[1]),
											pid_calculate(&pid_chassis_RR,RR_Speed,speed[2]),
											pid_calculate(&pid_chassis_LR,LR_Speed,speed[3]));
}
void Chassis_Offline(void)
{
	set_chassis_current(0,0,0,0);
}

void Chassis_Calculate_RemoteControl(void)
{
	int16_t speed[4];
	int16_t LF_Speed,RF_Speed,RR_Speed,LR_Speed;
	int16_t vx;
	int16_t vy;
	int16_t omega;
	vx = -rc.ch1*9;
	vy = rc.ch2*9;
	omega = rc.sw*9;
	if(yaw_angle == -1320)
	{omega -= 1000;}
	else if(yaw_angle == 1320)
	{omega += 1000;}
	else
	{}
	LF_Speed = moto_chassis_LF.speed_rpm;
	RF_Speed = moto_chassis_RF.speed_rpm;
	RR_Speed = moto_chassis_RR.speed_rpm;
	LR_Speed = moto_chassis_LR.speed_rpm;
	Mecanum_calc(vx, vy, omega, MAX_CHASSIS_VX_SPEED, speed);
	
	set_chassis_current(pid_calculate(&pid_chassis_LF,LF_Speed,speed[0]),
											pid_calculate(&pid_chassis_RF,RF_Speed,speed[1]),
											pid_calculate(&pid_chassis_RR,RR_Speed,speed[2]),
											pid_calculate(&pid_chassis_LR,LR_Speed,speed[3]));
}
void ALLtoward_Mode(uint16_t correctAngle)
{

	
	cca = (correctAngle >= 4096 )?
						    (((moto_yaw.angle <= 8192 && moto_yaw.angle > correctAngle-4096)?(moto_yaw.angle):(moto_yaw.angle+8192)) - correctAngle):
                (((moto_yaw.angle <= 8192 && moto_yaw.angle > correctAngle+4096)?(moto_yaw.angle-8192):(moto_yaw.angle)) - correctAngle);
	
	
	gimbalangle=-cca*(180/4096.00);
	
	if(vx==0)
	{
		double angle=((gimbalangle)* PI )/180;
		cosdata=cos(angle);
    sindata=sin(angle);
  
    vx=-vy*sindata;
    vy=vy*cosdata;
	}
	else
	{
    rcdata=atan(vy/ vx);
    cosdata=cos(rcdata);  

    CHout=vx/cosdata;

    rcangle=rcdata*180/3.1415;

    double angle=((	rcangle+gimbalangle)* PI )/180;
    cosdata=cos(angle);
    sindata=sin(angle);

    vx=CHout*cosdata;
    vy=CHout*sindata;
	}
}

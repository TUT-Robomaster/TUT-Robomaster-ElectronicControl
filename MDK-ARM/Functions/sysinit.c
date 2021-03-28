#include "stm32f4xx_hal.h"
#include "bsp_can.h"
#include "pid.h"

#define PIT_OFFSET 2600
#define YAW_OFFSET 4030
extern void Dbus_USRT_Init(void);
extern void Referee_USRT_Init(void);

extern void CAN_User_Init(CAN_HandleTypeDef* hcan );
struct pid pid_chassis_LF; //Left Front
struct pid pid_chassis_RF; //Right Front
struct pid pid_chassis_LR; //Left Rare
struct pid pid_chassis_RR; //Right Rare

struct pid pid_gimbal_yaw;
struct pid pid_gimbal_pit;
struct pid pid_gimbal_poke;

extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;

void Chassis_Init(void)
{
	CAN_User_Init(&hcan1);
	pid_struct_init(&pid_chassis_LF,PID_MAX_OUT,INTERGRAL_LIMIT,CHASSIS_KP,CHASSIS_KI,CHASSIS_KD);
	pid_struct_init(&pid_chassis_RF,PID_MAX_OUT,INTERGRAL_LIMIT,CHASSIS_KP,CHASSIS_KI,CHASSIS_KD);
	pid_struct_init(&pid_chassis_LR,PID_MAX_OUT,INTERGRAL_LIMIT,CHASSIS_KP,CHASSIS_KI,CHASSIS_KD);
	pid_struct_init(&pid_chassis_RR,PID_MAX_OUT,INTERGRAL_LIMIT,CHASSIS_KP,CHASSIS_KI,CHASSIS_KD);
}

void Gimbal_Init(void)
{
	CAN_User_Init(&hcan2);
	pid_struct_init(&pid_gimbal_yaw,PID_MAX_OUT,INTERGRAL_LIMIT,YAW_KP,YAW_KI,YAW_KD);
	pid_struct_init(&pid_gimbal_pit,PID_MAX_OUT,INTERGRAL_LIMIT,PIT_KP,PIT_KI,PIT_KD);
	pid_struct_init(&pid_gimbal_poke,PID_MAX_OUT,INTERGRAL_LIMIT,POKE_KP,POKE_KI,POKE_KD);
	moto_pit.correcting_angle = PIT_OFFSET;
	moto_yaw.correcting_angle = YAW_OFFSET;
}


void System_Init(void)
{
	Dbus_USRT_Init();
	Referee_USRT_Init();
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_5,GPIO_PIN_SET);
	Chassis_Init();
	Gimbal_Init();
}
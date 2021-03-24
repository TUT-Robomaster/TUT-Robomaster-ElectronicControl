
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#elif defined STM32F1
#include "stm32f1xx_hal.h"
#endif
#include "mytype.h"
#include "can.h"

typedef enum
{
	CAN_YAW_FEEDBACK_ID   = 0x205,		//云台Yaw
	CAN_PIT_FEEDBACK_ID  =  0x206,			//云台Pitch
	CAN_POKE_FEEDBACK_ID  = 0x207,

	CAN_MotorLF_ID 	= 0x201,    //左前
	CAN_MotorRF_ID 	= 0x202,		//右前
	CAN_MotorLB_ID 	= 0x203,    //左后
	CAN_MotorRB_ID 	= 0x204,		//右后
}CAN_Message_ID;

/*接收到的云台电机的参数结构体*/
typedef struct{
	int16_t	 	speed_rpm;
  int16_t  	real_current;
  int16_t  	given_current;
	int16_t   real_angle;
  uint8_t  	hall;
	uint16_t 	angle;				//abs angle range:[0,8191]
	uint16_t 	last_angle;	  //abs angle range:[0,8191]
	uint16_t	correcting_angle;
	uint16_t	offset_angle;
	int32_t		round_cnt;
	int32_t		total_angle;
}moto_measure_t;

typedef struct
{
        uint8_t Data[8];
}CAN_RecvMsg;

/* Extern  ------------------------------------------------------------------*/
extern moto_measure_t  moto_chassis[];
extern moto_measure_t  moto_yaw,moto_pit,moto_poke,moto_info;
extern float real_current_from_judgesys; //unit :mA
extern float dynamic_limit_current;	//unit :mA,;	//from judge_sys
extern float ZGyroModuleAngle,yaw_zgyro_angle;

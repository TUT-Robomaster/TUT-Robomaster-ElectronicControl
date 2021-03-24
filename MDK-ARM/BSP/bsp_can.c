#include "bsp_can.h"
#include "pid.h"

#define PID_MAX_OUT 30000
#define INTERGRAL_LIMIT 5000
#define CHASSIS_KP 1.0
#define CHASSIS_KI 0.0
#define CHASSIS_KD 0.0
#define YAW_KP 1.0
#define YAW_KI 0.0
#define YAW_KD 0.0
#define PIT_KP 1.0
#define PIT_KI 0.0
#define PIT_KD 0.0
#define POKE_KP 1.0
#define POKE_KI 0.0
#define POKE_KD 0.0

moto_measure_t moto_pit;
moto_measure_t moto_yaw;
moto_measure_t moto_poke;	//拨单电机
moto_measure_t moto_chassis[4] = {0};//4 chassis moto

CAN_RecvMsg can1_rx_buf;
CAN_RecvMsg can2_rx_buf;

struct pid pid_chassis_LF; //Left Front
struct pid pid_chassis_RF; //Right Front
struct pid pid_chassis_LR; //Left Rare
struct pid pid_chassis_RR; //Right Rare

struct pid pid_gimbal_yaw;
struct pid pid_gimbal_pit;
struct pid pid_gimbal_poke;
int test = 0;
CAN_TxHeaderTypeDef TxMeg;
CAN_RxHeaderTypeDef RxMeg;
HAL_StatusTypeDef  HAL_Status_CAN;
void CAN_User_Init(CAN_HandleTypeDef* hcan )   //用户初始化函数
{
	test++;
  CAN_FilterTypeDef  sFilterConfig;
  TxMeg.IDE=CAN_ID_STD;//CAN_ID_EXT;
  TxMeg.RTR=CAN_RTR_DATA;
	
  sFilterConfig.FilterBank = 0;                       //过滤器0
  sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;  //设为列表模式    
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;    
  sFilterConfig.FilterIdHigh = 0x0000;   //基本ID放入到STID中  
  sFilterConfig.FilterIdLow  = 0x0000;    
  sFilterConfig.FilterMaskIdHigh =0x0000;
  sFilterConfig.FilterMaskIdLow  =0x0000; 
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;    //接收到的报文放入到FIFO0中 
  sFilterConfig.FilterActivation = ENABLE;  	//激活过滤器
  sFilterConfig.SlaveStartFilterBank  = 14; 
 
  HAL_Status_CAN=HAL_CAN_ConfigFilter(hcan, &sFilterConfig);
  HAL_Status_CAN=HAL_CAN_Start(hcan);  //开启CAN
  if(HAL_Status_CAN!=HAL_OK)
		{
	
 }	
 HAL_Status_CAN=HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
 if(HAL_Status_CAN!=HAL_OK)
	 {
		 
  }
 }

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
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);
	pid_struct_init(&pid_gimbal_yaw,PID_MAX_OUT,INTERGRAL_LIMIT,YAW_KP,YAW_KI,YAW_KD);
	pid_struct_init(&pid_gimbal_pit,PID_MAX_OUT,INTERGRAL_LIMIT,PIT_KP,PIT_KI,PIT_KD);
	pid_struct_init(&pid_gimbal_poke,PID_MAX_OUT,INTERGRAL_LIMIT,POKE_KP,POKE_KI,POKE_KD);
}
int error=0;
uint8_t TR_BUF[8];
void set_chassis_current(int16_t rf,int16_t lf,int16_t lr,int16_t rr)
{
	static CAN_TxHeaderTypeDef TxMessage1;
	TxMessage1.DLC = 8;
	TxMessage1.StdId = 0x200;
	TxMessage1.IDE = CAN_ID_STD;
	TxMessage1.RTR = CAN_RTR_DATA;
	
	TR_BUF[0] = rf >> 8;
	TR_BUF[1] = rf;
	TR_BUF[2] = lf >> 8;
	TR_BUF[3] = lf;
	TR_BUF[4] = lr >> 8;
	TR_BUF[5] = lr;
	TR_BUF[6] = rr >> 8;
	TR_BUF[7] = rr;
	if(HAL_CAN_AddTxMessage(&hcan1,&TxMessage1,TR_BUF,(uint32_t *)CAN_TX_MAILBOX0)!=HAL_OK)
	{
		error++;
	}
}
CAN_RxHeaderTypeDef RxMessage1;
CAN_RecvMsg can_recvmsg;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if(hcan->Instance==CAN1)
  {
     HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxMessage1,can_recvmsg.Data);
  }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if(hcan->Instance==CAN2)
  {
     HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO1,&RxMessage1,can_recvmsg.Data);
  }
}

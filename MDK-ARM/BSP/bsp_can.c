#include "bsp_can.h"
#include "pid.h"

moto_measure_t moto_pit;
moto_measure_t moto_yaw;
moto_measure_t moto_poke;	//拨单电机
moto_measure_t moto_chassis_RF;
moto_measure_t moto_chassis_LF;
moto_measure_t moto_chassis_LR;
moto_measure_t moto_chassis_RR;


CAN_RxHeaderTypeDef RxMessage1;
CAN_RxHeaderTypeDef RxMessage2;
CAN_RecvMsg can1_recvmsg;
CAN_RecvMsg can2_recvmsg;
HAL_StatusTypeDef  HAL_Status_CAN;
int error=0;
void get_moto_measure(moto_measure_t *ptr, CAN_HandleTypeDef* hcan);
void CAN_User_Init(CAN_HandleTypeDef* hcan )   //用户初始化函数
{
  CAN_FilterTypeDef  sFilterConfig;
	if(hcan == &hcan1)
	{
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
	HAL_Status_CAN=HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
	}
	else
	{
	sFilterConfig.FilterBank = 14;                       //过滤器0
  sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;  //设为列表模式    
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;    
  sFilterConfig.FilterIdHigh = 0x0000;   //基本ID放入到STID中  
  sFilterConfig.FilterIdLow  = 0x0000;    
  sFilterConfig.FilterMaskIdHigh =0x0000;
  sFilterConfig.FilterMaskIdLow  =0x0000; 
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;    //接收到的报文放入到FIFO0中 
  sFilterConfig.FilterActivation = ENABLE;  	//激活过滤器
  sFilterConfig.SlaveStartFilterBank  = 28; 
  HAL_Status_CAN=HAL_CAN_ConfigFilter(hcan, &sFilterConfig);
  HAL_Status_CAN=HAL_CAN_Start(hcan);  //开启CAN
	HAL_Status_CAN=HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
		if(HAL_Status_CAN!=HAL_OK)
			error++;
	}
}


void set_chassis_current(int16_t rf,int16_t lf,int16_t lr,int16_t rr)
{
	static CAN_TxHeaderTypeDef TxMessage1;
	TxMessage1.DLC = 8;
	TxMessage1.StdId = 0x200;
	TxMessage1.IDE = CAN_ID_STD;
	TxMessage1.RTR = CAN_RTR_DATA;
	uint8_t TR_BUF_1[8];
	TR_BUF_1[0] = rf >> 8;
	TR_BUF_1[1] = rf;
	TR_BUF_1[2] = lf >> 8;
	TR_BUF_1[3] = lf;
	TR_BUF_1[4] = lr >> 8;
	TR_BUF_1[5] = lr;
	TR_BUF_1[6] = rr >> 8;
	TR_BUF_1[7] = rr;
	HAL_CAN_AddTxMessage(&hcan1,&TxMessage1,TR_BUF_1,(uint32_t *)CAN_TX_MAILBOX0);
}
void set_gimbal_voltage(int16_t yaw,int16_t pit,int16_t poke,int16_t zero)
{
	static CAN_TxHeaderTypeDef TxMessage2;
	TxMessage2.DLC = 8;
	TxMessage2.StdId = 0x1ff;
	TxMessage2.IDE = CAN_ID_STD;
	TxMessage2.RTR = CAN_RTR_DATA;
	uint8_t TR_BUF[8];
	TR_BUF[0] = yaw >> 8;
	TR_BUF[1] = yaw;
	TR_BUF[2] = pit >> 8;
	TR_BUF[3] = pit;
	TR_BUF[4] = poke >> 8;
	TR_BUF[5] = poke;
	TR_BUF[6] = zero >> 8;
	TR_BUF[7] = zero;
	HAL_CAN_AddTxMessage(&hcan2,&TxMessage2,TR_BUF,(uint32_t *)CAN_TX_MAILBOX1);
}
void Chassis_Decode(void)
{
	switch(RxMessage1.StdId)
	{
		case CAN_MotorLF_ID: get_moto_measure(&moto_chassis_LF, &hcan1);
		break;
		case CAN_MotorRF_ID: get_moto_measure(&moto_chassis_RF, &hcan1);
		break;
		case CAN_MotorLB_ID: get_moto_measure(&moto_chassis_LR, &hcan1);
		break;
		case CAN_MotorRB_ID: get_moto_measure(&moto_chassis_RR, &hcan1);
		break;
		default:break;
	}
}
void Gimbal_Decode(void)
{
	switch(RxMessage2.StdId)
	{
		case CAN_YAW_FEEDBACK_ID:get_moto_measure(&moto_yaw, &hcan2);
		break;
		case CAN_PIT_FEEDBACK_ID:get_moto_measure(&moto_pit, &hcan2);
		break;
		case CAN_POKE_FEEDBACK_ID:get_moto_measure(&moto_poke, &hcan2);
		break;
		default:break;
	}
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if(hcan->Instance==CAN1)
  {
    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxMessage1,can1_recvmsg.Data);
		Chassis_Decode();
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_7);
  }
	if(hcan->Instance==CAN2)
  {
    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxMessage2,can2_recvmsg.Data);
		Gimbal_Decode();
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_8);
  }
}

void get_moto_measure(moto_measure_t *ptr, CAN_HandleTypeDef* hcan)
{
	if(hcan == &hcan1)
	{
	ptr->last_angle    =  ptr->angle;
	ptr->angle         =  (uint16_t)(can1_recvmsg.Data[0]<<8 | can1_recvmsg.Data[1]) ;
	ptr->real_current  =  (int16_t)(can1_recvmsg.Data[2]<<8 | can1_recvmsg.Data[3]);
	ptr->speed_rpm     =  ptr->real_current;	//这里是因为两种电调对应位不一样的信息
	ptr->given_current =  (int16_t)(can1_recvmsg.Data[4]<<8 | can1_recvmsg.Data[5])/-5;
	ptr->hall          =  can1_recvmsg.Data[6];
	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
	}
	else
	{
	ptr->last_angle    =  ptr->angle;
	ptr->angle         =  (uint16_t)(can2_recvmsg.Data[0]<<8 | can2_recvmsg.Data[1]) ;
	ptr->real_current  =  (int16_t)(can2_recvmsg.Data[2]<<8 | can2_recvmsg.Data[3]);
	ptr->speed_rpm     =  ptr->real_current;	//这里是因为两种电调对应位不一样的信息
	ptr->given_current =  (int16_t)(can2_recvmsg.Data[4]<<8 | can2_recvmsg.Data[5])/-5;
	ptr->hall          =  can2_recvmsg.Data[6];
	if(ptr->angle - ptr->last_angle > 4096)
		ptr->round_cnt --;
	else if (ptr->angle - ptr->last_angle < -4096)
		ptr->round_cnt ++;
	ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
	}
}

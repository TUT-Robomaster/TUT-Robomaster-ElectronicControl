#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "bsp_uart.h"
#include "user_CRC.h"

uint8_t dbus_rx_buffer[DBUS_MAX_LEN];
rc_info_t rc;

uint8_t re_buf[RE_BUFLEN];
//re_info_t *re = (re_info_t*)re_buf;
re_info_t re;

uint8_t pc_buf[PC_BUFLEN];
pc_info_t *pc = (pc_info_t*)pc_buf;

uint8_t flag = 0x00; //00000000 dbus,refree,pc,rev,rev,rev,rev,rev

uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream)
{
  /* Return the number of remaining data units for DMAy Streamx */
  return ((uint16_t)(dma_stream->NDTR));
}

static void USRT_Rx_IDLE_Callback(UART_HandleTypeDef* huart)
{
	/* Clear IDLE it flag avoid IDLE interrupt all the time */
	__HAL_UART_CLEAR_IDLEFLAG(huart);

	/* Handle received data in IDLE interrupt */
	if (huart == &huart1)
	{
		/* Clear DMA transfer complete flag */
		__HAL_DMA_DISABLE(huart->hdmarx);

		/* Handle dbus data dbus_buf from DMA */
		if ((DBUS_MAX_LEN - dma_current_data_counter(huart->hdmarx->Instance)) == DBUS_BUFLEN)
		{
			flag |= 0x80;
		}
		/* Restart dma transmission */
		__HAL_DMA_SET_COUNTER(huart->hdmarx, DBUS_MAX_LEN);
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
	if (huart == &huart3)
	{
		/* Clear DMA transfer complete flag */
		__HAL_DMA_DISABLE(huart->hdmarx);
		

		/* Handle referee system data re_buf from DMA */
		if (re_buf[0] == 0xA5)
		{
			if(Verify_CRC8_Check_Sum(re_buf, 5))
			{
//				if(Verify_CRC16_Check_Sum(re_buf, (re_buf[5]|re_buf[6]<<8)+9))
//				{
					flag |= 0x40;
//				}
			}
		}

		/* restart dma transmission */
		__HAL_DMA_SET_COUNTER(huart->hdmarx, RE_MAX_LEN);
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
	if (huart == &PC_HUART)
	{
		/* Clear DMA transfer complete flag */
		__HAL_DMA_DISABLE(huart->hdmarx);

		/* Handle PC data pc_buf from DMA */
		if (pc_buf[0] == 0xCC)
		{

		}
		
		/* Restart DMA transmission */
		__HAL_DMA_SET_COUNTER(huart->hdmarx, PC_MAX_LEN);
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
}

void uart_receive_handler(UART_HandleTypeDef *huart)
{  
	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && 
			__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE))
	{
		USRT_Rx_IDLE_Callback(huart);
	}
}

static int USRT_DMA_Cfg(UART_HandleTypeDef* huart, uint8_t* pData, uint32_t Size)
{
  uint32_t tmp1 = 0;

  tmp1 = huart->RxState;
	
	if (tmp1 == HAL_UART_STATE_READY)
	{
		if ((pData == NULL) || (Size == 0))
		{
			return HAL_ERROR;
		}

		huart->pRxBuffPtr = pData;
		huart->RxXferSize = Size;
		huart->ErrorCode  = HAL_UART_ERROR_NONE;

		/* Enable the DMA Stream */
		HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)pData, Size);

		/* 
		 * Enable the DMA transfer for the receiver request by setting the DMAR bit
		 * in the UART CR3 register 
		 */
		SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}


/**
  * @brief   Initialize dbus uart device 
  * @param   NONE
  * @retval  
  */
void Dbus_USRT_Init(void)
{
	/*  */
//	rc = (rc_info_t *)malloc(sizeof(rc_info_t));
	
	/* Open USRT IDLE IT */
	__HAL_UART_CLEAR_IDLEFLAG(&DBUS_HUART);
	__HAL_UART_ENABLE_IT(&DBUS_HUART, UART_IT_IDLE);

	USRT_DMA_Cfg(&DBUS_HUART, dbus_rx_buffer, DBUS_MAX_LEN);
}

/**
  * @brief   Initialize referee system uart device 
  * @param   NONE
  * @retval  
  */
void Referee_USRT_Init(void)
{
	/* Open USRT IDLE IT */
	__HAL_UART_CLEAR_IDLEFLAG(&RE_HUART);
	__HAL_UART_ENABLE_IT(&RE_HUART, UART_IT_IDLE);

	USRT_DMA_Cfg(&RE_HUART, re_buf, RE_MAX_LEN);
}

/**
  * @brief   Initialize PC uart device 
  * @param   NONE
  * @retval  
  */
void PC_USRT_Init(void)
{
	/* Open USRT IDLE IT */
	__HAL_UART_CLEAR_IDLEFLAG(&PC_HUART);
	__HAL_UART_ENABLE_IT(&PC_HUART, UART_IT_IDLE);

	USRT_DMA_Cfg(&PC_HUART, pc_buf, PC_MAX_LEN);
}


void RC_Callback_Handler(rc_info_t *rc, uint8_t *buff)
{
  rc->ch1 = (buff[0] | buff[1] << 8) & 0x07FF;
  rc->ch1 -= 1024;
  rc->ch2 = (buff[1] >> 3 | buff[2] << 5) & 0x07FF;
  rc->ch2 -= 1024;
  rc->ch3 = (buff[2] >> 6 | buff[3] << 2 | buff[4] << 10) & 0x07FF;
  rc->ch3 -= 1024;
  rc->ch4 = (buff[4] >> 1 | buff[5] << 7) & 0x07FF;
  rc->ch4 -= 1024;
	
  rc->sw1 = ((buff[5] >> 4) & 0x000C) >> 2;
  rc->sw2 = (buff[5] >> 4) & 0x0003;
  
  if ((abs(rc->ch1) > 660) || \
      (abs(rc->ch2) > 660) || \
      (abs(rc->ch3) > 660) || \
      (abs(rc->ch4) > 660))
  {
    memset(rc, 0, sizeof(rc_info_t));
  }		
	
	rc->mouse.x = ((int16_t)buff[6]) | ((int16_t)buff[7] << 8);     
	rc->mouse.y = ((int16_t)buff[8]) | ((int16_t)buff[9] << 8);     
	rc->mouse.z = ((int16_t)buff[10]) | ((int16_t)buff[11] << 8);    
 
  rc->mouse.press_l = buff[12];
  rc->mouse.press_r = buff[13];
	
  rc->key = ((int16_t)buff[14]) | ((int16_t)buff[15] << 8);
	
	rc->sw  = (buff[16] | buff[17] << 8) & 0x07FF;
	rc->sw -= 1024;
	flag &= 0x7F;
}

void RE_Decode(re_info_t *re, uint8_t *buff)
{
	memcpy(re,buff+5,123);
	flag &=0xBF;
}

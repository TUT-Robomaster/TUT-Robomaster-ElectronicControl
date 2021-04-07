#include "cmsis_os.h"
#include "bsp_uart.h"
extern void refereeDataUnpack(void);
extern void RE_Decode(re_info_t *re, uint8_t *buff);
extern uint8_t flag; //00000000 dbus,refree,pc,rev,rev,rev,rev,rev

extern uint8_t dbus_rx_buffer[DBUS_MAX_LEN];
extern rc_info_t rc;

extern uint8_t re_buf[RE_BUFLEN];
extern re_info_t re;

/* USER CODE BEGIN Header_commonTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ccommonTask*/
void commonTaskEntry(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	refereeDataUnpack();
	
		switch(flag)
		{
			case 0x80: RC_Callback_Handler(&rc,dbus_rx_buffer);
								 break;
			case 0x40: RE_Decode(&re,re_buf);
								 break;
			case 0xC0: RC_Callback_Handler(&rc,dbus_rx_buffer);
								 RE_Decode(&re,re_buf);
								 break;
		}
		osDelay(1);
  }
  /* USER CODE END 5 */
}

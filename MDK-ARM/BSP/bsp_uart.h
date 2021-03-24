#include "usart.h"
/* For dji remote controler reciever */
#define DBUS_MAX_LEN     (50)
#define DBUS_BUFLEN      (18)
#define DBUS_HUART       huart1 

/* For referee system reciever */
#define RE_MAX_LEN     (256)
#define RE_BUFLEN      (128)
#define RE_HUART       huart3 

/* For PC reciever */
#define PC_MAX_LEN     (8)
#define PC_BUFLEN      (8)
#define PC_HUART       huart6 



/** 
  * @brief  mouse control information
  */
typedef struct
{
	/* mouse move information */
	int16_t x;
	int16_t y;
	int16_t z;
	
	/* mouse move information */
	uint8_t press_l ;
	uint8_t press_r;
} mouse_info_t;

typedef struct 
{
	uint16_t W:     1;
	uint16_t S:     1;
	uint16_t A:     1;
	uint16_t D:     1;
	uint16_t SHIFT: 1;
	uint16_t CTRL:  1;
	uint16_t Q:     1;
	uint16_t E:     1;
	uint16_t R:     1;
	uint16_t F:     1;
	uint16_t G:     1;
	uint16_t Z:     1;
	uint16_t X:     1;
	uint16_t C:     1;
	uint16_t V:     1;
	uint16_t B:     1;
} key_state_t;


/** 
  * @brief  remote control information
  */
typedef struct
{
  /* rocker channel information */
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;

  /* left and right lever information */
  uint8_t sw1;
  uint8_t sw2;
	int16_t sw;
	
	/* PC control information */
	mouse_info_t mouse;
	uint16_t key;
} rc_info_t;

typedef struct
{
	uint16_t cmd_id;
	uint8_t  data[119];
	uint16_t CRC16;
} re_info_t;

typedef struct
{
	uint8_t  frame_header;
	uint16_t yaw_angle;
	uint16_t pit_angle;
	uint16_t retain;
	uint8_t  frame_tail;
} pc_info_t;

void bsp_uart_init(void);
void RC_Callback_Handler(rc_info_t *rc, uint8_t *buff);
void RE_Decode(re_info_t *re, uint8_t *buff);
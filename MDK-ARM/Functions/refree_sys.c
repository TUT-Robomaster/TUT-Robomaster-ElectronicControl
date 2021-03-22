#include "refree_sys.h"
#include "bsp_uart.h"
#include "user_CRC.h"
#include <string.h>
extern re_info_t re;
referee_data_t reRxData;
uint8_t reTxData[12];
void refereeDataUnpack(void)
{
	/**Some times incorrect
	 * uint16_t data_length = (uint16_t)(re->frame_header[1] | re->frame_header[2] << 8);
	 */
	
	switch(re.cmd_id)
	{
		case EXT_SUPPLY_ACTION:
		{
			reRxData.supply_booking.supply_projectile_id = reRxData.supply_action.supply_projectile_id;
			if(reRxData.supply_action.supply_robot_id == reRxData.robot_state.robot_id)
			{
				reRxData.supply_booking.supply_robot_id = reRxData.supply_action.supply_robot_id;
			}
			reRxData.supply_booking.supply_num = 50;
			break;
		}
		
		case EXT_ROBO_STATE:
		{
			memcpy(&reRxData.robot_state, re.data, 15);
			break;
		}
		
		case EXT_POWER_HEAT_DATA:
		{
			memcpy(&reRxData.power_heat_data, re.data, 14);
			break;
		}
		
		case EXT_SHOOT_DATA:
		{
			memcpy(&reRxData.shoot_data, re.data, 6);
			break;
		}
		case EXT_KEY_DATA:
		{
			memcpy(&reRxData.input, re.data, 12);
			break;
		}
		
		default:
			break;
	}
}

void refereeDataPack(void)
{
	reTxData[0] = 0xA5;
	reTxData[1] = 0x03;
	reTxData[2] = 0x00;
	reTxData[3] = 0x01;
	Append_CRC8_Check_Sum(reTxData, 5);
	
	reTxData[5] = 0x03;
	reTxData[6] = 0x01;
	
	reTxData[7] = reRxData.supply_booking.supply_projectile_id;
	reTxData[8] = reRxData.supply_booking.supply_robot_id;
	reTxData[9] = reRxData.supply_booking.supply_num;
	
	Append_CRC16_Check_Sum(reTxData, 12);
}

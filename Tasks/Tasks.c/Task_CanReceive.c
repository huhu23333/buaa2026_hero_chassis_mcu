/**
 * @file Task_CanMsg.c
 * @author why
 * @brief
 * @version 0.1
 * @date 2021-03-30
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "Task_CanReceive.h"
#include "BSP_CAN.h"
#include "PowerControl.h"
#include "M3508_Motor.h"
#include "M6020_Motor.h"
#include "Steer_Chassis.h"
#include "Cloud_Control.h"
#include "BSP_BoardCommunication.h"
#include "BSP_Usart.h"

/**
  * @Data   2021-03-28
  * @brief  can1接收任务
  * @param  void
  * @retval void
  */
void Can1Receives(void const *argument)
{
    Can_Export_Data_t Can_Export_Data;

    uint32_t ID;
    for (;;)
    {
        xQueueReceive(CAN1_ReceiveHandle, &Can_Export_Data, portMAX_DELAY);
//      HAL_UART_Transmit(&huart6,"CAN1 Receive \n\r",sizeof("CAN1 Receive \n\r"),0xff);
        ID = Can_Export_Data.CAN_RxHeader.StdId;

        //接收 底盘电机 摩擦轮电机 拨盘电机 反馈数据
        if (ID >= M3508M_READID_START && ID <= M3508M_READID_END)
        {
            M3508_FUN.M3508_getInfo(Can_Export_Data);
        }
        //接收云台 yaw轴电机  反馈数据
        else if (ID == M6020_YAW_ID)
        {
            M6020_Fun.M6020_getInfo(Can_Export_Data);
        }
				else if (ID == 0x51)
				{
					PowerControl_Fun.PowerControl_MsgRec(Can_Export_Data);
				}
				
//      else if(ID >=0x211)
//      {
//          int in_v;
//          int cap_v;
//          int in_c;
//          int cap_pec;
//          int set_p;
//
//          in_v = (int16_t)(Can_Export_Data.CANx_Export_RxMessage[0] << 8 | Can_Export_Data.CANx_Export_RxMessage[1]);
//          cap_v = (int16_t)(Can_Export_Data.CANx_Export_RxMessage[2] << 8 | Can_Export_Data.CANx_Export_RxMessage[3]);
//          in_c = (int16_t) (Can_Export_Data.CANx_Export_RxMessage[4] << 8 | Can_Export_Data.CANx_Export_RxMessage[5]);
//          cap_pec =Can_Export_Data.CANx_Export_RxMessage[6];
//          set_p = Can_Export_Data.CANx_Export_RxMessage[7];
//
//      }
    }
}

/**
  * @Data   2021-03-28
  * @brief  can2接收任务
  * @param  void
  * @retval void
  */
void Can2Receives(void const *argument)
{
    Can_Export_Data_t Can_Export_Data;
    uint32_t ID;
    for (;;)
    {
        xQueueReceive(CAN2_ReceiveHandle, &Can_Export_Data, portMAX_DELAY);
        ID = Can_Export_Data.CAN_RxHeader.StdId;
			   if (ID >= M3508D_READID_START && ID <= M3508D_READID_END)
        {
            M3508_FUN.M3508_getInfo(Can_Export_Data);
        }
        //Board2接受Board1数据
        else if (ID == CAN_ID_CHASSIS)
        {
            Board2_FUN.Board2_getChassisInfo(Can_Export_Data);
        }
        else if (ID == CAN_ID_GIMBAL)
        {
            Board2_FUN.Board2_getGimbalInfo(Can_Export_Data);
        }
//        else if (ID == PowerFeedback_ID)
//        {
//            PowerControl_Fun.PowerControl_MsgRec(Can_Export_Data);
//        }

//      HAL_UART_Transmit(&huart6,"CAN2 Receive \n\r",sizeof("CAN2 Receive \n\r"),0xff);
    }
}

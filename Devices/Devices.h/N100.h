/**
 * @file N100.h
 * @author lxr(784457420@qq.com)
 * @brief 
 * @version 1.0
 * @date 2023-11-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
 #ifndef __N100_H
 #define __N100_H
 
 #include "main.h"
 #include "usart.h"

 //����֡�궨��
 #define FRAME_HEAD 0xFC
 #define FRAME_END 0xFD
 #define TYPE_IMU 0x40
 #define TYPE_AHRS 0x41
 #define TYPE_INSGPS 0x42
 #define TYPE_GROUND 0xF0
 #define IMU_LEN 0x38    //56+8 8������
 #define AHRS_LEN 0x30	 //48+8 7������
 #define INSGPS_LEN 0x42 //72+8 10������
 
 //������ݽṹ�嶨��
 typedef struct N100_AHRSData_Packet_t
{
	float RollSpeed;   //unit: rad/s
	float PitchSpeed;  //unit: rad/s
	float YawSpeed;//unit: rad/s
	float Roll;        //unit: rad
	float Pitch;       //unit: rad
	float Yaw;     //unit: rad
	float Qw;//w          //Quaternion
	float Qx;//x
	float Qy;//y
	float Qz;//z
	uint32_t  Timestamp; //unit: us
}N100_AHRSData_Packet_t;

//�������ݶ���
extern N100_AHRSData_Packet_t N100_Angle;//��Ž��������ݡ�
extern uint8_t N100_Rxbuffer[56];//���AHRS���ݡ�56=48+8��

//���⺯������
/**
  * @brief          N100_IMU��ʼ�����񣬴򿪴���
  * @retval         none
  */
extern void N100_Init(void);

/**
  * @brief          N100_IMU���ݽ����������ṹ��
  * @retval         none
  */
void N100_Read(void);
#endif



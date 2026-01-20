/**
 * @file M3508_Motor.h
 * @author Why
 * @brief 
 * @version 0.1
 * @date 2023-08-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __M3508_MOTOR_H
#define __M3508_MOTOR_H

#include "PID.h"
#include "can.h"
#include "steer_chassis.h"
#include "typedef.h"
#include <stdbool.h>
#include <stdint.h>

/* ��¼M3508�������ID
*/
#define M3508M_READID_START 0x201
#define M3508M_READID_END 0x204
#define M3508D_READID_START 0x205
#define M3508D_READID_END 0x208
#define M3508_SENDID_Chassis 0x200   //���Ƶ��̵��
#define M3508_SENDID_Fric_Dial 0x1FF //����Ħ���ֺͲ��̵��
#define M3508_MaxOutput 16384        //���͸������������ֵ
#define M3508_CurrentRatio 819.2f    //16384/20A = 819.2->1A
#define M3508_ReductionRatio 17.0642f //3508������ٱ�

#define M3508_FunGroundInit          \
    {                                \
        &M3508_getInfo,              \
		&M3508_setCurrent,           \
    }

/******�ؼ��趨��pid��3508��������ǰ�ĸ���һһ��Ӧ���ĸ�����
*******����������ط��õ�************************************/
	
	/**
  * @brief  ��������3508����������Ԫ�صĹ���
  * @param  Chassis_Left		��ǰ��
  *			Chassis_Forward  	��ǰ��
  *			Chassis_Right		�����
  *			Chassis_Back		�Һ���
  *			FricL_Wheel			��Ħ����
  *			FricR_Wheel			��Ħ����
  *			DialMotor			�������
  */
typedef enum
{
	Chassis_Left = 0,
	Chassis_Forward,
	Chassis_Right,
	Chassis_Back,
	FricL_Wheel,
	FricR_Wheel,
	Dial_Motor,
	TotalNum,
}M3508_MotorName;

typedef struct
{
	  uint16_t motor_id;
    uint16_t realAngle;  //�������Ļ�е�Ƕ�
    int16_t realSpeed;   //���������ٶ�
    int16_t realCurrent; //��������ʵ�ʵ���
    uint8_t temperture;  //�������ĵ���¶�

    int16_t targetSpeed;  //Ŀ���ٶ�
    int32_t targetAngle; //Ŀ��Ƕ�
    uint16_t lastAngle;   //�ϴεĽǶ�
    int32_t totalAngle;   //�ۻ��ܹ��Ƕ�
    int16_t turnCount;    //ת����Ȧ��

    int16_t outCurrent;   //�������
	  float targetLocation;  //Ŀ��λ��

    uint8_t InfoUpdateFlag;   //��Ϣ��ȡ���±�־
    uint16_t InfoUpdateFrame; //֡��
    uint8_t OffLineFlag;      //�豸���߱�־
	  
	  positionpid_t v_pid_object;
	  positionpid_t l_pid_object;
	
} M3508s_t;

extern M3508s_t M3508_Array[7];
extern M3508s_t M3508_Helm[8];

typedef struct
{
    void (*M3508_getInfo)(Can_Export_Data_t RxMessage);
	void (*M3508_setCurrent)(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4, uint8_t *data);
} M3508_FUN_t;

extern M3508_FUN_t M3508_FUN;

extern void M3508_Init(M3508s_t *motor, uint16_t _motor_id);
extern float encoder_to_circle(int32_t encoder);
extern int32_t circle_to_encoder(float circle);
extern void motor_velocity_change(M3508s_t *motor,pid_control model,CAN_HandleTypeDef *hcan,float target);
extern void motor_location_change(M3508s_t *motor,pid_control model,float target,float real);

#endif /*__M3508_MOTOR_H*/

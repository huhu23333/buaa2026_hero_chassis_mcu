/**
 * @file Gimbal_Chassis_Pitch_Angle.c
 * @author ZS
 * @brief
 * @version 1.0
 * @date 2024-11-2
 *
 * @copyright 
 *
 */

#include <math.h>  
#include "Saber_C3.h"
#include "N100.h"
#include "BSP_BoardCommunication.h"
#include "Gimbal_Chassis_Pitch_Angle.h"
  
// ����PI����  
#define M_PI 3.1415926536f 


// ���ǶȴӶ�ת��Ϊ����  
float degreesToRadians(float degrees) 
{  
    return degrees * M_PI / 180.0f;  
}  
    
float Pitch_Compensation;
float Gimbal_Chassis_offset = 38;   //GM6020�����λ�ú͵���yaw�����֮��ļнǣ�ƫ������
    
/**
 * @brief         ������̨pitch�Ჹ�����ĺ���
 * @param         ����Saber����Ƕ�
 * @param         ��̨����ڵ��̵�yaw�Ƕ�
 * @retval        ��̨pitch�Ჹ����
 */ 
float calculateGimbalPitchCompensation(IMUData chassisIMUData, float gimbalYawAngle) 
{  
  
    // ���̵�pitch��roll�ǶȽ�Ӱ����̨��ʵ��pitch�Ƕ�  
    // ����̨��yaw����ת�󣬵��̵�pitch��Ӱ����̨��roll���������̨��������ϵ��  
    // �����̵�roll��Ӱ����̨��pitch������Ҫͨ����̨yaw�Ƕȵ���ת�������任��  
  
    // ������ֻ̨��yaw����ת�����ǿ���ʹ�ö�ά��ת��ʽ�����Ƽ���   
  
    // ��̨pitch��Ĳ��������Կ����ǵ���pitch��roll�Ƕ�����̨����ϵ�е�ͶӰ  
    // ���ͶӰ����ͨ����ת���������㣬������������ʹ�ý��Ʒ���  
  
    // ���Ƽ�����̨pitch��Ĳ�����  
    // ������̨��yaw��ת��gimbalYawAngle�����̵�pitch����Ӱ����̨��"��Чroll"  
    // �����̵�roll���ڣ�ͨ����ת��Ӱ����̨��"��Чpitch"  
    // ������������Ҫ������̨����ڵ����pitch���䣬����ֻ��Ҫ���ǵ���pitch��"��ЧӰ��"  
    // ���"��ЧӰ��"����ͨ������pitch����һ������̨yaw�Ƕ���ص�ϵ�������ƣ����ϵ����cos(gimbalYawAngle)��  
    // ͬʱ�����ǻ���Ҫ����һ�����ڵ���roll�����"��Чpitch"�仯������仯����ͨ������roll����sin(gimbalYawAngle)������   
  
    float compensation = chassisIMUData.pitch * cos(gimbalYawAngle) + chassisIMUData.roll * sin(gimbalYawAngle);  
  
    return compensation;   // ���ز�����  
}  
 
/**
 * @brief  ��Saber��pitch�ǵ�ֵ����ControlMes�ṹ���еĶ�Ӧ��
 * @param  None
 * @retval None
 */
void Gimbal_Chassis_Pitch_Translate(void) 
{  
    IMUData chassisIMUData = { .pitch = Saber_Angle.Pitch, .yaw = Saber_Angle.Yaw, .roll = Saber_Angle.RoLL }; // ����Saber����
		/***********����begin************/
		if(chassisIMUData.pitch < 1.2f && chassisIMUData.pitch > -1.2f)
		{
			chassisIMUData.pitch = 0;
		}
		if(chassisIMUData.roll < 1.2f && chassisIMUData.roll > -1.2f)
		{
			chassisIMUData.roll = 0;
		}
		/***********����end************/
		
		chassisIMUData.pitch = degreesToRadians(chassisIMUData.pitch);   //�Ƕ�תΪ����
		chassisIMUData.roll = degreesToRadians(chassisIMUData.roll);
		chassisIMUData.yaw = degreesToRadians(chassisIMUData.yaw);
		
    float gimbalYawAngle = ControlMes.yaw_realAngle / 8192.f * 360.f - Saber_Angle.Yaw - Gimbal_Chassis_offset; // ��̨����ڵ��̵�yaw�Ƕ� 
		gimbalYawAngle = degreesToRadians(gimbalYawAngle);
  
    // ���ú������㲹����  
    Pitch_Compensation = calculateGimbalPitchCompensation(chassisIMUData, gimbalYawAngle);
    ControlMes.Gimbal_Chassis_Pitch_Angle = (int16_t)(Pitch_Compensation / M_PI *8192.f);
    
}

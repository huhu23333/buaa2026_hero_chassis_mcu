/**
 * @file FeedForward.c
 * @author Why
 * @brief ǰ������,ȡʵ�����İٷ�֮��ʮ
 * @version 0.1
 * @date 2023-08-28
 * @attention һ��Ҫ��PID�������֮�����������������������Ϊ��ֱ�Ӽ�������������� 
				����ֻ�Ƿ�����ļ�����ߵĶ�������û���õ��ľɰ汾����һ�����ӵĶ���
 *
 * @copyright 
 * 
 */

#include "M3508_Motor.h"
#include "M6020_Motor.h"
#include "Cloud_Control.h"
#include "FeedForward.h"
#include "Saber_C3.h"

/**************�û����ݶ���****************/
void FeedForward_Fric(void);
void FeedForward_Chassis(void);
//void FeedForward_Pitch(void);
void FeedForward_Yaw(void);
void Compensator_Yaw(void);

/****************�ӿڶ���******************/
FeedForward_FUN_t FeedForward_FUN = FeedForward_FunGroundInit;
#undef FeedForward_FunGroundInit

#include "FeedForward.h"

/**
  * @brief   ��Ħ���ַ���ǰ��
  * @param   M3508���ⲿ�ӿ�
  * @retval  void
  */
void FeedForward_Fric()
{
	/* ƽ��Ħ���� */
	if(M3508_Array[FricL_Wheel].realSpeed < 1000)
		M3508_Array[FricL_Wheel].outCurrent += 
			(212 - 0.124f *(float)M3508_Array[FricL_Wheel].realSpeed);
	else
		M3508_Array[FricL_Wheel].outCurrent += 
			(88 - 0.01f * (float)(M3508_Array[FricL_Wheel].realSpeed - 1000));
	
	if(M3508_Array[FricR_Wheel].realSpeed < 1000)
		M3508_Array[FricR_Wheel].outCurrent += 
			(212 - 0.124f * (float)M3508_Array[FricR_Wheel].realSpeed);
	else
		M3508_Array[FricR_Wheel].outCurrent += 
			(88 - 0.01f * (float)(M3508_Array[FricR_Wheel].realSpeed - 1000));
	
	/* ƽ�ⵯ���Ħ���ֵľ޴����������ٵ��� */
	M3508_Array[FricL_Wheel].outCurrent += 5000;
	M3508_Array[FricR_Wheel].outCurrent += 5000;
}

/**
  * @brief   ��Ħ���ַ���ǰ��
  * @param   
  * @retval  void
  */
void FeedForward_Chassis()
{
	/* ƽ��Ħ���� */
	
}

/***˵��������pitchʹ��J4310�������PID,�ò�������ǰ������***/

///**
//  * @brief   ��pitch��������ǰ��
//  * @param   ��̨����̨������ⲿ�ӿ�
//  * @retval  void
//  */
//void FeedForward_Pitch()
//{
//	/* ��ʱ���� */
//	float exp1,exp2, exp3, exp4;
//	
//	/* ƽ��Pitch���� */
//	/* �Ӵ�ת�ǵ�Сת�� y = 0.074 * x ^ 2 -20.9418 * x + 1291.57 */
//	exp1 = M6020s_Pitch.realAngle;
//	if(Cloud.Target_Pitch > M6020s_Pitch.realAngle + Pitch_Margin)
//	{
//		arm_mult_f32(&exp1, &exp1, &exp3, 1);
//		exp2 = 0.074f;
//		arm_mult_f32(&exp3, &exp2, &exp3, 1);
//		exp2 = 20.9418f;
//		arm_mult_f32(&exp2, &exp1, &exp4, 1);
//		arm_sub_f32(&exp3, &exp4, &exp3, 1);
//		exp2 = 1291.57f;
//		arm_add_f32(&exp3, &exp2, &exp4, 1);
//		M6020s_Pitch.outCurrent += 1.125f * exp4;
//	}
//	/* ��Сת�ǵ���ת�� y = 0.0065 * x ^ 2 -19.0685 * x + 1225.57 */
//	else if(Cloud.Target_Pitch < M6020s_Pitch.realAngle - Pitch_Margin)
//	{
//		arm_mult_f32(&exp1, &exp1, &exp3, 1);
//		exp2 = 0.0065f;
//		arm_mult_f32(&exp3, &exp2, &exp3, 1);
//		exp2 = 19.0685f;
//		arm_mult_f32(&exp2, &exp1, &exp4, 1);
//		arm_sub_f32(&exp3, &exp4, &exp3, 1);
//		exp2 = 1225.57f;
//		arm_add_f32(&exp3, &exp2, &exp4, 1);
//		M6020s_Pitch.outCurrent += exp4;
//	}
//	else M6020s_Pitch.outCurrent += 1.125f * exp4;
//}

/**
  * @brief   ��Yaw��������ǰ��
  * @param   ��̨����̨������ⲿ�ӿ�
  * @retval  void
  */
void FeedForward_Yaw()
{
	/* Yaw���ǰ�� */
	M6020s_Yaw.targetSpeed -= Saber_Angle.Z_Vel / 6.0f*0;  // �ӽǶ�ÿ��ת����תÿ����
}

/**
  * @brief   ��Yaw���������Ŷ��Ĳ���
  * @param   ��̨����̨������ⲿ�ӿ�
  * @retval  void
  */
void Compensator_Yaw()
{
	/* ��ʱ���� */
	float exp1,exp2, exp3, exp4;
	/* constant for yaw */
	static float Coefficient_2 = Yaw_J * Yaw_L / (Yaw_K * Yaw_K);
	static float Coefficient_1 = (Yaw_J * Yaw_R + Yaw_L * Yaw_b) / (Yaw_K * Yaw_K);
	static float Coefficient_0 = (Yaw_b * Yaw_R ) / (Yaw_K * Yaw_K) + 1;
	static float speed_last = 0;
	static float error = 0;
	static float error_last = 0;
	static float eerror  = 0;
	
	/* �����Ƕ��׵���һ�׵�����׵���ֵ,�˴��õ���ֵӦΪ��ֵ����λ���� */
	exp4 = Saber_Angle.Z_Vel / 57.29578f; // �������ǽǶ�ÿ�룬ת��Ϊ����
	error = exp4 - speed_last;  
	eerror = error - error_last;
	exp3 = Coefficient_2 * eerror;
	exp2 = Coefficient_1 * error;
	exp1 = Coefficient_0 * exp4;
	speed_last = exp4;
	error_last = error;
	
	M6020s_Yaw.outCurrent += 1000 * (exp1 + exp2 + exp3);
}

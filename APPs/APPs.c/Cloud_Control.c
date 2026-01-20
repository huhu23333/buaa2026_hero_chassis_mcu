/**
 * @file Cloud_control.c
 * @author Cyx 
 * @brief 云台控制模块 
 * @version 0.3
 * @date 2025-10-31
 * @copyright 
 */
#include "Cloud_Control.h"  // 包含云台控制头文件

/************云台PID实例定义***********/
// (重构) 全部改为新的高级增量式PID
pid_advanced_t M6020s_YawIPID;        // 偏航轴内环PID实例
pid_advanced_t M6020s_Yaw_SpeedPID;   // 偏航轴速度环PID（未使用）
pid_advanced_t M6020s_YawOPID;        // 偏航轴外环PID（位置环）
pid_advanced_t AutoAim_M6020s_YawIPID;  // 自动瞄准模式下的内环PID
pid_advanced_t AutoAim_M6020s_YawOPID;  // 自动瞄准模式下的外环PID
/************云台PID END***********/

/****************卡尔曼滤波器结构体定义*****************/
One_Kalman_t Cloud_YawMotorAngle_Error_Kalman;  // (原)偏航角度误差卡尔曼滤波 (重构后未使用)
One_Kalman_t Cloud_YawCurrent_Kalman;          // 自动模式-速度滤波
One_Kalman_t Cloud_YawCurrent_Kalman_manul;    // 手动模式-速度滤波
/****************卡尔曼滤波器结构体定义 End*****************/

/********局部变量********/
Cloud_t Cloud;            // 云台控制全局结构体
float Control_Self_Yaw;   // 备用自控偏航角（未使用）
float shit;               // (未使用)0
/********局部变量********/
// float Linear = 2.75f;             // (重构) 已被新的前馈逻辑替代
float Setup_Angleoffset = 6324.03;  // 云台安装角度偏差
// uint8_t kk = 8;                // (重构) 外环PID降频参数 (已移除)
/********全局变量引用********/
extern M6020s_t* M6020_Array[1];   // 电机指针数组
extern Saber_Angle_t Saber_Angle; // 外部IMU姿态角数据

/********函数声明********/
void Cloud_Init(void);            // 云台初始化
void Cloud_Yaw_Angle_Set(void);   // 偏航角度控制
void Cloud_Sport_Out(void);       // 云台数据输出
void Cloud_Self_Yaw(void);        // 备用功能（未实现）
void PID_Clear_Yaw(void);         // PID数据清除

/***************函数接口定义***************/
Cloud_FUN_t Cloud_FUN = Cloud_FUNGroundInit;  // 云台功能函数结构体初始化
#undef Cloud_FUNGroundInit  // 取消宏定义

/**
 * @brief  云台初始化：配置参数和复位云台
 * @param  None
 * @retval None
 */
void Cloud_Init(void)
{
    // 初始化目标角度：当前电机角度 + IMU偏航角
    Cloud.Target_Yaw = M6020s_Yaw.realAngle + Saber_Angle.Yaw / 360.0f * 8192.0f; 

    // 初始化卡尔曼滤波器
    One_Kalman_Create(&Cloud_YawMotorAngle_Error_Kalman, 1, 10);  // (原)误差滤波
    One_Kalman_Create(&Cloud_YAWODKalman, 1, 10);                // (未使用)
    One_Kalman_Create(&Cloud_YawCurrent_Kalman, 1, 6);           // 自瞄模式-速度滤波
    One_Kalman_Create(&Cloud_YawCurrent_Kalman_manul, 1, 6);     // 手动模式-速度滤波
}

/**
  * @brief  清除偏航轴PID数据
  * @param  void
  * @retval void
  */
void PID_Clear_Yaw(void)
{
    // (重构) 改为清除高级增量式PID数据
    PID_Advanced_Clear(&M6020s_YawIPID);  // 清除内环PID数据
    PID_Advanced_Clear(&M6020s_YawOPID);  // 清除外环PID数据
    PID_Advanced_Clear(&AutoAim_M6020s_YawIPID);
    PID_Advanced_Clear(&AutoAim_M6020s_YawOPID);
}

/**
  * @brief  偏航角度控制（核心函数）
  * @param  void
  * @retval void
  */
void Cloud_Yaw_Angle_Set(void)
{
    /*********************** 云台角度初始化 ***********************/
    if(M6020s_Yaw.InfoUpdateFrame <= 30) {
        Cloud.Target_Yaw = Saber_Angle.Yaw /360.0f * 8192.0f + M6020s_Yaw.realAngle;
    }

    // 目标角度归一化到 0-8191 范围
    while (Cloud.Target_Yaw > 8192) Cloud.Target_Yaw -= 8192;
    while (Cloud.Target_Yaw < -8192) Cloud.Target_Yaw += 8192;

    /*********************** 坐标系转换 ***********************/
    float Angle_Yaw_Chassis = Saber_Angle.Yaw / 360.0f * 8192.0f;
    float Angle_Yaw_Cloud = M6020s_Yaw.realAngle + Angle_Yaw_Chassis;
    if (Angle_Yaw_Cloud > 8192) 
		{
			Angle_Yaw_Cloud -= 8192;
		}
    else if (Angle_Yaw_Cloud < -8192) 
		{
			Angle_Yaw_Cloud += 8192;
		}
    ControlMes.yaw_realAngle = Angle_Yaw_Cloud; 

    /*********************** PID控制逻辑 ***********************/
    
    float world_target_speed = 0; 
    float chassis_real_speed = 0; 
    float filtered_real_speed = 0; 
    
    // 计算底盘旋转前馈
    chassis_real_speed = Saber_Angle.Z_Vel * (8192.0f / 360.0f);

    // 手动遥控模式
    if(ControlMes.AutoAimFlag == 0) {
        
        /* 1. 外环 (位置环) - [修改点] 改为位置式计算 */
        //    输入: 角度误差 -> 输出: 目标速度 (成比例的回正力)
        world_target_speed = PID_Advanced_Angle_Calc_Positional(&M6020s_YawOPID, 
                                                       Cloud.Target_Yaw, 
                                                       Angle_Yaw_Cloud, 
                                                       8191.0f);

        /* 2. 计算内环目标速度 (补偿底盘旋转) */
        M6020s_Yaw.targetSpeed = world_target_speed - chassis_real_speed;
        
        /* 3. 内环 (速度环) - 保持增量式不变 */
        filtered_real_speed = One_Kalman_Filter(&Cloud_YawCurrent_Kalman_manul, M6020s_Yaw.realSpeed);
        
        M6020s_Yaw.outCurrent = PID_Advanced_Calc(&M6020s_YawIPID, 
                                                M6020s_Yaw.targetSpeed, 
                                                filtered_real_speed); 
                                                
    } 
    // 自动瞄准模式
    else if(ControlMes.AutoAimFlag == 1) {
        
        /* 1. 外环 (位置环) - [修改点] 改为位置式计算 */
        world_target_speed = PID_Advanced_Angle_Calc_Positional(&AutoAim_M6020s_YawOPID, 
                                                       Cloud.Target_Yaw, 
                                                       Angle_Yaw_Cloud, 
                                                       8191.0f); 

        /* 2. 计算内环目标速度 */
        M6020s_Yaw.targetSpeed = world_target_speed - chassis_real_speed;
        
        /* 3. 内环 (速度环) - 保持增量式不变 */
        filtered_real_speed = One_Kalman_Filter(&Cloud_YawCurrent_Kalman, M6020s_Yaw.realSpeed);
        
        M6020s_Yaw.outCurrent = PID_Advanced_Calc(&AutoAim_M6020s_YawIPID, 
                                                M6020s_Yaw.targetSpeed, 
                                                filtered_real_speed);
    }
}

/**
  * @brief  云台运动数据输出
  * @param  void
  * @retval void
  */
void Cloud_Sport_Out(void)
{
    /********** 状态检查 **********/
    if(ControlMes.modelFlag == model_Record) {
        M6020s_Yaw.InfoUpdateFrame = 0;
        return;
    } 
    else if(M6020s_Yaw.InfoUpdateFlag == 1) {
        Cloud_FUN.Cloud_Yaw_Angle_Set();  // 调用角度控制核心函数
    } 
    else {
        return;
    }

    uint8_t data[8] = {0};  // CAN发送数据缓冲区

    /********** 云台角度反馈 **********/
    float Angle_Cloud = M6020s_Yaw.realAngle + Setup_Angleoffset;
    shit=Angle_Cloud;
    if(Angle_Cloud > 4096) Angle_Cloud -= 8192;
    else if (Angle_Cloud < -4096) Angle_Cloud += 8192;
    
    steer_getangle( -1*Angle_Cloud / 8192.0f * 360);

    /********** 电机电流设置 **********/
    M6020_Fun.M6020_setVoltage(M6020s_Yaw.outCurrent, 
                               M6020s_Yaw.outCurrent,
                               M6020s_Yaw.outCurrent, 
                               M6020s_Yaw.outCurrent, 
                               data);
    Can_Fun.CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, M6020_SENDID, data);
}
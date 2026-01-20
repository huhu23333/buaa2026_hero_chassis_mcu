/* USER CODE BEGIN Header */
/**
  * @file PID.h
  * @author Miraggio (w1159904119@gmail) / Gemini Modified
  * @brief 
  * @version 0.3 (Position Logic Added)
  * @date 2025-11-19
  * @copyright Copyright (c) 2021
  * */
/* USER CODE END Header */

#ifndef ___PID_H
#define ___PID_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kalman_filter.h"
#include "math.h"
#include "FuzzyPID.h" // [修复] 恢复包含，因为旧的 Position_PID_Yaw 仍需要它

/********pid的参数值********/
#define yaw_I_Aim_f 0.06f

/**********PID数据接口************/

// pid控制方式选择
typedef enum
{
  pid_control_increase,
  pid_control_normal,
  pid_control_frontfeed,
  pid_control_frontfuzzy,
  pid_control_angle  // 舵轮舵向电机专用PID(解决就近转位问题)
}pid_control;


/******** (保留) 旧版PID结构体 ********/
typedef struct incrementalpid_t
{
    float Target;         //设定目标值
    float Measured;       //测量值
    float err;            //本次偏差值
    float err_last;       //上一次偏差
    float err_beforeLast; //上上次偏差
    float Kp;
    float Ki;
    float Kd; //Kp, Ki, Kd
    float p_out;
    float i_out;
    float d_out;          //各分项输出值
    float pwm;            //pwm输出
    uint32_t MaxOutput;     //最大输出
    uint32_t IntegralLimit; //积分限幅
    float (*Incremental_PID)(struct incrementalpid_t *pid_t, float target, float measured);
} incrementalpid_t;

typedef struct positionpid_t
{
    float Target;   //设定目标值
    float Measured; //测量值
    float err;      //本次偏差值
    float err_last; //上一次偏差
    float err_change; //偏差变化量
    float error_target;   // 目标变化
    float last_set_point; //上一次目标值
    float Kp;
    float Ki;
    float Kd; 
    float Kf;     //Kp, Ki, Kd, Kf
    float p_out;
    float i_out;
    float d_out;          
    float f_out;          //各分项输出值
    float pwm;            //pwm输出
    float MaxOutput;      //最大输出
    float Integral_Separation; //积分分离阈值
    float IntegralLimit;       //积分限幅
    float (*Position_PID)(struct positionpid_t *pid_t, float target, float measured);
} positionpid_t;

/******** (新增) 高级PID结构体 ********/
/**
 * @brief (新增) 高级增量式PID结构体 (带前馈和微分先行)
 * @note 专为高性能伺服控制优化 (如云台Yaw轴)
 */
typedef struct pid_advanced_t
{
    float Target;           // 设定目标值
    float Measured;         // 测量值
    float err;              // e(k) 本次偏差
    float err_last;         // e(k-1) 上一次偏差
    float measured_last;    // PV(k-1)
    float measured_beforeLast; // PV(k-2)
    float target_last;      // Target(k-1)
    float Kp, Ki, Kd, Kf;
    float p_out, i_out, d_out, f_out;
    float delta_pwm;        // 本次总增量
    float pwm;              // 累计总输出
    float MaxOutput;        // 最大输出限幅
    float IntegralLimit;    // 积分限幅
} pid_advanced_t;


/******** (修改) 外部PID实例声明 ********/
extern pid_advanced_t M6020s_YawIPID;    //Yaw 
extern pid_advanced_t M6020s_Yaw_SpeedPID; //Yaw速度PID
extern pid_advanced_t M6020s_YawOPID;    //Yaw
extern pid_advanced_t AutoAim_M6020s_YawIPID;
extern pid_advanced_t AutoAim_M6020s_YawOPID;  //Yaw自瞄PID

// (保留) 旧版PID函数原型
extern float Incremental_PID(incrementalpid_t *pid_t, float target, float measured);
extern float Position_PID(positionpid_t *pid_t, float target, float measured);
extern float ClassisTwister_PID(positionpid_t *pid_t, float target, float measured);
extern float Angle_PID(positionpid_t *pid_t, float target, float measured,float ecd_max);
extern void Incremental_PIDInit(incrementalpid_t *pid_t, float Kp, float Kd, float Ki, uint32_t MaxOutput, uint32_t IntegralLimit);
extern void Position_PIDInit(positionpid_t *pid_t, float Kp, float Kd, float Ki, float Kf, float MaxOutput, float IntegralLimit, float Integral_Separation);
extern void Clear_PositionPIDData(positionpid_t *pid_t);
extern void Clear_IncrementalPIDData(incrementalpid_t *pid_t);
extern float Position_PID_Yaw(positionpid_t *pid_t, FUZZYPID_Data_t *fuzzy_t, float target, float measured);

/******** (新增) 高级PID函数原型 ********/
extern float PID_Advanced_Calc(pid_advanced_t *pid_t, float target, float measured);
extern float PID_Advanced_Angle_Calc(pid_advanced_t *pid_t, float target, float measured, float ecd_max); // 增量式(旧)
extern float PID_Advanced_Angle_Calc_Positional(pid_advanced_t *pid_t, float target, float measured, float ecd_max); // 位置式(新, 必须用于外环)
extern void PID_Advanced_Init(pid_advanced_t *pid_t, float Kp, float Kd, float Ki, float kf, float MaxOutput, float IntegralLimit);
extern void PID_Advanced_Clear(pid_advanced_t *pid_t);

// (保留) 卡尔曼滤波器声明
extern One_Kalman_t Cloud_YAWODKalman;
extern One_Kalman_t Cloud_PITCHODKalman;

#endif
/**
 * @file pid.c
 * @author centre 
 * @brief PID控制算法实现模块 
 * @version 0.3
 * @date 2025-10-31
 * @copyright Copyright (c) 2021
 */

#include "PID.h"

// 卡尔曼滤波器实例声明
One_Kalman_t Cloud_YAWODKalman;
One_Kalman_t Cloud_PITCHODKalman;

/**
 * @brief 绝对值限制函数
 */
static void abs_limit(float *a, float ABS_MAX)
{
    if (*a > ABS_MAX)
        *a = ABS_MAX;
    if (*a < -ABS_MAX)
        *a = -ABS_MAX;
}


/**
 * @brief  Yaw轴模糊位置式PID控制器
 */
float Position_PID_Yaw(positionpid_t *pid_t, FUZZYPID_Data_t *fuzzy_t, float target, float measured)
{

    FuzzyComputation(fuzzy_t, pid_t->err, pid_t->err_last);

    pid_t->Target = (float)target;
    pid_t->Measured = (float)measured;
    pid_t->err = pid_t->Target - pid_t->Measured;
    pid_t->err_change = pid_t->err - pid_t->err_last;
    pid_t->error_target = pid_t->Target - pid_t->last_set_point;

    pid_t->p_out = (pid_t->Kp + fuzzy_t->deta_kp) * pid_t->err;
    pid_t->i_out += (pid_t->Ki + fuzzy_t->date_ki) * pid_t->err;
    pid_t->d_out = (pid_t->Kd + fuzzy_t->date_kd) * (pid_t->Measured - pid_t->err_last);
    pid_t->f_out = pid_t->Kf * pid_t->error_target;
    
    if (pid_t->err >= pid_t->Integral_Separation) {
        pid_t->i_out = 0;
    } else {
        abs_limit(&pid_t->i_out, pid_t->IntegralLimit);
    }

    pid_t->pwm = (pid_t->p_out + pid_t->i_out + pid_t->d_out + pid_t->f_out);

    abs_limit(&pid_t->pwm, pid_t->MaxOutput);

    pid_t->err_last = pid_t->Measured;
    pid_t->last_set_point = pid_t->Target;
    
    return pid_t->pwm;
}

/**
 * @brief  增量式PID控制器
 */
float Incremental_PID(incrementalpid_t *pid_t, float target, float measured)
{
  
    pid_t->Target = target;
    pid_t->Measured = measured;
    pid_t->err = pid_t->Target - pid_t->Measured;

    pid_t->p_out = pid_t->Kp * (pid_t->err - pid_t->err_last);
    pid_t->i_out = pid_t->Ki * pid_t->err;
    pid_t->d_out = pid_t->Kd * (pid_t->err - 2.0f * pid_t->err_last + pid_t->err_beforeLast);

    abs_limit(&pid_t->i_out, pid_t->IntegralLimit);

    pid_t->pwm += (pid_t->p_out + pid_t->i_out + pid_t->d_out);

    abs_limit(&pid_t->pwm, pid_t->MaxOutput);


    pid_t->err_beforeLast = pid_t->err_last;
    pid_t->err_last = pid_t->err;

    return pid_t->pwm;
}

/**
 * @brief  初始化增量式PID控制器
 */
void Incremental_PIDInit(incrementalpid_t *pid_t, float Kp, float Ki, float Kd, uint32_t MaxOutput, uint32_t IntegralLimit)
{

    pid_t->Kp = Kp;
    pid_t->Ki = Ki;
    pid_t->Kd = Kd;
    
    pid_t->MaxOutput = MaxOutput;
    pid_t->IntegralLimit = IntegralLimit;

    pid_t->p_out = 0;
    pid_t->d_out = 0;
    pid_t->i_out = 0;
    pid_t->err = 0;
    pid_t->err_last = 0;
    pid_t->err_beforeLast = 0;
    pid_t->pwm = 0;
    pid_t->Measured = 0;
    pid_t->Target = 0;
}

/**
 * @brief 清除增量式PID控制器数据
 */
void Clear_IncrementalPIDData(incrementalpid_t *pid_t)
{
    pid_t->Target = 0;
    pid_t->Measured = 0;
    pid_t->err = 0;
    pid_t->err_last = 0;
    pid_t->err_beforeLast = 0;
    pid_t->p_out = 0;
    pid_t->i_out = 0;
    pid_t->d_out = 0;
    pid_t->pwm = 0;
}

/**
 * @brief  位置式PID控制器
 */
float Position_PID(positionpid_t *pid_t, float target, float measured)
{
 
    pid_t->Target = (float)target;
    pid_t->Measured = (float)measured;
    pid_t->err = pid_t->Target - pid_t->Measured;
    pid_t->err_change = pid_t->Measured - pid_t->err_last;
    pid_t->error_target = pid_t->Target - pid_t->last_set_point;
    
    pid_t->p_out = pid_t->Kp * pid_t->err;
    pid_t->i_out += pid_t->Ki * pid_t->err;
    pid_t->d_out = pid_t->Kd * (pid_t->err - pid_t->err_last);
    pid_t->f_out = pid_t->Kf * pid_t->error_target;
    
    abs_limit(&pid_t->i_out, pid_t->IntegralLimit);

    pid_t->pwm = (pid_t->p_out + pid_t->i_out + pid_t->d_out + pid_t->f_out);

    abs_limit(&pid_t->pwm, pid_t->MaxOutput);

    pid_t->err_last = pid_t->err;
    pid_t->last_set_point = pid_t->Target;
    
    return pid_t->pwm;
}

/**
 * @brief 初始化位置式PID控制器
 */
void Position_PIDInit(positionpid_t *pid_t, float Kp, float Ki, float Kd, float Kf, float MaxOutput, float Integral_Separation, float IntegralLimit)
{

    pid_t->Kp = Kp;
    pid_t->Ki = Ki;
    pid_t->Kd = Kd;
    pid_t->Kf = Kf;
    
    pid_t->MaxOutput = MaxOutput;
    pid_t->Integral_Separation = Integral_Separation;
    pid_t->IntegralLimit = IntegralLimit;
    
    pid_t->p_out = 0;
    pid_t->d_out = 0;
    pid_t->i_out = 0;
    pid_t->err = 0;
    pid_t->err_last = 0;
    pid_t->err_change = 0;
    pid_t->error_target = 0;
    pid_t->pwm = 0;
    pid_t->Measured = 0;
    pid_t->Target = 0;
}

/**
 * @brief 清除位置式PID控制器数据
 */
void Clear_PositionPIDData(positionpid_t *pid_t)
{
    pid_t->Target = 0;
    pid_t->Measured = 0;
    pid_t->err = 0;
    pid_t->err_change = 0;
    pid_t->err_last = 0;
    pid_t->p_out = 0;
    pid_t->i_out = 0;
    pid_t->d_out = 0;
    pid_t->pwm = 0;
}

/**
 * @brief  角度位置式PID控制器
 */
float Angle_PID(positionpid_t *pid_t, float target, float measured,float ecd_max)
{

    pid_t->Target = (float)target;
    pid_t->Measured = (float)measured;
    pid_t->err = pid_t->Target - pid_t->Measured;
    
    if(fabs(pid_t->err) > ecd_max/2) {
        if(pid_t->err > 0) {
            pid_t->err = pid_t->err - (ecd_max+1);
        } else {
            pid_t->err = pid_t->err + (ecd_max+1);
        }
    }
    
    pid_t->err_change = pid_t->Measured - pid_t->err_last;
    pid_t->error_target = pid_t->Target - pid_t->last_set_point;
    
    pid_t->p_out = pid_t->Kp * pid_t->err;
    pid_t->i_out += pid_t->Ki * pid_t->err;
    pid_t->d_out = pid_t->Kd * (pid_t->err - pid_t->err_last);
    pid_t->f_out = pid_t->Kf * pid_t->error_target;
    
    abs_limit(&pid_t->i_out, pid_t->IntegralLimit);

    pid_t->pwm = (pid_t->p_out + pid_t->i_out + pid_t->d_out + pid_t->f_out);

    abs_limit(&pid_t->pwm, pid_t->MaxOutput);


    pid_t->err_last = pid_t->err;
    pid_t->last_set_point = pid_t->Target;
    
    return pid_t->pwm;
}


/**
 * @brief  角度环绕帮助函数
 * @param err 差值
 * @param ecd_max_range 编码器全量程 (例如 8192)
 * @return 经过最短路径环绕的差值
 */
static float angle_wrap(float err, float ecd_max_range)
{
    if (fabs(err) > ecd_max_range / 2.0f) 
    {
        if (err > 0) {
            err -= ecd_max_range;
        } else {
            err += ecd_max_range;
        }
    }
    return err;
}


/**
 * @brief 标准增量式PID (带前馈和微分先行)
 * @note 适用于云台内环(速度环)
 */
float PID_Advanced_Calc(pid_advanced_t *pid_t, float target, float measured)
{
    pid_t->Target = target;
    pid_t->Measured = measured;
    pid_t->err = pid_t->Target - pid_t->Measured;

    pid_t->p_out = pid_t->Kp * (pid_t->err - pid_t->err_last);
    
    pid_t->i_out = pid_t->Ki * pid_t->err;
    abs_limit(&pid_t->i_out, pid_t->IntegralLimit);

    pid_t->d_out = pid_t->Kd * (2.0f * pid_t->measured_last - pid_t->Measured - pid_t->measured_beforeLast);

    pid_t->f_out = pid_t->Kf * (pid_t->Target - pid_t->target_last);

    pid_t->delta_pwm = pid_t->p_out + pid_t->i_out + pid_t->d_out + pid_t->f_out;
    
    pid_t->pwm += pid_t->delta_pwm;

    abs_limit(&pid_t->pwm, pid_t->MaxOutput);

    pid_t->err_last = pid_t->err;
    pid_t->measured_beforeLast = pid_t->measured_last;
    pid_t->measured_last = pid_t->Measured;
    pid_t->target_last = pid_t->Target;

    return pid_t->pwm;
}


/**
 * @brief 初始化高级PID控制器
 */
void PID_Advanced_Init(pid_advanced_t *pid_t, float Kp, float Kd, float Ki, float kf, float MaxOutput, float IntegralLimit)
{
    pid_t->Kp = Kp;
    pid_t->Ki = Ki;
    pid_t->Kd = Kd;
    pid_t->Kf = kf;
    pid_t->MaxOutput = MaxOutput;
    pid_t->IntegralLimit = IntegralLimit;
    
    PID_Advanced_Clear(pid_t);
}

/**
 * @brief  清除高级PID控制器数据
 */
void PID_Advanced_Clear(pid_advanced_t *pid_t)
{
    pid_t->Target = 0;
    pid_t->Measured = 0;
    pid_t->err = 0;
    pid_t->err_last = 0;
    pid_t->measured_last = 0;
    pid_t->measured_beforeLast = 0;
    pid_t->target_last = 0;
    pid_t->p_out = 0;
    pid_t->i_out = 0;
    pid_t->d_out = 0;
    pid_t->f_out = 0;
    pid_t->delta_pwm = 0;
    pid_t->pwm = 0;
}
float PID_Advanced_Angle_Calc_Positional(pid_advanced_t *pid_t, float target, float measured, float ecd_max)
{
    float ecd_range = ecd_max + 1.0f; // e.g., 8191 -> 8192

    pid_t->Target = target;
    pid_t->Measured = measured;
    pid_t->err = pid_t->Target - pid_t->Measured;
    pid_t->err = angle_wrap(pid_t->err, ecd_range);

    pid_t->p_out = pid_t->Kp * pid_t->err;
    pid_t->i_out += pid_t->Ki * pid_t->err;
    abs_limit(&pid_t->i_out, pid_t->IntegralLimit);

    float delta_measured = pid_t->Measured - pid_t->measured_last;
    delta_measured = angle_wrap(delta_measured, ecd_range);
    pid_t->d_out = pid_t->Kd * delta_measured; 
    float delta_err = pid_t->err - pid_t->err_last;
    pid_t->d_out = pid_t->Kd * delta_err;
    pid_t->f_out = 0; 
    pid_t->pwm = pid_t->p_out + pid_t->i_out + pid_t->d_out + pid_t->f_out;

    abs_limit(&pid_t->pwm, pid_t->MaxOutput);

    pid_t->err_last = pid_t->err;
    pid_t->measured_last = pid_t->Measured;
    pid_t->target_last = pid_t->Target;

    return pid_t->pwm;
}


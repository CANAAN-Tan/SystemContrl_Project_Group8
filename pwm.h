/*
 * Project Name : DCmoter PID control
 * File    Name : pwm.h
 * Author       : GROUP-8: TAN Chunyang.  MAO Longwei.  WANG Xuhao.
 * Date         : 2023/05/29
 */

#ifndef PWM_H_
#define PWM_H_

void Port_Init();
void Timer_Init();
void PWM_Init();
void backward();
void PIDControl();

#endif /* PWM_H_ */

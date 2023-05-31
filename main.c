/*
 * Project Name : DCmoter PID control
 * File    Name : main.c
 * Author       : GROUP-8: TAN Chunyang.  MAO Longwei.  WANG Xuhao.
 * Date         : 2023/05/29
 */

#include <msp430.h>
#include "Library_Display.h"
#include "pwm.h"

//  Function main
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;      // setting the system clock

    Port_Init();
    Timer_Init();
    PWM_Init();
    backward();
    Display_Init();
    Display_Clear();
    affiche_Vitesse(0);
    __enable_interrupt();       // enable interrupts

    while(1){
    }
}





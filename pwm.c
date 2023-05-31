/*
 * Project Name : DCmoter PID control
 * File    Name : pwm.c
 * Author       : GROUP-8: TAN Chunyang.  MAO Longwei.  WANG Xuhao.
 * Date         : 2023/05/29
 */

#include <msp430.h>

// DCmotor speed
#define speed1 100
#define speed2 140
#define speed3 160

// variable define
int   target_speed=0;
int   pwm_value = 130;
int   current_speed = 0;
long  turns = 0;                            // incrementalEncoder turns
float E1=0,E2=0,E3=0;                       // PID error
float Kp=25, Ki=35, Kd=1.5;                 // PID parameter
float final=0,temp=0.0,increment=0.0;       // PID out value

void Port_Init(){
    /********************************/
    /*        Pin connection        */
    /********************************/
    /*    MSP430    |     Board     */
    /*     P2.0     |      IN1      */
    /*     P2.1     |      ENA      */
    /*     P2.2     |      IN2      */
    /*     P2.6     |    Output A   */
    /*     P2.7     |    Output B   */
    /*     P1.3     |     Button    */
    /*..............................*/

    P2DIR |= (BIT0 | BIT2);
    P2DIR |= BIT1;

    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1IE  |= BIT3 ;
    P1IES |= BIT3;
    P1IFG &= ~BIT3;

    P2SEL  &= ~BIT6;
    P2SEL2 &= ~BIT6;
    P2DIR  &= ~BIT6;
    P2REN  |= BIT6;
    P2OUT  |= BIT6;
    P2IES  |= BIT6;
    P2IE   |= BIT6 ;
}

void Timer_Init()
{
    TA0CTL = 0 ;
    TA0CTL |= (TASSEL_2 | ID_1);
    TA0CTL |= MC_1;
    TA0CTL |= TAIE;
    TA0CCR0 = 50000;                    // Set instruction cycle:100ms
    TA0CTL &= ~TAIFG;
}

void PWM_Init()
{
    P2DIR  |= BIT1;
    P2SEL  |= BIT1;
    P2SEL2 &= ~BIT1;
    TA1CTL = MC_1 + TASSEL_2 + ID_0;
    TA1CCTL1 |= OUTMOD_7;
    TA1CCR0 = 20000;
}

void backward()
{
    P2DIR &= ~BIT0;
    P2DIR |= BIT2;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void SpeedControl(void)
{
    current_speed = (turns / 3) * 10;   // calculate the speed for 1 second
    affiche_Vitesse(current_speed);     // display
    turns = 0;
    PIDControl();                       // PID control
    TA0CTL &= ~TAIFG;                   // clear flag
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    turns++;
    P2IFG &= ~(BIT6);
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    __delay_cycles(300);
    int gear = (P2IN & BIT6) ? 0 : ((P2IN & BIT0) ? 1 : 2);   // Read the value of the encoder

    if (gear == 0) {
        pwm_value = speed1;
    } else if (gear == 1) {
        pwm_value = speed2;
    } else if (gear == 2) {
        pwm_value = speed3;
    }
    P1IFG &= ~(BIT3);               // clear interrupt flag
}

void PIDControl()
{
    E1 = pwm_value-current_speed;
    increment = (Kp*(E1-E2)+Ki*E1+Kd*(E1-2*E2+E3))/100;
    final = temp+increment;
    target_speed = (int)final;      //Convert the output value
    if(target_speed > 200)
    {
        target_speed = 200;         //To prevent more than the maximum
    }
    else if(target_speed < 0)
    {
        target_speed = 0;           //To prevent the lower than the maximum
    }
    temp = final;
    E3 = E2;
    E2 = E1;
    TA1CCR1 = (100*target_speed);   //store the result in Timer1
}

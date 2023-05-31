#include "Library_Display.h"
#include "msp430.h"

/********************************/
/*        Pin connection        */
/********************************/
/* Connection for MSP430G2452   */
/*    MSP430    |    Display    */
/*     P2.0     |      SSN      */      
/*     P1.6     |      SDI       */
/*     P1.5     |      SCK      */
/*..............................*/
/* Connection for MSP430G2553   */
/*    MSP430    |    Display    */
/*     P1.0     |      SSN      */      
/*     P1.7     |      SDI       */
/*     P1.5     |      SCK      */
/*..............................*/


/*...................................................................................*/
/* void Init_USI_SPI()                                                               */
/*...................................................................................*/
/* Description : Initialisation of the SPI bus                                       */
/* Inputs :                                                                          */
/*      - void                                                                       */
/* Output :                                                                          */                                  
/*      - void                                                                       */
/*...................................................................................*/
void Init_USI_SPI()
{
  #if defined __MSP430G2452__
    P2DIR |= (0 | BIT0);
    P2OUT &= ~(0 | BIT0);
    USICTL0 = 0;
    USICTL0 |= (0 | USIOE | USIGE | USIMST | USIPE5 | USIPE6);
    USICTL1 |= (0 );
    USICKCTL |= (0 | USISSEL1 | USIDIV1 | USIDIV0);
  #elif defined __MSP430G2553__
    UCB0CTL1 = UCSWRST;
    P1DIR |= BIT0; // CS output
    P1OUT |= BIT0; // CS high
    P1SEL |= BIT7 + BIT5;  // Enable SIMO + SCLK
    P1SEL2 |= BIT7 + BIT5; // Enable SIMO + SCLK  
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2; // SMCLK
    UCB0CTL1 &= ~UCSWRST;    
  #endif
}

/*...................................................................................*/
/* void Display_Init()                                                               */
/*...................................................................................*/
/* Description : Initialisation of the display                                       */
/* Inputs :                                                                          */
/*      - void                                                                       */
/* Output :                                                                          */                                  
/*      - void                                                                       */
/*...................................................................................*/
void Display_Init()
{
  Init_USI_SPI();
  Display_Clear();
  Display_4char("8888");
  Display_Points(0x3F);
  Display_SetLuminosity(255);
  __delay_cycles(1000000);
  Display_Clear();
}

/*...................................................................................*/
/* void Send_char_SPI(unsigned char carac)                                           */
/*...................................................................................*/
/* Description : Send a char via SPI                                                 */
/* Inputs :                                                                          */
/*      - unsigned char carac : the char to send                                     */
/* Output :                                                                          */                                  
/*      - void                                                                       */
/*...................................................................................*/
void Send_char_SPI(unsigned char carac)
{
  #if defined __MSP430G2452__
    P2OUT &= ~BIT0;
    USISRL = carac;
    USICNT = 8;
    while ((USICTL1 & USIIFG) == 0);
    P2OUT |= BIT0;
  #elif defined __MSP430G2553__
    P1OUT &= ~BIT0;  // CS low
    UCB0TXBUF = carac; // Put caracter in transmit buffer
    while (!(IFG2 & UCB0TXIFG)); // USCI_A0 TX buffer ready?
    P1OUT |= BIT0; // CS high
  #endif
}

/*...................................................................................*/
/* void Display_4char(unsigned char buffer[4])                                       */
/*...................................................................................*/
/* Description : Display four characters                                             */
/* Inputs :                                                                          */
/*      - unsigned char buffer[4] : characters to display                            */
/* Output :                                                                          */                                  
/*      - void                                                                       */
/*...................................................................................*/
void Display_4char(unsigned char buffer[4])
{
  Send_char_SPI(buffer[0]);
  Send_char_SPI(buffer[1]);
  Send_char_SPI(buffer[2]);
  Send_char_SPI(buffer[3]);
} 

/*...................................................................................*/
/* void Display_SetLuminosity(unsigned char lum)                                     */
/*...................................................................................*/
/* Description : Clear the display                                                   */
/* Inputs :                                                                          */
/*      - void                                                                       */
/* Output :                                                                          */                                  
/*      - void                                                                       */
/*...................................................................................*/
void Display_Clear()
{
  Send_char_SPI(0x76);
  Display_Points(0x00);
}

/*...................................................................................*/
/* void Display_SetLuminosity(unsigned char lum)                                     */
/*...................................................................................*/
/* Description : Change the luminosity of the display                                */
/* Inputs :                                                                          */
/*      - unsigned char lum : Value of the luminosity between 0 (min) and 255 (max)  */
/* Output : void                                                                     */
/*...................................................................................*/
void Display_SetLuminosity(unsigned char lum)
{
  Send_char_SPI(0x7A);
  Send_char_SPI(lum);
}

/*...................................................................................*/
/* void Display_Points(unsigned char code)                                           */
/*...................................................................................*/
/* Description : Swicth on points on the display                                     */
/* Inputs :                                                                          */
/*      - unsigned char code :  points to switch on                                  */
/* Output : void                                                                     */
/*...................................................................................*/
void Display_Points(unsigned char code)
{
  Send_char_SPI(0x77);
  Send_char_SPI(code);
}

/*...................................................................................*/
/* void Display_1char( unsigned char num, unsigned char code)                        */
/*...................................................................................*/
/* Description : Display only one character                                          */
/* Inputs :                                                                          */
/*      - unsigned char num : number of the character (between 1 and 4 )             */
/*      - unsigned char code : segments to swicth on                                 */
/* Output : void                                                                     */
/*...................................................................................*/
void Display_1char(unsigned char num, unsigned char code)
{
  num %=5;
  num +=0x7A;
  Send_char_SPI(num);
  Send_char_SPI(code);
}


void affiche_Vitesse( int vitesse )  //defined a file called affiche_ Vitesse's function, which takes an integer parameter vitesse
{
  int chiffre[10] = {63,6,91,79,102,109,125,7,127,111};
  //Declared and initialized an integer array called chiffre, which contains the display codes corresponding to each number from 0 to 9
  int vit = vitesse; //Assign the passed in vitesse parameter value to the variable vit
  int k, l, m, n;
  //Declared four integer variables u, d, c, and m to store the single, tens, hundreds, and thousands of digits of speed
  k = vit%10;   //Module the speed vit to 10 to obtain one digit
  l = ((vit-k)/10)%10;  //obtain the remaining digits after removing one digit, and then modulo 10 to obtain ten digits
  m = ((vit-k-10*l)/100)%10;    //obtain the remaining digits after removing the ones and tens, and then modulo 10 to obtain the hundreds
  n = (vit-k-10*l-100*m)/1000;  //Obtain the remaining digits after removing the ones, tens, and hundreds and assign them to thousands of digits

  if(n<10)  // If the thousands are less than 10, the speed is less than 10000
  {
    Display_1char(1, chiffre[n]);
    Display_1char(2, chiffre[m]);
    Display_1char(3, chiffre[l]);
    Display_1char(4, chiffre[k]);
  }//displays the display code chiffre corresponding to thousands of digits in a position order by order
}

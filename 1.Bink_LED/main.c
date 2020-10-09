/*
 * LED1�̵ơ���P1.0 LED2��ơ���P1.6
*/
#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	/*��P1.0����Ϊ�����P1DIR��P1�ڵķ���Ĵ���
	 * PxDIR     0�������룬1�������*/

	P1DIR |= BIT0 | BIT6;
	
	/*��P1.0����Ϊ��P1DIR��P1�ڵ�д�Ĵ���
	 * PxOUT    0�����͵�ƽ��1�����ߵ�ƽ
	 * PxIN     ���Ĵ���*/

	/*��LED*/
    //P1OUT |= BIT0 | BIT6;

	/*�ر�LED*/
	P1OUT &= ~BIT0 & ~BIT6;

	while(1)
	{
	    /*��˸*/
	    P1OUT ^= BIT0 ^ BIT6;
	    __delay_cycles(500000);
	}
	return 0;
}

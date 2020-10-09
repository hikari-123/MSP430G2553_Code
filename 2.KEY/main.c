#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/*��ʼ��LED2Ϊ���*/
	P1DIR |= BIT6;

	/*��ʼ��LED2Ϊ�͵�ƽ��Ϩ��*/
	P1OUT &= ~BIT6;

	/*��ʼ��KEY P1.3Ϊ����*/
	P1DIR &= ~BIT3;
	/*ʹ��P1.3�ڵ���������*/
	P1REN |= BIT3;
	P1OUT |= BIT3;

	while(1)
	{
	    if(P1IN & BIT3) /*���P1.3��Ϊ�ߵ�ƽ��֤������û�б�����*/
	    {
	        P1OUT &= ~BIT6; //Ϩ��LED2
	    }
	    else
	    {
	        P1OUT |= BIT6;  //����LED2
	    }
	}
	return 0;
}

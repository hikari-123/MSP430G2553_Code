#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    /*��ʼ��LED2Ϊ���*/
    P1DIR |= BIT6;

    /*��ʼ��LED2Ϊ�͵�ƽ��Ϩ��*/
    P1OUT &= ~BIT6;

    /*��ʼ��KEY P1.3Ϊ����*/
    P1DIR &= ~BIT3;
    /*ʹ��P1.3�ڵ���������*/
    P1REN |= BIT3;
    P1OUT |= BIT3;

    /*��P1.3�ڵ��ж�*/
    P1IE |= BIT3;
    /*�趨Ϊ�½��ش���*/
    P1IES |= BIT3;
    /*����жϱ�־λ*/
    P1IFG &= ~BIT3;
    /*��ȫ���ж�*/
    __bis_SR_register(GIE);

    while(1)
    {
    }
    return 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void)    //Port1_ISR(void)   �жϷ�����
{
    if(P1IFG & BIT3)   //�ж��Ƿ�P1.3�����ж�
    {
        P1OUT ^= BIT6;
        P1IFG  &= ~ BIT3;   //�����־λ
    }
}

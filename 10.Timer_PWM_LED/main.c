#include <msp430.h> 

int main(void)
{
    unsigned int cnt = 0;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/*����DCOΪ1MHZʱ��*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    /*��ʼ��P1.6Ϊ����*/
    P1DIR &= ~BIT6;

    /*����ʱ��ԴΪSMCLK*/
    TA1CTL |= TASSEL_2;
    /*���ù���ģʽΪUp & Dowm*/
    TA1CTL |= MC_0 | MC_1;
    /*����TA1CCR0Ϊ0x00ff*/
    TA1CCR0 = 0x00FF;
    /*����TA1CCR2Ϊ0x00ff*/
    TA1CCR2 = 0x00FF;   //ռ�ձ� = ��TACCR0 - TACCR2��/ TACCR0 Ƶ�� = SMCLK / (TACCR0+1)/2

    /*����Ϊ�Ƚ�ģʽ*/
    TA1CCTL0 &= ~CAP;
    TA1CCTL2 &= ~CAP;
    /*���ñȽ����ģʽ*/
    TA1CCTL2 |= OUTMOD_6;
    /*����IO����*/
    P2SEL |= BIT5;
    P2DIR |= BIT5;

    while(1)
    {
        for(cnt = 0;cnt < 0x00FF; cnt++)
        {
           TA1CCR2 =  cnt;
           __delay_cycles(5000);
        }

        for(cnt = 0x00FF;cnt > 0; cnt--)
        {
           TA1CCR2 =  cnt;
           __delay_cycles(5000);
        }
    }

	return 0;
}

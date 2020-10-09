#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    /*����MCLKƵ��1,8,12,16*/
    DCOCTL = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;

    /*P1.6����Ϊ���*/
    P1DIR |= BIT0 | BIT6;
    /*��LED*/
    //P1OUT |= BIT0 | BIT6;
    /*�ر�LED*/
    P1OUT &= ~BIT0 & ~BIT6;

    while(1)
    {
        P1OUT ^= BIT0 ^ BIT6;
        __delay_cycles(500000);
    }
    return 0;
}

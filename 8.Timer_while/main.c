#include <msp430.h> 
#include "stdint.h"

int main(void)
{
    uint8_t cnt = 0;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	/*����DCOΪ1MHZʱ��*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

   P1DIR |= BIT0 ;

   /*����ʱ��ԴΪSMCLK*/
   TA1CTL |= TASSEL_2;
   /*���ù���ģʽΪUp Mode*/
   TA1CTL |= MC_1;
   /*���ö�ʱʱ����*/
   TA1CCR0 = 49999;     //0.05s
	while(1)
	{
	    if(TA1CTL & TAIFG)
	    {
	        cnt++;
	        TA1CTL &= ~TAIFG;
	        if(cnt == 20)
	        {
	            P1OUT ^= BIT0;
	            cnt = 0;
	        }
	    }
	}

	return 0;
}

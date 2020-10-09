#include <msp430.h> 
#include "stdint.h"

float freq = 0;
float capvalue_1 = 0;
float capvalue_2 = 0;
float timestamp_1 = 0;
float timestamp_2 = 0;
float timestamp = 0;
float totaltime = 0;


/*
 * @fn:    void InitSystemClock(void)
 * @brief: ��ʼ��ϵͳʱ��
 * @para:  none
 * @return:none
 * @comment: ��ʼ��ϵͳʱ��
 */
void InitSystemClock(void)
{
    /*����DCOΪ1MHZʱ��*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    /*����SMCLK��ʱ��ԴΪDCO*/
    BCSCTL2 &= ~SELS;
    /*SMCLK�ķ�Ƶϵ����Ϊ1*/
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

/*
 * @fn:    void InitUART(void)
 * @brief: ��ʼ�����ڣ��������ò����ʣ�����λ��У��λ��
 * @para:  none
 * @return:none
 * @comment: ��ʼ������
 */
void InitUART(void)
{
    /*��λUSCI_Ax*/
    UCA0CTL1 |= UCSWRST;

    /*����Ϊ�첽ģʽ*/
    UCA0CTL0 &= ~UCSYNC;

    /*����UARTʱ��ԴΪSMCLK*/
    UCA0CTL1 |= UCSSEL1;

    /*���ò�����Ϊ9600*/
    UCA0BR0 = 0x68;
    UCA0BR1 = 0x00;
    UCA0MCTL = 1 << 1;

    /*���ö˿�,ʹ�ܶ˿ڸ���*/
    P1SEL   |= BIT1 + BIT2;
    P1SEL2  |= BIT1 + BIT2;

    /*�����λλ��ʹ��UART*/
    UCA0CTL1 &= ~UCSWRST;

    /*�����ж�����*/
    IE2 |= UCA0RXIE;
    /*��ս����жϱ�־*/
    IFG2 &= ~UCA0RXIFG;
}

/*
 * @fn:    void UARTSendString(uint8_t *pbuff, uint_8 num)
 * @brief: ��ʼ�����ڷ����ַ���
 * @para:  pbuff:ָ��Ҫ�����ַ�����ָ��
 *         num:Ҫ���͵��ַ�����
 * @return:none
 * @comment: ��ʼ�����ڷ����ַ���
 */
void UARTSendString(uint8_t *pbuff, uint8_t num)
{
    uint8_t cnt = 0;
    for(cnt = 0; cnt < num; cnt++)
    {
        /*�ж��Ƿ����������ڷ���*/
        while(UCA0STAT & UCBUSY);
        UCA0TXBUF = *(pbuff + cnt);
    }
}

/*
 * @fn:    void PrintFreq(float freq)
 * @brief: ͨ�����ڷ���Ƶ��
 * @para:  freq:Ƶ��
 * @return:none
 * @comment: ͨ�����ڷ���Ƶ��
 */
void PrintFreq(float freq)
{
    uint32_t temp = (uint32_t)(freq * 1000);
    uint8_t charbuff[] = {0,0,0,0,0,0,0,0,0};   //���Ϊ999999.999Hz
    int8_t cnt = 0;
    for(cnt = 8;cnt >= 0; cnt--)
    {
        charbuff[cnt] = (uint8_t)(temp % 10) + '0';
        temp /= 10;
    }
    UARTSendString("Ƶ��Ϊ��",8);
    UARTSendString(charbuff,6);
    UARTSendString(".",1);
    UARTSendString(charbuff + 6, 3);
    UARTSendString("Hz\n",4);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    InitSystemClock();
    InitUART();

    /*����ʱ��ԴΪSMCLK*/
    TA1CTL |= TASSEL_2;
    /*���ù���ģʽΪUp Mode*/
    TA1CTL |= MC_1;
    /*����ʱ����*/
    TA1CCR0 = 49999;
    /*����TAIFG�ж�*/
    TA1CTL = TAIE;

    /*TA1CCR2���ڲ�׽����*/
    TA1CCTL2 |= CAP;
    /*�����ز�׽*/
    TA1CCTL2 |= CM0;
    /*P2.5��Ϊ��׽����(CCIS2B)*/
    TA1CCTL2 |= CCIS0;
    P2SEL |= BIT5;
    /*����׽�Ƚ��ж�*/
    TA1CCTL2 |= CCIE;

    /*����ʱ��ԴΪSMCLK*/
    TA0CTL |= TASSEL_1;
    /*���ù���ģʽΪUp & Dowm*/
    TA0CTL |= MC_0 | MC_1;
    /*����TA1CCR0Ϊ0x00ff*/
    TA0CCR0 = 0x0AAA;
    /*����TA1CCR2Ϊ0x00ff*/
    TA0CCR1 = 0x0555;   //ռ�ձ� = ��TACCR0 - TACCR2��/ TACCR0 Ƶ�� = SMCLK / (TACCR0+1)/2

    /*����Ϊ�Ƚ�ģʽ*/
    TA0CCTL0 &= ~CAP;
    TA0CCTL1 &= ~CAP;
    /*���ñȽ����ģʽ*/
    TA0CCTL1 |= OUTMOD_6;
    /*����IO����*/
    P1SEL |= BIT6;
    P1DIR |= BIT6;

    /*�����ж�*/
    __bis_SR_register(GIE);
    while(1)
    {
        __delay_cycles(500000);
        freq = (float)(1000000.0) / totaltime;
        PrintFreq(freq);
    }

    return 0;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_Tick(void)
{
    static uint8_t cnt = 0;
    __bis_SR_register(GIE);//�����ж�Ƕ��
    switch(TA1IV) //��ȡ�Ļ������ֶ������־λ
    {
    case 0x02:  //��׽�Ƚ��ж�1
        break;
    case 0x04:  //��׽�Ƚ��ж�2
        if(cnt == 0)
        {
            capvalue_1 = TA1CCR2;   //�����һ�β�׽ֵ
            timestamp_1 = timestamp;    //�����һ��ʱ���
            cnt++;
        }
        else
        {
            capvalue_2 = TA1CCR2;   //����ڶ��β�׽ֵ
            timestamp_2 = timestamp;    //����ڶ���ʱ���
            cnt = 0;
            totaltime = (timestamp_2 - timestamp_1) * 50000 + capvalue_2 - capvalue_1; //������ʱ��
        }
        break;
    case 0x0A:  //����ж�
        timestamp ++;
        break;
    default:
        break;
    }
}

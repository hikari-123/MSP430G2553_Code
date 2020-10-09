#include <msp430.h> 
#include "stdint.h"

uint32_t currenttime = 40500;   //��������ʱ��ı�������ֵ����11:15:00
uint8_t flag = 0;
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
 * @fn:    void PrintTime(uint32_t time)
 * @brief: ��ʼ�����ڷ����ַ���
 * @para:  pbuff:ָ��Ҫ�����ַ�����ָ��
 *         num:Ҫ���͵��ַ�����
 * @return:none
 * @comment: ��ʼ�����ڷ����ַ���
 */
void PrintTime(uint32_t time)
{
    uint8_t charbuff[] = {0,0,':',0,0,':',0,0,'\n'};
    charbuff[7] = (uint8_t)((time % 60) % 10) + '0';    //�õ���ǰ���λ
    charbuff[6] = (uint8_t)((time % 60) / 10) + '0';    //�õ���ǰ��ʮλ
    charbuff[4] = (uint8_t)((time % 3600) / 60 % 10) + '0';    //�õ���ǰ�ָ�λ
    charbuff[3] = (uint8_t)((time % 3600) / 60 / 10) + '0';    //�õ���ǰ��ʮλ
    charbuff[1] = (uint8_t)((time / 3600) % 10) + '0';    //�õ���ǰʱ��λ
    charbuff[0] = (uint8_t)(time / 3600 / 10) + '0';    //�õ���ǰʱʮλ
    UARTSendString("��ǰʱ�䣺",10);
    UARTSendString(charbuff,9);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    /*����DCOΪ1MHZʱ��*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    InitSystemClock();
    InitUART();

    P1DIR |= BIT0 ;

    /*����ʱ��ԴΪSMCLK*/
    TA1CTL |= TASSEL_2;
    /*���ù���ģʽΪUp Mode*/
    TA1CTL |= MC_1;
    /*���ö�ʱʱ����*/
    TA1CCR0 = 49999;     //0.05s

    /*�򿪶�ʱ��TAIFG�ж�*/
    TA1CTL |= TAIE;
    /*��ȫ���ж�*/
    __bis_SR_register(GIE);

    while(1)
    {
        if(flag == 1)
        {
            flag = 0;
            P1OUT ^= BIT0;
            PrintTime(currenttime);
        }
    }

    return 0;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_Tick(void)
{
    static uint8_t cnt = 0;
    switch(TA1IV) //��ȡ�Ļ������ֶ������־λ
    {
    case 0x02:
        break;
    case 0x04:
        break;
    case 0x0A:
        cnt++;
        if(cnt == 20)
        {
            cnt = 0;
            flag = 1;   //1sʱ�䵽��
            currenttime ++; //ʱ���1
            currenttime %= 86400;   //һ��24Сʱ����ֹ���
        }
        break;
    default:
        break;
    }
}


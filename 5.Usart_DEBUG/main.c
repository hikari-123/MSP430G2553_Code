#include <msp430.h> 
#include "stdint.h"
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
 * @fn:    void PrintNumber(uint16_t num)
 * @brief: ��ʼ�����ڷ�������
 * @para:  num������
 * @return:none
 * @comment: ��ʼ�����ڷ�������
 */
void PrintNumber(uint16_t num)
{
    uint8_t cnt = 0;
    uint8_t buff[6] = {0,0,0,0,0,'\n'};

    for(cnt = 0; cnt < 5; cnt++)
    {
        buff[4 - cnt] = (uint8_t)(num % 10 + '0');
        num /= 10;
    }
    UARTSendString(buff,6);
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    InitSystemClock();
    InitUART();

    while(1)
    {
        PrintNumber(23456);
        __delay_cycles(500000);
    }

    return 0;
}

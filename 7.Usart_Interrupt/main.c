#include <msp430.h> 
#include "stdint.h"

uint8_t combuff[20] = {0};  //����Ϊ20��������������ڱ��洮�ڽ��յ�������
uint8_t iscomend = 0;   //���������־λ
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

/*
 * @fn:    void Execute(uint8_t *combuff)
 * @brief: ��������ִ�к���
 * @para:  combuff��ָ�򴮿����������ָ��
 * @return:none
 * @comment: ��������ִ�к���
 */
void Execute(uint8_t *combuff)
{
    const uint8_t charbuff[5][10] = {"����","������","�೤","LED1 ON!","LED1 OFF!"};
    if(combuff[0] == charbuff[0][0] && combuff[1] == charbuff[0][1])
    {
        UARTSendString("aa",2);
    }
    else if(combuff[0] == charbuff[1][0] && combuff[1] == charbuff[1][1])
    {
        UARTSendString("bb",2);
    }
    else if(combuff[0] == charbuff[2][0] && combuff[1] == charbuff[2][1])
    {
        UARTSendString("cc",2);
    }
    else if(combuff[0] == charbuff[3][0] && combuff[6] == charbuff[3][6])
    {
        UARTSendString("Yes,LED ON!",11);
        P1OUT |= BIT0;
    }
    if(combuff[0] == charbuff[4][0] && combuff[6] == charbuff[4][6])
    {
        UARTSendString("Yes,LED OFF!",12);
        P1OUT &= ~BIT0;
    }
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    InitSystemClock();
    InitUART();
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    __bis_SR_register(GIE);//�����ж�
    while(1)
    {
       if(iscomend)
       {
           iscomend = 0; //�����־λ����ֹ�ظ�ִ��
           Execute(combuff);
       }
    }

    return 0;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void UART_Receive_ISR(void)    //Port1_ISR(void)   �жϷ�����
{
    static uint8_t cnt = 0;
    if(IFG2 & UCA0RXIFG)//����Ƿ���USCI_AO�Ľ����жϣ�USCI_AO��USCI_BO�Ľ����жϹ���ͬһ����
    {

        IFG2 &= ~UCA0RXIFG; //��ս����жϱ�־
        combuff[cnt++] = UCA0RXBUF; //��������
        cnt %= 20;  //��ֹcnt����20�����»��������
        if(combuff[cnt - 1] == '\n')
        {
            cnt = 0;    //��λ������
            iscomend = 1;   //���������ϱ�־
        }
    }
    /*��ս����жϱ�־*/
    IFG2 &= ~UCA0RXIFG;
}


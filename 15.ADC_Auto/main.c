#include <msp430.h> 
#include "stdint.h"

uint16_t adcbuff[50] = {0};
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
 * @fn:    void PrintFloat(float num)
 * @brief: ��ʼ�����ڷ��͸���������
 * @para:  num�������ͱ���
 * @return:none
 * @comment: ��ʼ�����ڷ��͸���������
 */
void PrintFloat(float num)
{
    uint8_t buff[] = {0,'.',0,0,0,'\n'};
    uint16_t temp = (uint16_t)(num * 1000);
    buff[0] = (uint8_t)(temp / 1000) + '0';
    buff[2] = (uint8_t)((temp % 1000) / 100) + '0';
    buff[3] = (uint8_t)((temp / 100) / 10) + '0';
    buff[4] = (uint8_t)(temp % 10) + '0';
    UARTSendString(buff,6);
}

/*
 * @fn:    void InitADCTrigByTimerA(void)
 * @brief: ADC��ʼ��
 * @para:  none
 * @return:none
 * @comment: ADC��ʼ��
 */
void InitADCTrigByTimerA(void)
{
    /*����ADCʱ��MCLK*/
    ADC10CTL1 |= ADC10SSEL_2;
    /*ADC 2��Ƶ*/
    //ADC10CTL1 |= ADC10DIV_0;

    /*ADC����Դʹ��TimerA_OUT1*/
    ADC10CTL1 |= SHS0;
    /*��ͨ������ת��ģʽ*/
    ADC10CTL1 |= CONSEQ1;

    /*����ADC��׼Դ*/
    ADC10CTL0 |= SREF_1;
    /*����ADC��������ʱ��16CLK*/
    ADC10CTL0 |= ADC10SHT_2;
    /*����ADC������200k*/
    ADC10CTL0 &= ~ADC10SR;
    /*ADC��׼ѡ��2.5V*/
    ADC10CTL0 |= REF2_5V;
    /*������׼*/
    ADC10CTL0 |= REFON;
    /*ѡ��ADC����ͨ��A4*/
    ADC10CTL1 |= INCH_4;
    /*����A4ģ������*/
    ADC10AE0 |= 1 << 4;

    /*����DTC�Ĵ���ģʽ*/
    ADC10DTC0 |= ADC10CT;
    /*���ô���Ĵ���*/
    ADC10DTC1 = 50;
    /*��ʼ��ַ*/
    ADC10SA = (uint16_t)(adcbuff);

    /*����ADC*/
    ADC10CTL0 |= ADC10ON;
    /*����ת��*/
    ADC10CTL0 |= ENC;
}

/*
 * @fn:    void InitTimerA(void)
 * @brief: ��ʼ����ʱ��
 * @para:  none
 * @return:none
 * @comment: ��ʼ����ʱ��
 */
void InitTimerA(void)
{
    /*����ʱ��ԴΪACLK*/
   TA0CTL |= TASSEL1;
   /*���ù���ģʽΪUp & Dowm*/
   TA0CTL |= MC_0 | MC_1;
   /*����TA1CCR0Ϊ0x00ff*/
   TA0CCR0 = 0x00FF;
   /*����TA1CCR2Ϊ0x00ff*/
   TA0CCR1 = 0x007F;   //ռ�ձ� = ��TACCR0 - TACCR2��/ TACCR0 Ƶ�� = SMCLK / (TACCR0+1)/2

   /*����Ϊ�Ƚ�ģʽ*/
   TA0CCTL0 &= ~CAP;
   TA0CCTL1 &= ~CAP;
   /*���ñȽ����ģʽ*/
   TA0CCTL1 |= OUTMOD_6;
   /*����IO����*/
   P1SEL |= BIT6;
   P1DIR |= BIT6;
}

/*
 * @fn:    uint16_t GetADCValue(void)
 * @brief: ����һ��ADCת��������ADCת�����
 * @para:  none
 * @return:ADCת�����
 * @comment: ADCת�����Ϊ10bit����uint16_t���ͷ��أ���10λ��Ч����
 */
uint16_t GetADCValue(void)
{
    /*��ʼת��*/
    ADC10CTL0 |= ADC10SC|ENC;
    /*�ȴ�ת�����*/
    while(ADC10CTL1 & ADC10BUSY);
    /*���ؽ��*/
    return ADC10MEM;
}

/*
 * @fn:    void StartADCConvert(void)
 * @brief: ����һ��ADCת��
 * @para:  none
 * @return:ADCת�����
 * @comment: ADCת�����Ϊ10bit
 */
void StartADCConvert(void)
{
    /*��ʼת��*/
    ADC10CTL0 |= ADC10SC|ENC;
    /*�ȴ�ת�����*/
    while(ADC10CTL1 & ADC10BUSY);
}

int main(void)
{
    uint8_t cnt = 0;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    InitSystemClock();
    InitUART();
    InitADCTrigByTimerA();
    InitTimerA();

    while(1)
    {
        for(cnt = 0;cnt < 50; cnt++)
        {
            PrintNumber(*(adcbuff+cnt));
        }
        UARTSendString("ADC Sample Finished!\n",21);
        __delay_cycles(1000000);
    }
    return 0;
}

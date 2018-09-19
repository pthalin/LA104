/**********************(C) COPYRIGHT 2017 e-Design Co.,Ltd.*********************
 * FileName    : Analyze.c
 * Description : Э�����
 * Author      : SNAKE 
 * History     :
*******************************************************************************/
#include "MyDefine.h"

u8  AnalyzeBuf[RECORD_DEPTH*2];
//            [0]:��¼λ��
//            [1]:MOSI��������
//            [2]:MISO��������

uc16 BaudRate[10] = { 48, 96, 144, 192, 384, 560, 576, 1152 }; // ��С100��������
u32  AnalyzeTime[RECORD_DEPTH/2];

/*******************************************************************************
 * FunctionName : SPI_AnalyzeFrame
 * Description  : SPIЭ�����
*******************************************************************************/
void SPI_AnalyzeFrame(void)
{
//  SPI_CS_POLAR, SPI_CLK_POLAR, SPI_CLK_PHASE, SPI_BIT_ORDER, SPI_WORD_SIZE, SPI_DATA_FORMAT

//  Э��������ò���
    u8 CS_Polarity = gInputSubParam[IN_SPI][SPI_CS_POLAR];
    u8 CLK_Polarity = gInputSubParam[IN_SPI][SPI_CLK_POLAR];
    u8 CLK_Phase = gInputSubParam[IN_SPI][SPI_CLK_PHASE];
    u8 BIT_Order = gInputSubParam[IN_SPI][SPI_BIT_ORDER];
    u8 Word_Size = gInputSubParam[IN_SPI][SPI_WORD_SIZE];
//  u8 Data_Format = gInputSubParam[IN_SPI][SPI_DATA_FORMAT];

    u8 StartFlag = 0;
    u8 Status = 0;
    u8 SCK_Now = 0;
    u8 SCK_Last = 0;
    u8 SCK_Cnt = 0;
    u8 NextFlag = 0;
    u8 k = 0;

    u16 i = 0, j = 0;

    memset(AnalyzeBuf, 0, RECORD_DEPTH * 2);
    SCK_Last = CLK_Polarity;                               // ʱ��ȱʡΪCPOL����ֵ

    for(i = TRI_START_NUM; i < RECORD_DEPTH; i++)
    {
        Status = TransStatusData(i);                       // 4bit����״̬
        SCK_Now = (Status >> 2) & 0x01;                    // Bit2 SCK�ĵ�ǰ״̬
        if(((Status >> 3) & 0x01) == CS_Polarity)          // Bit3 CSλ���ݷ�������ֵ
        {
//          ���ݿ�ʼλ
            if((StartFlag == 0) && (SCK_Last == CLK_Polarity) && (SCK_Now != CLK_Polarity))
            {
                StartFlag = 1;
                gSamplBuf[4 * i] |= 0xC0;                  // Э���ʼ

                if(CLK_Phase == 1)                         // �ڶ��������ز���
                {
                    if(SCK_Now == CLK_Polarity)
                    {
                        AnalyzeBuf[j * 3 + 0] |= BIT_Order ? ((Status & 0x01) << k) : ((Status & 0x01) << (7 - k));
                        AnalyzeBuf[j * 3 + 1] |= BIT_Order ? (((Status >> 1) & 0x01) << k) : (((Status >> 1) & 0x01) << (7 - k));
                        if(k < 7) k++;
                    }
                }
                else                                       // ��һ�������ز���
                {
                    if(SCK_Now != CLK_Polarity)
                    {
                        AnalyzeBuf[j * 3 + 0] |= BIT_Order ? ((Status & 0x01) << k) : ((Status & 0x01) << (7 - k));
                        AnalyzeBuf[j * 3 + 1] |= BIT_Order ? (((Status >> 1) & 0x01) << k) : (((Status >> 1) & 0x01) << (7 - k));
                        if(k < 7) k++;
                    }
                }
                SCK_Cnt++;
            }
            else if(StartFlag == 1)
            {
                if(SCK_Now != SCK_Last)                    // SCK�仯
                {
                    SCK_Cnt++;
                    if(CLK_Phase == 1)
                    {
                        if(SCK_Now == CLK_Polarity)
                        {
                            AnalyzeBuf[j * 3 + 0] |= BIT_Order ? ((Status & 0x01) << k) : ((Status & 0x01) << (7 - k));
                            AnalyzeBuf[j * 3 + 1] |= BIT_Order ? (((Status >> 1) & 0x01) << k) : (((Status >> 1) & 0x01) << (7 - k));
                            if(k < 7) k++;
                        }
                    }
                    else
                    {
                        if(SCK_Now != CLK_Polarity)
                        {
                            AnalyzeBuf[j * 3 + 0] |= BIT_Order ? ((Status & 0x01) << k) : ((Status & 0x01) << (7 - k));
                            AnalyzeBuf[j * 3 + 1] |= BIT_Order ? (((Status >> 1) & 0x01) << k) : (((Status >> 1) & 0x01) << (7 - k));
                            if(k < 7) k++;
                        }
                    }
                }
                if(SCK_Cnt < (Word_Size * 2))
                {
                    gSamplBuf[4 * i] |= 0xC0;
                }
                else
                {
                    NextFlag = 1;
                    StartFlag = 0;
                    SCK_Cnt = 0;
                    k = 0;
                    j++;
                }
            }
        }
        else
        {
            StartFlag = 0;
            SCK_Cnt = 0;
            k = 0;
            if(NextFlag == 0) j++;
            else NextFlag = 0;
        }
        SCK_Last = SCK_Now;                                // SCK����һ��״̬
    }
}

/*******************************************************************************
 * FunctionName : IIC_AnalyzeFrame
 * Description  : I2CЭ�����
*******************************************************************************/
void IIC_AnalyzeFrame(void)
{
    u8 SDA_Now = 1, SDA_Last = 1;
    u8 SCL_Now = 1, SCL_Last = 1;
    u8 StartFlag = 0;
    u8 All_Status = 0;
    u8 SCK_Status = 0;
    u8 Ack = 0;
    u8 k = 0;
    u8 n = 0;

    u16 i = 0;
    u16 j = 0;

    memset(AnalyzeBuf, 0, RECORD_DEPTH * 2);

    for(i = TRI_START_NUM; i < RECORD_DEPTH; i++)
    {
        SDA_Now = TransStatusData(i) & 0x01;               // ͨ��1ΪSDA
        SCL_Now = (TransStatusData(i) >> 1) & 0x01;        // ͨ��2ΪSCL
        if((k == 0) && (SCL_Now == 1) && (SCL_Last == 1) && (SDA_Last == 1) && (SDA_Now == 0))   // SCLΪ��ʱ��SDA�ɸ߱��Ϊstart��ʶ
        {
            All_Status = 1;
            StartFlag = 1;
        }
        if((Ack == 0) && (SCL_Now == 1) && (SCL_Last == 1) && (SDA_Last == 0) && (SDA_Now == 1)) // SCLΪ��ʱ��SDA�ɵͱ��Ϊstop��ʶ
        {
            for(n = 1; n <= k; n++)
            {
                gSamplBuf[4 * (i - n)] &= 0xAF;            // 0x40λ����
            }
            All_Status = 0;
            k = 0;
            SCK_Status = 0;
        }
        if(All_Status == 1)
        {
            if(SCL_Now ^ SCL_Last)
            {
                if((k == 0) && (SCL_Now == 1)) SCK_Status = 1;

                if(SCL_Now == 1)                           // SCLΪ��
                {
                    k++;
                    if(k < 9)
                    {
                        AnalyzeBuf[j * 3 + 0] |= (SDA_Now << (8 - k));
                    }
                    else if(k == 9)
                    {
                        if(SDA_Now)  Ack = 0;
                        else         Ack = 1;
                    }
                }
            }
            if(k <= 9)
            {
                if(SCK_Status) gSamplBuf[4 * i] |= 0x40;   // 0x40Ϊ���ݽ�����һͨ��
                if(k == 9)
                {
                    if(StartFlag)
                    {
                        StartFlag = 0;
                        AnalyzeBuf[j * 3 + 1] |= 0x01;     // start
                        AnalyzeBuf[j * 3 + 1] |= (AnalyzeBuf[j * 3 + 0] & 0x01) << 1; // ���λ��Ϊ��д 0:W,1:R
                        AnalyzeBuf[j * 3 + 1] |= (Ack & 0x01) << 2;
                    }
                    else
                    {
                        AnalyzeBuf[j * 3 + 1] |= 0x00;     // no start
                        AnalyzeBuf[j * 3 + 1] |= (Ack & 0x01) << 2;
                    }
                    k = 0;
                    j++;
                    SCK_Status = 0;
                }
            }
        }
        SDA_Last = SDA_Now;
        SCL_Last = SCL_Now;
    }
}

/*******************************************************************************
 * FunctionName : Show_I2C_AnalyzeData
 * Description  : 
*******************************************************************************/
void Show_I2C_AnalyzeData(void)
{
    u8  Str[10];
//  u8  Length = 0;
    u8  StopFlag = 0;
    u8  Data_Format = 0;
    u16 StartCnt = 0;
    u16 StopCnt = 0;
    u16 DateCnt = 0;
    u16 DecodeWidth = 0;
    u16 LCD_Point = 0;
    u16 FrameWidth = 0;
    u16 RW_Posi = 0;
    u16 StartPosi = 0;

    Data_Format = gInputSubParam[IN_I2C][I2C_DATA_FORMAT];
    DateCnt = I2C_AnalyzeStartCnt(gLastX_Time);

//  ������ʾ���ַ����Ϳ��
    switch(Data_Format)
    {
    case 0:
        DecodeWidth = ASCII_WIDTH;
        break;
    case 1:
        DecodeWidth = DEX_WIDTH;
        break;
    case 2:
        DecodeWidth = HEX_WIDTH;
        break;
    default:
        break;
    }
//  ���ݵ�ǰ��ʾ��ͼ�ν�������
    while(LCD_Point < (SHOW_WIDTH - 1))
    {
//      һ�����ݴ������
        if((StopFlag == 0) && ((gLCD_Backup[LCD_Point] & 0x10) == 0) && ((gLCD_Backup[LCD_Point + 1] & 0x10) == 0x10))      // �ɵͱ��
        {
            StartCnt = LCD_Point + 3;
        }
        else if((StopFlag == 0) && ((gLCD_Backup[LCD_Point] & 0x10) == 0x10) && ((gLCD_Backup[LCD_Point + 1] & 0x10) == 0)) // �ɸ߱��
        {
            StopCnt = LCD_Point;
            FrameWidth = StopCnt - StartCnt;
            StopFlag = 1;
        }
        if(StopFlag == 1)
        {
            if(AnalyzeBuf[DateCnt * 3 + 1] & 0x01)                                 // ��ʼλ
            {
                if(gInputSubParam[IN_I2C][0]) AnalyzeBuf[DateCnt * 3 + 0] &= 0xFE; // R/W ��0
            }
            switch(Data_Format)                                                    // ������ʾ����
            {
            case ASCII:
                Char2Str(Str, AnalyzeBuf[DateCnt * 3 + 0]);
                break;
            case DEX:
                u8ToDec3Str(Str, AnalyzeBuf[DateCnt * 3 + 0]);
                break;
            case HEX:
                Char2HexFullStr(Str, AnalyzeBuf[DateCnt * 3 + 0]);
                break;
            default:
                break;
            }
            if(FrameWidth > (DecodeWidth / 2))
            {
                if(AnalyzeBuf[DateCnt * 3 + 1] & 0x01)                             // ��ʼλ
                {
                    RW_Posi = 7 * 2;
                    Disp_Str7x9(40 + StartCnt + StartPosi, 102, BLK, WHT, PRN, THIN, (u8*)((AnalyzeBuf[DateCnt * 3 + 1] & 0x02) ? "r:" : "w:"));
                }
                else
                {
                    RW_Posi = 0;
                }
                Disp_Str7x9(40 + StartCnt + RW_Posi + StartPosi, 102, BLK, WHT, PRN, THIN, Str);
                Disp_Str7x9(40 + StartCnt + DecodeWidth + RW_Posi + StartPosi, 102, BLK, WHT, PRN, THIN, (u8*)((AnalyzeBuf[DateCnt * 3 + 1] & 0x04) ? " ACK" : " NAK"));
            }
            StopFlag = 0;
            DateCnt++;
        }
        LCD_Point++;
    }
}

/*******************************************************************************
 * FunctionName : I2C_AnalyzeStartCnt
 * Description  : 
 * Param        : u64 StartTime 
 * Return       : u16   
*******************************************************************************/
u16 I2C_AnalyzeStartCnt(u64 StartTime)
{
    u8  Status = 0;
    u8  StartFlag = 0;
//  u8  StopFlag = 0;
    u8  SCK_Cnt = 0;
    u8  ALL_Start = 0;
    u8  SDA_Now = 1, SDA_Last = 1;
    u8  SCL_Now = 1, SCL_Last = 1;
    u16 i = 0;
    u16 DataCnt = 0;
    u32 TransTime = 0;
    u64 TempTime = 0;

    memset(AnalyzeTime, 0, RECORD_DEPTH / 2);
    for(i = TRI_START_NUM; i < RECORD_DEPTH; i++)
    {
        Status = TransStatusData(i);                        // ��ȡ״̬
        SDA_Now = Status & 0x01;                            // sda״̬
        SCL_Now = (Status >> 1) & 0x01;                     // sck״̬
        TransTime = TransformTime(i + 1);
        AnalyzeTime[DataCnt] += TransTime;
        if((SCL_Now == 1) && (SCL_Last == 1) && (SDA_Last == 1) && (SDA_Now == 0)) // SCLΪ��ʱ��SDA�ɸ߱��Ϊstart��ʶ
        {
            ALL_Start = 1;
        }
        if((SCL_Now == 1) && (SCL_Last == 1) && (SDA_Last == 0) && (SDA_Now == 1)) // SCLΪ��ʱ��SDA�ɵͱ��Ϊstop��ʶ
        {
            ALL_Start = 0;
        }
        if(ALL_Start == 1)
        {
            if((SCL_Last == 0) && (SCL_Now == 1) && (StartFlag == 0))
            {
                StartFlag = 1;
            }
        }
        else
        {
            StartFlag = 0;
            SCK_Cnt = 0;
        }
        if((StartFlag == 1) && (SCL_Now != SCL_Last)) SCK_Cnt++;

        if(StartFlag && (SCK_Cnt == 17))                   // ����һ���ֽ��������
        {
            StartFlag = 0;
            SCK_Cnt = 0;
            DataCnt++;
        }
        SDA_Last = SDA_Now;
        SCL_Last = SCL_Now;
    }
    for(i = 0; i < (RECORD_DEPTH / 2); i++)
    {
        TempTime += AnalyzeTime[i];

        if(TempTime >= StartTime) return i;
    }
    return 0;
}

/*******************************************************************************
 * FunctionName : UART_AnalyzeFrame
 * Description  : UARTЭ�������ȷ��Э���
*******************************************************************************/
void UART_AnalyzeFrame(void)
{
#define  NONE  0
#define  ODD   1                     // ������֤
#define  EVEN  2                     // ż����֤

    u8  NowStatus = 1;               // ��ǰ״̬
    u8  LastStatus = 1;
    u8  StartFlag = 0;
    u8  BIT_Cnt = 0;
    u8  BIT_Sum = 0;
    u8  DataBits = 0;
//  u8  StopBits = 0;
    u8  ParityTpye = 0;
//  u8  InvertSignal = 0;
    u16 i = 0;
    u16 Baud = 0;
    u32 BIT_UnitTime = 0;
    u32 TransTime = 0;               // Transform Time

//  ��ȡ����ѡ���еĲ���
    Baud = BaudRate[gInputSubParam[IN_UART][UART_BAUD_RATE]];
    DataBits = gInputSubParam[IN_UART][UART_DATA_BITS];
//  StopBits = gInputSubParam[IN_UART][UART_STOP_BITS];
    ParityTpye = gInputSubParam[IN_UART][UART_PARITY_TYPE];
//  InvertSignal = gInputSubParam[IN_UART][UART_INVERT_SIGN];

//  BIT_UnitTime = (100000000 / (Baud*100));                     // ��λ��10ns
    BIT_UnitTime = (1000000 / Baud) - (1000000 / Baud / 20);     // ����1/20

    memset(AnalyzeBuf, 0, RECORD_DEPTH * 2);

    for(i = TRI_START_NUM; i < RECORD_DEPTH; i++)
    {
        NowStatus = TransStatusData(i) & 0x01;                   // ͨ��1ΪUART
        TransTime = TransformTime(i + 1);
        if(LastStatus != NowStatus)
        {
//          ����������ڵ�λʱ���1/2����bit��1
            if((TransTime % BIT_UnitTime) > (BIT_UnitTime / 2))
            {
                BIT_Cnt = (TransTime / BIT_UnitTime) + 1;
            }
            else
            {
                BIT_Cnt = TransTime / BIT_UnitTime;
            }
            if((StartFlag == 0) && (LastStatus == 1) && (NowStatus == 0))   // �ɸ߱��Ϊ��ʼλ
            {
                StartFlag = 1;                                              // ���俪ʼλ
                gSamplBuf[4 * i] |= 0x40;                                   // Э���״̬
                if(BIT_Cnt > 0) BIT_Cnt--;                                  // ��ȥ��ʼλ
                if(BIT_Cnt > 0)
                {
                    BIT_Sum += BIT_Cnt;
                    if(ParityTpye == NONE)                                  // ��У��
                    {
                        if(BIT_Sum >= DataBits)
                        {
                            StartFlag = 0;                                  // ���ݽ������
                            BIT_Sum = 0;
                        }
                    }
                    else                                                    // ����У��
                    {
                        if(BIT_Sum >= (DataBits + 1))
                        {
                            StartFlag = 0;
                            BIT_Sum = 0;
                        }
                    }
                }
            }
            else
            {
                if(StartFlag == 1)                                          // �ҵ���ʼλ����ȡ�����DataBitsλ����
                {
                    gSamplBuf[4 * i] |= 0x40;

                    BIT_Sum += BIT_Cnt;
                    if(ParityTpye == NONE)
                    {
                        if(BIT_Sum >= DataBits)
                        {
                            StartFlag = 0;
                            BIT_Sum = 0;
                        }
                    }
                    else
                    {
                        if(BIT_Sum >= (DataBits + 1))
                        {
                            StartFlag = 0;
                            BIT_Sum = 0;
                        }
                    }
                }
            }
        }
        LastStatus = NowStatus;
    }
}

/*******************************************************************************
 * FunctionName : Show_UART_AnalyzeData
 * Description  : ��ʾuartЭ�����������
*******************************************************************************/
void Show_UART_AnalyzeData(void)
{
    u8  i = 0;
    u8  Str[10];
    u8  DataBits = 0;
    u8  AnalyzeData = 0;
    u8  BIT_Order = 0;
    u8  Data_Format = 0;
    u16 Baud = 0;
    u16 DecodeWidth = 0;
    u16 StartPosi = 0;
    u16 LCD_Point = 0;
    u32 FrameWidth = 0;
    u32 BIT_UnitTime = 0;
    u32 BIT_UnitWidth = 0;

    Data_Format = gInputSubParam[IN_UART][UART_DATA_FORMAT];
    BIT_Order = gInputSubParam[IN_UART][UART_BIT_ORDER];
    DataBits = gInputSubParam[IN_UART][UART_DATA_BITS];
    Baud = BaudRate[gInputSubParam[IN_UART][UART_BAUD_RATE]];
//  BIT_UnitTime = (100000000 / (Baud*100));                                         // ��λ��10ns
    BIT_UnitTime = (1000000 / Baud);                                                 // ��λ��10ns
    BIT_UnitWidth = BIT_UnitTime * TIME_TO_POINT / gTB_Scale[gItemParam[TIMEBASE]];  // ����һ��bit��ʱ��ת��ΪLCD����
    FrameWidth = BIT_UnitTime * 8 * TIME_TO_POINT / gTB_Scale[gItemParam[TIMEBASE]]; // ����Э�����

//  ������ʾ���ַ����Ϳ��
    switch(Data_Format)
    {
    case 0:
        DecodeWidth = ASCII_WIDTH;
        break;
    case 1:
        DecodeWidth = DEX_WIDTH;
        break;
    case 2:
        DecodeWidth = HEX_WIDTH;
        break;
    default:
        break;
    }
//  �ַ���ʾ�Ŀ�ʼλ��
    StartPosi = (FrameWidth - DecodeWidth) / 2;
//  ���ݵ�ǰ��ʾ��ͼ�ν�������
    while(LCD_Point < (SHOW_WIDTH - 1))
    {
//      һ�����ݴ������
        if((gLCD_Backup[LCD_Point] & 0x10) && ((gLCD_Backup[LCD_Point + 1] & 0x10) == 0)) // �ɸ߱��
        {
            if(LCD_Point > BIT_UnitWidth * DataBits)
            {
                AnalyzeData = 0;                                                          // ����ǰ��������

                if(BIT_Order == LSB)
                {
                    for(i = 0; i < DataBits; i++)
                    {
                        AnalyzeData |= ((gLCD_Backup[(LCD_Point - BIT_UnitWidth / 2) - (BIT_UnitWidth * i)] & 0x01) << (7 - i));
                    }
                }
                else                                                                      // MSBģʽ
                {
                    for(i = DataBits; i > 0; i--)
                    {
                        AnalyzeData |= ((gLCD_Backup[(LCD_Point - BIT_UnitWidth / 2) - (BIT_UnitWidth * i)] & 0x01) << i);
                    }
                }
                switch(Data_Format)                                                       // ������ʾ����
                {
                case ASCII:
                    Char2Str(Str, AnalyzeData);
                    break;
                case DEX:
                    u8ToDec3Str(Str, AnalyzeData);
                    break;
                case HEX:
                    Char2HexFullStr(Str, AnalyzeData);
                    break;
                default:
                    break;
                }
                if(LCD_Point > (StartPosi + DecodeWidth))
                {
                    Disp_Str7x9(40 + (LCD_Point - StartPosi - DecodeWidth), 102, BLK, WHT, PRN, THIN, Str);
                }
            }
        }
        LCD_Point++;
    }
}

/*******************************************************************************
 * FunctionName : Show_SPI_AnalyzeData
 * Description  : 
*******************************************************************************/
void Show_SPI_AnalyzeData(void)
{
    u8  Str[10];
    u8  Str1[10];
    u8  StopFlag = 0;
//  u8  CLK_Phase = 0;
//  u8  DataBits = 0;
//  u8  BIT_Order = 0;
//  u8  CS_Polarity = 0;
    u8  Data_Format = 0;
    u16 StartCnt = 0;
    u16 StopCnt = 0;
    u16 DateCnt = 0;
    u16 DecodeWidth = 0;
    u16 LCD_Point = 0;
    u16 FrameWidth = 0;
//  u8  StartFlag = 0;
//  u8  AnalyzeData = 0;
//  u8  BIT_Cnt = 0;
//  u16 i = 0;
//  u16 StartPosi = 0; 
//  u32 BIT_UnitTime = 0;
//  u32 BIT_UnitWidth = 0;

//  CS_Polarity = gInputSubParam[IN_SPI][SPI_CS_POLAR];
//  CLK_Phase = gInputSubParam[IN_SPI][SPI_CLK_PHASE];
//  DataBits = gInputSubParam[IN_SPI][SPI_WORD_SIZE];
//  BIT_Order = gInputSubParam[IN_SPI][SPI_BIT_ORDER];
    Data_Format = gInputSubParam[IN_SPI][SPI_DATA_FORMAT];
    DateCnt = SPI_AnalyzeDataStartCnt(gLastX_Time);

//  ������ʾ���ַ����Ϳ��
    switch(Data_Format)
    {
    case 0:
        DecodeWidth = ASCII_WIDTH;
        break;
    case 1:
        DecodeWidth = DEX_WIDTH;
        break;
    case 2:
        DecodeWidth = HEX_WIDTH;
        break;
    default:
        break;
    }
//  ���ݵ�ǰ��ʾ��ͼ�ν�������
    while(LCD_Point < (SHOW_WIDTH - 1))
    {
//      һ�����ݴ������
        if((StopFlag == 0) && ((gLCD_Backup[LCD_Point] & 0x10) == 0) && ((gLCD_Backup[LCD_Point + 1] & 0x10) == 0x10))     // �ɵͱ��
        {
            StartCnt = LCD_Point + 3;
        }
        else if((StopFlag == 0) && ((gLCD_Backup[LCD_Point] & 0x10) == 0x10) && ((gLCD_Backup[LCD_Point + 1] & 0x10) == 0)) // �ɸ߱��
        {
            StopCnt = LCD_Point;
            FrameWidth = StopCnt - StartCnt;
            StopFlag = 1;
        }
        if(StopFlag == 1)
        {
            switch(Data_Format)    
            {
            case ASCII:
                Char2Str(Str, AnalyzeBuf[DateCnt * 3 + 0]);
                Char2Str(Str1, AnalyzeBuf[DateCnt * 3 + 1]);
                break;
            case DEX:
                u8ToDec3Str(Str, AnalyzeBuf[DateCnt * 3 + 0]);
                u8ToDec3Str(Str1, AnalyzeBuf[DateCnt * 3 + 1]);
                break;
            case HEX:
                Char2HexFullStr(Str, AnalyzeBuf[DateCnt * 3 + 0]);
                Char2HexFullStr(Str1, AnalyzeBuf[DateCnt * 3 + 1]);
                break;
            default:
                break;
            }
            if(FrameWidth > (DecodeWidth / 2))
            {
                Disp_Str7x9(40 + StartCnt, 102, BLK, WHT, PRN, THIN, Str);
                Disp_Str7x9(40 + StartCnt,  77, BLK, WHT, PRN, THIN, Str1);
            }
            StopFlag = 0;
            DateCnt++;
        }
        LCD_Point++;
    }
}

/*******************************************************************************
 * FunctionName : SPI_AnalyzeDataStartCnt
 * Description  : 
 * Param        : u64 StartTime 
 * Return       : u16   
*******************************************************************************/
u16 SPI_AnalyzeDataStartCnt(u64 StartTime)
{
    u8  Status = 0;
    u8  CS_Status = 0;
    u8  StartFlag = 0;
    u8  StopFlag = 0;
    u8  CS_Polarity = 0;
    u8  SCK_Now = 0;
    u8  SCK_Last = 0;
    u8  SCK_Cnt = 0;
    u16 i = 0;
    u16 DataCnt = 0;
    u32 TransTime = 0;
    u64 TempTime = 0;

    CS_Polarity = gInputSubParam[IN_SPI][SPI_CS_POLAR];
    SCK_Last = CS_Polarity;
    memset(AnalyzeTime, 0, RECORD_DEPTH / 2);
    for(i = TRI_START_NUM; i < RECORD_DEPTH; i++)
    {
        Status = TransStatusData(i);             // ��ȡ״̬
        CS_Status = (Status >> 3) & 0x01;        // cs״̬
        SCK_Now = (Status >> 2) & 0x01;          // sck״̬

        TransTime = TransformTime(i + 1);
        AnalyzeTime[DataCnt] += TransTime;

        if(CS_Status == CS_Polarity)
        {
            StartFlag = 1;
        }

        if((StartFlag) && (SCK_Now != SCK_Last)) SCK_Cnt++;

        if((StopFlag == 0) && (CS_Status != CS_Polarity))  // cs����ʱһ�����ݽ���
        {
            StartFlag = 0;
            StopFlag = 1;
            SCK_Cnt = 0;
            DataCnt++;
        }
        else if(StartFlag && (SCK_Cnt >= 16))              // ����һ���ֽ��������
        {
            StartFlag = 0;
            SCK_Cnt = 0;
            DataCnt++;
            StopFlag = 1;
        }
        SCK_Last = SCK_Now;
    }

    for(i = 0; i < (RECORD_DEPTH / 2); i++)
    {
        TempTime += AnalyzeTime[i];

        if(TempTime >= StartTime) return i;
    }
    return 0;
}

/*******************************************************************************
 * FunctionName : AnalyzeFrame
 * Description  : Э��������ݿ��
 * Param        : u8 Type  Э������
*******************************************************************************/
void AnalyzeFrame(u8 Type)
{
    switch(Type)
    {
    case IN_SPI:
        SPI_AnalyzeFrame();
        break;
    case IN_I2C:
        IIC_AnalyzeFrame();
        break;
    case IN_UART:
        UART_AnalyzeFrame();
        break;
    default:
        break;
    }
}

/*******************************************************************************
 * FunctionName : Show_AnalyzeData
 * Description  : ��ʾЭ���������
 * Param        : u8 Type 
*******************************************************************************/
void Show_AnalyzeData(u8 Type)
{
    switch(Type)
    {
    case IN_SPI:
        Show_SPI_AnalyzeData();
        break;
    case IN_I2C:
        Show_I2C_AnalyzeData();
        break;
    case IN_UART:
        Show_UART_AnalyzeData();
        break;
    default:
        break;
    }
}

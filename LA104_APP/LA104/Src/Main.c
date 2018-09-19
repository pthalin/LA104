/**********************(C) COPYRIGHT 2017 e-Design Co.,Ltd.*********************
 * FileName    : Main.c
 * Description : 
 * Author      : SNAKE
 * History     : 
 *              Beta 1.04  ����BMPͼƬΪ4λ��Ϊ16λͼƬ
 *              APP V1.01  ��ػ�����
 *              APP V1.02   �޸�SPI������ʽ
 *              APP V1.03   uart����ʱ������ϴν�������
 *******************************************************************************/
#include "MyDefine.h"

void Hardware_Init(void);
void Show_Startup_Info(void);
void FPGA_Startup_Wave(void);
void Set_SysParam(void);

//===============================APP�汾��======================================
u8   APP_VERSION[] = "APP V1.03";                 // ���ó���12���ַ�


/*******************************************************************************
 * FunctionName : main
 * Description  : ���������
*******************************************************************************/
void main(void)
{
    u8  M_Flag = 0;
    u8  File_ST = 0;

    Hardware_Init();                             // Ӳ����ʼ��
    Show_Startup_Info();                         // ������Ϣ
    ReadParameter();                             // ��ȡϵͳ����
    Clr_Scrn(BLK);                               // ����
    FPGA_Startup_Wave();
    Set_SysParam();
    ShowAllMenu();
    File_Num();                                  // ��ȡ�ļ����

    while(1)
    {
        ShutdownTest();                          // ��ػ�����
        ShortcutBMP();                           // K1��K4��ݽ�ͼ
        if(Menu.flag)KeyQuickAct();              // T1��T2��ˮƽλ�ƿ�ݲ���

        if(gKeyActv)
        {
            switch(gKeyActv)
            {
            case K1_ACTp:
                gKeyActv = 0;
                {
                    Menu.flag = 1;
                    Item.flag = 0;
                    SubItem.flag = 0;
                    Process();
                    ShowSmplStatus(STOP);
                    if(gBeepFlag)
                    {
                        gBeepFlag = 0;
                        Beep_mS(50);
                    }
                }
                break;

            case K2_ACTp:
                if(Menu.flag)
                {
                    ShowPopItem(Menu.index);
                    Menu.flag = 0;
                    Item.flag = 1;
                }
                else if(Item.flag)
                {
                    if(Menu.index == IN_TYPE)
                    {
                        if(gItemParam[INTYPE]!=IN_USER)
                        {
                            ShowSubItem(Menu.index, gItemIndexNum[Menu.index]);
                            Item.flag = 0;
                            SubItem.flag = 1;
                        }
                    }
                    else if(Menu.index == OUT_TYPE)
                    {
                        if(gItemParam[OUTTYPE]==OUT_PWM)
                        {
                            ShowSubItem(Menu.index, gItemIndexNum[Menu.index]);
                            Item.flag = 0;
                            SubItem.flag = 1;
                        }
                    }
                }
                break;

            case K3_ACTp:

                if(Item.flag)
                {
                    if(Menu.index == TIME_SET)gRunFlag = 1; // TimeSet �˳�ʱˢ�²���
                    Menu.flag = 1;
                    Item.flag = 0;
                    M_Flag = 1;
                }
                else if(SubItem.flag)
                {
                    AnalyzeFrame(gItemParam[INTYPE]);       // Э���Ӳ˵��˳�ʱˢ�½���
                    memcpy(gLCD_Buf, gLCD_Backup, LCD_BUF_WIDTH);
                    ShowWaveToLCD();
                    Menu.flag = 1;
                    ShowPopItem(Menu.index);
                    SubItem.flag = 0;
                    Item.flag = 1;
                    Menu.flag = 0;
                }
                else
                {
                    Menu.flag = 1;
                    Item.flag = 0;
                    M_Flag = 1;
                }
                break;

            case K4_ACTp:
                if(Item.flag)
                {
                    if(Menu.index == TRIGGER)
                    {
                        // TriCond�Ӳ˵�
                        if((gItemIndexNum[TRIGGER] == 1) && ((gItemParam[TRICOND] & 0x00FF) != 0) && ((gItemParam[TRICOND] & 0x00FF) != 5))
                        {
                            if((gItemParam[TRICOND] >> (8 + (gItemParam[TRICOND] & 0x00FF) - 1)) & 0x01)
                                // ��0
                                gItemParam[TRICOND] = gItemParam[TRICOND] & (~(0x01 << (8 + (gItemParam[TRICOND] & 0x00FF) - 1)));
                            else
                                // ��1
                                gItemParam[TRICOND] = gItemParam[TRICOND] | (0x01 << (8 + (gItemParam[TRICOND] & 0x00FF) - 1));
                        }
                        // TriMask�Ӳ˵�
                        else if((gItemIndexNum[TRIGGER] == 2) && ((gItemParam[TRIMASK] & 0x00FF) != 0) && ((gItemParam[TRIMASK] & 0x00FF) != 5))
                        {
                            if((gItemParam[TRIMASK] >> (8 + (gItemParam[TRIMASK] & 0x00FF) - 1)) & 0x01)
                                // ��0
                                gItemParam[TRIMASK] = gItemParam[TRIMASK] & (~(0x01 << (8 + (gItemParam[TRIMASK] & 0x00FF) - 1)));
                            else
                                // ��1
                                gItemParam[TRIMASK] = gItemParam[TRIMASK] | (0x01 << (8 + (gItemParam[TRIMASK] & 0x00FF) - 1));
                        }
                    }
                    else if(Menu.index == OUT_TYPE)
                    {
                        PIO_SendData(gItemParam[OUTTYPE]);
                    }
                    else if(Menu.index == FILE_CTRL)
                    {
                        if(gItemIndexNum[FILE_CTRL] == BMP)
                        {
                            Clear_File_ICO(SAVE_ICO_X0, SAVE_ICO_Y0);
                            File_ST = Save_Bmp(gItemParam[SAVEBMP]);
                        }
                        else if(gItemIndexNum[FILE_CTRL] == CSV)
                        {
                            Clear_File_ICO(SAVE_ICO_X0, SAVE_ICO_Y0);
                            File_ST = Save_Csv(gItemParam[SAVECSV]);
                        }
                        DispFileInfo(File_ST);
                        gBatFlag = 1;
                        ShowBattery();
                    }
                    else if(Menu.index == SETTING)
                    {
                        if(gItemIndexNum[SETTING] == SAVEPARAM)
                        {
                            File_ST = SaveParameter();
                        }
                        else if(gItemIndexNum[SETTING] == RSTPARAM)
                        {
                            M_Flag = 1;
                            RestoreParameter();
                            File_ST = SaveParameter();
                            gBatFlag = 1;
                            ShowAllMenu();
                        }
                        if(gItemIndexNum[SETTING]<=RSTPARAM)
                        {
                            DispFileInfo(File_ST);
                            gBatFlag = 1;
                            ShowBattery();
                        }  
                    }
                }
                else if(Menu.flag)                // ���˵�ʱ��ʱ��ѡ���л�
                {
                    if(gItemIndexNum[TIME_SET] < gItemMaxNum[TIME_SET])
                    {
                        gItemIndexNum[TIME_SET]++;
                    }
                    else
                    {
                        gItemIndexNum[TIME_SET] = 0;
                    }
                }
                break;
            case ENCD_2p: // Up
                if(Menu.flag)
                {
                    if(gItemIndexNum[TIME_SET] == TB_T1)
                    {
                        if((gItemParam[T1POSI] + gItemParamStep[T1POSI]) < gItemParam[T2POSI])
                        {
                            gItemParam[T1POSI] = gItemParam[T1POSI] + gItemParamStep[T1POSI];
                        }
                        else
                        {
                            gItemParam[T1POSI] =  gItemParam[T2POSI];
                        }
                    }
                    else
                    {
                        if(gItemParam[TIMEBASE + gItemIndexNum[TIME_SET]] < gItemParamMax[TIMEBASE + gItemIndexNum[TIME_SET]])
                        {
                            gItemParam[TIMEBASE + gItemIndexNum[TIME_SET]] =
                                gItemParam[TIMEBASE + gItemIndexNum[TIME_SET]] + gItemParamStep[TIMEBASE + gItemIndexNum[TIME_SET]];
                        }
                    }

                    if(gItemIndexNum[TIME_SET] == TB_TB)
                    {
                        ZC_Scale();
                    }
                    else if(gItemIndexNum[TIME_SET] == TB_ZC) 
                    {
                        gTimeBase = gItemParam[TIMEBASE];
                        memcpy(gLCD_Buf, gLCD_Backup, LCD_BUF_WIDTH);
                        ShowWaveToLCD();
                    }
                    else if(gItemIndexNum[TIME_SET] == TB_X) 
                    {
                        gXposiAdd = 1;
                        ZC_Scale();
                    }
                    else
                    {
                        memcpy(gLCD_Buf, gLCD_Backup, LCD_BUF_WIDTH);
                        ShowWaveToLCD();
                    }
                }
                else if(Item.flag)                  
                {
                    if(gItemIndexNum[Menu.index] > 0)
                    {
                        gItemIndexNum[Menu.index]--;
                    }
                }
                else if(SubItem.flag)         
                {
                    switch(Menu.index)
                    {
                    case IN_TYPE:
                        if(INPUT_SUB_INDEX_NUM > 0)
                        {
                            INPUT_SUB_INDEX_NUM--;
                        }
                        break;
                    case OUT_TYPE:
                        if(OUT_SUB_INDEX_NUM > 0)
                        {
                            OUT_SUB_INDEX_NUM--;
                        }
                        break;
                    }  
                }

                break;
            case ENCD_2n: // Down
                if(Menu.flag)
                {
                    if(gItemIndexNum[TIME_SET] == TB_T2)
                    {
                        if((gItemParam[T2POSI] - gItemParamStep[T2POSI]) >= gItemParam[T1POSI])
                        {
                            gItemParam[T2POSI] = gItemParam[T2POSI] - gItemParamStep[T2POSI];
                        }
                        else
                        {
                            gItemParam[T2POSI] = gItemParam[T1POSI];
                        }
                    }
                    else
                    {
                        if(gItemParam[TIMEBASE + gItemIndexNum[TIME_SET]] > gItemParamMin[TIMEBASE + gItemIndexNum[TIME_SET]])
                        {
                            gItemParam[TIMEBASE + gItemIndexNum[TIME_SET]] =
                                gItemParam[TIMEBASE + gItemIndexNum[TIME_SET]] - gItemParamStep[TIMEBASE + gItemIndexNum[TIME_SET]];
                        }
                    }

                    if(gItemIndexNum[TIME_SET] == TB_TB)
                    {
                        ZC_Scale();
                    }
                    else if(gItemIndexNum[TIME_SET] == TB_ZC) 
                    {
                        gTimeBase = gItemParam[TIMEBASE];
                        memcpy(gLCD_Buf, gLCD_Backup, LCD_BUF_WIDTH);
                        ShowWaveToLCD();
                    }
                    else if(gItemIndexNum[TIME_SET] == TB_X) 
                    {
                        gXposiDec = 1;
                        ZC_Scale();
                    }
                    else
                    {
                        memcpy(gLCD_Buf, gLCD_Backup, LCD_BUF_WIDTH);
                        ShowWaveToLCD();
                    }
                }
                else if(Item.flag)
                {
                    if(gItemIndexNum[Menu.index] < gItemMaxNum[Menu.index])
                    {
                        gItemIndexNum[Menu.index]++;
                    }
                }
                else if(SubItem.flag)                
                {
                    switch(Menu.index)
                    {
                    case IN_TYPE:
                        if(INPUT_SUB_INDEX_NUM < INPUT_SUB_MAX_NUM)
                        {
                            INPUT_SUB_INDEX_NUM++;
                        }
                        break;
                    case OUT_TYPE:
                        if(OUT_SUB_INDEX_NUM < OUT_SUB_MAX_NUM)
                        {
                            OUT_SUB_INDEX_NUM++;
                        }
                        break;
                    }
                }

                break;

            case ENCD_1n: // Left
                if(Menu.flag)
                {
                    if(Menu.index > 0) Menu.index--;
                }
                else if(Item.flag)
                {
                    if((Menu.index == TIME_SET) && (gItemIndexNum[TIME_SET] == TB_X))
                    {
                        gXposiDec = 1;
                        UpdateXposiTime();
                        ShowScaleRuleTime(RULE_X, RULE_Y, SHOW_WIDTH, gItemParam[TIMEBASE]);
                    }
                    else if((Menu.index == TIME_SET) && (gItemIndexNum[TIME_SET] == TB_T1))
                    {
                        if((gItemParam[I_INDEX] - gItemParamStep[I_INDEX]) >= gItemParamMin[I_INDEX])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] - gItemParamStep[I_INDEX];
                        }
                        else
                        {
                            gItemParam[I_INDEX] = gItemParamMin[I_INDEX];
                        }
                        while(READ_KEY == K4_HOLD)
                        {
                            if((gItemParam[I_INDEX] - gItemParamStep[I_INDEX]) >= gItemParamMin[I_INDEX])
                            {
                                gItemParam[I_INDEX] = gItemParam[I_INDEX] - gItemParamStep[I_INDEX];
                            }
                            else
                            {
                                gItemParam[I_INDEX] = gItemParamMin[I_INDEX];
                            }
                            ShowPopItem(Menu.index);
                        }
                    }
                    else if((Menu.index == TIME_SET) && (gItemIndexNum[TIME_SET] == TB_T2))
                    {
                        if((gItemParam[I_INDEX] - gItemParamStep[I_INDEX]) >= gItemParam[T1POSI])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] - gItemParamStep[I_INDEX];
                        }
                        else
                        {
                            gItemParam[I_INDEX] = gItemParam[T1POSI];
                        }
                        while(READ_KEY == K4_HOLD)
                        {
                            if((gItemParam[I_INDEX] - gItemParamStep[I_INDEX]) >= gItemParam[T1POSI])
                            {
                                gItemParam[I_INDEX] = gItemParam[I_INDEX] - gItemParamStep[I_INDEX];
                            }
                            else
                            {
                                gItemParam[I_INDEX] = gItemParam[T1POSI];
                            }
                            ShowPopItem(Menu.index);
                        }
                    }
                    else if((Menu.index == TRIGGER) && ((gItemIndexNum[TRIGGER] == 1) || (gItemIndexNum[TRIGGER] == 2)))
                    {
                        if((gItemParam[I_INDEX] & 0x00FF) > gItemParamMin[I_INDEX])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] - gItemParamStep[I_INDEX];
                        }
                    }
                    else
                    {
                        if((gItemParam[I_INDEX] - gItemParamStep[I_INDEX]) >= gItemParamMin[I_INDEX])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] - gItemParamStep[I_INDEX];
                        }
                        else
                        {
                            gItemParam[I_INDEX] = gItemParamMin[I_INDEX];
                        }
                    }

                    if(Menu.index == OUT_TYPE) PIO_Init(gItemParam[OUTTYPE]);
                    if(Menu.index == IN_TYPE) DefaultTir(gItemParam[INTYPE]);
                }
                else if(SubItem.flag)                
                {
                    if((Menu.index == IN_TYPE) && (gItemIndexNum[IN_TYPE] == 0))
                    {
                        if(INPUT_SUB_PARAM > INPUT_SUB_PARAM_MIN)
                        {
                            INPUT_SUB_PARAM--;
                        }
                    }
                    else if(Menu.index == OUT_TYPE)
                    {
                        if(OUT_SUB_PARAM > OUT_SUB_PARAM_MIN)
                        {
                            OUT_SUB_PARAM--;
                        }
                    }
                }

                break;
            case ENCD_1p: // Right
                if(Menu.flag)
                {
                    if(Menu.index < Menu.total) Menu.index++;
                }
                else if(Item.flag)
                {
                    if((Menu.index == TIME_SET) && (gItemIndexNum[TIME_SET] == TB_X))
                    {
                        gXposiAdd = 1;
                        UpdateXposiTime();
                        ShowScaleRuleTime(RULE_X, RULE_Y, SHOW_WIDTH, gItemParam[TIMEBASE]);
                    }
                    else if((Menu.index == TIME_SET) && (gItemIndexNum[TIME_SET] == TB_T1))
                    {
                        if((gItemParam[I_INDEX] + gItemParamStep[I_INDEX]) < gItemParam[T2POSI])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] + gItemParamStep[I_INDEX];
                        }
                        else
                        {
                            gItemParam[I_INDEX] =  gItemParam[T2POSI];
                        }
                        while(READ_KEY == K4_HOLD)
                        {
                            if((gItemParam[I_INDEX] + gItemParamStep[I_INDEX]) < gItemParam[T2POSI])
                            {
                                gItemParam[I_INDEX] = gItemParam[I_INDEX] + gItemParamStep[I_INDEX];
                            }
                            else
                            {
                                gItemParam[I_INDEX] =  gItemParam[T2POSI];
                            }
                            ShowPopItem(Menu.index);
                        }
                    }
                    else if((Menu.index == TIME_SET) && (gItemIndexNum[TIME_SET] == TB_T2))
                    {
                        if((gItemParam[I_INDEX] + gItemParamStep[I_INDEX]) < gItemParamMax[I_INDEX])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] + gItemParamStep[I_INDEX];
                        }
                        else
                        {
                            gItemParam[I_INDEX] =  gItemParamMax[I_INDEX];
                        }
                        while(READ_KEY == K4_HOLD)
                        {
                            if((gItemParam[I_INDEX] + gItemParamStep[I_INDEX]) < gItemParamMax[I_INDEX])
                            {
                                gItemParam[I_INDEX] = gItemParam[I_INDEX] + gItemParamStep[I_INDEX];
                            }
                            else
                            {
                                gItemParam[I_INDEX] =  gItemParamMax[I_INDEX];
                            }
                            ShowPopItem(Menu.index);
                        }
                    }
                    else if((Menu.index == TRIGGER) && ((gItemIndexNum[TRIGGER] == 1) || (gItemIndexNum[TRIGGER] == 2)))
                    {
                        if((gItemParam[I_INDEX] & 0x00FF) < gItemParamMax[I_INDEX])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] + gItemParamStep[I_INDEX];
                        }
                    }
                    else
                    {
                        if((gItemParam[I_INDEX] + gItemParamStep[I_INDEX]) < gItemParamMax[I_INDEX])
                        {
                            gItemParam[I_INDEX] = gItemParam[I_INDEX] + gItemParamStep[I_INDEX];
                        }
                        else
                        {
                            gItemParam[I_INDEX] =  gItemParamMax[I_INDEX];
                        }
                    }
                    if(Menu.index == OUT_TYPE) PIO_Init(gItemParam[OUTTYPE]);
                    if(Menu.index == IN_TYPE) DefaultTir(gItemParam[INTYPE]);
                }
                else if(SubItem.flag)               
                {
                    if((Menu.index == IN_TYPE) && (gItemIndexNum[IN_TYPE] == 0))
                    {
                        if(INPUT_SUB_PARAM < INPUT_SUB_PARAM_MAX)
                        {
                            INPUT_SUB_PARAM++;
                        }
                    }
                    else if(Menu.index == OUT_TYPE)
                    {
                        if(OUT_SUB_PARAM < OUT_SUB_PARAM_MAX)
                        {
                            OUT_SUB_PARAM++;
                        }
                    }
                }
                break;
            }
//          if(gKeyActv != K1_ACTp) Beep_mS(50);
            if(gKeyActv & 0xF0F0) Beep_mS(50);  // �����Ͱ��²�Beep

            ShowTopTitle();
            if(Item.flag && (gKeyActv != K1_ACTn)) ShowPopItem(Menu.index);
            if(SubItem.flag)                    // ��ʾ�Ӳ˵�ѡ��
            {
                ShowSubItem(Menu.index, gItemIndexNum[Menu.index]);
            }
            HW_Ctrl(Menu.index, gItemIndexNum[Menu.index]);
            ShowWindowPrecent();
            if(Menu.index == TIME_SET) ShowTimeBaseStr();
            ShowTimeMarkValue();
            ShowMeasureValue();
            ResetPowerOffTime();
            gKeyActv = 0;
        }
        //===================Test==========================
        if(M_Flag)  //�Ӳ˵�״̬�²�ˢ��FPGA��ʾ
        {
            M_Flag = 0;
            memcpy(gLCD_Buf, gLCD_Backup, LCD_BUF_WIDTH);
            ShowWaveToLCD();
            if(Menu.index == TIME_SET)
            {
                ZC_Scale();
            }
        }
        StandbyAndPowerOff();
        ShowBattery();
    }
}

/*******************************************************************************
 * FunctionName : Hardware_Init
 * Description  : Ӳ����ʼ��
 * Param        : void 
*******************************************************************************/
void Hardware_Init(void)
{
    __Bios(PWRCTRL, INIT);        // ��Դ�豸��ʼ������
    __Bios(KEYnDEV, INIT);        // �����������豸��ʼ������
    __Bios(NIVCPTR, 0x8000);      // �ж�������ַ = 0x0000
//  __Bios(SYSTICK, 1000);        // ��������ж�ʱ�� = 1000uS
    SysTick_Config(SystemCoreClock / 1000);
    __Bios(BUZZDEV, INIT);        // ��������ʼ������ �������ʼ��SI2302�ᷢ��
    __Bios(BUZZDEV, 50);
    Beep_mS(200);
    __Bios(FLSHDEV, INIT);        // SPI���̳�ʼ������
    __Bios(USBDEV, INIT);         // USB��ʼ��
    DiskConfig();                 // �ļ�ϵͳ��ʼ��
    __Bios(IN_PORT, INIT);        // DAC�˿ڳ�ʼ��
//  __Bios(IN_PORT, 3000);        // 3V
    __Bios(IN_PORT, gItemParam[THRESHOLD]*100);

}

/*******************************************************************************
 * FunctionName : Show_Startup_Info
 * Description  : ��ʾ������ʾ��Ϣҳ��
 * Param        : void 
*******************************************************************************/
void Show_Startup_Info(void)
{
    Disp_Str8x14(0,      90, BLK, BLK, PRN, "                                        ");
    Disp_Str8x14(0,      70, BLK, BLK, PRN, "                                        ");
    Disp_Str8x14(8,      90, WHT, BLK, PRN, "       Logic Analyzer APP");
    Disp_Str8x14(27 * 8, 90, WHT, BLK, PRN,                           APP_VERSION);
    Disp_Str8x14(8,      70, WHT, BLK, PRN, "        System Initializing...       ");
    Delay_mS(1000);
    if(__Bios(FPGADEV, DONE) == 0) while(1){}       // FPGA�������ʱ������APP
    
}

/*******************************************************************************
 * FunctionName : FPGA_Startup_Wave
 * Description  : ����FPGA����
 * Param        : void 
*******************************************************************************/
void FPGA_Startup_Wave(void)
{
    u8  FpgaInfo = 0;

    FPGA_ReadWrite(FPGA_RST, 0, 0);                        // оƬ�ڲ��߼���λ
    FPGA_ReadWrite(WINDOW_Y, 1, YHIGH);                    // ������ʾ����ֱ�߶�
    FPGA_ReadWrite(WINDOW_X, 2, XWDTH);                    // ������ʾ��ˮƽ���
    memset(gLCD_Buf, 0, LCD_BUF_WIDTH);
    memset(gLCD_Backup, 0, LCD_BUF_WIDTH);
    FPGA_LCD_RW(WFRM_SAVE, XWDTH, gLCD_Buf);               // ���ò�����������
    Delay_mS(5);
    gT0Posi = gItemParam[T0POSI];
    gT1Posi = gItemParam[T1POSI];
    gT2Posi = gItemParam[T2POSI];
    gZCPosi = gItemParam[ZCPOSI];
    FPGA_ReadWrite(T0_POSN, 2, gItemParam[T0POSI]);        // ���� T0 ����λ��
    FPGA_ReadWrite(T1_POSN, 2, gItemParam[T1POSI]);        // ���� T1 ����λ��
    FPGA_ReadWrite(T2_POSN, 2, gItemParam[T2POSI]);        // ���� T2 ����λ��
    FPGA_ReadWrite(ZC_POSN, 2, gItemParam[ZCPOSI]);        // ���� ZC ����λ��
//  FPGA_ReadWrite(ZC_POSN, 2, 150);                       // ���� ZC ����λ��
    Delay_mS(2);
//  FPGA_ReadWrite(FLSH_ATTR, 2, 0xFFFF);                  // ������˸���Ա�־
//  FPGA_LCD_RW(WFRM_SAVE, XWDTH, gLCD_Buf);               // ���ò�����������
    Set_Block(X0, Y0, XWDTH + X0 - 1, YHIGH + Y0 - 1);     // ���� LCD ������ʾ����
    __Bios(DISPDEV, FPGA_MOD);                             // �л�Ϊ FPGA ������ʾ
    FPGA_ReadWrite(WFRM_DRAW, 0, 0);                       // ˢ�²�����ʾͼ��
    do
    {
        FpgaInfo = FPGA_ReadWrite(FPGA_INFO, 0, 0) >> 8;   // �� FPGA ״̬
    } while(FpgaInfo & BUSY);                              // ���� DRAW ״̬ʱ�˳�
    __Bios(DISPDEV, MCU_DRV);
}
/*******************************************************************************
 * FunctionName : Set_SysParam
 * Description  : ��ʼ��ϵͳ����
 * Param        : void 
*******************************************************************************/
void Set_SysParam(void)
{
    Menu.flag = 1;                               // �˵���־λ
    Menu.index = 0;                              // �˵���ǰ�ڼ���ѡ��
    Menu.total = 6;                              // �˵�ѡ������

    Item.flag = 0;                               // �Ӳ˵���־λ
    Item.index = 0;                              // �Ӳ˵���־λ
    Item.total = 0;                              // �Ӳ�ѡ������
    gTimeBase = gItemParam[TIMEBASE];

    gTriCond_X = (u8)gItemParam[TRICOND];
    gTriMask_X = (u8)gItemParam[TRIMASK];
    gKeyActv = 0;                                // ��������
    gStandByFlag = 1;
    ResetPowerOffTime();
    PIO_Init(gItemParam[OUTTYPE]);               // ��ʼ���ɱ��IOЭ������
//  DefaultTir(gItemParam[INTYPE]);
    __Bios(DISPDEV, gItemParam[BKLIGHT] * 10);
    __Bios(BUZZDEV, gItemParam[VOLUME] * 10);
}

/******************************  END OF FILE  *********************************/

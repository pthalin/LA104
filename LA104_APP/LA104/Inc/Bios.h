#ifndef __BIOS_H
#define __BIOS_H

#include "STM32F10x.h"

typedef struct
{
    u16  n;
    u8  *pBuf;
}SPI_ParamDef;

typedef struct
{
    u8 flag;
    u8 total;
    u8 index;
}MENU_Struct;

typedef struct
{
    u8 flag;
    u8 total;
    u8 index;
}ITEM_Struct;

typedef struct
{
    u8 flag;
    u8 total;
    u8 index;
}SUBITEM_Struct;

#define OFF             0
#define ON              1
#define SUCC            0
#define FAIL            1
#define HIGH            1
#define LOW             0
#define NONE            0
#define ENBL            1
#define DSBL            0
#define INIT            0xFFFF0000
#define STRING          0xFFFE0000
#define VALUE           0xFFFD0000

//----------------------��ʾ�豸��غ궨��---------------------------
#define MCU_DRV         0xFFFB0000
#define FPGA_DRV        0xFFFA0000
#define XWDTH           280
#define YHIGH           154
#define X0              40
#define Y0              66

//--------------------- ��չ�����źŶ˿���غ궨��-------------------
#define PWM1_PSC        0xFFFC0000
#define PWM1_ARR        0xFFFB0000
#define PWM1_DUTY1      0xFFFA0000
#define PWM1_DUTY2      0xFFF90000
#define PWM1_CTRL       0xFFF80000
#define PWM2_PSC        0xFFF60000
#define PWM2_ARR        0xFFF50000
#define PWM2_DUTY1      0xFFF40000
#define PWM2_DUTY2      0xFFF30000
#define PWM2_CTRL       0xFFF20000

//----------------------��Դ���Ƽ������豸��غ궨��-----------------
#define VIN_ST          0xFFFC0000
#define VBTYmV          0xFFFB0000

//----------------------�������豸��غ궨��-------------------------
#define BITMAP          0xFFFC0000
#define ENC1a           0x1000
#define ENC1b           0x2000
#define ENC2a           0x4000
#define ENC2b           0x8000
#define K1_HOLD         0x0001
#define K2_HOLD         0x0002
#define K3_HOLD         0x0004
#define K4_HOLD         0x0008
#define K1_ACTn         0x0010
#define K2_ACTn         0x0020
#define K3_ACTn         0x0040
#define K4_ACTn         0x0080
#define K1_ACTp         0x0100
#define K2_ACTp         0x0200
#define K3_ACTp         0x0400
#define K4_ACTp         0x0800
#define Kn_ACTn         0x00F0
#define Kn_ACTp         0x0F00
#define KEYSBIT         0x000F
#define ENCD_1p         0x1000
#define ENCD_1n         0x2000
#define ENCD_2p         0x4000
#define ENCD_2n         0x8000
#define ENCDBIT         0xF000
#define READ_RUN_ST     (((~GPIOC->IDR) & K1_PIN)? 1:0)
#define READ_KEY        (~__Bios(KEYnDEV, BITMAP) & KEYSBIT)

//----------------------��ʾ�豸��غ궨��---------------------------
#define SIZE            0xFFFC0000
#define MCU_WR          0xFFFB0000
#define MCU_RD          0xFFFA0000
#define FPGA_MOD        0xFFF90000
#define LCDCOL          320
#define LCDROW          240
#define LOGO_X          4*8
#define LOGO_Y          160

//----------------------FPGA ������غ궨��--------------------------
#define CNFG            0xFFFC0000
#define DONE            0xFFFA0000
#define VREF            3000
#define FPGA_BUSY       0x01
#define FPGA_FULL       0x02
#define FPGA_EMPTY      0x04
#define FPGA_PRES       0x08
#define FPGA_TRI        0x10

#define RECORD_DEPTH    3584   // ��Ч�仯3584
#define CNTx4K          4
#define PSMP_NUM        2
//----------------------USB DISK ��غ궨��--------------------------

#define PAGE            0xFFFC0000
#define SECTOR          0xFFFB0000
#define AMOUNT          0xFFFA0000

extern MENU_Struct Menu;
extern ITEM_Struct Item;
extern SUBITEM_Struct SubItem;

extern u8  gSamplBuf[], gLCD_Buf[], gLCD_Backup[];
extern u8  gBeepFlag, gStandByFlag;
extern u16 gSysCnt, gFullmS, gStandbyCnt, gAutoPwrCnt;
extern u16 gTimeBase, gBatVol, gUSB_Vol;

u32  __Bios(u8 Item, u32 Var);
u8   FPGA_LCD_RW(u8 Cmd, u16 Cnt, u8 *Buf);
u16  FPGA_ReadWrite(u8 Cmd, u16 Cnt, u16 Data);
u16  FPGA_Read(u8 Cmd, u16 Cnt, u16 Data);
u32  FPGA_RW_4Byte(u8 Cmd, u16 Cnt, u32 Data);
void DiskConfig(void);
void Delay_mS(vu32 mS);
void Delay_uS(vu32 uS);
void FpgaRW(u8 Cmd, u16 n, u8 *pBuf);
void ExtFlash_CS_LOW(void);
void ExtFlash_CS_HIGH(void);
void ProgmIO_SPIWr(u16 n, u8 *pBuf);
void ProgmIO_UARTTx(u8 Data);
void ProgmIO_I2CWr(u8 DevID, u8 Reg, u8 Data);

enum
{
    SYSINFO, // ϵͳ�ۺ���Ϣ      Var: PRDT_SN  ��Ʒ����  Rtn: (u16)��Ʒ����
             //                   Var: PRODUCT  ��Ʒ�ͺ�  Rtn: �ͺ��ִ�ָ��
             //                   Var: PRDTNUM  ��Ʒ���  Rtn: (u16)��Ʒ���
             //                   Var: SCH_VER  Ӳ���汾  Rtn: �汾�ִ�ָ��
             //                   Var: MCU_TYP  MCU �ͺ�  Rtn: �ͺ��ִ�ָ��
             //                   Var: DFU_VER  DFU �汾  Rtn: �汾�ִ�ָ��
             //                   Var: OEM_NUM  OEM ���  Rtn: ����ִ�ָ��
             //                   Var: MANUFAC ��Ʒ����Ϣ Rtn: ��Ϣ�ִ�ָ��
             //                   Var: LICENCE ���֤״̬ Rtn: SUCC/FAIL
             // ϵͳ������
    NIVCPTR, // �ж�ʸ����ַ����  Var: (u16)   ʸ����ַ  Rtn: SUCC/FAIL
    SYSTICK, // ϵͳ����ж�����  Var: (u16)uS �ж�ʱ��  Rtn: SUCC/FAIL
    AF_RMAP, // �豸 IO ���¶�λ  Var: (u32)   AF REMAP  Rtn: SUCC/FAIL
    PWRCTRL, // ��Դ����          Var: INIT  ��ʼ������  Rtn: SUCC/FAIL
             //                   Var: VIN_ST  ��ӵ�Դ  Rtn: ENBL/DSBL
             //                   Var: VBAT    ��ص�ѹ  Rtn: (u16)mV
             //                   Var: DSBL    ��Դ�ر�  Rtn: SUCC/FAIL
    BUZZDEV, // �������ӿ�        Var: INIT  ��ʼ������  Rtn: SUCC/FAIL
             //                   Var: (MUTE~100)  ����  Rtn: SUCC/FAIL
    KEYnDEV, // �����������豸    Var: INIT  ��ʼ������  Rtn: SUCC/FAIL
             //                   Var: BITMAP  ��ȡ��λ  Rtn: (u16)��λ״̬
    DELAYuS, // ΢���ӳ�          Var: (u32)uS �ӳ�ʱ��  Rtn: SUCC/FAIL
             // ��ʾ������
    DISPDEV, // LCD ��ʾ�豸      Var: STRING  ģ���ͺ�  Rtn: �ͺ��ִ�ָ��
             //                   Var: INIT  ��ʼ������  Rtn: SUCC/FAIL
             //                   Var: VALUE   ��Ļ���  Rtn: (MaxX<<16+MaxY)
             //                   Var: (0~100) ��������  Rtn: SUCC/FAIL
             //                   Var: (MCU/FPGA)����Դ  Rtn: SUCC/FAIL
    BLOCK_X, // ��ʾ�� x ������   Var: (x1 << 16)+x2)    Rtn: SUCC/FAIL
    BLOCK_Y, // ��ʾ�� y ������   Var: (y1 << 16)+y2)    Rtn: SUCC/FAIL
    PIXEL_X, // ���� x λ��       Var: (u16)λ��         Rtn: SUCC/FAIL
    PIXEL_Y, // ���� y λ��       Var: (u16)λ��         Rtn: SUCC/FAIL
    WrPIXEL, // ��ʾ����д        Var: (u16)������ɫ     Rtn: SUCC/FAIL
    RdPIXEL, // ��ȡ��Ļ����      Var:                   Rtn: (u16)������ɫ
    FONTPTR, //                   Var: ASCII Code        Rtn: ptr �ִ�ָ��
             // FPGA ������
    FPGADEV, // FPGA �豸����     Var: STRING            Rtn: �ͺ��ִ�ָ��
             //                   Var: INIT  ��ʼ������  Rtn: SUCC/FAIL
             //                   Var: CNFG    �ϵ�����  Rtn: SUCC/FAIL
             //                   Var: COMM    ͨ��ģʽ  Rtn: SUCC/FAIL
             //                   Var: ENBL    �ӿ�Ƭѡ  Rtn: SUCC/FAIL
             //                   Var: (SPI_ParamDef*)   Rtn: SUCC/FAIL
             // �������������
    IN_PORT, // �߼��������      Var: INIT  ��ʼ������  Rtn: SUCC/FAIL
             //                   Var: VREF �߼��ο���ѹ Rtn: SUCC/FAIL
             // U �̿�����
    USBDEV,  // USB ��ʼ��
    U_DISK,  // USB DISK �豸     Var: INIT  ��ʼ������  Rtn: SUCC/FAIL
             //                   Var: STRING  U �̹��  Rtn: U �̹���ִ�ָ��
             //                   Var: PAGE  �洢ҳ�ߴ�  Rtn: SUCC/FAIL
             //                   Var: SECTOR  ��������  Rtn: (u16)��������
             //                   Var: AMOUNT  ��������  Rtn: (u16)��������
             //                   Var: (SPI_ParamDef*)   Rtn: SUCC/FAIL
             // SPI ������
    SPI_DEV, // SPI �豸�ӿ�      Var: (SPI_ParamDef*)   Rtn: SUCC/FAIL
    FLSHDEV, // Flash SPI
             // ��չ���Խӿڿ�����
    EXT_INP, // ��չ PIO �ӿ�     Var: PIOCFG+PinDef      Rtn: SUCC
    EXT_OUT, // ��չ PIO �ӿ�     Var: PIOCFG+PinDef      Rtn: SUCC
             //                   Var: PINS_OUT+Status0~3 Rtn: SUCC
             //                   Var: PINS_IN+BitMask0~3 Rtn: PinStatus 0~3
    EXT_PWM, // ��չ PWM �ӿ�     Var: PWM_PSC+Var[0~15]  Rtn: SUCC
             //                   Var: PWM_ARR+Var[0~15]  Rtn: SUCC
             //                   Var: CH1_CCR+Var[0~15]  Rtn: SUCC
             //                   Var: CH2_CCR+Var[0~15]  Rtn: SUCC
             //                   Var: CH1_CTRL+ENBL/DSBL Rtn: SUCC
             //                   Var: CH2_CTRL+ENBL/DSBL Rtn: SUCC
    EXT_INF, // ��չ PIO �ӿ�     Var: INIT  ��ʼ������ ����Ϊ��������
    EXT_SPI, // ��չ PIO �ӿ�     Var: INIT  ��ʼ������   Rtn: SUCC/FAIL
             //                   Var: (SPI_ParamDef*)    Rtn: SUCC/FAIL
    EXT_UART,// ��չ PIO �ӿ�     Var: INIT  ��ʼ������   Rtn: SUCC/FAIL
             //                   Var: u8 Data            Rtn: SUCC/FAIL
    EXT_I2C, // ��չ PIO �ӿ�
             //                   Var: 0x00&ID&REG&DATA   Rtn: SUCC/FAIL
    EXT_RXD, // ��չ PIO �ӿ�     Var: PIOCFG+PinDef      Rtn: SUCC
    EXT_TXD, // ��չ PIO �ӿ�     Var: PIOCFG+PinDef      Rtn: SUCC
};

enum
{
    PRDT_SN, // ��Ʒ����
    PRODUCT, // ��Ʒ�ͺ��ַ���
    PRDTNUM, // ��Ʒ���
    HDW_VER, // Ӳ���汾�ַ���
    MCU_TYP, // MCU �ͺ��ַ���
    DFU_VER, // DFU �汾�ַ���
    OEM_NUM, // OEM ����ַ���
    MANUFAC, // ��Ʒ����Ϣ�ַ���
    LICENCE, // ���֤��Ȩ״̬
};

//���� void FpgaRW(u8 Item, u16 n, u8* p) Item �ĵ���˵��
enum
{
    FPGA_INFO = 0x00, // ״̬���̼��汾��  n = 1, Rtn p[0] = St, p[1]=δ֪����, p[2]=0, p[3] = VerNum
    FPGA_RST  = 0x01, // оƬ�ڲ��߼���λ  n = 0, p* = Dummny
    SMPL_CLR  = 0x02, // оƬ�ڲ��߼���λ  n = 1, p* = Dummny
    WFRM_SAVE = 0x03, // ���ò�����������  n = Row+1, p = &DataBytes

    WINDOW_Y  = 0x04, // ������ʾ���ڸ߶�  n = 1, p[1]   = Y Size
    WINDOW_X  = 0x05, // ������ʾ���ڿ��  n = 2, p[1~2] = X Size
    WFRM_DRAW = 0x06, // ˢ�²�����ʾͼ��  n = 0, p[1] = Col, p[2~3] = Row
    FLSH_ATTR = 0x07, // ������˸���Ա�־  n = 1, p[1~2] = Attr

    RUL_POSN  = 0x10, // ���� RUL ���λ��  n = 1, p[1] = RUL Position
    CH1_POSN  = 0x11, // ���� CH1 ����λ��  n = 1, p[1] = CH1 Position
    CH2_POSN  = 0x12, // ���� CH2 ����λ��  n = 1, p[1] = CH2 Position
    CH3_POSN  = 0x13, // ���� CH3 ����λ��  n = 1, p[1] = CH3 Position
    CH4_POSN  = 0x14, // ���� CH4 ����λ��  n = 1, p[1] = CH4 Position
    CH5_POSN  = 0x15, // ���� CH5 ����λ��  n = 1, p[1] = CH5 Position
    CH6_POSN  = 0x16, // ���� CH6 ����λ��  n = 1, p[1] = CH6 Position

    ZC_POSN   = 0x18, // ���� ZC ����λ��  n = 2, p[1~2] = ZC Position
    T0_POSN   = 0x19, // ���� T0 ����λ��  n = 2, p[1~2] = T0 Position
    T1_POSN   = 0x1A, // ���� T1 ����λ��  n = 2, p[1~2] = T1 Position
    T2_POSN   = 0x1B, // ���� T2 ����λ��  n = 2, p[1~2] = T2 Position

    TRG_KIND  = 0x20, // ���� ��������  n = 1, p[1]   = Trigger Kind
    TRG_COND  = 0x21, // ���� ��������  n = 1, p[1]   = Trigger COND
    MEM_MASK  = 0x22, // ���� ��������  n = 1, p[1]   = Trigger Mask
    MEM_TWTH  = 0x23, // ���� ��������  n = 2, p[1~2] = Trigger Twth

    MEM_PSMP  = 0x30, // ���� Ԥ�ɼ�¼���n = 1, p[1]   ȱʡֵΪ9
    MEM_RPTR  = 0x31, // ���� ��¼��ָ��  n = 2, p[1~2] = Record Rptr
    MEM_READ  = 0x32, // ���� ��¼����    n = 3, Rtn p[1~3] = Record Data
                      // [0]:Info ,[1]:Tcnt[7:0],[2]:Tcnt[13:8],[3]:Di
                      
    FPGA_ST   = 0x00, // ���ص� FPGA ״̬�ֽڵ�λ��
    VER_NUM   = 0x01, // ���ص� FPGA �̼��汾�ŵ�λ��
    BUSY      = 0x01, // FPGA ״̬�ֽ� St �е� ������ͼ��ˢ�¡� ��־λ

};
//Info״̬λ{3'd0, TrgRdy, PesRdy, Empty, Full, BusySt};
//��������
/*
    0: o_Trig <= 1'd0; // ��ֹ����
    1: o_Trig <= DiNE; // ��������߼���״̬����Ԥ����������
    2: o_Trig <= DiEQ; // ��������߼���״̬����Ԥ����������
    3: o_Trig <= 1'd1; // ���ⴥ��
    4: o_Trig <= DfLE; // ���벻�������ĳ������С����ֵ����
    5: o_Trig <= DtLE; // ������������ĳ������С����ֵ����
    6: o_Trig <= DfGT; // ���벻�������ĳ�����ȴ�����ֵ����
    7: o_Trig <= DtGT; // ������������ĳ�����ȴ�����ֵ����
*/

#endif
/*********************************  END OF FILE  ******************************/

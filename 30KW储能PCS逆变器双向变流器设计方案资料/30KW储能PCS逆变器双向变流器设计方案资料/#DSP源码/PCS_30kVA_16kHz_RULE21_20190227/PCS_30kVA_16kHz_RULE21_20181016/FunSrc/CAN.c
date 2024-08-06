/**********************************************************************
Copyright 2016, Chint Power
File Name: CAN.c
Description: DSP2833x Enhanced CAN Initialization & Support Functions.
Version:  
Author:  
Data: 2016-06-23
**********************************************************************/
/* head-files including*/
#include "..\FunSrc\Funheader.h"

#define cRead               0
#define cWrite              1
#define cResponse           2

#define cInputRegister      0
#define cHoldRegister       1
#define cRemoteRegister     2
#define cImportRegister     5

#define HoldRegisterNum     195
#define InputRegisterNum    80
#define RemoteDataNum       50
#define ImportDataNum       7

#define cHexToDec           640//  =10/16*1024 = 640 
#define cDecToHex           13107 //1.6,Q13

#define sFormatQ0To10Rate(wData)              (((INT32S)wData*10))               //Q0  ->  0.1
#define sFormatQ4To10Rate(wData)              (((INT32S)wData*10 + 8)>>4)        //Q4  ->  0.1
#define sFormatQ8To10Rate(wData)              (((INT32S)wData*10 + 128)>>8)      //Q8  ->  0.1
#define sFormatQ4ToR01Rate(dwData)            (((INT32S)dwData + 80)/160)              //Q4  ->  10
#define sFormatQ10To1000Rate(wData)           (((INT32S)wData*1000 + 512)>>10)   //Q10 ->  0.001
#define sFormatQ8To100Rate(wData)             (((INT32S)wData*25 + 32)>>6)       //Q8  ->  0.01
#define sFormatQ4To1Rate(wData)               ((wData)>>4)                       //Q4  ->  1


#define sFormatPowerStepToLcd(wData)          (((INT32S)wData*cPowerStepCoeff + 512)>>10)
#define sFormatPowerStepToEeprom(wData)       (((INT32S)wData*cPowerStepInvCoeff)>>8)

#define cPower600W         9600
#define cVolt100V          1600
#define cCurrNR5A          -8

#define cMasterPhyAdr       0  //主机地址为0
INT16U fLocalPhyAddr = 1; //从机地址初始化为1

INT16U fHoldRegisterStatus = 0;  //=1 holdregister->eeprom data, =2 eeprom data -> holdregister,=3 waitting

INT16S wHoldRegister[HoldRegisterNum];
INT16S wInputRegister[InputRegisterNum];
INT16S wRemoteData[RemoteDataNum];

INT16U fLcdRunOrder = 1,fLcdSoftRunOrder = 1;   
INT16U fStandardChgStopRequire = 0;

//==========================for bootloader===============================
INT16U fDisBootloaderOrder = 1;
INT8U ubBootloaderRecData[6] = {0,0,0,0,0,0}; 
INT16U uwBootloaderReceFlag = 0;
INT16U uwBootloaderStep = 0;

//for PV voltage display
INT16S wPVLongAvgVol[4] = {0,0};

INT16U fCANBCongfigFinish = 0;
INT16U uwDebugCANA1 = 0;
INT16U uwDebugCANA2 = 0;
INT16U uwDebugCANA3 = 0;
INT16U fECANRunOrder = 0;

const INT16S wRemoteDataMax[ ] = 
{
	0x0,                         0x1,                       0x1,                       0x1f40,              
    //reserver                   wRemoteCSVSSwitch          wRemoteVSCtrMode           wRemoteVSFreqSet
    0x1450,                      0x1,                       0x1,                       0x3e8,                                
    //wRemoteVSVoltSet           wRemoteCSPCtrEn            wRemoteCSPCtrModeSel       wRemoteCSPPer  
    0x1b58,                      0x1,                       0x1,                       0x3e8,            
    //wRemoteCSCurrSet           wRemoteCSQCtrEn            wRemoteCSQCtrModeSel       wRemoteCSPFSet
    0x258,                       1,                         1,                         0x3e8,                               
    //wRemoteCSQPer              wRemoteK7Ctr               fRemotePCSComdOn           wRemoteVSPSet    
    0x258,                       0,                         0,                         0,   
    //wRemoteVSQSet              reserver                   reserver                   reserver 
    0,                           0,                         0,                         0,   
    //reserver                   reserver                   reserver                   reserver 
    0,                           0,                         0,                         0,   
    //reserver                   reserver                   reserver                   reserver 
    0,                           0,                         0x23f0,                    0x1,   
    //reserver                   reserver                   wBatteryPortVolt           fBatteryReady 
    0x1,                         0x1388,                    0x36B0,                    0x23f0,   
    //fBatteryPreChargeEn        wBatteryPower              wBatteryCurr               wSOCChargeVoltLimit 
    0x23f0,                      0x0,                       0x1b58,                    0x23f0,   
    //wSOCDisChargeVoltLimit     wSOCChargeCurrLimt         wSOCDisChargeCurrLimit     wSOCFloatChargeVolt 
    0x0,                         0x1,                       31,                        0,   
    //wSOCSToFChargeCurr         reserver                   fLCDIOState                reserver
    0,                           0,                         0,                         0,   
    //reserver                   reserver                   reserver                   reserver 
    0,                           0  
    //reserver                   reserver      
};

const INT16S wRemoteDataMin[ ] = 
{
	0,                           0,                         0,                         0xbb8,              
    //reserver                   wRemoteCSVSSwitch          wRemoteVSCtrMode           wRemoteVSFreqSet
    0x7d0,                       0,                         0,                         0xfd65,                                
    //wRemoteVSVoltSet           wRemoteCSPCtrEn            wRemoteCSPCtrModeSel       wRemoteCSPPer 
    0xe4a8,                      0,                         0,                         0xfc18,            
    //wRemoteCSCurrSet           wRemoteCSQCtrEn            wRemoteCSQCtrModeSel       wRemoteCSPFSet
    0xfda8,                      0,                         0,                         0xfd65,                                
    //wRemoteCSQPer              wRemoteK7Ctr               fRemotePCSComdOn           wRemoteVSPSet    
    0xfdA8,                      0,                         0,                         0,   
    //wRemoteVSQSet              reserver                   reserver                   reserver 
    0,                           0,                         0,                         0,   
    //reserver                   reserver                   reserver                   reserver 
    0,                           0,                         0,                         0,   
    //reserver                   reserver                   reserver                   reserver 
    0,                           0,                         0,                         0,   
    //reserver                   reserver                   wBatteryPortVolt           fBatteryReady 
    0,                           0xec78,                    0xc950,                    0x9c4,   
    //fBatteryPreChargeEn        wBatteryPower              wBatteryCurr               wSOCChargeVoltLimit 
    0x9c4,                       0xe4a8,                    0x0,                       0x9c4,   
    //wSOCDisChargeVoltLimit     wSOCChargeCurrLimt         wSOCDisChargeCurrLimit     wSOCFloatChargeVolt 
    0xe4a8,                      0,                         0,                         0,   
    //wSOCSToFChargeCurr         reserver                   fLCDIOState                reserver 
    0,                           0,                         0,                         0,   
    //reserver                   reserver                   reserver                   reserver 
    0,                           0  
    //reserver                   reserver
};

INT16U fConfigeCANmbx;
INT16U uwHoldDataAddr;
INT16U uwHoldDataLength;
INT16U uwHoldDataCnt;
INT16U fHoldTx;

INT16U uwRemoteDataAddr;
INT16U uwRemoteDataLength;
INT16U uwRemoteDataCnt;
INT16U fRemoteTx;

INT16U uwInputDataCnt;
INT16U fInputTx;
INT16U uwInputDataAddr;
INT16U uwInputDataLength;

INT16U uwImportDataCnt;
INT16U fImportTx;
INT16U uwImportDataAddr;
INT16U uwImportDataLength;

INT16U fCANActive;

#pragma CODE_SECTION(sCANRXBootLoader, "eCanRamFuncs")
#pragma CODE_SECTION(sCANRXImportRead, "eCanRamFuncs")
#pragma CODE_SECTION(sCANRXHoldRead, "eCanRamFuncs")
#pragma CODE_SECTION(sCANRXInputRead, "eCanRamFuncs")
#pragma CODE_SECTION(sCANRXRemoteRead, "eCanRamFuncs")
#pragma CODE_SECTION(sCANRXImportWrite, "eCanRamFuncs")
#pragma CODE_SECTION(sCANRXHoldWrite, "eCanRamFuncs")
#pragma CODE_SECTION(sCANRXRemoteWrite, "eCanRamFuncs")
#pragma CODE_SECTION(CANTX, "eCanRamFuncs")
#pragma CODE_SECTION(ECanISR0, "eCanRamFuncs")
#pragma CODE_SECTION(ECanISR1, "eCanRamFuncs")


//---------------------------------------------------------------------------
// InitECanVar:
// Called by: main()
//---------------------------------------------------------------------------
void InitECanVar(void)
{
    fLocalPhyAddr = 1;
    fConfigeCANmbx = 0;
    
    uwHoldDataAddr = 0;
    uwHoldDataLength = 0;
    uwHoldDataCnt = 0;
    fHoldTx = 0;
    
    uwRemoteDataAddr = 0;
    uwRemoteDataLength = 0;
    uwRemoteDataCnt = 0;
    fRemoteTx = 0;
    
    uwInputDataAddr = 0;
    uwInputDataLength = 0;
    uwInputDataCnt = 0;
    fInputTx = 0;
        
    uwImportDataAddr = 0;
    uwImportDataLength = 0;
    uwImportDataCnt = 0;
    fImportTx = 0;
    
    fCANActive = 0;
	fEepromOperateRequire = 0;
	fECANRunOrder = 0;
    
    //第9组，命令参数
    wHoldRegister[185] = 0x5555;//fDeCapEnergy
    wHoldRegister[186] = 0xA5A5;//fLocalPCSComdOn
    wHoldRegister[187] = 0x5555;//fQuitShutModeOrder
    wHoldRegister[188] = 0x5555;//fCallDefaultValueOrder
    wHoldRegister[189] = 0x5555;//fAutoTest
    wHoldRegister[190] = 0;
    wHoldRegister[191] = 0xA5A5;//fLocalPCSConnect
    wHoldRegister[192] = 0;

    //Remote Data
    wRemoteData[0] = 0;     //reserver
    wRemoteData[1] = 0;     //wRemoteCSVSSwitch
    wRemoteData[2] = 0;     //wRemoteVSCtrMode
    wRemoteData[3] = 5000;  //wRemoteVSFreqSet
    wRemoteData[4] = 4000;  //wRemoteVSVoltSet
    wRemoteData[5] = 1;     //wRemoteCSPCtrEn;  
    wRemoteData[6] = 0;     //wRemoteCSPCtrModeSel;     
    //wRemoteData[7] = 1000;   //wRemoteCSPPer;
    wRemoteData[7] = 0;   //wRemoteCSPPer;
    wRemoteData[8] = 0;      //wRemoteCSCurrSet;     
    wRemoteData[9] = 0;     //wRemoteCSQCtrEn; 
    wRemoteData[10] = 1;    //wRemoteCSQCtrModeSel;
    wRemoteData[11] = 1000; //wRemotePFSet   
    wRemoteData[12] = 0;    //wRemoteQSet
    wRemoteData[13] = 1;    //wRemoteK7Ctr;   
    wRemoteData[14] = 0;    //fRemotePCSComdOn
    wRemoteData[15] = 0;    //wRemoteVSPSet
    wRemoteData[16] = 0;    //wRemoteVSQSet
    wRemoteData[17] = 0;//reserver
    wRemoteData[18] = 0;//reserver
    wRemoteData[19] = 0;//reserver
    wRemoteData[20] = 0;//reserver
    wRemoteData[21] = 0;//reserver
    wRemoteData[22] = 0;//reserver
    wRemoteData[23] = 0;//reserver
    wRemoteData[24] = 0;//reserver
    wRemoteData[25] = 1;//reserver
    wRemoteData[26] = 0xffff;//reserver
    wRemoteData[27] = 0;//reserver
    wRemoteData[28] = 0;//reserver
    wRemoteData[29] = 0;//reserver
    wRemoteData[30] = 0;//wBatteryPortVolt
    wRemoteData[31] = 1;//fBatteryReady
    wRemoteData[32] = 0;//fBatteryPreChargeEn
    wRemoteData[33] = 0;//wBatteryPower
    wRemoteData[34] = 0;//wBatteryCurr
    wRemoteData[35] = 0;//wSOCChargeVoltLimit
    wRemoteData[36] = 0;//wSOCDisChargeVoltLimit
    wRemoteData[37] = 0;//wSOCChargeCurrLimt
    wRemoteData[38] = 0;//wSOCDisChargeCurrLimit
    wRemoteData[39] = 0;//wSOCFloatChargeVolt
    wRemoteData[40] = 0;//wSOCSToFChargeCurr
    wRemoteData[41] = 0;//reserver    
    wRemoteData[42] = 0;//fLCDIOState
    wRemoteData[43] = 0;//reserver
    wRemoteData[44] = 0;//reserver
    wRemoteData[45] = 0;//reserver
    wRemoteData[46] = 0;//reserver
    wRemoteData[47] = 0;//reserver
    wRemoteData[48] = 0;//reserver
    wRemoteData[49] = 0;//reserver
    fPCSState.all = 0;
}
//---------------------------------------------------------------------------
// InitECan:
// Called by: main()
//---------------------------------------------------------------------------

void InitECana(void)		// Initialize eCAN-A module
{

struct ECAN_REGS ECanaShadow;
struct LAM_REGS ECanLamShadow;
    /* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/
    EALLOW;
    ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
    ECanaShadow.CANTIOC.bit.TXFUNC = 1;
    ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;
    ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
    ECanaShadow.CANRIOC.bit.RXFUNC = 1;
    ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;
    
    /* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
	// HECC mode also enables time-stamping feature
    
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.SCB = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
    /* Initialize all bits of 'Master Control Field' to zero */
    // Some bits of MSGCTRL register come up in an unknown state. For proper operation,
    // all bits (including reserved bits) of MSGCTRL must be initialized to zero
    //ECanaRegs.CANMC.bit.SCB = 1;
    
    ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX3.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX4.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX5.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX6.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX7.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX8.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX9.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;
   
    // TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
    //	as a matter of precaution.
    
	ECanaRegs.CANTA.all	= 0xFFFFFFFF;	/* Clear all TAn bits */

	ECanaRegs.CANRMP.all = 0xFFFFFFFF;	/* Clear all RMPn bits */

	ECanaRegs.CANGIF0.all = 0xFFFFFFFF;	/* Clear all interrupt flag bits */
	ECanaRegs.CANGIF1.all = 0xFFFFFFFF;
    

    /* Configure bit timing parameters for eCANA*/
    
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

    do
	{
	    ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 1 );  		// Wait for CCE bit to be set..

    /* Configure the eCAN timing parameters */
    
    ECanaShadow.CANBTC.all = ECanaRegs.CANBTC.all;
    ECanaShadow.CANBTC.bit.BRPREG = 10-1;//TQ 7.5M
    ECanaShadow.CANBTC.bit.TSEG2REG = 2;
    ECanaShadow.CANBTC.bit.TSEG1REG = 10;//Baud Rate = 7.5M/(TSEG1REG + 1 + TSEG2REG + 1 + 1) = 500k
    ECanaShadow.CANBTC.bit.SAM = 1;//0 1 = Samp Tree Timer
    ECanaShadow.CANBTC.bit.SJWREG = 0;
    ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;
    
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR = 0 ;      // Set CCR = 0
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    
	ECanLamShadow.LAM1.bit.LAMI = 0;
    ECanLamShadow.LAM1.bit.LAM_H = 0;
	ECanLamShadow.LAM1.bit.LAM_L = 0xff00;//本机地址、目标地址不需要匹配
	//ECanLamShadow.LAM6.bit.LAM_L = 0;//所有数据严格匹配
    
    ECanaLAMRegs.LAM0.all = ECanLamShadow.LAM1.all;
	ECanaLAMRegs.LAM1.all = ECanLamShadow.LAM1.all;
	ECanaLAMRegs.LAM2.all = ECanLamShadow.LAM1.all;
	ECanaLAMRegs.LAM3.all = ECanLamShadow.LAM1.all;	
	ECanaLAMRegs.LAM4.all = ECanLamShadow.LAM1.all;
	ECanaLAMRegs.LAM5.all = ECanLamShadow.LAM1.all;
	ECanaLAMRegs.LAM6.all = ECanLamShadow.LAM1.all;
	ECanaLAMRegs.LAM7.all = ECanLamShadow.LAM1.all;
	ECanaLAMRegs.LAM13.all = ECanLamShadow.LAM1.all;

	ECanLamShadow.LAM15.bit.LAMI = 0;
	ECanLamShadow.LAM15.bit.LAM_H = 0;
	ECanLamShadow.LAM15.bit.LAM_L = ((INT16U)(1<<(fLocalPhyAddr-1))) ^ 0xFFFF;
    ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    do
    {
       ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..

    /* Disable all Mailboxes  */
    ECanaRegs.CANME.all = 0;		// Required before writing the MSGIDs
      
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.STM = 0;       // Configure CAN for normal mode 0
    ECanaShadow.CANMC.bit.SCB = 1;       // User has selected eCAN mode
    ECanaShadow.CANMC.bit.DBO = 1;       // Set byte order: least significant first,低8位先发送
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
        
    /* Configure CAN interrupts */
    
    ECanaShadow.CANMIM.all = ECanaRegs.CANMIM.all;
    ECanaShadow.CANMIM.all = 0x00001FFF;  //32个邮箱，分别使能或者禁止
    ECanaRegs.CANMIM = ECanaShadow.CANMIM;
    
    ECanaRegs.CANOPC.all = ECanaShadow.CANMIM.all;

    ECanaShadow.CANMIL.all = ECanaRegs.CANMIL.all;
    ECanaShadow.CANMIL.all = 0xFFFFFFFF;      //eCanINT1,发送和接受中断在CANINT1中
    ECanaRegs.CANMIL = ECanaShadow.CANMIL;
    
    ECanaShadow.CANGIM.all = ECanaRegs.CANGIM.all;
	ECanaShadow.CANGIM.bit.MTOM = 1;     //超时中断使能
	ECanaShadow.CANGIM.bit.TCOM = 0;     //定时邮递中断禁止
	ECanaShadow.CANGIM.bit.AAIM = 1;
	ECanaShadow.CANGIM.bit.WDIM = 1;
	ECanaShadow.CANGIM.bit.WUIM = 1;
	ECanaShadow.CANGIM.bit.RMLIM = 1;
	ECanaShadow.CANGIM.bit.BOIM = 1;
	ECanaShadow.CANGIM.bit.EPIM = 1;
	ECanaShadow.CANGIM.bit.WLIM = 1;
    ECanaShadow.CANGIM.bit.GIL = 0;      //GIL value determines ECana(0/1)INT，其他所有系统中断在CANINT0中
    ECanaShadow.CANGIM.bit.I0EN = 1;     //使能ECanINT0中断
    ECanaShadow.CANGIM.bit.I1EN = 1;
    ECanaRegs.CANGIM.all = ECanaShadow.CANGIM.all;

	PieCtrlRegs.PIEIER9.bit.INTx5 = 1;	// Enable ECAP1_INT in PIE group 4
    sCANIntDisable();//PieCtrlRegs.PIEIER9.bit.INTx6 = 1;
	IER |= M_INT9;						// Enable INT4 in IER to enable PIE group 4
    EDIS;
}
//---------------------------------------------------------------------------
// ResetECan:
// Called by : protect.c : sCANActiveChk();;
//---------------------------------------------------------------------------
void sResetECana(void)		// Initialize eCAN-A module
{
struct ECAN_REGS ECanaShadow;
    //struct LAM_REGS ECanLamShadow;
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;//RESET
	ECanaShadow.CANMC.bit.SRES = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    /* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/
    EALLOW;
    
    ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX3.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX4.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX5.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX6.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX7.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX8.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX9.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;
   
    // TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
    //	as a matter of precaution.
    
	ECanaRegs.CANTA.all	= 0xFFFFFFFF;	/* Clear all TAn bits */
	ECanaRegs.CANRMP.all = 0xFFFFFFFF;	/* Clear all RMPn bits */
	ECanaRegs.CANGIF0.all = 0xFFFFFFFF;	/* Clear all interrupt flag bits */
	ECanaRegs.CANGIF1.all = 0xFFFFFFFF;
    

    /* Disable all Mailboxes  */
    ECanaRegs.CANME.all = 0;		// Required before writing the MSGIDs
      
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.STM = 0;       // Configure CAN for normal mode 0
    ECanaShadow.CANMC.bit.SCB = 1;       // User has selected eCAN mode
    ECanaShadow.CANMC.bit.DBO = 1;       // Set byte order: least significant first
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
        
    /* Configure CAN interrupts */
    
    ECanaShadow.CANMIM.all = ECanaRegs.CANMIM.all;
    ECanaShadow.CANMIM.all = 0x00001FFF;
    ECanaRegs.CANMIM = ECanaShadow.CANMIM;
    
    ECanaRegs.CANOPC.all = ECanaShadow.CANMIM.all;

    ECanaShadow.CANMIL.all = ECanaRegs.CANMIL.all;
    ECanaShadow.CANMIL.all = 0xFFFFFFFF;      //eCanINT1,发送和接受中断在CANINT1中
    ECanaRegs.CANMIL = ECanaShadow.CANMIL;
    
    ECanaShadow.CANGIM.all = ECanaRegs.CANGIM.all;
	ECanaShadow.CANGIM.bit.MTOM = 1;     //超时中断使能
	ECanaShadow.CANGIM.bit.TCOM = 0;     //定时邮递中断禁止
	ECanaShadow.CANGIM.bit.AAIM = 1;
	ECanaShadow.CANGIM.bit.WDIM = 1;
	ECanaShadow.CANGIM.bit.WUIM = 1;
	ECanaShadow.CANGIM.bit.RMLIM = 1;
	ECanaShadow.CANGIM.bit.BOIM = 1;
	ECanaShadow.CANGIM.bit.EPIM = 1;
	ECanaShadow.CANGIM.bit.WLIM = 1;
    ECanaShadow.CANGIM.bit.GIL = 0;      //GIL value determines ECana(0/1)INT，其他所有系统中断在CANINT0中
    ECanaShadow.CANGIM.bit.I0EN = 1;     //使能ECanINT0中断
    ECanaShadow.CANGIM.bit.I1EN = 1;
    ECanaRegs.CANGIM.all = ECanaShadow.CANGIM.all;

	PieCtrlRegs.PIEIER9.bit.INTx5 = 1;	// Enable ECAP1_INT in PIE group 4
    PieCtrlRegs.PIEIER9.bit.INTx6 = 1;
	IER |= M_INT9;						// Enable INT4 in IER to enable PIE group 4
    EDIS;
    
     fCANBCongfigFinish = 0;
}

/*********************************************************************                                
Function name:  void configeCANAmbx(INT16U mbxType, INT16U mbxNo, INT32U msgID, INT16U msgType)                                 
Description:  Config CAN BOX                                                    
Calls:       	                                                                                      
Called By: sCANConfigID()                       
Parameters:INT16U mbxType, INT16U mbxNo, INT32U msgID, INT16U msgType
           mbxType:0:Rx; 1:Tx
		   mbxNo: the CAN box No
           msgID:ID
           msgType:1,Extend ID 
                   0,Standby ID
           msgTPL: transmit priority level                                                                              
Return:void                                                                            
 *********************************************************************/ 
void configeCANAmbx(INT16U mbxType, INT16U mbxNo, INT32U msgID, INT16U msgType,INT16U msgTPL)
{
    INT32U maskRx, maskTx,i;
    struct ECAN_REGS ECanaShadow;
    volatile struct MBOX *mbx = &ECanaMboxes.MBOX0 + mbxNo;
    maskTx = mbxNo==15 ? 0x8000 : ( mbxNo<16 ? 0x1<<mbxNo : 0x10000<<mbxNo-16 );
    maskRx = mbxNo==15 ? 0x8000 : ( mbxNo<16 ? 0x1<<mbxNo : 0x10000<<mbxNo-16 );
    fConfigeCANmbx = 1;
    mbx->MSGCTRL.bit.RTR = 0;
   
    ECanaRegs.CANTRR.all = maskTx;
    do
    {
        ECanaShadow.CANTRS.all = ECanaRegs.CANTRS.all;
    }while((ECanaShadow.CANTRS.all&maskTx) == maskTx);
    
    //mbx->MSGCTRL.all = 0x00000000;
    ECanaShadow.CANOPC.all = ECanaRegs.CANOPC.all; 
    
    ECanaShadow.CANME.all = ECanaRegs.CANME.all; 
    ECanaRegs.CANME.all = 0x00000000;
    
    if (msgType==1)
    {
        mbx->MSGID.all = msgID;            /* For Extended ID use bits  0:28 */
    }
    else
    { 
        mbx->MSGID.bit.STDMSGID = msgID;   /* For Standard ID use bits 18:28 */
    }

    mbx->MSGID.bit.AME = 1;
    mbx->MSGID.bit.AAM = 0;
    mbx->MSGID.bit.IDE = msgType;
    mbx->MSGCTRL.bit.TPL = msgTPL;
	if(mbxType == 1)
    {
	    i = ~maskTx;
	    ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
        ECanaShadow.CANMD.all &= i;//为可重复配置，此处必须分开
        ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;   
        
        ECanaShadow.CANOPC.all &= i; 
        ECanaRegs.CANOPC.all = ECanaShadow.CANOPC.all; 
	}
	else if(mbxType == 0)
	{
	    ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
        ECanaShadow.CANMD.all |= maskRx;
        ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;
        
        ECanaShadow.CANOPC.all |= maskRx;
        ECanaRegs.CANOPC.all = ECanaShadow.CANOPC.all; 
	}
       
    ECanaShadow.CANME.all |= maskRx;
    ECanaRegs.CANME.all = ECanaShadow.CANME.all;
    fConfigeCANmbx = 0;
}
/*********************************************************************                                
Function name:void CANTX(INT16S MDHH,INT16S MDHL,INT16S MDLH,INT16S MDLL,INT16U DataNum,INT16U mbxNo)                                  
Description:                                                   
Calls:      	                                                                                      
Called By:                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void CANTX(INT16S MDHH,INT16S MDHL,INT16S MDLH,INT16S MDLL,INT16U DataNum,INT16U mbxNo)                
{  
    struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *mbx = &ECanaMboxes.MBOX0 + mbxNo;
	INT32U i;

    i = mbxNo==15 ? 0x8000 : ( mbxNo<16 ? 0x1<<mbxNo : 0x10000<<mbxNo-16 );
	mbx->MDL.word.LOW_WORD = MDHH;
    mbx->MDL.word.HI_WORD = MDHL;
	mbx->MDH.word.LOW_WORD = MDLH;
	mbx->MDH.word.HI_WORD = MDLL;
	mbx->MSGCTRL.bit.DLC = DataNum;
    ECanaShadow.CANTRS.all=ECanaRegs.CANTRS.all;    
    ECanaShadow.CANTRS.all |= i;  
    ECanaRegs.CANTRS.all =ECanaShadow.CANTRS.all;
}
/*********************************************************************                                
Function name:void sCANRXImportRead(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXImportRead(volatile struct MBOX *MailBOX)
{
U_CANCOMD wCANRXComd;
    
	wCANRXComd.all = MailBOX->MDL.word.LOW_WORD;
    
    if(fConfigeCANmbx == 0)
    {
        if(fImportTx == 0)
        {
        	fImportTx = 1;
        	if((wCANRXComd.bit.DataAddr < ImportDataNum)&&(wCANRXComd.bit.DataLength <= ImportDataNum))
        	{
        	    uwImportDataAddr = wCANRXComd.bit.DataAddr;
        	    
        	    if((uwImportDataAddr + wCANRXComd.bit.DataLength) > ImportDataNum)
        	    {
        	    	uwImportDataLength = ImportDataNum - uwImportDataAddr;
        	    }
        	    else
        	    {
        	    	uwImportDataLength = wCANRXComd.bit.DataLength;
        	    }
        	}
        	else
        	{
        		uwImportDataAddr = ImportDataNum - 1;
        		uwImportDataLength = 1;
        	}
        	
        	uwImportDataCnt = 0;
        }
	}
}
/*********************************************************************                                
Function name:void sCANRXImportWrite(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXImportWrite(volatile struct MBOX *MailBOX)
{
    U_CANCOMD wCANRXComd;
    INT16S wCANRXdat1,wCANRXdat2,wCANRXdat3;
    INT16U i;
    
	wCANRXComd.all = MailBOX->MDL.word.LOW_WORD;
    wCANRXdat1 = MailBOX->MDL.word.HI_WORD;
	wCANRXdat2 = MailBOX->MDH.word.LOW_WORD;
    wCANRXdat3 = MailBOX->MDH.word.HI_WORD;
    
    if(fConfigeCANmbx == 0)
    {
    	if((wCANRXComd.bit.DataAddr < ImportDataNum)&&(wCANRXComd.bit.DataLength <= ImportDataNum)&&((wCANRXComd.bit.DataAddr + wCANRXComd.bit.DataLength) <= ImportDataNum))
    	{    	
            if(wCANRXComd.bit.DataLength == 3)
            {
            	i = wCANRXComd.bit.DataAddr;
            	if((wCANRXdat1 <= wEepromDefaultImptMaxArray[i])&&(wCANRXdat1 >= wEepromDefaultImptMinArray[i]))
            	{
            		wLCDImptData[i] = wCANRXdat1;
            		uwLCDRxImptDataFlag.flag |= 1<<i;
            	}
            	
            	i = wCANRXComd.bit.DataAddr + 1;
            	if((wCANRXdat2 <= wEepromDefaultImptMaxArray[i])&&(wCANRXdat2 >= wEepromDefaultImptMinArray[i]))
            	{
            		wLCDImptData[i] = wCANRXdat2;
            		uwLCDRxImptDataFlag.flag |= 1<<i;
            	}
            	
            	i = wCANRXComd.bit.DataAddr + 2;
            	if((wCANRXdat3 <= wEepromDefaultImptMaxArray[i])&&(wCANRXdat3 >= wEepromDefaultImptMinArray[i]))
            	{
            		wLCDImptData[i] = wCANRXdat3;
            		uwLCDRxImptDataFlag.flag |= 1<<i;
            	}
            }
            else if(wCANRXComd.bit.DataLength == 2)
            {
            	i = wCANRXComd.bit.DataAddr;
            	if((wCANRXdat1 <= wEepromDefaultImptMaxArray[i])&&(wCANRXdat1 >= wEepromDefaultImptMinArray[i]))
            	{
            		wLCDImptData[i] = wCANRXdat1;
            		uwLCDRxImptDataFlag.flag |= 1<<i;
            	}
            	
            	i = wCANRXComd.bit.DataAddr + 1;
            	if((wCANRXdat2 <= wEepromDefaultImptMaxArray[i])&&(wCANRXdat2 >= wEepromDefaultImptMinArray[i]))
            	{
            		wLCDImptData[i] = wCANRXdat2;
            		uwLCDRxImptDataFlag.flag |= 1<<i;
            	}
            }
            else
            {
            	i = wCANRXComd.bit.DataAddr;
            	if((wCANRXdat1 <= wEepromDefaultImptMaxArray[i])&&(wCANRXdat1 >= wEepromDefaultImptMinArray[i]))
            	{
            		wLCDImptData[i] = wCANRXdat1;
            		uwLCDRxImptDataFlag.flag |= 1<<i;
            	}
            }
        }
	}
} 
/*********************************************************************                                
Function name:void sCANRXHoldRead(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXHoldRead(volatile struct MBOX *MailBOX) 
{
U_CANCOMD wCANRXComd;
    
	wCANRXComd.all = MailBOX->MDL.word.LOW_WORD;
    
    if(fConfigeCANmbx == 0)
    {
        if(fHoldTx == 0)
        {
        	fHoldTx = 1;//发送位置位
        	if((wCANRXComd.bit.DataAddr < HoldRegisterNum)&&(wCANRXComd.bit.DataLength <= HoldRegisterNum))
        	{
        	    uwHoldDataAddr = wCANRXComd.bit.DataAddr;
        	    
        	    if((uwHoldDataAddr + wCANRXComd.bit.DataLength) > HoldRegisterNum)
        	    {
        	    	uwHoldDataLength = HoldRegisterNum - uwHoldDataAddr;
        	    }
        	    else
        	    {
        	    	uwHoldDataLength = wCANRXComd.bit.DataLength;
        	    }
        	}
        	else
        	{
        		uwHoldDataAddr = HoldRegisterNum - 1;
        		uwHoldDataLength = 1;
        	}
        	
        	uwHoldDataCnt = 0;
        }
	}
}
/*********************************************************************                                
Function name:void sCANRXInputRead(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXInputRead(volatile struct MBOX *MailBOX) 
{
U_CANCOMD wCANRXComd;
    
	wCANRXComd.all = MailBOX->MDL.word.LOW_WORD;
    
    if(fConfigeCANmbx == 0)
    {
        if(fInputTx == 0)
        {
        	fInputTx = 1;
            
            if((wCANRXComd.bit.DataAddr < InputRegisterNum)&&(wCANRXComd.bit.DataLength <= InputRegisterNum))
            {
                uwInputDataAddr = wCANRXComd.bit.DataAddr;
                
                if((uwInputDataAddr + wCANRXComd.bit.DataLength) > InputRegisterNum)
                {
                	uwInputDataLength = InputRegisterNum - uwInputDataAddr;
                }
                else
                {
                	uwInputDataLength = wCANRXComd.bit.DataLength;
                }
            }
            else
            {
            	uwInputDataAddr = InputRegisterNum - 1;
            	uwInputDataLength = 1;
            }
            
            uwInputDataCnt = 0;
        }
	}
}   
/*********************************************************************                                
Function name:void sCANRXRemoteRead(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXRemoteRead(volatile struct MBOX *MailBOX) 
{
U_CANCOMD wCANRXComd;
    
	wCANRXComd.all = MailBOX->MDL.word.LOW_WORD;
    
    if(fConfigeCANmbx == 0)
    {
        if(fRemoteTx == 0)
        {
        	fRemoteTx = 1;
        	
        	if((wCANRXComd.bit.DataAddr < RemoteDataNum)&&(wCANRXComd.bit.DataLength <= RemoteDataNum))
        	{
        	    uwRemoteDataAddr = wCANRXComd.bit.DataAddr;
        	    
        	    if((uwRemoteDataAddr + wCANRXComd.bit.DataLength) > RemoteDataNum)
        	    {
        	    	uwRemoteDataLength = RemoteDataNum - uwRemoteDataAddr;
        	    }
        	    else
        	    {
        	    	uwRemoteDataLength = wCANRXComd.bit.DataLength;
        	    }
        	}
        	else
        	{
        		uwRemoteDataAddr = RemoteDataNum - 1;
        		uwRemoteDataLength = 1;
        	}
        	        	      	
        	uwRemoteDataCnt = 0;
        }
	}
}  
/*********************************************************************                                
Function name:void sCANRXHoldWrite(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXHoldWrite(volatile struct MBOX *MailBOX) 
{
    U_CANCOMD wCANRXComd;
    INT16S wCANRXdat1,wCANRXdat2,wCANRXdat3;
    
	wCANRXComd.all = MailBOX->MDL.word.LOW_WORD;
    wCANRXdat1 = MailBOX->MDL.word.HI_WORD;
	wCANRXdat2 = MailBOX->MDH.word.LOW_WORD;
    wCANRXdat3 = MailBOX->MDH.word.HI_WORD;
    
    if(fConfigeCANmbx == 0)
    {
    	if((wCANRXComd.bit.DataAddr < HoldRegisterNum)&&(wCANRXComd.bit.DataLength <= HoldRegisterNum)&&((wCANRXComd.bit.DataAddr + wCANRXComd.bit.DataLength) <= HoldRegisterNum))
    	{    	
            if(wCANRXComd.bit.DataLength == 3)
            {
            	wHoldRegister[wCANRXComd.bit.DataAddr] = wCANRXdat1;
            	wHoldRegister[wCANRXComd.bit.DataAddr + 1] = wCANRXdat2;
            	wHoldRegister[wCANRXComd.bit.DataAddr + 2] = wCANRXdat3;
            }
            else if(wCANRXComd.bit.DataLength == 2)
            {
            	wHoldRegister[wCANRXComd.bit.DataAddr] = wCANRXdat1;
            	wHoldRegister[wCANRXComd.bit.DataAddr + 1] = wCANRXdat2;
            }
            else
            {
            	wHoldRegister[wCANRXComd.bit.DataAddr] = wCANRXdat1;
            }
            fHoldRegisterStatus = 1;
        }
	}
} 
/*********************************************************************                                
Function name:void sCANRXRemoteWrite(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXRemoteWrite(volatile struct MBOX *MailBOX) 
{
    U_CANCOMD wCANRXComd;
    INT16S wCANRXdat1,wCANRXdat2,wCANRXdat3;
    INT16U i;
    
	wCANRXComd.all = MailBOX->MDL.word.LOW_WORD;
    wCANRXdat1 = MailBOX->MDL.word.HI_WORD;
	wCANRXdat2 = MailBOX->MDH.word.LOW_WORD;
    wCANRXdat3 = MailBOX->MDH.word.HI_WORD;
    
    if(fConfigeCANmbx == 0)
    {
        if(((wCANRXComd.bit.DataAddr + wCANRXComd.bit.DataLength) <= RemoteDataNum)&&(wCANRXComd.bit.DataAddr < RemoteDataNum)&&(wCANRXComd.bit.DataLength <= RemoteDataNum))
        {
            if(wCANRXComd.bit.DataLength == 3)
            {
            	i = wCANRXComd.bit.DataAddr;
            	if((wCANRXdat1 <= wRemoteDataMax[i])&&(wCANRXdat1 >= wRemoteDataMin[i]))
            	{
            	    if(i == 11)
            	    {
            	        if((wCANRXdat1 <= -800)||(wCANRXdat1 >= 800))
            	        {
            	            wRemoteData[i] = wCANRXdat1;
            	        }
            	    }
            	    else
            	    {
            	        wRemoteData[i] = wCANRXdat1;
            	    }
            	}
            	i = wCANRXComd.bit.DataAddr + 1;
            	if((wCANRXdat2 <= wRemoteDataMax[i])&&(wCANRXdat2 >= wRemoteDataMin[i]))
            	{
            	    if(i == 11)
            	    {
            	        if((wCANRXdat2 <= -800)||(wCANRXdat2 >= 800))
            	        {
            	            wRemoteData[i] = wCANRXdat2;
            	        }
            	    }
            	    else
            	    {
            	        wRemoteData[i] = wCANRXdat2;
            	    }
            	}
            	i = wCANRXComd.bit.DataAddr + 2;
            	if((wCANRXdat3 <= wRemoteDataMax[i])&&(wCANRXdat3 >= wRemoteDataMin[i]))
            	{
            	    if(i == 11)
            	    {
            	        if((wCANRXdat3 <= -800)||(wCANRXdat3 >= 800))
            	        {
            	            wRemoteData[i] = wCANRXdat3;
            	        }
            	    }
            	    else
            	    {
            	        wRemoteData[i] = wCANRXdat3;
            	    }
            	}
            }
            else if(wCANRXComd.bit.DataLength == 2)
            {
            	i = wCANRXComd.bit.DataAddr;
            	if((wCANRXdat1 <= wRemoteDataMax[i])&&(wCANRXdat1 >= wRemoteDataMin[i]))
            	{
            	    if(i == 11)
            	    {
            	        if((wCANRXdat1 <= -800)||(wCANRXdat1 >= 800))
            	        {
            	            wRemoteData[i] = wCANRXdat1;
            	        }
            	    }
            	    else
            	    {
            	        wRemoteData[i] = wCANRXdat1;
            	    }   	    
            	}
            	i = wCANRXComd.bit.DataAddr + 1;
            	if((wCANRXdat2 <= wRemoteDataMax[i])&&(wCANRXdat2 >= wRemoteDataMin[i]))
            	{           	    
            	    if(i == 11)
            	    {
            	        if((wCANRXdat2 <= -800)||(wCANRXdat2 >= 800))
            	        {
            	            wRemoteData[i] = wCANRXdat2;
            	        }
            	    }
            	    else
            	    {
            	        wRemoteData[i] = wCANRXdat2;
            	    }   	    
            	}
            }
            else
            {
            	i = wCANRXComd.bit.DataAddr;
            	if((wCANRXdat1 <= wRemoteDataMax[i])&&(wCANRXdat1 >= wRemoteDataMin[i]))
            	{ 
            	    if(i == 11)
            	    {
            	        if((wCANRXdat1 <= -800)||(wCANRXdat1 >= 800))
            	        {
            	            wRemoteData[i] = wCANRXdat1;
            	        }
            	    }
            	    else
            	    {
            	        wRemoteData[i] = wCANRXdat1;
            	    } 
            	}
            }
        }
    }
}
/*********************************************************************                                
Function name:void sCANRXBootLoader(volatile struct MBOX *MailBOX)                                   
Description:                                                   
Calls:     	                                                                                      
Called By: ECanISR1                       
Parameters:                                                                              
Return:void                                                                            
 *********************************************************************/ 
void sCANRXBootLoader(volatile struct MBOX *MailBOX) 
{
    INT16S wCANRXdat1,wCANRXdat2,wCANRXdat3;
    INT16U wCANTxdat1,wCANTxdat2,wCANTxdat3;
    wCANRXdat1 = MailBOX->MDL.word.LOW_WORD;
	wCANRXdat2 = MailBOX->MDL.word.HI_WORD;
    wCANRXdat3 = MailBOX->MDH.word.LOW_WORD;
    if((wCANRXdat1 == 0x6161)&&(wCANRXdat2 == 0x5555)&&(wCANRXdat3 == 0x0101))
    {
    	fDisBootloaderOrder = 0;  //此时发出停机命令
    	wCANTxdat1 = 0x6161;
    	wCANTxdat2 = 0x5555;
    	if(uwBootloaderStep	== 2)
        {
        	wCANTxdat3 = 0x0202;
        }
        else if(uwBootloaderStep == 3)
        {
        	wCANTxdat3 = 0x0102;
        }
        else
        {
            wCANTxdat3 = 0x0002;
        }           
       	CANTX(wCANTxdat1,wCANTxdat2,wCANTxdat3,0,6,11);
    }
}     
/*********************************************************************                                
Function name:void ECanISR0(void)                                
Description:ECANINT0 Interrupt                                                    
Calls:Interrupt      	                                                                                      
Called By:Interrupt                       
Parameters:void                                                                              
Return:void                                                                            
 *********************************************************************/   
void ECanISR0(void)
{
struct ECAN_REGS ECanaShadow;

    uwDebugCANA1 = 0;
    ECanaShadow.CANGIF0.all = ECanaRegs.CANGIF0.all;
    if(ECanaShadow.CANGIF0.bit.AAIF0 == 1)//发送失败
	{
        ECanaShadow.CANAA.all = 0xFFFFFFFF;
        ECanaRegs.CANAA.all = ECanaShadow.CANAA.all;  
        uwDebugCANA1 |= 1;		
	}
	else if(ECanaShadow.CANGIF0.bit.RMLIF0 == 1)//接受覆盖或丢失
	{
        ECanaShadow.CANRMP.all = 0x0;
        ECanaShadow.CANRMP.all = 0xFFFFFFFF;//dwClear;
        ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;
        uwDebugCANA1 |= 2;
	}
	else if(ECanaShadow.CANGIF0.bit.TCOF0 == 1)
	{
	    ECanaShadow.CANGIF0.all = 0x0;
        ECanaShadow.CANGIF0.bit.TCOF0 = 1;
        ECanaRegs.CANGIF0.all = ECanaShadow.CANGIF0.all;
        uwDebugCANA1 |= 4;
	}
	else if(ECanaShadow.CANGIF0.bit.WDIF0 == 1)
	{
        ECanaShadow.CANGIF0.all = 0x0;
        ECanaShadow.CANGIF0.bit.WDIF0 = 1;
        ECanaRegs.CANGIF0.all = ECanaShadow.CANGIF0.all;
        uwDebugCANA1 |= 8;
	}
	else if(ECanaShadow.CANGIF0.bit.WLIF0 == 1)
	{
	    ECanaShadow.CANGIF0.all = 0x0;
        ECanaShadow.CANGIF0.bit.WLIF0 = 1;
        ECanaRegs.CANGIF0.all = ECanaShadow.CANGIF0.all;
        uwDebugCANA1 |= 16;
	}
	else if(ECanaShadow.CANGIF0.bit.WUIF0 == 1)
	{
    	ECanaShadow.CANGIF0.all = 0x0;
        ECanaShadow.CANGIF0.bit.WUIF0 = 1;
        ECanaRegs.CANGIF0.all = ECanaShadow.CANGIF0.all;
        uwDebugCANA1 |= 32;
	}
	else if(ECanaShadow.CANGIF0.bit.BOIF0 == 1)
	{
	    ECanaShadow.CANGIF0.all = 0x0;
        ECanaShadow.CANGIF0.bit.BOIF0 = 1;
        ECanaRegs.CANGIF0.all = ECanaShadow.CANGIF0.all;
        uwDebugCANA1 |= 64;
	}
	else if(ECanaShadow.CANGIF0.bit.EPIF0 == 1)
	{
	    ECanaShadow.CANGIF0.all = 0x0;
        ECanaShadow.CANGIF0.bit.EPIF0 = 1;
        ECanaRegs.CANGIF0.all = ECanaShadow.CANGIF0.all;
        uwDebugCANA1 |= 128;
	}
}
/*********************************************************************                                
Function name:void ECanISR1(void)                                
Description:ECANINT1 Interrupt                                                    
Calls:Interrupt      	                                                                                      
Called By:Interrupt                       
Parameters:void                                                                              
Return:void                                                                            
 *********************************************************************/  
void ECanISR1(void)
{
struct ECAN_REGS ECanaShadow;
volatile struct MBOX *mbx = &ECanaMboxes.MBOX0 + ECanaRegs.CANGIF1.bit.MIV1;
INT32U dwCANRMP = 0;
INT16U uwMbxNum = 0;
U_CANCOMD wCANTXComd;

    ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
    ECanaShadow.CANGIF1.all = ECanaRegs.CANGIF1.all;
	uwMbxNum = ECanaShadow.CANGIF1.bit.MIV1;
	dwCANRMP = uwMbxNum ==15 ? 0x8000 : ( uwMbxNum<16 ? 0x1<<uwMbxNum : 0x10000<<uwMbxNum-16 );
    if(ECanaShadow.CANGIF1.bit.MTOF1 == 1)
	{
        ECanaShadow.CANTOS.all = 0x0;
        ECanaShadow.CANTOS.all = dwCANRMP;
        ECanaRegs.CANTOS.all = ECanaShadow.CANTOS.all;
	}
	else //if(ECanaShadow.CANGIF1.bit.GMIF1 == 1)
	{
	    if((ECanaShadow.CANMD.all & dwCANRMP) == dwCANRMP)
	    {
	    		switch(uwMbxNum)
	            {
	            	case 0:
	                	sCANRXImportWrite(mbx);
	                	fCANActive = 1;
	                	break;
	                	
	                case 1:
	                	sCANRXRemoteWrite(mbx);
	                	fCANActive = 1;
	                	break;
	                	
	                case 2:
	                	sCANRXHoldWrite(mbx);
	                    fCANActive = 1;
	                    break;	                    	               
	                	
	                case 4:
	                	sCANRXRemoteRead(mbx);
	                	fCANActive = 1;
	                	break;
                    
                    case 3:
	                    sCANRXHoldRead(mbx);
	                    fCANActive = 1;
	                    break;
	                
	                case 6:
	                	sCANRXInputRead(mbx);
	                	fCANActive = 1;
	                    break;
	                
	                case 7:
	                	sCANRXImportRead(mbx);
	                	fCANActive = 1;
	                    break;
	                    
	               case 5:
	                	sCANRXBootLoader(mbx);
	                	fCANActive = 1;
	                	uwDebugCANA2++;
	                	if(uwDebugCANA2>1000)uwDebugCANA2 = 0;
	                    break;
	                default:
	                	break;
	            }     
	        
			ECanaShadow.CANRMP.all = 0x0;
            ECanaShadow.CANRMP.all = dwCANRMP; // request clear RMP for this mailbox only
            ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;// 32-bit register access is reliable only
            //ECanbRegs.CANTA.all = ECanaShadow.CANRMP.all;			
	    }
	    else
	    {
	    	ECanaShadow.CANTA.all = 0x0;
            ECanaShadow.CANTA.all = dwCANRMP; // request clear RMP for this mailbox only
            ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;// 32-bit register access is reliable only
            ECanaRegs.CANRMP.all = ECanaShadow.CANTA.all;
            
	    	switch(uwMbxNum)
	    	{
	    		case 8:
	    			wCANTXComd.bit.DataAddr = uwInputDataAddr + uwInputDataCnt;
	    	    	if((uwInputDataCnt + 3) <= uwInputDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 3;
	    	    		uwInputDataCnt += 3;
        		        CANTX(wCANTXComd.all,wInputRegister[wCANTXComd.bit.DataAddr],wInputRegister[wCANTXComd.bit.DataAddr+1],wInputRegister[wCANTXComd.bit.DataAddr+2],8,8);
	    	    	}
	    	    	else if((uwInputDataCnt + 2) <= uwInputDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 2;
	    	    		uwInputDataCnt += 2;
	    	    		CANTX(wCANTXComd.all,wInputRegister[wCANTXComd.bit.DataAddr],wInputRegister[wCANTXComd.bit.DataAddr+1],0,6,8);
	    	    	}
	    	    	else if((uwInputDataCnt + 1) <= uwInputDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 1;
	    	    		uwInputDataCnt += 1;
	    	    		CANTX(wCANTXComd.all,wInputRegister[wCANTXComd.bit.DataAddr],0,0,4,8);
	    	    	}
	    	    	else
	    	    	{
	    	    		uwInputDataCnt = 0;
	    	    		fInputTx = 0;
        	            uwInputDataAddr = 0;
        	            uwInputDataLength = 0;
	    	    	}
	    	    	break;
	    			
	    	    case 9:
	    	    	wCANTXComd.bit.DataAddr = uwHoldDataAddr + uwHoldDataCnt;
	    	    	if((uwHoldDataCnt + 3) <= uwHoldDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 3;
	    	    		uwHoldDataCnt += 3;
        		        CANTX(wCANTXComd.all,wHoldRegister[wCANTXComd.bit.DataAddr],wHoldRegister[wCANTXComd.bit.DataAddr+1],wHoldRegister[wCANTXComd.bit.DataAddr+2],8,9);
	    	    	}
	    	    	else if((uwHoldDataCnt + 2) <= uwHoldDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 2;
	    	    		uwHoldDataCnt += 2;
	    	    		CANTX(wCANTXComd.all,wHoldRegister[wCANTXComd.bit.DataAddr],wHoldRegister[wCANTXComd.bit.DataAddr+1],0,6,9);
	    	    	}
	    	    	else if((uwHoldDataCnt + 1) <= uwHoldDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 1;
	    	    		uwHoldDataCnt += 1;
	    	    		CANTX(wCANTXComd.all,wHoldRegister[wCANTXComd.bit.DataAddr],0,0,4,9);
	    	    	}
	    	    	else
	    	    	{
	    	    		uwHoldDataCnt = 0;
	    	    		fHoldTx = 0;
        	            uwHoldDataAddr = 0;
        	            uwHoldDataLength = 0;
	    	    	}
	    	    	break;
	    	    	
	    	    case 10:	    	    	
	    	    	wCANTXComd.bit.DataAddr = uwRemoteDataAddr + uwRemoteDataCnt;	    	    	
	    	    	if((uwRemoteDataCnt + 3) <= uwRemoteDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 3;
	    	    		uwRemoteDataCnt += 3;
        		        CANTX(wCANTXComd.all,wRemoteData[wCANTXComd.bit.DataAddr],wRemoteData[wCANTXComd.bit.DataAddr+1],wRemoteData[wCANTXComd.bit.DataAddr+2],8,10);
	    	    	}
	    	    	else if((uwRemoteDataCnt + 2) <= uwRemoteDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 2;
	    	    		uwRemoteDataCnt += 2;
	    	    		CANTX(wCANTXComd.all,wRemoteData[wCANTXComd.bit.DataAddr],wRemoteData[wCANTXComd.bit.DataAddr+1],0,6,10);
	    	    	}
	    	    	else if((uwRemoteDataCnt + 1) <= uwRemoteDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 1;
	    	    		uwRemoteDataCnt += 1;
	    	    		CANTX(wCANTXComd.all,wRemoteData[wCANTXComd.bit.DataAddr],0,0,4,10);
	    	    	}
	    	    	else
	    	    	{
	    	    		uwRemoteDataCnt = 0;
	    	    		fRemoteTx = 0;
        	            uwRemoteDataAddr = 0;
        	            uwRemoteDataLength = 0;
	    	    	}
	    	    	break;
	    	    		    	    	
//	    	    case 11:
//	    	    	fRemoteDCBreakerCtrEn = 0;
//	    	    	break; 
	    	    	
	    	    case 12:	    	    	
	    	    	wCANTXComd.bit.DataAddr = uwImportDataAddr + uwImportDataCnt;	    	    	
	    	    	if((uwImportDataCnt + 3) <= uwImportDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 3;
	    	    		uwImportDataCnt += 3;
        		        CANTX(wCANTXComd.all,wLCDImptData[wCANTXComd.bit.DataAddr],wLCDImptData[wCANTXComd.bit.DataAddr+1],wLCDImptData[wCANTXComd.bit.DataAddr+2],8,12);
	    	    	}
	    	    	else if((uwImportDataCnt + 2) <= uwImportDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 2;
	    	    		uwImportDataCnt += 2;
	    	    		CANTX(wCANTXComd.all,wLCDImptData[wCANTXComd.bit.DataAddr],wLCDImptData[wCANTXComd.bit.DataAddr+1],0,6,12);
	    	    	}
	    	    	else if((uwImportDataCnt + 1) <= uwImportDataLength)
	    	    	{
	    	    		wCANTXComd.bit.DataLength = 1;
	    	    		uwImportDataCnt += 1;
	    	    		CANTX(wCANTXComd.all,wLCDImptData[wCANTXComd.bit.DataAddr],0,0,4,12);
	    	    	}
	    	    	else
	    	    	{
	    	    		uwImportDataCnt = 0;
	    	    		fImportTx = 0;
        	            uwImportDataAddr = 0;
        	            uwImportDataLength = 0;
	    	    	}
	    	    	break;
	    	    
	    	    default:
	    	    	break;
	    	}	        
	    }
	}
}
/*********************************************************************     
Function name: void sHoldComdDeal(void)                                             
Description:  Command parameter of HoldRegister                             
Calls:        	                                                           
Called By:   sCANDataMange(), period: 20ms                                                          
Parameters: void                                                         
Return:     void		                                                       
 *********************************************************************/  
void sHoldComdDeal(void)
{
INT16S wSysFreq;
    if(wEepromRatedFreqSel == 1)
    {
        wSysFreq = 6000;
    }
    else
    {
        wSysFreq = 5000;
    }
    if(wHoldRegister[188] == 0xAAAA)  //恢复出厂
    {
        if(fCallDefaultValueOrder == 0)
        {
            fCallDefaultValueOrder = 2;
            wHoldRegister[188] = 0xA5A5; 
            sEepromProtectDataSetDefaultVaule(84); //恢复所有的数值出厂值
            sEepromProtectSupplDataSetDefaultVaule(70);
			//sEepromwHisDataSetDefaultVaule(7);  //恢复历史数据
			fEepromOperateRequire = 1;
            
            //第一组，电网电压保护设置
            wHoldRegister[0] = wEepromGridVolMax_1;
            wHoldRegister[1] = wEepromVolMaxTripTime_1;
            wHoldRegister[2] = wEepromGridVolMin_1;
            wHoldRegister[3] = wEepromVolMinTripTime_1;
            wHoldRegister[4] = wEepromGridVolMax_2;
            wHoldRegister[5] = wEepromVolMaxTripTime_2;
            wHoldRegister[6] = wEepromGridVolMin_2; 
            wHoldRegister[7] = wEepromVolMinTripTime_2;//
            wHoldRegister[8] = wEepromGridVolRecover_Max;
            wHoldRegister[9] = wEepromGridVolRecover_Min;
            wHoldRegister[10] = wEepromGridVolRecover_Time;
            wHoldRegister[11] = wEepromGridVoltBalanceMax;			
            wHoldRegister[12] = wEepromGridVolMax_3;
            wHoldRegister[13] = wEepromVolMaxTripTime_3;           
        	wHoldRegister[14] = wEepromGridVolMin_3;			
            wHoldRegister[15] = wEepromVolMinTripTime_3; 
            wHoldRegister[16] = wEepromPowerOnWaitTime;
            wHoldRegister[17] = 0;//reserver
            wHoldRegister[18] = 0;//reserver
            wHoldRegister[19] = 0;//reserver
            wHoldRegister[20] = 0;//reserver
            wHoldRegister[21] = 0;//reserver
            wHoldRegister[22] = 0;//reserver
            wHoldRegister[23] = 0;//reserver
            wHoldRegister[24] = 0;//reserver
            wHoldRegister[25] = 0;//reserver    
            wHoldRegister[26] = 0;//reserver
            wHoldRegister[27] = 0;//reserver
            wHoldRegister[28] = 0;//reserver
            wHoldRegister[29] = 0;//reserver
            //第2组，电网频率保护设置    
            wHoldRegister[30] = wEepromGridFreqMax_1 + wSysFreq;
            wHoldRegister[31] = wEepromFreqMaxTripTime_1;
            wHoldRegister[32] = wEepromGridFreqMin_1 + wSysFreq;
            wHoldRegister[33] = wEepromFreqMinTripTime_1;
            wHoldRegister[34] = wEepromGridFreqMax_2 + wSysFreq;
            wHoldRegister[35] = wEepromFreqMaxTripTime_2;
            wHoldRegister[36] = wEepromGridFreqMin_2 + wSysFreq;
            wHoldRegister[37] = wEepromFreqMinTripTime_2;
            wHoldRegister[38] = wEepromGridFreqRecover_Max + wSysFreq;
            wHoldRegister[39] = wEepromGridFreqRecover_Min + wSysFreq;
            wHoldRegister[40] = wEepromGridFreqRecover_Time;
            wHoldRegister[41] = 0;//reserver
            wHoldRegister[42] = 0;//reserver
            wHoldRegister[43] = 0;//reserver
            wHoldRegister[44] = 0;//reserver
            wHoldRegister[45] = 0;//reserver
            wHoldRegister[46] = 0;//reserver
            wHoldRegister[47] = 0;//reserver
            wHoldRegister[48] = 0;//reserver
            wHoldRegister[49] = wEepromPVFitFactor;
            //第3组，开关机的参数设置
            wHoldRegister[50] = wEepromPVStartUpVol;			
            wHoldRegister[51] = wEepromSoftPowerStep;
            wHoldRegister[52] = 0;
            wHoldRegister[53] = wEepromSoftStopEn;
            wHoldRegister[54] = wEepromStopPowerStep;
            wHoldRegister[55] = wEepromIsoImpMin;
			wHoldRegister[56] = wEepromGridFaultPowerStep;
            wHoldRegister[57] = wEepromAIEnable;//reserver
            wHoldRegister[58] = 0;//reserver
            wHoldRegister[59] = 0;//reserver
            wHoldRegister[60] = wEepromGFCIMax;//reserver
            wHoldRegister[61] = wEepromDCIMax;//reserver
            wHoldRegister[62] = 0;//reserver
            wHoldRegister[63] = 0;//reserver
            wHoldRegister[64] = wEepromISOEnable;
            wHoldRegister[65] = wEepromGFCICTChkEn; 
            wHoldRegister[66] = wEepromAPFOption;
            wHoldRegister[67] = wEepromArcingEn;
            wHoldRegister[68] = wEepromFullRangeMpptEnable;//reserver
            wHoldRegister[69] = wEepromFullRangeMpptCycle;//reserver
            //第4组，功率参数
            wHoldRegister[70] = wEepromCSPCtrModeSel;
            wHoldRegister[71] = wEepromCSPPer;     
            wHoldRegister[72] = wEepromCSCurrSet;
            wHoldRegister[73] = wEepromPActStep;
            wHoldRegister[74] = wEepromCSQCtrModeSel;
            wHoldRegister[75] = wEepromCSPFSet;
            wHoldRegister[76] = wEepromCSQPer;
            wHoldRegister[77] = wEepromQActStep;
            wHoldRegister[78] = 0;//reserver
            wHoldRegister[79] = 0;//reserver
            wHoldRegister[80] = wEepromFWRecPowerStep; 
            wHoldRegister[81] = wEepromFWActTime;
            wHoldRegister[82] = wEepromQUActTime;
            wHoldRegister[83] = wEepromVWActTime;
            wHoldRegister[84] = wEepromPF_PCurveActPw1; 
            wHoldRegister[85] = wEepromPF_PCurvePF1;
            wHoldRegister[86] = wEepromPF_PCurveActPw2;
            wHoldRegister[87] = wEepromPF_PCurvePF2;
            wHoldRegister[88] = wEepromPF_PCurveLockInV;
            wHoldRegister[89] = wEepromPF_PCurveLockOutV;
            wHoldRegister[90] = wEepromQ_UCurveVolt1s;
            wHoldRegister[91] = wEepromQ_UCurveReactPw1s;
            wHoldRegister[92] = wEepromQ_UCurveVolt2s;  
            wHoldRegister[93] = wEepromQ_UCurveReactPw2s;
            wHoldRegister[94] = wEepromQ_UCurveVolt1i;   
            wHoldRegister[95] = wEepromQ_UCurveReactPw1i;
            wHoldRegister[96] = wEepromQ_UCurveVolt2i;   
            wHoldRegister[97] = wEepromQ_UCurveReactPw2i;
            wHoldRegister[98] = wEepromQ_UCurveLockInP;    
            wHoldRegister[99] = wEepromQ_UCurveLockOutP;
            wHoldRegister[100] = wEepromOvFWEn;
            wHoldRegister[101] = wEepromOvFWTripFreq + wSysFreq;
            wHoldRegister[102] = wEepromOvFWRatio;            
            wHoldRegister[103] = wEepromOvFWExitFreq + wSysFreq;			
            wHoldRegister[104] = wEepromUnFWEn;            
            wHoldRegister[105] = wEepromUnFWTripFreq + wSysFreq;            
            wHoldRegister[106] = wEepromUnFWRatio;
            wHoldRegister[107] = wEepromUnFWExitFreq + wSysFreq;
            wHoldRegister[108] = wEerpomOvVWEnable;
            wHoldRegister[109] = wEerpomUnVWEnable;
            wHoldRegister[110] = wEepromOvVWStartVolt;
            wHoldRegister[111] = wEepromOvVWStopVolt;
            wHoldRegister[112] = wEepromOvVWRatio;
            wHoldRegister[113] = wEepromUnVWStartVolt;
            wHoldRegister[114] = wEepromUnVWStopVolt;
            wHoldRegister[115] = wEepromUnVWRatio;
            wHoldRegister[116] = 0;//reserver
            wHoldRegister[117] = 0;//reserver
            wHoldRegister[118] = 0;//reserver
            wHoldRegister[119] = 0;//reserver
            wHoldRegister[120] = 0;//reserver
            wHoldRegister[121] = 0;//reserver
            wHoldRegister[122] = 0;//reserver
            wHoldRegister[123] = 0;//reserver
            wHoldRegister[124] = 0;//reserver
            //第6组，LVRT功能设置
            wHoldRegister[125] = wEepromLVRTOption;
            wHoldRegister[126] = wEepromLVRTTripVol; 
            wHoldRegister[127] = wEepromFVRTPosCurrK;
            wHoldRegister[128] = wEepromFVRTNegCurrK;
			wHoldRegister[129] = wEepromHVRTOption;//reserver
            wHoldRegister[130] = wEepromHVRTTripVol;//reserver
            wHoldRegister[131] = wEepromFVRTPowerStep;//reserver
            wHoldRegister[132] = 0;//reserver
            wHoldRegister[133] = 0;//reserver
            wHoldRegister[134] = 0;//reserver
            //第7组，电池参数设置
            wHoldRegister[135] = wEepromChargeVoltLimt;
            wHoldRegister[136] = wEepromDisChargeVoltLimt;
            wHoldRegister[137] = wEepromChargeCurrLimt;
            wHoldRegister[138] = wEepromDisChargeCurrLimt; 
            wHoldRegister[139] = wEepromFloatChargeVolt;
            wHoldRegister[140] = wEepromSToFChargeCurr;
            wHoldRegister[141] = wEepromBatteryVoltMax;
            wHoldRegister[142] = wEepromBatteryVoltMin;
            wHoldRegister[143] = wEepromBatChargeCurrMax;
            wHoldRegister[144] = wEepromBatDisChargeCurrMax; 
            wHoldRegister[145] = wEepromDCRelayVoltDiff;      
            wHoldRegister[146] = wEepromBatPreChargeCurrMax; 
			wHoldRegister[147] = 0;
            wHoldRegister[148] = wEepromCSVSSwitch;
            wHoldRegister[149] = wEepromAutoCSVSSwitchEn;
            wHoldRegister[150] = wEepromVSCtrMode;        
            wHoldRegister[151] = wEepromVSVoltSet;    
            wHoldRegister[152] = wEepromVSFreqSet;        
            wHoldRegister[153] = wEepromVSPSet;
            wHoldRegister[154] = wEepromVSQSet;
            wHoldRegister[155] = wEepromVSDroopEn;
            wHoldRegister[156] = wEepromVSFreqDroopKByP;
            wHoldRegister[157] = wEepromVSVoltDroopKByQ;
            wHoldRegister[158] = 0;//reserver
            wHoldRegister[159] = 0;//reserver
            wHoldRegister[160] = 0;//reserver
            // 其它参数
            wHoldRegister[161] = wEepromInvCurrMax;
            wHoldRegister[162] = wEepromDCIOffsetDiffMax;
            wHoldRegister[163] = wEepromOffsetDiffMax;
            wHoldRegister[164] = wEepromEnvironmentTmpMax;
            wHoldRegister[165] = wEepromPowerTmpMax;
            wHoldRegister[166] = wEepromXFMRTmpMax;
            wHoldRegister[167] = wEepromTransformerTmpMax;
            wHoldRegister[168] = wEepromBusVolMax;
            wHoldRegister[169] = wEepromCapVolMax;
            wHoldRegister[170] = wEepromDCRelayActProt;
            wHoldRegister[171] = wEepromBiDirDCSwitch;
            wHoldRegister[172] = wEepromRemoteSel;
            wHoldRegister[173] = wEepromBatteryType;
            wHoldRegister[174] = 0;
            wHoldRegister[175] = 0;
            wHoldRegister[176] = 0;
            wHoldRegister[177] = 0;
            wHoldRegister[178] = 0;
            wHoldRegister[179] = 0;
            wHoldRegister[180] = 0;
            wHoldRegister[181] = 0;
            wHoldRegister[182] = 0;
            wHoldRegister[183] = 0;
            wHoldRegister[184] = 0; 
        }
        else if(fCallDefaultValueOrder == 2)
        {
            wHoldRegister[188] = 0xA5A5; 
        }
    }
    else if(wHoldRegister[188] == 0xA5A5)
    {
        if(fCallDefaultValueOrder == 0)
        {
            wHoldRegister[188] = 0x5555;
        }   
    }    	    
    else if(wHoldRegister[188] == 0x5555)
    {
        wHoldRegister[188] = 0x5555;
    }  
    else
    {
        wHoldRegister[188] = 0x7777;
    }
    
    if(wHoldRegister[185] == 0xAAAA) //快速放电...
    {
        if(fRunOrder == 0)
        {
            fDeCapEnergy = 1;
            wHoldRegister[185] = 0xA5A5;
        }
        else
        {
            wHoldRegister[185] = 0x7777;
        }
    }
    else if(wHoldRegister[185] == 0xA5A5) 
    {
        if(fDeCapEnergy == 0)
        {
            wHoldRegister[185] = 0x5555;
        }
        else if(fDeCapEnergy == 2)
        {
            wHoldRegister[185] = 0x7777;
        }
    }
    
    if(wHoldRegister[186] == 0xAAAA)//开机
    {
        fLocalPCSComdOnOff = 1;
    }
    else
    {
    	fLocalPCSComdOnOff = 0;
    }
    
    if(fQuitShutModeOrder == 0)
	{
	    if(wHoldRegister[187] == 0xaaaa) 
	    {	
	        if(uwSysMode == ShutMode)
	        {	
  	    	    fQuitShutModeOrder = 1;
  	    	    wHoldRegister[187] = 0xa5a5;      	
	        }	            
  	        else
  	        {
  	     	    fQuitShutModeOrder = 0;
  	            wHoldRegister[187] = 0x5555;
  	        }      	
	    }
	    else if(wHoldRegister[187] == 0xa5a5)
	    {
	    	wHoldRegister[187] = 0x5555;
	    }            
	}
    else
    {
    	wHoldRegister[187] = 0xa5a5;
    }    
    
    if(wHoldRegister[191] == 0xAAAA)
    {
        fLocalPCSConnect = 1;
    }
    else
    {
    	fLocalPCSConnect = 0;
    }   
}
/*********************************************************************     
Function name: void sHoldDataDeal(void)                                             
Description:   Data parameter of HoleRegister                            
Calls:        	                                                           
Called By:   sCANDataMange(), period: 20ms                                                          
Parameters: void                                                         
Return:     void		                                                       
 *********************************************************************/  
void sHoldDataDeal(void)
{
    INT16S i = 0;
    INT16S j = 0;
    const INT16S *pwEepromDefaultProtMaxArray,*pwEepromDefaultProtMinArray;
    const INT16S *pwEepromDefaultProtMaxSupplArray,*pwEepromDefaultProtMinSupplArray;
    static INT16S wSysFrebak = 0;
    static INT16S wSysFreq = 5000;
    U_SysCtrSignal wSysCtrSignalMax,wSysCtrSignalMin; 
    
    if(wEepromRatedFreqSel != wSysFrebak)
    {        
        if(wEepromRatedFreqSel == 1)
        {
            wSysFreq = 6000;
            wHoldRegister[30] = wEepromGridFreqMax_1 + wSysFreq;
            wHoldRegister[32] = wEepromGridFreqMin_1 + wSysFreq;
            wHoldRegister[34] = wEepromGridFreqMax_2 + wSysFreq;
            wHoldRegister[36] = wEepromGridFreqMin_2 + wSysFreq;
            wHoldRegister[38] = wEepromGridFreqRecover_Max + wSysFreq;
            wHoldRegister[39] = wEepromGridFreqRecover_Min + wSysFreq;
            wHoldRegister[101] = wEepromOvFWTripFreq + wSysFreq;
    		wHoldRegister[103] = wEepromOvFWExitFreq + wSysFreq;
    		wHoldRegister[105] = wEepromUnFWTripFreq + wSysFreq;
    		wHoldRegister[107] = wEepromUnFWExitFreq + wSysFreq;    		
        }
        else
        {
            wSysFreq = 5000;
            wHoldRegister[30] = wEepromGridFreqMax_1 + wSysFreq;
            wHoldRegister[32] = wEepromGridFreqMin_1 + wSysFreq;
            wHoldRegister[34] = wEepromGridFreqMax_2 + wSysFreq;
            wHoldRegister[36] = wEepromGridFreqMin_2 + wSysFreq;
            wHoldRegister[38] = wEepromGridFreqRecover_Max + wSysFreq;
            wHoldRegister[39] = wEepromGridFreqRecover_Min + wSysFreq;
            wHoldRegister[101] = wEepromOvFWTripFreq + wSysFreq;
    		wHoldRegister[103] = wEepromOvFWExitFreq + wSysFreq;
    		wHoldRegister[105] = wEepromUnFWTripFreq + wSysFreq;
    		wHoldRegister[107] = wEepromUnFWExitFreq + wSysFreq;
        }
    }
    wSysFrebak = wEepromRatedFreqSel;
    
    if(fHoldRegisterStatus == 1) //Hold Register -> Eeprom data
    {  	        
	    if(wEepromStandardValue == cIEEE1547Standard)
        {
            pwEepromDefaultProtMaxArray = &wEepromDefaultProtMaxArrayIEEE1547[0];
            pwEepromDefaultProtMinArray = &wEepromDefaultProtMinArrayIEEE1547[0];
            wSysCtrSignalMax.all = wEepromDefaultProtMaxArrayIEEE1547[33];
            wSysCtrSignalMin.all = wEepromDefaultProtMinArrayIEEE1547[33];
        }
        else if(wEepromStandardValue == cRule21Standard)
        {
            pwEepromDefaultProtMaxArray = &wEepromDefaultProtMaxArrayRule21[0];
            pwEepromDefaultProtMinArray = &wEepromDefaultProtMinArrayRule21[0];
            wSysCtrSignalMax.all = wEepromDefaultProtMaxArrayRule21[33];
            wSysCtrSignalMin.all = wEepromDefaultProtMinArrayRule21[33];
        }
        
        pwEepromDefaultProtMaxSupplArray = &wEepromDefaultProtMaxSupplArray[0];
        pwEepromDefaultProtMinSupplArray = &wEepromDefaultProtMinSupplArray[0];
        		
        
        j = &wEepromGridVolMax_1 - &wEepromProtectData[0];	  
        if(wHoldRegister[0] != wEepromGridVolMax_1)
        {
        	i = wHoldRegister[0]; 
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolMax_1 = i;
                wHoldRegister[0] = wEepromGridVolMax_1;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[0] = wEepromGridVolMax_1;
            }
        } 
        
        j = &wEepromVolMaxTripTime_1 - &wEepromProtectData[0];   
        if(wHoldRegister[1] != wEepromVolMaxTripTime_1) 
        {
            i = wHoldRegister[1];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVolMaxTripTime_1 = i;
                wHoldRegister[1] = wEepromVolMaxTripTime_1;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[1] = wEepromVolMaxTripTime_1;
            }
        } 
        
        j = &wEepromGridVolMin_1 - &wEepromProtectData[0];	  
        if(wHoldRegister[2] != wEepromGridVolMin_1)
        {
        	i = wHoldRegister[2];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolMin_1 = i;
                wHoldRegister[2] = wEepromGridVolMin_1;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[2] = wEepromGridVolMin_1;
            }
        }         
           
        j = &wEepromVolMinTripTime_1 - &wEepromProtectData[0];   
        if(wHoldRegister[3] != wEepromVolMinTripTime_1) 
        {
            i = wHoldRegister[3];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVolMinTripTime_1 = i;
                wHoldRegister[3] = wEepromVolMinTripTime_1;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[3] = wEepromVolMinTripTime_1;
            }
        } 
        
        j = &wEepromGridVolMax_2 - &wEepromProtectData[0];	  
        if(wHoldRegister[4] != wEepromGridVolMax_2)
        {
        	i = wHoldRegister[4];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolMax_2 = i;
                wHoldRegister[4] = wEepromGridVolMax_2;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[4] = wEepromGridVolMax_2;
            }
        }
           
        j = &wEepromVolMaxTripTime_2 - &wEepromProtectData[0];   
        if(wHoldRegister[5] != wEepromVolMaxTripTime_2) 
        {
            i = wHoldRegister[5];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVolMaxTripTime_2 = i;
                wHoldRegister[5] = wEepromVolMaxTripTime_2;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[5] = wEepromVolMaxTripTime_2;
            }
        } 
        wEepromSum00 = (INT16U)wEepromGridVolMax_1 + (INT16U)wEepromVolMaxTripTime_1 + (INT16U)wEepromGridVolMin_1 + 
                       (INT16U)wEepromVolMinTripTime_1 + (INT16U)wEepromGridVolMax_2 + (INT16U)wEepromVolMaxTripTime_2;                  
        
        
        j = &wEepromGridVolMin_2 - &wEepromProtectData[0];	  
        if(wHoldRegister[6] != wEepromGridVolMin_2)
        {
        	i = wHoldRegister[6];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolMin_2 = i;
                wHoldRegister[6] = wEepromGridVolMin_2;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[6] = wEepromGridVolMin_2;
            }
        }
                
        j = &wEepromVolMinTripTime_2 - &wEepromProtectData[0];   
        if(wHoldRegister[7] != wEepromVolMinTripTime_2) 
        {
            i = wHoldRegister[7];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVolMinTripTime_2 = i;
                wHoldRegister[7] = wEepromVolMinTripTime_2;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[7] = wEepromVolMinTripTime_2;
            }
        } 
        
        j = &wEepromGridVolMax_3 - &wEepromProtectData[0];	  
        if(wHoldRegister[12] != wEepromGridVolMax_3)
        {
        	i = wHoldRegister[12];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolMax_3 = i;
                wHoldRegister[12] = wEepromGridVolMax_3;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[12] = wEepromGridVolMax_3;
            }
        }
                
        j = &wEepromVolMaxTripTime_3 - &wEepromProtectData[0];   
        if(wHoldRegister[13] != wEepromVolMaxTripTime_3) 
        {
            i = wHoldRegister[13];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVolMaxTripTime_3 = i;
                wHoldRegister[13] = wEepromVolMaxTripTime_3;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[13] = wEepromVolMaxTripTime_3;
            }
        } 
        
        j = &wEepromGridVolMin_3 - &wEepromProtectData[0];	  
        if(wHoldRegister[14] != wEepromGridVolMin_3)
        {
        	i = wHoldRegister[14];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolMin_3 = i;
                wHoldRegister[14] = wEepromGridVolMin_3;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[14] = wEepromGridVolMin_3;
            }
        }
                
        j = &wEepromVolMinTripTime_3 - &wEepromProtectData[0];   
        if(wHoldRegister[15] != wEepromVolMinTripTime_3) 
        {
            i = wHoldRegister[15];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVolMinTripTime_3 = i;
                wHoldRegister[15] = wEepromVolMinTripTime_3;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[15] = wEepromVolMinTripTime_3;
            }
        } 
        wEepromSum01 = (INT16U)wEepromGridVolMin_2 + (INT16U)wEepromVolMinTripTime_2 + (INT16U)wEepromGridVolMax_3 +     
                       (INT16U)wEepromVolMaxTripTime_3 + (INT16U)wEepromGridVolMin_3 + (INT16U)wEepromVolMinTripTime_3;                 
    
    
        j = &wEepromGridVolRecover_Max - &wEepromProtectData[0];	  
        if(wHoldRegister[8] != wEepromGridVolRecover_Max)
        {
        	i = wHoldRegister[8];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolRecover_Max = i;
                wHoldRegister[8] = wEepromGridVolRecover_Max;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[8] = wEepromGridVolRecover_Max;
            }
        } 
         
        j = &wEepromGridVolRecover_Min - &wEepromProtectData[0];	  
        if(wHoldRegister[9] != wEepromGridVolRecover_Min)
        {
        	i = wHoldRegister[9];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolRecover_Min = i;
                wHoldRegister[9] = wEepromGridVolRecover_Min;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[9] = wEepromGridVolRecover_Min;
            }
        }
        j = &wEepromGridVolRecover_Time - &wEepromProtectData[0];   
        if(wHoldRegister[10] != wEepromGridVolRecover_Time) 
        {
            i = wHoldRegister[10];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVolRecover_Time = i;
                wHoldRegister[10] = wEepromGridVolRecover_Time;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[10] = wEepromGridVolRecover_Time;
            }
        }
        j = &wEepromGridVoltBalanceMax - &wEepromProtectData[0]; 
        if(wHoldRegister[11] != wEepromGridVoltBalanceMax) 
        {
            i = wHoldRegister[11];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridVoltBalanceMax = i;
                wHoldRegister[11] = wEepromGridVoltBalanceMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[11] = wEepromGridVoltBalanceMax;
            }
        }
        j = &wEepromGridFaultPowerStep - &wEepromProtectData[0];        
	    if(wHoldRegister[56] != wEepromGridFaultPowerStep) 
	    {
	        i = wHoldRegister[56];
	        if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
	        {
	            wEepromGridFaultPowerStep = i;
	            wHoldRegister[56] = wEepromGridFaultPowerStep;
	            fEepromOperateRequire = 1; 
	        }
	        else
	        {
	        	wHoldRegister[56] = wEepromGridFaultPowerStep; 
	        }
	    }
        j = &wEepromPowerOnWaitTime - &wEepromProtectData[0];   
	    if(wHoldRegister[16] != wEepromPowerOnWaitTime) 
		{
	        i = wHoldRegister[16];
	        if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
		    {
	                wEepromPowerOnWaitTime = i;
	                wHoldRegister[16] = wEepromPowerOnWaitTime;
	                fEepromOperateRequire = 1;
	            
		    }
		    else
		    {
		    	wHoldRegister[16] = wEepromPowerOnWaitTime;
		    }      	     
		}  
        wEepromSum02 = (INT16U)wEepromGridVolRecover_Max + (INT16U)wEepromGridVolRecover_Min + (INT16U)wEepromGridVolRecover_Time +     
                       (INT16U)wEepromGridVoltBalanceMax + (INT16U)wEepromGridFaultPowerStep + (INT16U)wEepromPowerOnWaitTime; 
        
        
        j = &wEepromGridFreqMax_1 - &wEepromProtectData[0];	  
        if(wHoldRegister[30] != (wEepromGridFreqMax_1 + wSysFreq))
        {
        	i = wHoldRegister[30] - wSysFreq;        	
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridFreqMax_1 = i;
                wHoldRegister[30] = wEepromGridFreqMax_1 + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[30] = wEepromGridFreqMax_1 + wSysFreq;
            }
        }
        j = &wEepromFreqMaxTripTime_1 - &wEepromProtectData[0];   
        if(wHoldRegister[31] != wEepromFreqMaxTripTime_1) 
        {
            i = wHoldRegister[31];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFreqMaxTripTime_1 = i;
                wHoldRegister[31] = wEepromFreqMaxTripTime_1;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[31] = wEepromFreqMaxTripTime_1;
            }
        }
        
        j = &wEepromGridFreqMin_1 - &wEepromProtectData[0];   
        if(wHoldRegister[32] != (wEepromGridFreqMin_1 + wSysFreq))
        {
            i = wHoldRegister[32] - wSysFreq;             
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridFreqMin_1 = i;
                wHoldRegister[32] = wEepromGridFreqMin_1 + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[32] = wEepromGridFreqMin_1 + wSysFreq;
            }
        }  
        j = &wEepromFreqMinTripTime_1 - &wEepromProtectData[0];   
        if(wHoldRegister[33] != wEepromFreqMinTripTime_1) 
        {
            i = wHoldRegister[33];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFreqMinTripTime_1 = i;
                wHoldRegister[33] = wEepromFreqMinTripTime_1;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[33] = wEepromFreqMinTripTime_1;
            }
        }
        j = &wEepromGridFreqMax_2 - &wEepromProtectData[0];   
        if(wHoldRegister[34] != (wEepromGridFreqMax_2 + wSysFreq))
        {
            i = wHoldRegister[34] - wSysFreq;            
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridFreqMax_2 = i;
                wHoldRegister[34] = wEepromGridFreqMax_2 + wSysFreq;  
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[34] = wEepromGridFreqMax_2 + wSysFreq; 
            }   
        }
        j = &wEepromFreqMaxTripTime_2 - &wEepromProtectData[0];   
        if(wHoldRegister[35] != wEepromFreqMaxTripTime_2) 
        {
            i = wHoldRegister[35];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFreqMaxTripTime_2 = i;
                wHoldRegister[35] = wEepromFreqMaxTripTime_2;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[35] = wEepromFreqMaxTripTime_2;
            }
        }
        wEepromSum03 = (INT16U)wEepromGridFreqMax_1 + (INT16U)wEepromFreqMaxTripTime_1 + (INT16U)wEepromGridFreqMin_1 +
                       (INT16U)wEepromFreqMinTripTime_1 + (INT16U)wEepromGridFreqMax_2 + (INT16U)wEepromFreqMaxTripTime_2;                                    
        
        
        j = &wEepromGridFreqMin_2 - &wEepromProtectData[0];   
        if(wHoldRegister[36] != (wEepromGridFreqMin_2 + wSysFreq))
        {
            i = wHoldRegister[36] - wSysFreq; 
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridFreqMin_2 = i;
                wHoldRegister[36] = wEepromGridFreqMin_2 + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[36] = wEepromGridFreqMin_2 + wSysFreq;
            }           
        }        
        j = &wEepromFreqMinTripTime_2 - &wEepromProtectData[0];   
        if(wHoldRegister[37] != wEepromFreqMinTripTime_2) 
        {
            i = wHoldRegister[37];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFreqMinTripTime_2 = i;
                wHoldRegister[37] = wEepromFreqMinTripTime_2;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[37] = wEepromFreqMinTripTime_2;
            }
        }
        j = &wEepromGridFreqRecover_Max - &wEepromProtectData[0];   
        if(wHoldRegister[38] != (wEepromGridFreqRecover_Max + wSysFreq))
        {
            i = wHoldRegister[38] - wSysFreq; 
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridFreqRecover_Max = i;
                wHoldRegister[38] = wEepromGridFreqRecover_Max + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[38] = wEepromGridFreqRecover_Max + wSysFreq;
            }
        } 
          
        j = &wEepromGridFreqRecover_Min - &wEepromProtectData[0];   
        if(wHoldRegister[39] != (wEepromGridFreqRecover_Min + wSysFreq))
        {
            i = wHoldRegister[39] - wSysFreq;
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridFreqRecover_Min = i;
                wHoldRegister[39] = wEepromGridFreqRecover_Min + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[39] = wEepromGridFreqRecover_Min + wSysFreq;
            }
        }
        j = &wEepromGridFreqRecover_Time - &wEepromProtectData[0];   
        if(wHoldRegister[40] != wEepromGridFreqRecover_Time) 
        {
            i = wHoldRegister[40];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGridFreqRecover_Time = i;
                wHoldRegister[40] = wEepromGridFreqRecover_Time;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[40] = wEepromGridFreqRecover_Time;
            }
        }
	    
        if(wHoldRegister[57] != wEepromAIEnable)
        {
            i = wHoldRegister[57];
            if((i <= (INT16S)wSysCtrSignalMax.bit.AIEnable) && (i >= (INT16S)wSysCtrSignalMin.bit.AIEnable) && (wEepromStandardValue != cNoStandard))
            {
                wEepromAIEnable = i;
                wHoldRegister[57] = wEepromAIEnable; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[57] = wEepromAIEnable; 
            }
        }
 
        if(wHoldRegister[125] != wEepromLVRTOption) 
	    {
            i = wHoldRegister[125];
            if((i <= (INT16S)wSysCtrSignalMax.bit.LVRTEnable) && (i >= (INT16S)wSysCtrSignalMin.bit.LVRTEnable) && (wEepromStandardValue != cNoStandard))
	        {
                wEepromLVRTOption = i;
                wHoldRegister[125] = wEepromLVRTOption;
                fEepromOperateRequire = 1;
		    } 
		    else
		    {
		    	wHoldRegister[125] = wEepromLVRTOption;
		    }     	     
	    }	    
  
        if(wHoldRegister[129] != wEepromHVRTOption) 
	    {
            i = wHoldRegister[129];
            if((i <= (INT16S)wSysCtrSignalMax.bit.HVRTEnable) && (i >= (INT16S)wSysCtrSignalMin.bit.HVRTEnable) && (wEepromStandardValue != cNoStandard))
	        {
                wEepromHVRTOption = i;
                wHoldRegister[129] = wEepromHVRTOption;
                fEepromOperateRequire = 1;
		    } 
		    else
		    {
		    	wHoldRegister[129] = wEepromHVRTOption;
		    }     	     
	    }
  
        if(wHoldRegister[100] != wEepromOvFWEn) 
        {
            i = wHoldRegister[100];
            if((i <= (INT16S)wSysCtrSignalMax.bit.OvFWEnable) && (i >= (INT16S)wSysCtrSignalMin.bit.OvFWEnable) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOvFWEn = i;                   
                wHoldRegister[100] = wEepromOvFWEn;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[100] = wEepromOvFWEn;
            }            
        }
  
        if(wHoldRegister[104] != wEepromUnFWEn)
        {
            i = wHoldRegister[104];
            if((i <= (INT16S)wSysCtrSignalMax.bit.UnFWEnable) && (i >= (INT16S)wSysCtrSignalMin.bit.UnFWEnable) && (wEepromStandardValue != cNoStandard))
            {
                wEepromUnFWEn = i;
                wHoldRegister[104] = wEepromUnFWEn;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[104] = wEepromUnFWEn;
            }
        }        
  
        if(wHoldRegister[108] != wEerpomOvVWEnable)
        {
            i = wHoldRegister[108];
            if((i <= (INT16S)wSysCtrSignalMax.bit.OvVWEnable) && (i >= (INT16S)wSysCtrSignalMin.bit.OvVWEnable) && (wEepromStandardValue != cNoStandard))
            {
                wEerpomOvVWEnable = i;
                wHoldRegister[108] = wEerpomOvVWEnable;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[108] = wEerpomOvVWEnable;
            }
        }
  
        if(wHoldRegister[109] != wEerpomUnVWEnable)
        {
            i = wHoldRegister[109];
            if((i <= (INT16S)wSysCtrSignalMax.bit.UnVWEnable) && (i >= (INT16S)wSysCtrSignalMin.bit.UnVWEnable) && (wEepromStandardValue != cNoStandard))
            {
                wEerpomUnVWEnable = i;
                wHoldRegister[109] = wEerpomUnVWEnable;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[109] = wEerpomUnVWEnable;
            }
        }
        wEepromSysCtrSignal = fSysCtrSignal.all; 
        
        wEepromSum04 = (INT16U)wEepromGridFreqMin_2 + (INT16U)wEepromFreqMinTripTime_2 + (INT16U)wEepromGridFreqRecover_Max +
                       (INT16U)wEepromGridFreqRecover_Min + (INT16U)wEepromGridFreqRecover_Time + (INT16U)wEepromSysCtrSignal;                                    
                
        j = &wEepromLVRTTripVol - &wEepromProtectData[0];	    
        if(wHoldRegister[126] != wEepromLVRTTripVol)  
        {
            i = wHoldRegister[126];  
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromLVRTTripVol = i;
                wHoldRegister[126] = wEepromLVRTTripVol;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[126] = wEepromLVRTTripVol;
            }
        }
        
	    j = &wEepromHVRTTripVol - &wEepromProtectData[0];	    
        if(wHoldRegister[130] != wEepromHVRTTripVol)  
        {
            i = wHoldRegister[130];  
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromHVRTTripVol = i;
                wHoldRegister[130] = wEepromHVRTTripVol;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[130] = wEepromHVRTTripVol;
            }
        }
        j = &wEepromFVRTPosCurrK - &wEepromProtectData[0]; 
        if(wHoldRegister[127] != wEepromFVRTPosCurrK) 
        {
            i = wHoldRegister[127];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFVRTPosCurrK = i;
                wHoldRegister[127] = wEepromFVRTPosCurrK;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[127] = wEepromFVRTPosCurrK;
            }
        }
        j = &wEepromFVRTNegCurrK - &wEepromProtectData[0]; 
        if(wHoldRegister[128] != wEepromFVRTNegCurrK) 
        {
            i = wHoldRegister[128];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFVRTNegCurrK = i;
                wHoldRegister[128] = wEepromFVRTNegCurrK;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[128] = wEepromFVRTNegCurrK;
            }
        }
        j = &wEepromFVRTPowerStep - &wEepromProtectData[0];        
	    if(wHoldRegister[131] != wEepromFVRTPowerStep) 
	    {
	        i = wHoldRegister[131];
	        if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
	        {
	            wEepromFVRTPowerStep = i;
	            wHoldRegister[131] = wEepromFVRTPowerStep;
	            fEepromOperateRequire = 1; 
	        }
	        else
	        {
	        	wHoldRegister[131] = wEepromFVRTPowerStep; 
	        }
	    }
		j = &wEepromOvFWTripFreq - &wEepromProtectData[0];   
        if(wHoldRegister[101] != (wEepromOvFWTripFreq + wSysFreq))
        {
            i = wHoldRegister[101] - wSysFreq;            
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOvFWTripFreq = i;
                wHoldRegister[101] = wEepromOvFWTripFreq + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[101] = wEepromOvFWTripFreq + wSysFreq;
            }
        }
        wEepromSum05 = (INT16U)wEepromLVRTTripVol + (INT16U)wEepromHVRTTripVol + (INT16U)wEepromFVRTPosCurrK +
                       (INT16U)wEepromFVRTNegCurrK + (INT16U)wEepromFVRTPowerStep + (INT16U)wEepromOvFWTripFreq;    
                     
        j = &wEepromOvFWRatio - &wEepromProtectData[0];        
        if(wHoldRegister[102] != wEepromOvFWRatio)
        {
            i = wHoldRegister[102];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOvFWRatio = i;
                wHoldRegister[102] = wEepromOvFWRatio;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[102] = wEepromOvFWRatio;
            }
        }
        j = &wEepromOvFWExitFreq - &wEepromProtectData[0];   
        if(wHoldRegister[103] != (wEepromOvFWExitFreq + wSysFreq))
        {
            i = wHoldRegister[103] - wSysFreq;
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOvFWExitFreq = i;
                wHoldRegister[103] = wEepromOvFWExitFreq + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[103] = wEepromOvFWExitFreq + wSysFreq;
            }
        } 
        j = &wEepromUnFWTripFreq - &wEepromProtectData[0];   
        if(wHoldRegister[105] != (wEepromUnFWTripFreq + wSysFreq))
        {
            i = wHoldRegister[105] - wSysFreq;            
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromUnFWTripFreq = i;
                wHoldRegister[105] = wEepromUnFWTripFreq + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[105] = wEepromUnFWTripFreq + wSysFreq;
            }
        } 
		j = &wEepromUnFWRatio - &wEepromProtectData[0];   
        if(wHoldRegister[106] != wEepromUnFWRatio)
        {
            i = wHoldRegister[106];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromUnFWRatio = i;
                wHoldRegister[106] = wEepromUnFWRatio;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[106] = wEepromUnFWRatio;
            }
        } 
        j = &wEepromUnFWExitFreq - &wEepromProtectData[0];   
        if(wHoldRegister[107] != (wEepromUnFWExitFreq + wSysFreq))
        {
            i = wHoldRegister[107] - wSysFreq;
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromUnFWExitFreq = i;
                wHoldRegister[107] = wEepromUnFWExitFreq + wSysFreq;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[107] = wEepromUnFWExitFreq + wSysFreq;
            }
        }
        j = &wEepromFWRecPowerStep - &wEepromProtectData[0];   
        if(wHoldRegister[80] != wEepromFWRecPowerStep) 
        {
            i = wHoldRegister[80];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFWRecPowerStep = i;
                wHoldRegister[80] = wEepromFWRecPowerStep;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[80] = wEepromFWRecPowerStep;
            }
        } 
        wEepromSum06 = (INT16U)wEepromOvFWRatio + (INT16U)wEepromOvFWExitFreq + (INT16U)wEepromUnFWTripFreq +
                       (INT16U)wEepromUnFWRatio + (INT16U)wEepromUnFWExitFreq + (INT16U)wEepromFWRecPowerStep;
        
        j = &wEepromFWActTime - &wEepromProtectData[0];   
        if(wHoldRegister[81] != wEepromFWActTime) 
        {
            i = wHoldRegister[81];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFWActTime = i;
                wHoldRegister[81] = wEepromFWActTime;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[81] = wEepromFWActTime;
            }
        }
        j = &wEepromVWActTime - &wEepromProtectData[0];   
        if(wHoldRegister[83] != wEepromVWActTime) 
        {
            i = wHoldRegister[83];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVWActTime = i;
                wHoldRegister[83] = wEepromVWActTime;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[83] = wEepromVWActTime;
            }
        }         
        j = &wEepromPF_PCurveActPw1 - &wEepromProtectData[0];   
        if(wHoldRegister[84] != wEepromPF_PCurveActPw1) 
        {
            i = wHoldRegister[84];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromPF_PCurveActPw1 = i;
                wHoldRegister[84] = wEepromPF_PCurveActPw1;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[84] = wEepromPF_PCurveActPw1;
            }
        } 
        j = &wEepromPF_PCurvePF1 - &wEepromProtectData[0];   
        if(wHoldRegister[85] != wEepromPF_PCurvePF1) 
        {
            i = wHoldRegister[85];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {   
            	if((i >= -1000 && i <= -800) || (i >= 800 && i <= 1000))
                {        
                    wEepromPF_PCurvePF1 = i;
                    wHoldRegister[85] = wEepromPF_PCurvePF1;
                    fEepromOperateRequire = 1;
                }
                else
                {
                	wHoldRegister[85] = wEepromPF_PCurvePF1;
                }
            }
            else
            {
            	wHoldRegister[85] = wEepromPF_PCurvePF1;
            }
        }
        j = &wEepromPF_PCurveActPw2 - &wEepromProtectData[0]; 
        if(wHoldRegister[86] != wEepromPF_PCurveActPw2) 
        {
            i = wHoldRegister[86];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromPF_PCurveActPw2 = i;
                wHoldRegister[86] = wEepromPF_PCurveActPw2;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[86] = wEepromPF_PCurveActPw2;
            }
        }
        j = &wEepromPF_PCurvePF2 - &wEepromProtectData[0]; 
        if(wHoldRegister[87] != wEepromPF_PCurvePF2) 
        {
            i = wHoldRegister[87];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            { 
            	if((i >= -1000 && i <= -800) || (i >= 800 && i <= 1000))
                {          
                    wEepromPF_PCurvePF2 = i;
                    wHoldRegister[87] = wEepromPF_PCurvePF2;
                    fEepromOperateRequire = 1;
                }
                else
                {
                	wHoldRegister[87] = wEepromPF_PCurvePF2;
                }
            }
            else
            {
            	wHoldRegister[87] = wEepromPF_PCurvePF2;
            }
        }                              
        wEepromSum07 = (INT16U)wEepromFWActTime + (INT16U)wEepromVWActTime + (INT16U)wEepromPF_PCurveActPw1 +         
                       (INT16U)wEepromPF_PCurvePF1 + (INT16U)wEepromPF_PCurveActPw2 + (INT16U)wEepromPF_PCurvePF2;
        
        
        j = &wEepromPF_PCurveLockInV - &wEepromProtectData[0]; 
        if(wHoldRegister[88] != wEepromPF_PCurveLockInV)
        {
            i = wHoldRegister[88];  
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromPF_PCurveLockInV = i;
                wHoldRegister[88] = wEepromPF_PCurveLockInV; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[88] = wEepromPF_PCurveLockInV; 
            }
        }
        j = &wEepromPF_PCurveLockOutV - &wEepromProtectData[0]; 
        if(wHoldRegister[89] != wEepromPF_PCurveLockOutV)
        {
            i = wHoldRegister[89]; 
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromPF_PCurveLockOutV = i;
                wHoldRegister[89] = wEepromPF_PCurveLockOutV;
                fEepromOperateRequire = 1; 
            }
            else
            {
            	wHoldRegister[89] = wEepromPF_PCurveLockOutV; 
            }
        }                       
        j = &wEepromQ_UCurveVolt1s - &wEepromProtectData[0]; 
        if(wHoldRegister[90] != wEepromQ_UCurveVolt1s)
        {
            i = wHoldRegister[90];  
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromQ_UCurveVolt1s = i;
                wHoldRegister[90] = wEepromQ_UCurveVolt1s; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[90] = wEepromQ_UCurveVolt1s;
            }
        }
        j = &wEepromQ_UCurveReactPw1s - &wEepromProtectData[0]; 
        if(wHoldRegister[91] != wEepromQ_UCurveReactPw1s) 
        {
            i = wHoldRegister[91];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {          
                wEepromQ_UCurveReactPw1s = i;
                wHoldRegister[91] = wEepromQ_UCurveReactPw1s;
                fEepromOperateRequire = 1;                
            }
            else
            {
            	wHoldRegister[91] = wEepromQ_UCurveReactPw1s;
            }
        }
        j = &wEepromQ_UCurveVolt2s - &wEepromProtectData[0]; 
        if(wHoldRegister[92] != wEepromQ_UCurveVolt2s) 
        {
            i = wHoldRegister[92]; 
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromQ_UCurveVolt2s = i;
                wHoldRegister[92] = wEepromQ_UCurveVolt2s;
                fEepromOperateRequire = 1; 
            }
            else
            {
            	wHoldRegister[92] = wEepromQ_UCurveVolt2s;
            }
        }
        j = &wEepromQ_UCurveReactPw2s - &wEepromProtectData[0]; 
        if(wHoldRegister[93] != wEepromQ_UCurveReactPw2s) 
        {
            i = wHoldRegister[93];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {                           
                wEepromQ_UCurveReactPw2s = i;
                wHoldRegister[93] = wEepromQ_UCurveReactPw2s;
                fEepromOperateRequire = 1;                
            }
            else
            {
            	wHoldRegister[93] = wEepromQ_UCurveReactPw2s;
            }
        }
        wEepromSum08 = (INT16U)wEepromPF_PCurveLockInV + (INT16U)wEepromPF_PCurveLockOutV + (INT16U)wEepromQ_UCurveVolt1s +  
                       (INT16U)wEepromQ_UCurveReactPw1s + wEepromQ_UCurveVolt2s + wEepromQ_UCurveReactPw2s; 
 
        
        j = &wEepromQ_UCurveVolt1i - &wEepromProtectData[0]; 
        if(wHoldRegister[94] != wEepromQ_UCurveVolt1i) 
        {
            i = wHoldRegister[94];  
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromQ_UCurveVolt1i = i;
                wHoldRegister[94] = wEepromQ_UCurveVolt1i; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[94] = wEepromQ_UCurveVolt1i;
            }
        }
        j = &wEepromQ_UCurveReactPw1i - &wEepromProtectData[0]; 
        if(wHoldRegister[95] != wEepromQ_UCurveReactPw1i) 
        {
            i = wHoldRegister[95];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {                           
                wEepromQ_UCurveReactPw1i = i;
                wHoldRegister[95] = wEepromQ_UCurveReactPw1i;
                fEepromOperateRequire = 1;                
            }
            else
            {
            	wHoldRegister[95] = wEepromQ_UCurveReactPw1i;
            }
        }                     
        j = &wEepromQ_UCurveVolt2i - &wEepromProtectData[0]; 
        if(wHoldRegister[96] != wEepromQ_UCurveVolt2i)
        {
            i = wHoldRegister[96]; 
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromQ_UCurveVolt2i = i;
                wHoldRegister[96] = wEepromQ_UCurveVolt2i;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[96] = wEepromQ_UCurveVolt2i;
            }
        }
        j = &wEepromQ_UCurveReactPw2i - &wEepromProtectData[0]; 
        if(wHoldRegister[97] != wEepromQ_UCurveReactPw2i) 
        {
            i = wHoldRegister[97];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromQ_UCurveReactPw2i = i;
                wHoldRegister[97] = wEepromQ_UCurveReactPw2i;
                fEepromOperateRequire = 1;               
            }
            else
            {
            	wHoldRegister[97] = wEepromQ_UCurveReactPw2i;
            }
        }
        j = &wEepromQ_UCurveLockInP - &wEepromProtectData[0]; 
        if(wHoldRegister[98] != wEepromQ_UCurveLockInP) 
        {
            i = wHoldRegister[98];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromQ_UCurveLockInP = i;
                wHoldRegister[98] = wEepromQ_UCurveLockInP;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[98] = wEepromQ_UCurveLockInP;
            }
        }
        j = &wEepromQ_UCurveLockOutP - &wEepromProtectData[0]; 
        if(wHoldRegister[99] != wEepromQ_UCurveLockOutP) 
        {
            i = wHoldRegister[99];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {           
                wEepromQ_UCurveLockOutP = i;
                wHoldRegister[99] = wEepromQ_UCurveLockOutP;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[99] = wEepromQ_UCurveLockOutP;
            }
        }               
        wEepromSum09 = (INT16U)wEepromQ_UCurveVolt1i + (INT16U)wEepromQ_UCurveReactPw1i + (INT16U)wEepromQ_UCurveVolt2i +   
                       (INT16U)wEepromQ_UCurveReactPw2i + (INT16U)wEepromQ_UCurveLockInP + (INT16U)wEepromQ_UCurveLockOutP;
                      
        
        j = &wEepromIsoImpMin - &wEepromProtectData[0];	    
        if(wHoldRegister[55] != wEepromIsoImpMin)
        {
            i = wHoldRegister[55];  
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromIsoImpMin = i;
                wHoldRegister[55] = wEepromIsoImpMin;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[55] = wEepromIsoImpMin;
            }
        }
        j = &wEepromGFCIMax - &wEepromProtectData[0];	    
        if(wHoldRegister[60] != wEepromGFCIMax)
        {
            i = wHoldRegister[60]; 
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGFCIMax = i;
                wHoldRegister[60] = wEepromGFCIMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[60] = wEepromGFCIMax;
            }
        }
        j = &wEepromDCIMax - &wEepromProtectData[0];	    
        if(wHoldRegister[61] != wEepromDCIMax)
        {
            i = wHoldRegister[61];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromDCIMax = i;
                wHoldRegister[61] = wEepromDCIMax; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[61] = wEepromDCIMax; 
            }
        }
        j = &wEepromOvVWStartVolt - &wEepromProtectData[0];	    
        if(wHoldRegister[110] != wEepromOvVWStartVolt)
        {
            i = wHoldRegister[110];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOvVWStartVolt = i;
                wHoldRegister[110] = wEepromOvVWStartVolt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[110] = wEepromOvVWStartVolt; 
            }
        }
        j = &wEepromOvVWStopVolt - &wEepromProtectData[0];	    
        if(wHoldRegister[111] != wEepromOvVWStopVolt)
        {
            i = wHoldRegister[111];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOvVWStopVolt = i;
                wHoldRegister[111] = wEepromOvVWStopVolt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[111] = wEepromOvVWStopVolt; 
            }
        }
        j = &wEepromOvVWRatio - &wEepromProtectData[0];	    
        if(wHoldRegister[112] != wEepromOvVWRatio)
        {
            i = wHoldRegister[112];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOvVWRatio = i;
                wHoldRegister[112] = wEepromOvVWRatio; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[112] = wEepromOvVWRatio; 
            }
        }
        wEepromSum10 = (INT16U)wEepromIsoImpMin + (INT16U)wEepromGFCIMax + (INT16U)wEepromDCIMax +  
                       (INT16U)wEepromOvVWStartVolt + (INT16U)wEepromOvVWStopVolt + (INT16U)wEepromOvVWRatio; 
        
        j = &wEepromUnVWStartVolt - &wEepromProtectData[0];	    
        if(wHoldRegister[113] != wEepromUnVWStartVolt)
        {
            i = wHoldRegister[113];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromUnVWStartVolt = i;
                wHoldRegister[113] = wEepromUnVWStartVolt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[113] = wEepromUnVWStartVolt; 
            }
        }
        j = &wEepromUnVWStopVolt - &wEepromProtectData[0];	    
        if(wHoldRegister[114] != wEepromUnVWStopVolt)
        {
            i = wHoldRegister[114];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromUnVWStopVolt = i;
                wHoldRegister[114] = wEepromUnVWStopVolt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[114] = wEepromUnVWStopVolt; 
            }
        }
        j = &wEepromUnVWRatio - &wEepromProtectData[0];	    
        if(wHoldRegister[115] != wEepromUnVWRatio)
        {
            i = wHoldRegister[115];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromUnVWRatio = i;
                wHoldRegister[115] = wEepromUnVWRatio; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[115] = wEepromUnVWRatio; 
            }
        }
        j = &wEepromQUActTime - &wEepromProtectData[0];	    
        if(wHoldRegister[82] != wEepromQUActTime)
        {
            i = wHoldRegister[82];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromQUActTime = i;
                wHoldRegister[82] = wEepromQUActTime; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[82] = wEepromQUActTime; 
            }
        }
		/*j = &wEepromIsoRes - &wEepromProtectData[0];	    
        if(wHoldRegister[116] != wEepromIsoRes)
        {
            i = wHoldRegister[116];
            if((i <= *(pwEepromDefaultProtMaxArray+j)) && (i >= *(pwEepromDefaultProtMinArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromIsoRes = i;
                wHoldRegister[116] = wEepromIsoRes; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[116] = wEepromIsoRes; 
            }
        }*/
        wEepromSum11 = (INT16U)wEepromUnVWStartVolt + (INT16U)wEepromUnVWStopVolt + (INT16U)wEepromUnVWRatio +  
                       (INT16U)wEepromQUActTime + (INT16U)wEepromIsoRes + (INT16U)wEepromUserSave1; 
        
        j = &wEepromChargeVoltLimt - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[135] != wEepromChargeVoltLimt)
        {
            i = wHoldRegister[135];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromChargeVoltLimt = i;
                wHoldRegister[135] = wEepromChargeVoltLimt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[135] = wEepromChargeVoltLimt; 
            }
        }
        j = &wEepromDisChargeVoltLimt - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[136] != wEepromDisChargeVoltLimt)
        {
            i = wHoldRegister[136];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromDisChargeVoltLimt = i;
                wHoldRegister[136] = wEepromDisChargeVoltLimt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[136] = wEepromDisChargeVoltLimt; 
            }
        }
        j = &wEepromChargeCurrLimt - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[137] != wEepromChargeCurrLimt)
        {
            i = wHoldRegister[137];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromChargeCurrLimt = i;
                wHoldRegister[137] = wEepromChargeCurrLimt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[137] = wEepromChargeCurrLimt; 
            }
        }
        j = &wEepromDisChargeCurrLimt - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[138] != wEepromDisChargeCurrLimt)
        {
            i = wHoldRegister[138];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromDisChargeCurrLimt = i;
                wHoldRegister[138] = wEepromDisChargeCurrLimt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[138] = wEepromDisChargeCurrLimt; 
            }
        }
        
        j = &wEepromFloatChargeVolt - &wEepromProtectSupplData[0];
        if(wHoldRegister[139] > wEepromChargeVoltLimt)
        {
        	wHoldRegister[139] = wEepromChargeVoltLimt;
        }
        else if(wHoldRegister[139] < (wEepromChargeVoltLimt - 500))
        {
        	wHoldRegister[139] = wEepromChargeVoltLimt - 500;
        }   
        if(wHoldRegister[139] != wEepromFloatChargeVolt)
        {
            i = wHoldRegister[139];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFloatChargeVolt = i;
                wHoldRegister[139] = wEepromFloatChargeVolt; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[139] = wEepromFloatChargeVolt; 
            }
        }
        j = &wEepromSToFChargeCurr - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[140] != wEepromSToFChargeCurr)
        {
            i = wHoldRegister[140];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromSToFChargeCurr = i;
                wHoldRegister[140] = wEepromSToFChargeCurr; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[140] = wEepromSToFChargeCurr; 
            }
        }
        wEepromSupplSum01 = (INT16U)wEepromChargeVoltLimt + (INT16U)wEepromDisChargeVoltLimt + (INT16U)wEepromChargeCurrLimt +            
                            (INT16U)wEepromDisChargeCurrLimt + (INT16U)wEepromFloatChargeVolt + (INT16U)wEepromSToFChargeCurr; 
        
        
        j = &wEepromBatteryVoltMax - &wEepromProtectSupplData[0];
        if(wHoldRegister[141] <= (wEepromChargeVoltLimt + 10))
        {
        	wHoldRegister[141] = wEepromChargeVoltLimt + 10;
        } 
        if(wHoldRegister[141] != wEepromBatteryVoltMax)
        {
            i = wHoldRegister[141];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBatteryVoltMax = i;
                wHoldRegister[141] = wEepromBatteryVoltMax; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[141] = wEepromBatteryVoltMax; 
            }
        }
        j = &wEepromBatteryVoltMin - &wEepromProtectSupplData[0];	
        if(wHoldRegister[142] >= (wEepromDisChargeVoltLimt - 10))
        {
        	wHoldRegister[142] = wEepromDisChargeVoltLimt - 10;
        }     
        if(wHoldRegister[142] != wEepromBatteryVoltMin)
        {
            i = wHoldRegister[142];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBatteryVoltMin = i;
                wHoldRegister[142] = wEepromBatteryVoltMin; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[142] = wEepromBatteryVoltMin; 
            }
        }
        j = &wEepromBatChargeCurrMax - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[143] != wEepromBatChargeCurrMax)
        {
            i = wHoldRegister[143];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBatChargeCurrMax = i;
                wHoldRegister[143] = wEepromBatChargeCurrMax; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[143] = wEepromBatChargeCurrMax; 
            }
        }
        j = &wEepromBatDisChargeCurrMax - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[144] != wEepromBatDisChargeCurrMax)
        {
            i = wHoldRegister[144];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBatDisChargeCurrMax = i;
                wHoldRegister[144] = wEepromBatDisChargeCurrMax; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[144] = wEepromBatDisChargeCurrMax; 
            }
        }
        j = &wEepromDCRelayVoltDiff - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[145] != wEepromDCRelayVoltDiff)
        {
            i = wHoldRegister[145];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromDCRelayVoltDiff = i;
                wHoldRegister[145] = wEepromDCRelayVoltDiff; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[145] = wEepromDCRelayVoltDiff; 
            }
        }
        j = &wEepromBatPreChargeCurrMax - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[146] != wEepromBatPreChargeCurrMax)
        {
            i = wHoldRegister[146];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBatPreChargeCurrMax = i;
                wHoldRegister[146] = wEepromBatPreChargeCurrMax; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[146] = wEepromBatPreChargeCurrMax; 
            }
        }
        wEepromSupplSum02 = (INT16U)wEepromBatteryVoltMax + (INT16U)wEepromBatteryVoltMin + (INT16U)wEepromBatChargeCurrMax +            
                            (INT16U)wEepromBatDisChargeCurrMax + (INT16U)wEepromDCRelayVoltDiff + (INT16U)wEepromBatPreChargeCurrMax; 


        j = &wEepromRemoteSel - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[172] != wEepromRemoteSel)
        {
            i = wHoldRegister[172];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromRemoteSel = i;
                wHoldRegister[172] = wEepromRemoteSel; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[172] = wEepromRemoteSel; 
            }
        }
        j = &wEepromCSVSSwitch - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[148] != wEepromCSVSSwitch)
        {
            i = wHoldRegister[148];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromCSVSSwitch = i;
                wHoldRegister[148] = wEepromCSVSSwitch; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[148] = wEepromCSVSSwitch; 
            }
        }
        j = &wEepromAutoCSVSSwitchEn - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[149] != wEepromAutoCSVSSwitchEn)
        {
            i = wHoldRegister[149];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromAutoCSVSSwitchEn = i;
                wHoldRegister[149] = wEepromAutoCSVSSwitchEn; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[149] = wEepromAutoCSVSSwitchEn; 
            }
        }
        j = &wEepromVSCtrMode - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[150] != wEepromVSCtrMode)
        {
            i = wHoldRegister[150];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSCtrMode = i;
                wHoldRegister[150] = wEepromVSCtrMode; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[150] = wEepromVSCtrMode; 
            }
        }
        j = &wEepromVSVoltSet - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[151] != wEepromVSVoltSet)
        {
            i = wHoldRegister[151];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSVoltSet = i;
                wHoldRegister[151] = wEepromVSVoltSet; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[151] = wEepromVSVoltSet; 
            }
        }
        j = &wEepromVSFreqSet - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[152] != wEepromVSFreqSet)
        {
            i = wHoldRegister[152];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSFreqSet = i;
                wHoldRegister[152] = wEepromVSFreqSet; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[152] = wEepromVSFreqSet; 
            }
        }
        wEepromSupplSum03 = (INT16U)wEepromRemoteSel + (INT16U)wEepromCSVSSwitch + (INT16U)wEepromAutoCSVSSwitchEn +            
                            (INT16U)wEepromVSCtrMode + (INT16U)wEepromVSVoltSet + (INT16U)wEepromVSFreqSet; 

        
        j = &wEepromVSPSet - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[153] != wEepromVSPSet)
        {
            i = wHoldRegister[153];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSPSet = i;
                wHoldRegister[153] = wEepromVSPSet; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[153] = wEepromVSPSet; 
            }
        }
        j = &wEepromVSQSet - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[154] != wEepromVSQSet)
        {
            i = wHoldRegister[154];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSQSet = i;
                wHoldRegister[154] = wEepromVSQSet; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[154] = wEepromVSQSet; 
            }
        }
        j = &wEepromVSDroopEn - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[155] != wEepromVSDroopEn)
        {
            i = wHoldRegister[155];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSDroopEn = i;
                wHoldRegister[155] = wEepromVSDroopEn; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[155] = wEepromVSDroopEn; 
            }
        }
        j = &wEepromVSFreqDroopKByP - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[156] != wEepromVSFreqDroopKByP)
        {
            i = wHoldRegister[156];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSFreqDroopKByP = i;
                wHoldRegister[156] = wEepromVSFreqDroopKByP; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[156] = wEepromVSFreqDroopKByP; 
            }
        }
        j = &wEepromVSVoltDroopKByQ - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[157] != wEepromVSVoltDroopKByQ)
        {
            i = wHoldRegister[157];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromVSVoltDroopKByQ = i;
                wHoldRegister[157] = wEepromVSVoltDroopKByQ; 
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[157] = wEepromVSVoltDroopKByQ; 
            }
        }
        j = &wEepromCSPCtrModeSel - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[70] != wEepromCSPCtrModeSel) 
        {
            i = wHoldRegister[70];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromCSPCtrModeSel = i;
                wHoldRegister[70] = wEepromCSPCtrModeSel;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[70] = wEepromCSPCtrModeSel;
            }
        }
        wEepromSupplSum04 = (INT16U)wEepromVSPSet + (INT16U)wEepromVSQSet + (INT16U)wEepromVSDroopEn +            
                            (INT16U)wEepromVSFreqDroopKByP + (INT16U)wEepromVSVoltDroopKByQ + (INT16U)wEepromCSPCtrModeSel; 

        j = &wEepromCSPPer - &wEepromProtectSupplData[0];   
        if(wHoldRegister[71] != wEepromCSPPer) 
        {
            i = wHoldRegister[71];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromCSPPer = i;                   
                wHoldRegister[71] = wEepromCSPPer;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[71] = wEepromCSPPer;
            }            
        }
        j = &wEepromCSCurrSet - &wEepromProtectSupplData[0];   
        if(wHoldRegister[72] != wEepromCSCurrSet) 
        {
            i = wHoldRegister[72];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromCSCurrSet = i;                   
                wHoldRegister[72] = wEepromCSCurrSet;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[72] = wEepromCSCurrSet;
            }            
        }
         

        j = &wEepromPActStep - &wEepromProtectSupplData[0]; 
	    if(wHoldRegister[73] != wEepromPActStep) 
	    {
	        i = wHoldRegister[73];
	        if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
	        {
	            wEepromPActStep = i;
	            wHoldRegister[73] = wEepromPActStep;
	            fEepromOperateRequire = 1;
	        }
	        else
	        {
	        	wHoldRegister[73] = wEepromPActStep;
	        }
	    }
	    j = &wEepromCSQCtrModeSel - &wEepromProtectSupplData[0];   
        if(wHoldRegister[74] != wEepromCSQCtrModeSel) 
	    {
            i = wHoldRegister[74];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
	        {
                wEepromCSQCtrModeSel = i;
                wHoldRegister[74] = wEepromCSQCtrModeSel;
                fEepromOperateRequire = 1;
		    }
		    else
		    {
		    	wHoldRegister[74] = wEepromCSQCtrModeSel;
		    }      	     
	    }
        j = &wEepromCSPFSet - &wEepromProtectSupplData[0];   
        if(wHoldRegister[75] != wEepromCSPFSet) 
	    {
            i = wHoldRegister[75];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
	        {
                if(((i >= -1000) && (i <= -800)) || ((i >= 800) && (i <= 1000)))
                {
                    wEepromCSPFSet = i;
                    wHoldRegister[75] = wEepromCSPFSet;
                    fEepromOperateRequire = 1;
                }
                else wHoldRegister[75] = wEepromCSPFSet;
		    }
		    else
		    {
		    	wHoldRegister[75] = wEepromCSPFSet;
		    }      	     
	    }
        j = &wEepromCSQPer - &wEepromProtectSupplData[0];   
        if(wHoldRegister[76] != wEepromCSQPer) 
	    {
            i = wHoldRegister[76];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
	        {                
                wEepromCSQPer = i;
                wHoldRegister[76] = wEepromCSQPer;
                fEepromOperateRequire = 1;                
		    }
		    else
		    {
		    	wHoldRegister[76] = wEepromCSQPer;
		    }      	     
	    }
        wEepromSupplSum05 = (INT16U)wEepromCSPPer + (INT16U)wEepromCSCurrSet + (INT16U)wEepromPActStep +            
                            (INT16U)wEepromCSQCtrModeSel + (INT16U)wEepromCSPFSet + (INT16U)wEepromCSQPer; 



        j = &wEepromQActStep - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[77] != wEepromQActStep)
        {
        	i = wHoldRegister[77];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromQActStep = i;
                wHoldRegister[77] = wEepromQActStep;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[77] = wEepromQActStep;
            }
        }
        j = &wEepromSoftPowerStep - &wEepromProtectSupplData[0];        
	    if(wHoldRegister[51] != wEepromSoftPowerStep) 
	    {
	        i = wHoldRegister[51];
	        if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
		    {
	            wEepromSoftPowerStep = i;
	            wHoldRegister[51] = wEepromSoftPowerStep;
	            fEepromOperateRequire = 1;
		    }
		    else
		    {
		    	wHoldRegister[51] = wEepromSoftPowerStep;
		    }
		}
		j = &wEepromSoftStopEn - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[53] != wEepromSoftStopEn) 
        {
            i = wHoldRegister[53];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromSoftStopEn = i;
                wHoldRegister[53] = wEepromSoftStopEn;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[53] = wEepromSoftStopEn;
            }
        }
        j = &wEepromStopPowerStep - &wEepromProtectSupplData[0];
	    if(wHoldRegister[54] != wEepromStopPowerStep) 
	    {
	        i = wHoldRegister[54];
	        if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
	        {
	            wEepromStopPowerStep = i;
	            wHoldRegister[54] = wEepromStopPowerStep;
	            fEepromOperateRequire = 1;
	        }
	        else
	        {
	        	wHoldRegister[54] = wEepromStopPowerStep;
	        }
	    }                                            
        j = &wEepromPVStartUpVol - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[50] != wEepromPVStartUpVol)
        {
        	i = wHoldRegister[50];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromPVStartUpVol = i;
                wHoldRegister[50] = wEepromPVStartUpVol;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[50] = wEepromPVStartUpVol;
            }
        }
        j = &wEepromPVFitFactor - &wEepromProtectSupplData[0];	    
        if(wHoldRegister[49] != wEepromPVFitFactor)
        {
        	i = wHoldRegister[49];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromPVFitFactor = i;
                wHoldRegister[49] = wEepromPVFitFactor;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[49] = wEepromPVFitFactor;
            }
        }
        wEepromSupplSum06 = (INT16U)wEepromQActStep + (INT16U)wEepromSoftPowerStep + (INT16U)wEepromSoftStopEn +            
                            (INT16U)wEepromStopPowerStep + (INT16U)wEepromPVStartUpVol + (INT16U)wEepromPVFitFactor; 

        j = &wEepromISOEnable - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[64] != wEepromISOEnable) 
        {
            i = wHoldRegister[64];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromISOEnable = i;
                wHoldRegister[64] = wEepromISOEnable;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[64] = wEepromISOEnable;
            }
        }
        j = &wEepromGFCICTChkEn - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[65] != wEepromGFCICTChkEn) 
        {
            i = wHoldRegister[65];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromGFCICTChkEn = i;
                wHoldRegister[65] = wEepromGFCICTChkEn;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[65] = wEepromGFCICTChkEn;
            }
        }
        j = &wEepromAPFOption - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[66] != wEepromAPFOption) 
        {
            i = wHoldRegister[66];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromAPFOption = i;
                wHoldRegister[66] = wEepromAPFOption;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[66] = wEepromAPFOption;
            }
        }
        
        j = &wEepromFullRangeMpptEnable - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[68] != wEepromFullRangeMpptEnable) 
        {
            i = wHoldRegister[68];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFullRangeMpptEnable = i;
                wHoldRegister[68] = wEepromFullRangeMpptEnable;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[68] = wEepromFullRangeMpptEnable;
            }
        } 
        
        j = &wEepromFullRangeMpptCycle - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[69] != wEepromFullRangeMpptCycle) 
        {
            i = wHoldRegister[69];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromFullRangeMpptCycle = i;
                wHoldRegister[69] = wEepromFullRangeMpptCycle;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[69] = wEepromFullRangeMpptCycle;
            }
        }
        wEepromSupplSum07 = (INT16U)wEepromISOEnable + (INT16U)wEepromGFCICTChkEn + (INT16U)wEepromAPFOption +            
                            (INT16U)wEepromArcingEn + (INT16U)wEepromFullRangeMpptEnable + (INT16U)wEepromFullRangeMpptCycle; 


        j = &wEepromInvCurrMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[161] != wEepromInvCurrMax) 
        {
            i = wHoldRegister[161];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromInvCurrMax = i;
                wHoldRegister[161] = wEepromInvCurrMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[161] = wEepromInvCurrMax;
            }
        }
        j = &wEepromDCIOffsetDiffMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[162] != wEepromDCIOffsetDiffMax) 
        {
            i = wHoldRegister[162];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromDCIOffsetDiffMax = i;
                wHoldRegister[162] = wEepromDCIOffsetDiffMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[162] = wEepromDCIOffsetDiffMax;
            }
        }
        j = &wEepromOffsetDiffMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[163] != wEepromOffsetDiffMax) 
        {
            i = wHoldRegister[163];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromOffsetDiffMax = i;
                wHoldRegister[163] = wEepromOffsetDiffMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[163] = wEepromOffsetDiffMax;
            }
        }
        j = &wEepromEnvironmentTmpMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[164] != wEepromEnvironmentTmpMax) 
        {
            i = wHoldRegister[164];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromEnvironmentTmpMax = i;
                wHoldRegister[164] = wEepromEnvironmentTmpMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[164] = wEepromEnvironmentTmpMax;
            }
        }
        j = &wEepromPowerTmpMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[165] != wEepromPowerTmpMax) 
        {
            i = wHoldRegister[165];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromPowerTmpMax = i;
                wHoldRegister[165] = wEepromPowerTmpMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[165] = wEepromPowerTmpMax;
            }
        }
        j = &wEepromXFMRTmpMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[166] != wEepromXFMRTmpMax) 
        {
            i = wHoldRegister[166];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromXFMRTmpMax = i;
                wHoldRegister[166] = wEepromXFMRTmpMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[166] = wEepromXFMRTmpMax;
            }
        }
        wEepromSupplSum08 = (INT16U)wEepromInvCurrMax + (INT16U)wEepromDCIOffsetDiffMax + (INT16U)wEepromOffsetDiffMax +            
                            (INT16U)wEepromEnvironmentTmpMax + (INT16U)wEepromPowerTmpMax + (INT16U)wEepromXFMRTmpMax; 


        j = &wEepromTransformerTmpMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[167] != wEepromTransformerTmpMax) 
        {
            i = wHoldRegister[167];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromTransformerTmpMax = i;
                wHoldRegister[167] = wEepromTransformerTmpMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[167] = wEepromTransformerTmpMax;
            }
        }
        j = &wEepromBusVolMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[168] != wEepromBusVolMax) 
        {
            i = wHoldRegister[168];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBusVolMax = i;
                wHoldRegister[168] = wEepromBusVolMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[168] = wEepromBusVolMax;
            }
        }
        j = &wEepromCapVolMax - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[169] != wEepromCapVolMax) 
        {
            i = wHoldRegister[169];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromCapVolMax = i;
                wHoldRegister[169] = wEepromCapVolMax;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[169] = wEepromCapVolMax;
            }
        }
        j = &wEepromDCRelayActProt - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[170] != wEepromDCRelayActProt) 
        {
            i = wHoldRegister[170];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromDCRelayActProt = i;
                wHoldRegister[170] = wEepromDCRelayActProt;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[170] = wEepromDCRelayActProt;
            }
        }
        j = &wEepromBiDirDCSwitch - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[171] != wEepromBiDirDCSwitch) 
        {
            i = wHoldRegister[171];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBiDirDCSwitch = i;
                wHoldRegister[171] = wEepromBiDirDCSwitch;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[171] = wEepromBiDirDCSwitch;
            }
        }
        j = &wEepromBatteryType - &wEepromProtectSupplData[0]; 
        if(wHoldRegister[173] != wEepromBatteryType) 
        {
            i = wHoldRegister[173];
            if((i <= *(pwEepromDefaultProtMaxSupplArray+j)) && (i >= *(pwEepromDefaultProtMinSupplArray+j)) && (wEepromStandardValue != cNoStandard))
            {
                wEepromBatteryType = i;
                wHoldRegister[173] = wEepromBatteryType;
                fEepromOperateRequire = 1;
            }
            else
            {
            	wHoldRegister[173] = wEepromBatteryType;
            }
        }        
        wEepromSupplSum09 = (INT16U)wEepromTransformerTmpMax + (INT16U)wEepromBusVolMax + (INT16U)wEepromCapVolMax +            
                            (INT16U)wEepromDCRelayActProt + (INT16U)wEepromBiDirDCSwitch + (INT16U)wEepromBatteryType; 
                   
        fHoldRegisterStatus = 0;
    }
    else if(fHoldRegisterStatus == 2)        //Eeprom data ->Hold Register
    {
    INT16U j;
    INT16S wSysFreq;
        if(wEepromRatedFreqSel == 1)
        {
            wSysFreq = 6000;
        }
        else
        {
            wSysFreq = 5000;
        }
        
    	for(j=0;j<7;j++)
	    {
		    wLCDImptData[j] = wEepromImptData[j];
		    uwLCDRxImptDataFlag.flag |= 1<<j;
	    } 
        //第一组，电网电压保护设置
        wHoldRegister[0] = wEepromGridVolMax_1;
        wHoldRegister[1] = wEepromVolMaxTripTime_1;
        wHoldRegister[2] = wEepromGridVolMin_1;
        wHoldRegister[3] = wEepromVolMinTripTime_1;
        wHoldRegister[4] = wEepromGridVolMax_2;
        wHoldRegister[5] = wEepromVolMaxTripTime_2;
        wHoldRegister[6] = wEepromGridVolMin_2; 
        wHoldRegister[7] = wEepromVolMinTripTime_2;//
        wHoldRegister[8] = wEepromGridVolRecover_Max;
        wHoldRegister[9] = wEepromGridVolRecover_Min;
        wHoldRegister[10] = wEepromGridVolRecover_Time;
        wHoldRegister[11] = wEepromGridVoltBalanceMax;			
        wHoldRegister[12] = wEepromGridVolMax_3;
        wHoldRegister[13] = wEepromVolMaxTripTime_3;
        wHoldRegister[14] = wEepromGridVolMin_3;			
        wHoldRegister[15] = wEepromVolMinTripTime_3; 
        wHoldRegister[16] = wEepromPowerOnWaitTime;
        wHoldRegister[17] = 0;//reserver
        wHoldRegister[18] = 0;//reserver
        wHoldRegister[19] = 0;//reserver
        wHoldRegister[20] = 0;//reserver
        wHoldRegister[21] = 0;//reserver
        wHoldRegister[22] = 0;//reserver
        wHoldRegister[23] = 0;//reserver
        wHoldRegister[24] = 0;//reserver
        wHoldRegister[25] = 0;//reserver    
        wHoldRegister[26] = 0;//reserver
        wHoldRegister[27] = 0;//reserver
        wHoldRegister[28] = 0;//reserver
        wHoldRegister[29] = 0;//reserver
        //第2组，电网频率保护设置    
        wHoldRegister[30] = wEepromGridFreqMax_1 + wSysFreq;
        wHoldRegister[31] = wEepromFreqMaxTripTime_1;
        wHoldRegister[32] = wEepromGridFreqMin_1 + wSysFreq;
        wHoldRegister[33] = wEepromFreqMinTripTime_1;
        wHoldRegister[34] = wEepromGridFreqMax_2 + wSysFreq;
        wHoldRegister[35] = wEepromFreqMaxTripTime_2;
        wHoldRegister[36] = wEepromGridFreqMin_2 + wSysFreq;
        wHoldRegister[37] = wEepromFreqMinTripTime_2;
        wHoldRegister[38] = wEepromGridFreqRecover_Max + wSysFreq;
        wHoldRegister[39] = wEepromGridFreqRecover_Min + wSysFreq;
        wHoldRegister[40] = wEepromGridFreqRecover_Time;
        wHoldRegister[41] = 0;//reserver
        wHoldRegister[42] = 0;//reserver
        wHoldRegister[43] = 0;//reserver
        wHoldRegister[44] = 0;//reserver
        wHoldRegister[45] = 0;//reserver
        wHoldRegister[46] = 0;//reserver
        wHoldRegister[47] = 0;//reserver
        wHoldRegister[48] = 0;//reserver
        wHoldRegister[49] = wEepromPVFitFactor;
        //第3组，开关机的参数设置
        wHoldRegister[50] = wEepromPVStartUpVol;		
        wHoldRegister[51] = wEepromSoftPowerStep;		
        wHoldRegister[52] = 0;
        wHoldRegister[53] = wEepromSoftStopEn;		
        wHoldRegister[54] = wEepromStopPowerStep;		
        wHoldRegister[55] = wEepromIsoImpMin;     
        wHoldRegister[56] = wEepromGridFaultPowerStep;
		
        wHoldRegister[57] = wEepromAIEnable;//reserver
        wHoldRegister[58] = 0;//reserver
        wHoldRegister[59] = 0;//reserver
        wHoldRegister[60] = wEepromGFCIMax;//reserver
        wHoldRegister[61] = wEepromDCIMax;//reserver
        wHoldRegister[62] = 0;//reserver
        wHoldRegister[63] = 0;//reserver
        wHoldRegister[64] = wEepromISOEnable;
        wHoldRegister[65] = wEepromGFCICTChkEn;
        wHoldRegister[66] = wEepromAPFOption;
        wHoldRegister[67] = wEepromArcingEn;//reserver
        wHoldRegister[68] = wEepromFullRangeMpptEnable;//reserver
        wHoldRegister[69] = wEepromFullRangeMpptCycle;//reserver
        //第4组，功率参数
        wHoldRegister[70] = wEepromCSPCtrModeSel;
        wHoldRegister[71] = wEepromCSPPer;     
        wHoldRegister[72] = wEepromCSCurrSet;
        wHoldRegister[73] = wEepromPActStep;
        wHoldRegister[74] = wEepromCSQCtrModeSel;
        wHoldRegister[75] = wEepromCSPFSet;
        wHoldRegister[76] = wEepromCSQPer;
        wHoldRegister[77] = wEepromQActStep;
        wHoldRegister[78] = 0;//reserver
        wHoldRegister[79] = 0;//reserver
        wHoldRegister[80] = wEepromFWRecPowerStep; 
        wHoldRegister[81] = wEepromFWActTime;
        wHoldRegister[82] = wEepromQUActTime;
        wHoldRegister[83] = wEepromVWActTime;
        wHoldRegister[84] = wEepromPF_PCurveActPw1; 
        wHoldRegister[85] = wEepromPF_PCurvePF1;
        wHoldRegister[86] = wEepromPF_PCurveActPw2;
        wHoldRegister[87] = wEepromPF_PCurvePF2;
        wHoldRegister[88] = wEepromPF_PCurveLockInV;
        wHoldRegister[89] = wEepromPF_PCurveLockOutV;
        wHoldRegister[90] = wEepromQ_UCurveVolt1s;
        wHoldRegister[91] = wEepromQ_UCurveReactPw1s;
        wHoldRegister[92] = wEepromQ_UCurveVolt2s;  
        wHoldRegister[93] = wEepromQ_UCurveReactPw2s;
        wHoldRegister[94] = wEepromQ_UCurveVolt1i;   
        wHoldRegister[95] = wEepromQ_UCurveReactPw1i;
        wHoldRegister[96] = wEepromQ_UCurveVolt2i;   
        wHoldRegister[97] = wEepromQ_UCurveReactPw2i;
        wHoldRegister[98] = wEepromQ_UCurveLockInP;    
        wHoldRegister[99] = wEepromQ_UCurveLockOutP;
        wHoldRegister[100] = wEepromOvFWEn;
        wHoldRegister[101] = wEepromOvFWTripFreq + wSysFreq;;
        wHoldRegister[102] = wEepromOvFWRatio;            
        wHoldRegister[103] = wEepromOvFWExitFreq + wSysFreq;;			
        wHoldRegister[104] = wEepromUnFWEn;            
        wHoldRegister[105] = wEepromUnFWTripFreq + wSysFreq;;            
        wHoldRegister[106] = wEepromUnFWRatio;
        wHoldRegister[107] = wEepromUnFWExitFreq + wSysFreq;;
        wHoldRegister[108] = wEerpomOvVWEnable;
        wHoldRegister[109] = wEerpomUnVWEnable;
        wHoldRegister[110] = wEepromOvVWStartVolt;
        wHoldRegister[111] = wEepromOvVWStopVolt;
        wHoldRegister[112] = wEepromOvVWRatio;
        wHoldRegister[113] = wEepromUnVWStartVolt;
        wHoldRegister[114] = wEepromUnVWStopVolt;
        wHoldRegister[115] = wEepromUnVWRatio;
        wHoldRegister[116] = 0;//reserver
        wHoldRegister[117] = 0;//reserver
        wHoldRegister[118] = 0;//reserver
        wHoldRegister[119] = 0;//reserver
        wHoldRegister[120] = 0;//reserver
        wHoldRegister[121] = 0;//reserver
        wHoldRegister[122] = 0;//reserver
        wHoldRegister[123] = 0;//reserver
        wHoldRegister[124] = 0;//reserver
        //第6组，LVRT功能设置
        wHoldRegister[125] = wEepromLVRTOption;
        wHoldRegister[126] = wEepromLVRTTripVol; 
        wHoldRegister[127] = wEepromFVRTPosCurrK;
        wHoldRegister[128] = wEepromFVRTNegCurrK;
		wHoldRegister[129] = wEepromHVRTOption;//reserver
        wHoldRegister[130] = wEepromHVRTTripVol;//reserver
        wHoldRegister[131] = wEepromFVRTPowerStep;//reserver
        wHoldRegister[132] = 0;//reserver
        wHoldRegister[133] = 0;//reserver
        wHoldRegister[134] = 0;//reserver
        //第7组，电池相关参数
        wHoldRegister[135] = wEepromChargeVoltLimt;
        wHoldRegister[136] = wEepromDisChargeVoltLimt;
        wHoldRegister[137] = wEepromChargeCurrLimt;
        wHoldRegister[138] = wEepromDisChargeCurrLimt; 
        wHoldRegister[139] = wEepromFloatChargeVolt;
        wHoldRegister[140] = wEepromSToFChargeCurr;
        wHoldRegister[141] = wEepromBatteryVoltMax;
        wHoldRegister[142] = wEepromBatteryVoltMin;
        wHoldRegister[143] = wEepromBatChargeCurrMax;
        wHoldRegister[144] = wEepromBatDisChargeCurrMax; 
        wHoldRegister[145] = wEepromDCRelayVoltDiff;     
        wHoldRegister[146] = wEepromBatPreChargeCurrMax;
        wHoldRegister[147] = 0;
        wHoldRegister[148] = wEepromCSVSSwitch;
        wHoldRegister[149] = wEepromAutoCSVSSwitchEn;
        wHoldRegister[150] = wEepromVSCtrMode;      
        wHoldRegister[151] = wEepromVSVoltSet;    
        wHoldRegister[152] = wEepromVSFreqSet;    
        wHoldRegister[153] = wEepromVSPSet;
        wHoldRegister[154] = wEepromVSQSet;
        wHoldRegister[155] = wEepromVSDroopEn;
        wHoldRegister[156] = wEepromVSFreqDroopKByP;
        wHoldRegister[157] = wEepromVSVoltDroopKByQ;
        wHoldRegister[158] = 0;//reserver
        wHoldRegister[159] = 0;//reserver
        wHoldRegister[160] = 0;//reserver
        // 其它参数
        wHoldRegister[161] = wEepromInvCurrMax;
        wHoldRegister[162] = wEepromDCIOffsetDiffMax;
        wHoldRegister[163] = wEepromOffsetDiffMax;
        wHoldRegister[164] = wEepromEnvironmentTmpMax;
        wHoldRegister[165] = wEepromPowerTmpMax;
        wHoldRegister[166] = wEepromXFMRTmpMax;
        wHoldRegister[167] = wEepromTransformerTmpMax;
        wHoldRegister[168] = wEepromBusVolMax;
        wHoldRegister[169] = wEepromCapVolMax;
        wHoldRegister[170] = wEepromDCRelayActProt;
        wHoldRegister[171] = wEepromBiDirDCSwitch;
        wHoldRegister[172] = wEepromRemoteSel;
        wHoldRegister[173] = wEepromBatteryType;
        wHoldRegister[174] = 0;
        wHoldRegister[175] = 0;
        wHoldRegister[176] = 0;
        wHoldRegister[177] = 0;
        wHoldRegister[178] = 0;
        wHoldRegister[179] = 0;
        wHoldRegister[180] = 0;
        wHoldRegister[181] = 0;
        wHoldRegister[182] = 0;
        wHoldRegister[183] = 0;
        wHoldRegister[184] = 0;
        wHoldRegister[185] = 0;
	    
        fHoldRegisterStatus = 0;      
	}    
}
/*********************************************************************     
Function name: void sInputDataUpdate(void)                                             
Description:                               
Calls:        	                                                           
Called By:   sCANDataMange(), period: 20ms                                                          
Parameters: void                                                         
Return:     void		                                                       
 *********************************************************************/  
void sInputDataUpdate(void)
{
    INT16S i = 0;
    static INT64U uddwSerialNumberbak = 0;    
    INT64U uddwSerialNumber;
    INT64U uddwSerialNumberTemp;
    INT16U uwProductCode;
    INT16U high,low;
	INT32S DisPowerTemp=0;
	INT32S PowerTemp=0;
		
	if(wEepromSerialNumberH > 1000)
	{
	    uddwSerialNumberTemp=(((INT32U)wEepromSerialNumberM)<<16) + (INT16U)wEepromSerialNumberL;
        if(uddwSerialNumberTemp > 9999999)uddwSerialNumberTemp = 9999999;  
        uwProductCode = wEepromSerialNumberH;
        if(uwProductCode > 9999) uwProductCode = 9999;
	    uddwSerialNumber = 1000000000000 + (INT64U)uwProductCode*10000000 + uddwSerialNumberTemp;
	}
	else
	{
		uddwSerialNumber = (((INT64U)wEepromSerialNumberH)<<32) + (((INT32U)wEepromSerialNumberM)<<16) + (INT16U)wEepromSerialNumberL;            
    }
    
    if(uddwSerialNumberbak != uddwSerialNumber) 
    {    
        uddwSerialNumberbak = uddwSerialNumber;
                
        wInputRegister[0] = wEepromMachineVersion; 
        wInputRegister[1] = cSoftwareVersion; 
        high = (((INT16U)(uddwSerialNumber/1000000000000000))<<4) + (INT16U)((uddwSerialNumber%1000000000000000)/100000000000000);
        low = (((INT16U)((uddwSerialNumber%100000000000000)/10000000000000))<<4) + (INT16U)((uddwSerialNumber%10000000000000)/1000000000000);          
        wInputRegister[2] = (high<<8) + low;
        high = (((INT16U)((uddwSerialNumber%1000000000000)/100000000000))<<4) + (INT16U)((uddwSerialNumber%100000000000)/10000000000); 
        low = (((INT16U)((uddwSerialNumber%10000000000)/1000000000))<<4) + (INT16U)((uddwSerialNumber%1000000000)/100000000);
        wInputRegister[3] = (high<<8) + low;
        high = (((INT16U)((uddwSerialNumber%100000000)/10000000))<<4) + (INT16U)((uddwSerialNumber%10000000)/1000000);
        low = (((INT16U)((uddwSerialNumber%1000000)/100000))<<4) + (INT16U)((uddwSerialNumber%100000)/10000);
        wInputRegister[4] = (high<<8) + low;
        high = (((INT16U)((uddwSerialNumber%10000)/1000))<<4) + (INT16U)((uddwSerialNumber%1000)/100);
        low = (((INT16U)((uddwSerialNumber%100)/10))<<4) + (INT16U)(uddwSerialNumber%10);
        wInputRegister[5] = (high<<8) + low;  
    }
    
    wInputRegister[6] = 0;
    wInputRegister[7] = 256;
    wInputRegister[8] = 1024;
    wInputRegister[9] = 4096;
    wInputRegister[10] = sFormatQ4To10Rate(wRSGridRmsAvgVol);
    wInputRegister[11] = sFormatQ4To10Rate(wSTGridRmsAvgVol);
    wInputRegister[12] = sFormatQ4To10Rate(wTRGridRmsAvgVol);
    wInputRegister[13] = sFormatQ8To100Rate(uwRFreq);
    wInputRegister[14] = sFormatQ8To100Rate(wRInvRmsCurr);             	     
    wInputRegister[15] = sFormatQ8To100Rate(wSInvRmsCurr);
    wInputRegister[16] = sFormatQ8To100Rate(wTInvRmsCurr);
    wInputRegister[17] = sFormatQ4To10Rate(wHeatFitTemp);//模块温度
    wInputRegister[18] = sFormatQ4To10Rate(wEnvFitTemp);      
    wInputRegister[19] = 0;
    wInputRegister[20] = sFormatQ4To10Rate(wBusAvgVol);
    if(fDCRelayPCtr == 0)
    {
        wInputRegister[21] = sFormatQ4To10Rate(wBatteryOutAvgVol);
    }
    else
    {
    	wInputRegister[21] = sFormatQ4To10Rate(wBatteryInAvgVol);
    } 
    wInputRegister[22] = sFormatQ8To100Rate(wBatteryAvgCurr);    // BatteryCurrentt, PVAttribute[1].wPVLongAvgCurr
    
    if(fCharge == 1)
    {
        if((dwInvActivePower[3] < 0)||(dwInvActivePower[3] > 16000))
        {
            wInputRegister[23] = sFormatQ4ToR01Rate(dwDCPower);
            wInputRegister[24] = sFormatQ4ToR01Rate(dwInvActivePower[3]);  //0.01kW
        }
        else
        {
            wInputRegister[23] = 0;
            wInputRegister[24] = 0;  //0.01kW
        }        
    }
    else
    {
        if((dwInvActivePower[3] > 0)||(dwInvActivePower[3] < -16000))
        {
            wInputRegister[23] = sFormatQ4ToR01Rate(dwDCPower);
            wInputRegister[24] = sFormatQ4ToR01Rate(dwInvActivePower[3]);  //0.01kW
        }
        else
        {
            wInputRegister[23] = 0;
            wInputRegister[24] = 0;  //0.01kW
        }
    }    
    wInputRegister[25] = sFormatQ4ToR01Rate(dwInvReactivePower[3]);
    wInputRegister[26] = uwPowerEffAvg;  //三相效率，空缺
    wInputRegister[27] = sFormatQ10To1000Rate(wPowerFactorAvg[3]);     //功率因数不对
    wInputRegister[28] = wActivePowerPer;
    wInputRegister[29] = sFormatQ8To100Rate(wDCCurrAim);
    wInputRegister[30] = ((INT32S)wPFSet*1000)>>14;
    wInputRegister[31] = wReactivePowerPer;
    wInputRegister[32] = sFormatQ4ToR01Rate(dwInvActivePower[0]);
    wInputRegister[33] = sFormatQ4ToR01Rate(dwInvReactivePower[0]);
    wInputRegister[34] = sFormatQ10To1000Rate(wPowerFactorAvg[0]);
    wInputRegister[35] = sFormatQ4ToR01Rate(dwInvActivePower[1]);;
    wInputRegister[36] = sFormatQ4ToR01Rate(dwInvReactivePower[1]);
    wInputRegister[37] = sFormatQ10To1000Rate(wPowerFactorAvg[1]);
    wInputRegister[38] = sFormatQ4ToR01Rate(dwInvActivePower[2]);;
    wInputRegister[39] = sFormatQ4ToR01Rate(dwInvReactivePower[2]);
    wInputRegister[40] = sFormatQ10To1000Rate(wPowerFactorAvg[2]);
    wInputRegister[41] = sFormatQ4ToR01Rate(udwApparentPower[3]);
	DisPowerTemp=(INT32S)((INT64S)DisChargeMaxPower*dwRatedPower+8192)>>14;
	DisPowerTemp=DisPowerTemp/10;
	if(DisPowerTemp>3000)DisPowerTemp=3000;

	PowerTemp=(INT32S)((INT64S)ChargeMaxPower*dwRatedPower+8192)>>14;
	PowerTemp=PowerTemp/10;
	if(PowerTemp>2000)PowerTemp=2000;
    wDebugTest4=(INT16S)PowerTemp;
    wInputRegister[42] = (INT16S)PowerTemp;
    wInputRegister[43] = (INT16S)DisPowerTemp;
    wInputRegister[44] = (INT16S)fPCSState.all;
    wInputRegister[45] = 0;
    wInputRegister[46] = 0;
    wInputRegister[47] = 0;
    wInputRegister[48] = 0;
    wInputRegister[49] = 0;
    wInputRegister[50] = 0;
    wInputRegister[51] = 0;
    wInputRegister[52] = 0;
    wInputRegister[53] = 0;
    wInputRegister[54] = 0;
    wInputRegister[55] = 0;
    wInputRegister[56] = 0;
    wInputRegister[57] = 0;
    wInputRegister[58] = 0;
    wInputRegister[59] = uwWarrningCode;
    wInputRegister[60] = uwFaultCode[0]; 
    wInputRegister[61] = uwFaultCode[1]; 
    wInputRegister[62] = uwFaultCode[2];
    wInputRegister[63] = uwFaultCode[3];
    wInputRegister[64] = uwFaultCode[4];
    wInputRegister[65] = uwPFCode; 
        
    i = 0;    
    if((uwSysMode == FaultMode)||(uwSysMode == ShutMode))//Bit 15  Fault
    {
    	i |= 32768;
    }
    
    if(fStartOrNormal == 0)//Bit 14 SysCheck
    {
    	i |= 16384;
    }
    
    if((uwSysMode == PowerOnMode)&&(fStartOrNormal == 1))//Bit 13 Standby
    {
        i |= 8192;
    }
    
    if(uwSysMode == InvRunMode)//Bit 12   Runing
    {
        i |= 4096;
        if(fHotBackup == 1)//Bit 11   HotBack
        {
        	i |= 2048;
        }
    }
    
    if(fCSVSSwitch == 1)// Bit 10 //Current Source,Voltage Source State
    {
    	i |= 1024;
    	
    	if(dwInvActivePower[3] > 0)
    	{
    	    i |= 512;
    	}
    }
    else
    {
        if(fCharge == 0)// Bit 9   Charge Or Discharge
        {
        	i |= 512;
        }
    }
    
    if(fBattDarkStart == 1)//Bit 8    DarkStart 
    {
    	i |= 256;
    }    
    
    if(uwSysMode == PreCheckMode)//((fRunInvAhdChkRequire == 1)&&(uwSysMode == PreCheckMode))// Bit 6，Inv Check
    {
        i |= 64;
    }
    
    i |= 32;//Bit 5   PCS Init
    
    if(fBatteryInitiStateOK >= 2)//Bit 4 Battery Init
    {
        i |= 16;  
    }

    if((fDerating != 0)&&(uwSysMode == InvRunMode)) // Bit 3   Derating
    {
    	i |= 8; 
    }
		
	if((fLineGridRmsVolErrEvent == 0)&&(fGridVoltBalanceDegreeErrEvent == 0) &&
	  (fFreHighErrEvent == 0)&&(fFreLowErrEvent == 0)&&(fNoUtilityErrEvent == 0))//Bit 2
	{
		i |= 4;
	} 
	
	if(uwSysMode == InvRunMode)//Bit 1,Runing
	{
		i |= 2;
	}
	
	if(fCtrPowerErrEvent == 0)//Bit 0,SysPower
	{
		i |= 1;
	}
		
	wInputRegister[66] = i; //系统状态字
    
    wInputRegister[69] = 0; 
    wInputRegister[70] = 0;
    wInputRegister[71] = 0; 
    wInputRegister[72] = 0; 
    wInputRegister[73] = 0;
    wInputRegister[74] = 0; 
    wInputRegister[75] = 0; 
    wInputRegister[76] = 0; 
    wInputRegister[77] = 0; 
    wInputRegister[78] = 0;
    wInputRegister[79] = 0; 
        	
	if((uwLCDRxImptDataFlag.flag != 0) && (wEepromStandardValue != wLCDImptData[6])) fEepromOperateRequire = 1;  //set StandValue
}

/*********************************************************************     
Function name: void sCANDataMange(void)                                             
Description:                               
Calls:        	                                                           
Called By:   main(), period: 20ms                                                          
Parameters: void                                                         
Return:     void		                                                       
 *********************************************************************/  
void sCANDataMange(void)
{
    U_CANCOMD wCANTXComd;
    
    sInputDataUpdate();
    sHoldComdDeal();
    sHoldDataDeal();    
        
    //--------------------------------------eCAN Tx Deal------------------------------------------
    if((fHoldTx == 1)&&(uwHoldDataCnt == 0))
    {
    	wCANTXComd.bit.DataAddr = uwHoldDataAddr;
    	wCANTXComd.bit.DataLength = uwHoldDataLength;     	
        	
        if(wCANTXComd.bit.DataLength <= 1)
        {
            wCANTXComd.bit.DataLength = 1;
            CANTX(wCANTXComd.all,wHoldRegister[uwHoldDataAddr],0,0,4,9);
            uwHoldDataCnt = 1;
        }
        else if(wCANTXComd.bit.DataLength == 2)
        {
        	wCANTXComd.bit.DataLength = 2;
        	CANTX(wCANTXComd.all,wHoldRegister[uwHoldDataAddr],wHoldRegister[uwHoldDataAddr+1],0,6,9);
        	uwHoldDataCnt = 2;
        }
        else
        {
        	wCANTXComd.bit.DataLength = 3;
        	CANTX(wCANTXComd.all,wHoldRegister[uwHoldDataAddr],wHoldRegister[uwHoldDataAddr+1],wHoldRegister[uwHoldDataAddr+2],8,9);
        	uwHoldDataCnt = 3;
        }        
    }
    
    if((fRemoteTx == 1)&&(uwRemoteDataCnt == 0))
    {
    	wCANTXComd.bit.DataAddr = uwRemoteDataAddr;
    	
        if(uwRemoteDataLength <= 1)
        {
            wCANTXComd.bit.DataLength = 1;
            CANTX(wCANTXComd.all,wRemoteData[uwRemoteDataAddr],0,0,4,10);
            uwRemoteDataCnt = 1;
        }
        else if(uwRemoteDataLength == 2)
        {
        	wCANTXComd.bit.DataLength = 2;
        	CANTX(wCANTXComd.all,wRemoteData[uwRemoteDataAddr],wRemoteData[uwRemoteDataAddr+1],0,6,10);
        	uwRemoteDataCnt = 2;
        }
        else
        {
        	wCANTXComd.bit.DataLength = 3;
        	CANTX(wCANTXComd.all,wRemoteData[uwRemoteDataAddr],wRemoteData[uwRemoteDataAddr+1],wRemoteData[uwRemoteDataAddr+2],8,10);
        	uwRemoteDataCnt = 3;
        }
    }
    
    if((fInputTx == 1)&&(uwInputDataCnt == 0))
    {
        wCANTXComd.bit.DataAddr = uwInputDataAddr;
    	wCANTXComd.bit.DataLength = uwInputDataLength;      	
        	
        if(wCANTXComd.bit.DataLength <= 1)
        {
            wCANTXComd.bit.DataLength = 1;
            CANTX(wCANTXComd.all,wInputRegister[uwInputDataAddr],0,0,4,8);
            uwInputDataCnt = 1;
        }
        else if(wCANTXComd.bit.DataLength == 2)
        {
        	wCANTXComd.bit.DataLength = 2;
        	CANTX(wCANTXComd.all,wInputRegister[uwInputDataAddr],wInputRegister[uwInputDataAddr+1],0,6,8);
        	uwInputDataCnt = 2;
        }
        else
        {
        	wCANTXComd.bit.DataLength = 3;
        	CANTX(wCANTXComd.all,wInputRegister[uwInputDataAddr],wInputRegister[uwInputDataAddr+1],wInputRegister[uwInputDataAddr+2],8,8);
        	uwInputDataCnt = 3;
        }        
    } 
    if((fImportTx == 1)&&(uwImportDataCnt == 0))
    {
        wCANTXComd.bit.DataAddr = uwImportDataAddr;
    	wCANTXComd.bit.DataLength = uwImportDataLength;      	
        	
        if(wCANTXComd.bit.DataLength <= 1)
        {
            wCANTXComd.bit.DataLength = 1;
            CANTX(wCANTXComd.all,wLCDImptData[uwImportDataAddr],0,0,4,12);
            uwImportDataCnt = 1;
        }
        else if(wCANTXComd.bit.DataLength == 2)
        {
        	wCANTXComd.bit.DataLength = 2;
        	CANTX(wCANTXComd.all,wLCDImptData[uwImportDataAddr],wLCDImptData[uwImportDataAddr+1],0,6,12);
        	uwImportDataCnt = 2;
        }
        else
        {
        	wCANTXComd.bit.DataLength = 3;
        	CANTX(wCANTXComd.all,wLCDImptData[uwImportDataAddr],wLCDImptData[uwImportDataAddr+1],wLCDImptData[uwImportDataAddr+2],8,12);
        	uwImportDataCnt = 3;
        }        
    }  
}
/*********************************************************************     
Function name: void sCANConfigMaster(void)                                             
Description:  用于将本机邮箱配置为主机                          
Calls:        	                                                           
Called By:  main();  sCANActiveChk();
Parameters: void                                                         
Return:     void		                                                       
 *********************************************************************/  
void sCANConfigID(void)
{
U_CANID uCANIDConfig;
U_CANBootID uCANBootIDConfig;
    if(fCANBCongfigFinish == 0)
    {    
	//-------------------------11 Tx Mail send DSP Control LCD Command-------------------------
	    uCANIDConfig.bit.Reserver = 0;
	    uCANIDConfig.bit.MachineNum = 0xA;
	    uCANIDConfig.bit.SoruAddr = fLocalPhyAddr; //1
	    uCANIDConfig.bit.AimAddr = cMasterPhyAdr; //0   
       
        //-------------------------10 Tx Mail send RemoteRegister Data-------------------------
        uCANIDConfig.bit.OperateType = cResponse;
        
        uCANIDConfig.bit.DataGroup = cRemoteRegister;
		configeCANAmbx(1, 10, uCANIDConfig.all,1,8);

        uCANIDConfig.bit.DataGroup = cHoldRegister;
        configeCANAmbx(1, 9, uCANIDConfig.all,1,5);

        uCANIDConfig.bit.DataGroup = cInputRegister;
        configeCANAmbx(1, 8, uCANIDConfig.all,1,1);

        uCANIDConfig.bit.DataGroup = cImportRegister;
        configeCANAmbx(1, 12, uCANIDConfig.all,1,7);
                
		uCANBootIDConfig.bit.DataSource   = 0x8A2;
		uCANBootIDConfig.bit.PhyAddr      = 1;//(INT16U)(1<<(fPhyAddr - 1));
		configeCANAmbx(1, 11, uCANBootIDConfig.all,1,1);//邮箱16作为Bootloader回复邮箱
        //-------------------Rx Box Config Write-------------------
        uCANIDConfig.bit.SoruAddr = cMasterPhyAdr;
	    uCANIDConfig.bit.AimAddr = fLocalPhyAddr;
        
        uCANIDConfig.bit.OperateType = cWrite;
        	    
	    uCANIDConfig.bit.DataGroup = cImportRegister;  
	    configeCANAmbx(0, 0, uCANIDConfig.all,1,0);	
	    
	    uCANIDConfig.bit.DataGroup = cRemoteRegister;  
	    configeCANAmbx(0, 1, uCANIDConfig.all,1,0);	
	    
	    uCANIDConfig.bit.DataGroup = cHoldRegister; 
	    configeCANAmbx(0, 2, uCANIDConfig.all,1,0);
	    
	    //-------------------Rx Box Config Read-------------------         
        uCANIDConfig.bit.OperateType = cRead;
        uCANIDConfig.bit.DataGroup = cRemoteRegister;//2
        
	    configeCANAmbx(0, 4, uCANIDConfig.all,1,0);	
	    
	    uCANIDConfig.bit.DataGroup = cHoldRegister;//1
	    configeCANAmbx(0, 3, uCANIDConfig.all,1,0);
	    
	    uCANIDConfig.bit.DataGroup = cInputRegister;//0 
	    configeCANAmbx(0, 6, uCANIDConfig.all,1,0);
	    
	    uCANIDConfig.bit.DataGroup = cImportRegister;//5 
        configeCANAmbx(0, 7, uCANIDConfig.all,1,0);  
	    
        uCANBootIDConfig.bit.DataSource   = 0x75D;
		configeCANAmbx(0, 5, uCANBootIDConfig.all,1,0);//邮箱15作为BootLoader接收邮箱 
	   
	    fCANBCongfigFinish = 1;
    } 
}
//--------------------------end of file-------------------------------

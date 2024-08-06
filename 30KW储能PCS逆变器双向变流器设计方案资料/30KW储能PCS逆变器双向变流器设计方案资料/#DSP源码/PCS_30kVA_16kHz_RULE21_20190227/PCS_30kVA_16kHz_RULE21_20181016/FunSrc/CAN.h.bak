/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef CAN_H
#define CAN_H

//#include "..\FunSrc\SysDefine.h"
//#include "..\FunSrc\SYSCTR.h"

/*****************SCILCD.h_Reserved Declartion********/

extern INT16U fLcdRunOrder,fLcdSoftRunOrder;
extern INT16U fHoldRegisterStatus;
extern INT16U fStandardChgStopRequire;

extern INT8U ubBootloaderRecData[];
extern INT16U fDisBootloaderOrder; 
extern INT16U uwBootloaderStep;
extern INT16S wPVLongAvgVol[];
extern INT16U uwDebugCANA1,uwDebugCANA2,uwDebugCANA3;
extern INT16U fCANBCongfigFinish;
extern INT16S wRemoteData[];
extern INT16S wInputRegister[];
extern INT16S wHoldRegister[];
extern INT16U fECANRunOrder;
 
#define SCI_DataNum   7    //=0:1 char bit,=1:2 char bits,=2:3 char bits ... =7:8 char bits 
#define SCI_StopNum   0    //=0:1 stop bit,      =1:2 stop bits  
#define SCI_Check     0    //=0:None check bit,  =1:Odd check   ,=2:Even check
/*************************/


//---------------------------------------------------------------------------
// Function Prototypes
//

typedef struct
{
	INT16U OperateType:4;	    
    INT16U DataGroup:4;
    INT16U AimAddr:8;    
    INT16U SoruAddr:8;        
    INT16U MachineNum:5;
	INT16U Reserver:3;	
}T_CANID;

typedef union
{
    INT32U      all;
    T_CANID     bit;
}U_CANID;

typedef struct
{
	Uint16 PhyAddr        :16;
	Uint16 DataSource     :13;
	Uint16 rsvr1          :3;
}T_CANBootID;

typedef union
{
    Uint32          all;
    T_CANBootID     bit;
}U_CANBootID;
typedef struct
{
	INT16U DataLength:8;
	INT16U DataAddr:8;	
}T_CANCOMD;

typedef union
{
    INT16U      all;
    T_CANCOMD   bit;
}U_CANCOMD;


extern void InitECana(void);
extern void ECanISR0(void);
extern void ECanISR1(void);
extern void CANTX(INT16S MDHH,INT16S MDHL,INT16S MDLH,INT16S MDLL,INT16U DataNum,INT16U mbxNo);  
extern void CANRX(volatile struct MBOX *MailBOX);
extern void InitECanVar(void);
extern void sCANDataMange(void);
extern void sResetECana(void);
extern void sCANConfigID(void);

extern INT16U fLocalPhyAddr; 
extern INT16U fCANActive;

//#define wRemoteCSOrVSSelEn          wRemoteData[0]
#define wRemoteCSVSSwitch           wRemoteData[1]
#define wRemoteVSCtrEn              wRemoteData[2]
#define wRemoteVSFreSet             wRemoteData[3]
#define wRemoteVSVolSet             wRemoteData[4]
#define wRemoteCSPCtrEn             wRemoteData[5]
#define wRemoteCSPCtrModeSel        wRemoteData[6]
#define wRemoteCSPPer               wRemoteData[7]   
#define wRemoteCSCurrSet            wRemoteData[8] 
#define wRemoteCSQCtrEn             wRemoteData[9]
#define wRemoteCSQCtrModeSel        wRemoteData[10]
#define wRemoteCSPFSet              wRemoteData[11]  
#define wRemoteCSQPer               wRemoteData[12]
#define fRemoteK7Ctr                wRemoteData[13]
#define fRemotePCSComdOn            wRemoteData[14]
//#define fRemotePCSComdOff           wRemoteData[15]

#define wBatteryPortVolt            wRemoteData[30]
#define fBatteryReady               wRemoteData[31]
#define fBatteryPreChargeEn         wRemoteData[32]
#define wBatteryPower               wRemoteData[33]
#define wBatteryCurrFromOut         wRemoteData[34]
#define wSOCChargeVoltLimit         wRemoteData[35]
#define wSOCDisChargeVoltLimit      wRemoteData[36]
#define wSOCChargeCurrLimt          wRemoteData[37]
#define wSOCDisChargeCurrLimit      wRemoteData[38]
#define wSOCFloatChargeVolt         wRemoteData[39]
#define wSOCSToFChargeCurr          wRemoteData[40]
//#define fRemotePCSDisConnect        wRemoteData[41]
#define fLCDIOState                 wRemoteData[42]

#define sCANIntEnable()   {PieCtrlRegs.PIEIER9.bit.INTx6 = 1;}
#define sCANIntDisable()  {PieCtrlRegs.PIEIER9.bit.INTx6 = 0;}

//---------------------------------------------------------------------------
#endif  // end of CAN_H definition

/*** end of file *****************************************************/

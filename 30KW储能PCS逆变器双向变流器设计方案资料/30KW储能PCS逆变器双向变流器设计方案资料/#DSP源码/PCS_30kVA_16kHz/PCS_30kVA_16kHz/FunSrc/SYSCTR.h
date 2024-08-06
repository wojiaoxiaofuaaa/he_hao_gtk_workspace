/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef SYSCTR_H
#define SYSCTR_H
#include "..\FunSrc\SysDefine.h"

typedef enum  
{
	BootloaderMode,
    ShutMode,
    FaultMode,  
    PowerOnMode,   
    PreCheckMode,
    InvRunMode    
}T_SYSMODE;

//---------------------------------------------------------------------------
// Function Prototypes
//
extern void InitSysCtrVar(void);
extern void sBatteryInitiStateJudge(INT16S wBattVolt);
extern INT16S swModuleEnvTempFit(INT16S wAvgTemp);
extern INT16S swModuleHeatTempFit(INT16S wAvgTemp);
extern INT16U sFanOpenLoopCtrl(INT16S wEnvTemp, INT16S wHeatTemp,INT16U fTmpflag,INT32S dwPPower,INT32S dwQPower,INT32U udwAppPower);
extern INT16S swCalBalanceDegree(T_THREE_PHASE_ABC *ptGridPhaseRmsVolt);
extern void sCalSignedLongAvg(INT16S *pData,INT16U uwAvgTimeCnt,INT16S *pAvgData);
extern void sCalBusVolRef(INT16S *pRSRmsVol,INT16S *pSTRmsVol,INT16S *pTRRmsVol);
extern void sRestartVarInit(void);
extern INT16U sDecCapEnergy(INT16S *pBusVol);
extern INT16U swEfficiencyFit(INT32S dwPPower,INT16S wBatVolt);
extern INT16U sfPowerStaChk(INT16S wBusOpenCirVol,INT16S wBusRunVol,INT32U udwPowerSta);
extern INT16U sfRunInvAhdChk(void);
extern void sPowerFactor(INT32S *pInvLongActivePower,INT32U *pLongApparentPower,INT16S *pPowerFactor);
extern void sAdjustPF(void);
extern INT16U suwMidGridFreqExtract(INT16U uwRGirdFreq,INT16U uwSGirdFreq,INT16U uwTGirdFreq,INT32S *pAngIngSub);
extern INT16S sIqRefExtract(T_THREE_PHASE_DQO *ptInvCurrRefDqo);
extern INT16U suMaxOutputCurrCal(INT16S wEnvTemp, INT16S wHeatTemp, INT16S wBusVoltage,INT16S wMaxRmsCurr);
extern void sCalIdLimit(void);
extern void sCalIqRef(void);
extern void sfPowerComdCal(void);
extern void sSysSleepChk(void);
extern void sSysRateCoeffChangeInit(void);
extern INT16U suwInvAndDCPwmKCal(INT16U wDCVol);
extern INT16S swGirdDivBatteryVolCoeff(INT16U wDCVol);
extern void sBiDirDCModeSwitch(INT16S wBatVolt,INT16S wBatCurr,INT32S dwPower);
extern INT16U sfBiDirDCCtrSoftStart(INT16S wBatInVolt,INT16S wBatOutVolt);
extern INT16U sfBiDirDCCtrSoftStartByAC(INT16S wBatInVolt,INT16S wBatOutVolt);
extern INT16U sfRunInvAhdChkByAC(void);
extern void sBatteryChargeDeal(INT16S wBattVolt,INT16S wBattCurr);
extern void sDC12And34OptSwitch(void);
extern void sRelayDelayDeal(void);
extern void sACPowerDeal(INT16S wBatVolt);
extern void sBatteryDataUpdateChk(INT16S wBatPortVolt);
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern T_SYSMODE uwSysMode,uwSysModePrepare;
extern INT16U fPhaseSequ,fPhaseSequBak;

extern INT16U uwRFre,uwSFre,uwTFre;
extern INT16U uwRFreq,uwSFreq,uwTFreq,uwGridFreq;

extern INT16U fIsoStatue;
extern INT16U fRelayChkRequire;
extern INT16U fInvSoftStaRequire;
extern INT16U fBatteryInitiStateOK;
extern INT16U fRunOrder;
extern INT16U fCallDefaultValueOrder;
extern INT16U fQuitShutModeOrder;
extern INT16U fMpptEnable;
extern INT32U udwPowerOnWaitTime;
extern INT16U fError;
extern INT16U fFanStatus;
extern INT16U fStartPowerCheck;
extern INT16S wGridVolBalanceDegree;
extern INT16S wGridVolAvgBalanceDegree;
extern INT16U fRunInvAhdChkRequire,fInvLittleVolOk,fPvPowEnough,fPLLStaOk,fRunInvAhdChkRestart;  
extern INT16S wIqRefBakToLVRT;
extern INT16U fDeCapEnergy;
extern INT16U uwDECapEnergyStep;
extern INT16U fClearBusCtr;
extern INT16U uwLVRTActiveValue;
extern INT16U uwHVRTActiveValue;
extern INT32S dwI2TValue; 
extern INT16U fCANRunOrder;
extern INT16S wAdjustPF,wReactiveCurrErr;
extern INT16U uwCurrMax;
extern INT16U fAuxiRelayLost;
extern INT16U fPowerDanger;
extern INT16S wActivePowerPer,wPFSet;
extern INT16S wReactivePowerPer;
extern INT16S wPFSet;
extern INT16S fReactiveMode,fActiveMode;
extern INT16U fDerating;
extern INT32S dwReactivePowerAim;
extern INT16S wPowerDeratingIdmax;
extern INT16S wDebugIqStep;
extern INT16S wAdjustStep;
extern INT16U fSysFreChange;
extern INT16U uwpuApparentPowerMax;
extern INT16U uwpuReactivePowerMax;
extern INT16U fGridPretect;
extern INT16U fQCtrEnDelay2s;
extern INT32S dwIqRef;
extern INT16U wDebugRunInvChkStep;
extern INT16S wDebugTest1;
extern INT16S wDebugTest2;
extern INT16S wDebugTest3;
extern INT16S wDebugTest4;
extern INT16S wDebugTest5;
extern INT16S wDebugTest6;
extern INT32S dwRatePower;
extern INT16S wRateInvCurrPeak;
extern INT16S wRateGridLineVol;
extern INT16S wRateRmsCurr;
extern INT16S wInvCurrPeakMax;
extern INT16S wInvCurrResolving;
extern INT16S wRateInvCurrInv;
extern INT32S dwSoftStopIdLimMax;
extern INT16U fFVRTMaxCurrDelay2s;
extern INT16U fDC1And2En;
extern INT16U fDC3And4En;
extern INT16U fStartByDCOrAC;
extern INT16U fGridRelayCloseDeal;
extern INT16S wIdRefBak;
extern INT16S wIqRefBak;
extern INT16U fCharge;
extern INT16U uwDCSoftStep;
extern INT16U fDCCtrBusSoftSta;
extern INT16U fBattDarkStart;
extern INT16U fChargeToDisCharge;
extern INT16U fDisChargeToCharge;
extern INT16U fDCChargeToDisCharge;
extern INT16U fDCDisChargeToCharge;
extern INT32S dwIdLimitHigh;
extern INT32S dwIdLimitLow;
extern INT16U uwPFSetMax;
extern INT16S wDCCurrSet;
extern INT16S wDCCurrAim;
extern INT16S wDCCurrAimBak;
extern INT32S dwDCCurrSetIng;
extern INT16S wStdBusVolRef;
extern INT16S wInvMaxRmsCurr;
extern INT16U fGFCICTStatue;   
extern INT16U uwfGFCICTChkFinished;
extern INT16U uwfGFCICTChkOk;
extern INT16U uwGFCICTChkStep; 
extern INT32S dwActivePowerAim;
extern INT16U fCSVSSwitch;
extern INT16U fAdjustQEn;
extern INT16U fHotBackup;
extern INT16U fLocalPCSComdOnOff;
extern INT16U fLocalPCSConnect;
extern INT16U fFloatCharge;
extern INT16U fGridRelayCtr;
extern INT16U fDCRelayPCtr;
extern INT16U fDCRelayNCtr;
extern INT16U fDCSoftRelayCtr;
extern INT16U fAuxiRelayCtr;
extern INT16U fGridRelayCtrBak;
extern INT16U fDCRelayCtrBak;
extern INT16U fAuxiRelayCtrBak;
extern INT16U fInvEnOutBak;
extern INT16U uwAuxiRelayDelay;
extern INT16U uwDCSoftRelayDelay;
extern INT16U uwFVRTCurrSoftDelay;
extern INT32S dwregister0;
extern INT32S dwregister1;
extern INT32S dwregister2;
extern INT16S wregister0;
extern INT16S wregister1;
extern INT16S wRegistercheckFlag;
extern INT16S wBatChargeVoltLimt;
extern INT16S wBatDisChargeVoltLimt;
extern INT16S wBatChargeCurrLimt;
extern INT16S wBatDisChargeCurrLimt;
extern INT16S wBatFloatChargeVolt;
extern INT16S wBatSToFChargeCurr;
extern INT16U fACPowerCtr;
extern INT16U fDCOCPEnable;
extern INT16U fBMSActive;
//---------------------------------------------------------------------------
// Global symbols defined in source files
// State Input
#define fMCUActState                GpioDataRegs.GPADAT.bit.GPIO13               //1,OK           0,Fault 
#define fACOCPState                 GpioDataRegs.GPADAT.bit.GPIO15               //1,OCP          0,NO OCP
#define fWakeUp                     GpioDataRegs.GPADAT.bit.GPIO28               //NC
#define fFuseState                  GpioDataRegs.GPADAT.bit.GPIO29               //NC
#define fCtrPowerState              GpioDataRegs.GPADAT.bit.GPIO31               //1,OK           0,Fault
#define fIGBTHalt                   GpioDataRegs.GPBDAT.bit.GPIO34               //1,OK           0,Fault
#define fDCOCPState                 GpioDataRegs.GPBDAT.bit.GPIO37               //1,NO OCP       0,OCP
#define fCPLDShutInvPWM             GpioDataRegs.GPBDAT.bit.GPIO38               //1,No Shut      0,Shut
#define fCPLDShutDCPWM              GpioDataRegs.GPBDAT.bit.GPIO43               //1,No Shut      0,Shut
#define fArcingState                GpioDataRegs.GPCDAT.bit.GPIO66               //NC
#define fFanState                   GpioDataRegs.GPCDAT.bit.GPIO68               //1,OK           0,Fault
#define fDCRelayState               GpioDataRegs.GPCDAT.bit.GPIO70               //1,Close        0,Open
#define fSpdState                   GpioDataRegs.GPCDAT.bit.GPIO71               //1,OK           0,Fault
#define fBUSOVPState                GpioDataRegs.GPCDAT.bit.GPIO73               //1,OK           0,Fault
#define fPhyAddrState               GpioDataRegs.GPCDAT.bit.GPIO76 + GpioDataRegs.GPCDAT.bit.GPIO77 * 2 + GpioDataRegs.GPCDAT.bit.GPIO78 * 4 + GpioDataRegs.GPCDAT.bit.GPIO79 * 8;
//---------------------------------------------------------------------------
// Global symbols defined in source files
//-----------------DC Relay Ctr--------------------
#define sDCRelayPOpen()             {GpioDataRegs.GPCSET.bit.GPIO81 = 1;fDCRelayPCtr = 0;}
#define sDCRelayPClose()            {GpioDataRegs.GPCCLEAR.bit.GPIO81 = 1;fDCRelayPCtr = 1;}

#define sDCRelayNOpen()             {GpioDataRegs.GPCSET.bit.GPIO66 = 1;fDCRelayNCtr = 0;}
#define sDCRelayNClose()            {GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;fDCRelayNCtr = 1;}

#define sDCSoftRelayOpen()          {GpioDataRegs.GPCSET.bit.GPIO69 = 1;fDCSoftRelayCtr = 0;}
#define sDCSoftRelayClose()         {GpioDataRegs.GPCCLEAR.bit.GPIO69 = 1;fDCSoftRelayCtr = 1;}

//-----------------ISO Relay Ctr--------------------
#define sRelayPowerLPSOn()          {GpioDataRegs.GPCSET.bit.GPIO75 = 1;}
#define sRelayPowerLPSOff()         {GpioDataRegs.GPCCLEAR.bit.GPIO75 = 1;}

#define sIsoRelayOpen()             {GpioDataRegs.GPCCLEAR.bit.GPIO75 = 1;}
#define sIsoRelayClose()            {GpioDataRegs.GPCSET.bit.GPIO75 = 1;}

#define sPERelayOpen()              {GpioDataRegs.GPBSET.bit.GPIO48 = 1;}
#define sPERelayClose()             {GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;}

#define sGFCIRelayOpen()            {GpioDataRegs.GPBSET.bit.GPIO48 = 1;}
#define sGFCIRelayClose()           {GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;}

#define sEnArcBoardChk()            {GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;}
#define sDisArcBoardChk()           {GpioDataRegs.GPBSET.bit.GPIO48 = 1;}

//-----------------AC Relay Ctr--------------------
#define sAuxiRelayOpen()            {GpioDataRegs.GPASET.bit.GPIO14 = 1;fAuxiRelayCtr = 0;}
#define sAuxiRelayClose()           {GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;fAuxiRelayCtr = 1;}

#define sGridRelayOpen()            {GpioDataRegs.GPBSET.bit.GPIO39 = 1;fGridRelayCtr = 0;} 
#define sGridRelayClose()           {GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;fGridRelayCtr = 1;}

#define sInvRelayOpen()             {GpioDataRegs.GPBSET.bit.GPIO45 = 1;}
#define sInvRelayClose()            {GpioDataRegs.GPBCLEAR.bit.GPIO45 = 1;}

//----------------Power Relay Ctr------------------
#define sEnDCSps()                  {GpioDataRegs.GPASET.bit.GPIO16 = 1;GpioDataRegs.GPBCLEAR.bit.GPIO46 = 1;}
#define sDisDCSps()                 {GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;GpioDataRegs.GPBSET.bit.GPIO46 = 1;}

#define sEnInvSps()                 {GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;}
#define sDisInvSps()                {GpioDataRegs.GPBSET.bit.GPIO47 = 1;}

#define sACPowerEnable()            {GpioDataRegs.GPCSET.bit.GPIO72 = 1;fACPowerCtr = 1;}
#define sACPowerDisable()           {GpioDataRegs.GPCCLEAR.bit.GPIO72 = 1;fACPowerCtr = 0;}

//---------------------Other-----------------------
#define sRS485Send()                {GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;}
#define sRS485Recv()                {GpioDataRegs.GPCSET.bit.GPIO64 = 1;}

#define sInFanOn()                  {GpioDataRegs.GPCSET.bit.GPIO74 = 1;}
#define sInFanOff()                 {GpioDataRegs.GPCCLEAR.bit.GPIO74 = 1;} 

//---------------------CPLD Ctr---------------------
#define sDCOCPEnable()             {GpioDataRegs.GPBSET.bit.GPIO44 = 1;fDCOCPEnable = 1;}
#define sDCOCPDisable()            {GpioDataRegs.GPBCLEAR.bit.GPIO44 = 1;fDCOCPEnable = 0;}

#define sEnInvCpldOut()             {GpioDataRegs.GPCSET.bit.GPIO83 = 1;}
#define sDisInvCpldOut()            {GpioDataRegs.GPCCLEAR.bit.GPIO83 = 1;}

#define sEnBiDirDC3And4CpldOut()    {GpioDataRegs.GPBSET.bit.GPIO42 = 1; fDC3And4En = 1;}
#define sDisBiDirDC3And4CpldOut()   {GpioDataRegs.GPBCLEAR.bit.GPIO42 = 1; fDC3And4En = 0;}

#define sEnBiDirDC1And2CpldOut()    {GpioDataRegs.GPCSET.bit.GPIO82 = 1;fDC1And2En = 1;}
#define sDisBiDirDC1And2CpldOut()   {GpioDataRegs.GPCCLEAR.bit.GPIO82 = 1;fDC1And2En = 0;} 

#define sPWMTZClr()                 {GpioDataRegs.GPCCLEAR.bit.GPIO80 = 1;DelayUs(50);GpioDataRegs.GPCSET.bit.GPIO80 = 1;DelayUs(50);\
					                 EALLOW;EPwm1Regs.TZCLR.all = 7;EPwm2Regs.TZCLR.all = 7;EPwm3Regs.TZCLR.all = 7;\
					                 EPwm4Regs.TZCLR.all = 7;EPwm5Regs.TZCLR.all = 7;EDIS;}


//#define sPWMTZClr()                 {GpioDataRegs.GPCCLEAR.bit.GPIO80 = 1;DelayUs(50);GpioDataRegs.GPCSET.bit.GPIO80 = 1;DelayUs(50);\
//					                 EALLOW;EPwm1Regs.TZCLR.bit.OST = 1;EPwm2Regs.TZCLR.bit.OST = 1;EPwm3Regs.TZCLR.bit.OST = 1;
//					                 EPwm4Regs.TZCLR.bit.OST = 1;EPwm5Regs.TZCLR.bit.OST = 1;EDIS;}
					

#define SET    1
#define CLEAR  0
#define OK     1
#define FAIL   0
#define OPEN   0
#define CLOSE  1
#define HIGH   1
#define LOW    0

#define cSqrt2                 23170
#define cBusVolRefLow          8000
#define cBusVolRefHigh         13600
#define cPowerResolving        328 //2%     Q14
#define cDCCurrResolving       768 //3A 

#define sChipReset()  {DINT;EALLOW;SysCtrlRegs.WDCR = 0x0000;EDIS;}
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

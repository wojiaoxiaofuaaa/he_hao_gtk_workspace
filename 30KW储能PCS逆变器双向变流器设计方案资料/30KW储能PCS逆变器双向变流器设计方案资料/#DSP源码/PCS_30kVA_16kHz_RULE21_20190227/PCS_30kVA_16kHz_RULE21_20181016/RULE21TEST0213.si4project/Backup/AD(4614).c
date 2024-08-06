/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  AD.c 
Description:  AD中断和模拟量处理功能函数 
Version :  V1.0
Author:  Zhouxu	 
Date:2011-11-03
*********************************************************************/
#include "..\FunSrc\Funheader.h"

#define cBus5V              80
#define cBus200V            3200
#define cDivide3            5461 
#define cSqrt3divide3       9459             //Q14

#define cDCICurrFiltDen       1014            //8kHz
#define cDCICurrFiltNum       10

#define cGridOffsetFiltDen    32763          //8kHz  0.2Hz
#define cGridOffsetFiltNum    5

#define sSetNextADInt32kHz() {EPwm6Regs.ETPS.all = 0x0100;}
#define sSetNextADInt16kHz() {EPwm6Regs.ETPS.all = 0x0200;}

//------------------------------DC Sampling------------------------------------
INT16S wPOBusVol,wONBusVol,wIsoVol,wBatteryOutVol,wBatteryInVol;
INT16S wBatteryCurr,wBatteryCurr1,wBatteryCurr2,wBatteryCurr3,wBatteryCurr4;

INT16S wBusAvgVol,wPOBusAvgVol,wONBusAvgVol,wIsoAvgVol,wBatteryOutAvgVol,wBatteryInAvgVol;
INT16S wBatteryAvgCurr,wBatteryAvgCurr1,wBatteryAvgCurr2,wBatteryAvgCurr3,wBatteryAvgCurr4;

INT16U uwBatCurr1Offset,uwBatCurr2Offset,uwBatCurr3Offset,uwBatCurr4Offset;

INT32S dwPOBusVolSum,dwONBusVolSum,dwIsoVolSum,dwBatteryOutVolSum,dwBatteryInVolSum;
INT32S dwBatteryCurr1Sum,dwBatteryCurr2Sum,dwBatteryCurr3Sum,dwBatteryCurr4Sum;
INT32U udwBatCurr1OffsetSum,udwBatCurr2OffsetSum,udwBatCurr3OffsetSum,udwBatCurr4OffsetSum;

INT16U uwPOBusVolCnt,uwONBusVolCnt,uwIsoVolCnt,uwBatteryOutVolCnt,uwBatteryInVolCnt;
INT16U uwBatteryCurr1Cnt,uwBatteryCurr2Cnt,uwBatteryCurr3Cnt,uwBatteryCurr4Cnt;
INT16U uwBatteryCurr1HighCnt,uwBatteryCurr2HighCnt,uwBatteryCurr3HighCnt,uwBatteryCurr4HighCnt;
INT16U uwBatteryCurr1OKCnt,uwBatteryCurr2OKCnt,uwBatteryCurr3OKCnt,uwBatteryCurr4OKCnt;
INT16U uwBatCurr1OffsetSumCnt,uwBatCurr2OffsetSumCnt,uwBatCurr3OffsetSumCnt,uwBatCurr4OffsetSumCnt;

INT16U fPOBusVolClear,fONBusVolClear,fIsoVolClear,fBatteryOutVolClear,fBatteryInVolClear;
INT16U fBatteryCurr1Clear,fBatteryCurr2Clear,fBatteryCurr3Clear,fBatteryCurr4Clear;
INT16U fBatCurr1OffsetSumClear,fBatCurr2OffsetSumClear,fBatCurr3OffsetSumClear,fBatCurr4OffsetSumClear;
//------------------------------AC Sampling------------------------------------
T_THREE_PHASE_ABC twInvCurr,twInvCurrCalRms;
T_THREE_PHASE_ABC wDCIAvgCurr;
T_THREE_PHASE_ABC twInvPhaseVol;
T_THREE_PHASE_ABC twGridLineVol;
T_THREE_PHASE_ABC twGridPhaseVol;
T_THREE_PHASE_ABC twDCICurr;
T_THREE_PHASE_ABC twDCICurrFilt;
T_THREE_PHASE_ABC twInvRmsCurr;      
T_THREE_PHASE_ABC twGridLineRmsVol;
T_THREE_PHASE_ABC twInvPhaseRmsVol;
T_THREE_PHASE_ABC twInvPhaseAvgVol;
T_THREE_PHASE_ABC twInvRmsAvgCurr;      
T_THREE_PHASE_ABC twGridLineRmsAvgVol; 
T_THREE_PHASE_ABC twInvRmsFifoAvgCurr;     
T_THREE_PHASE_ABC twGridLineRmsFifoAvgVol; 
INT16S wGFCICurrRms,wGFCICurr,wGFCIAvgCurr;

INT16U uwRCurCntDebug,uwSCurCntDebug,uwTCurCntDebug;
INT16U uwRInvCurrOffset,uwSInvCurrOffset,uwTInvCurrOffset;
INT16U uwRSGridVolOffset,uwSTGridVolOffset,uwTRGridVolOffset;
INT16U uwRDCICurrOffset,uwSDCICurrOffset,uwTDCICurrOffset,uwGFCICurrOffset;
INT32U udwRSGridVolFiltoffset,udwSTGridVolFiltoffset,udwTRGridVolFiltoffset;
INT16S wRInvRmsAvgCurrArray[4],wSInvRmsAvgCurrArray[4],wTInvRmsAvgCurrArray[4];  
INT16S wRSGridRmsAvgVolArray[4],wSTGridRmsAvgVolArray[4],wTRGridRmsAvgVolArray[4]; 

INT32S dwRDCICurrSum,dwSDCICurrSum,dwTDCICurrSum;
INT32S dwRInvRmsCurrSum,dwSInvRmsCurrSum,dwTInvRmsCurrSum;
INT32S dwRSGridRmsVolSum,dwSTGridRmsVolSum,dwTRGridRmsVolSum;
INT32S dwRSInvRmsVolSum,dwSTInvRmsVolSum,dwTRInvRmsVolSum;
INT32S dwRInvRmsVolSum,dwSInvRmsVolSum,dwTInvRmsVolSum;
INT32U udwRInvCurrOffsetSum,udwSInvCurrOffsetSum,udwTInvCurrOffsetSum,udwGFCICurrOffsetSum;
INT32U udwRSGridVolOffsetSum,udwSTGridVolOffsetSum,udwTRGridVolOffsetSum;
INT32U udwRDCICurrOffsetSum,udwSDCICurrOffsetSum,udwTDCICurrOffsetSum;
INT32S dwRInvRmsPrdCurrSum,dwSInvRmsPrdCurrSum,dwTInvRmsPrdCurrSum;
INT32S dwRSGridRmsPrdVolSum,dwSTGridRmsPrdVolSum,dwTRGridRmsPrdVolSum; 
INT32S dwGFCICurrSum,dwGFCIAvgCurrSum;
INT32S dwRInvAvgVolSum,dwSInvAvgVolSum,dwTInvAvgVolSum;

INT16U uwRDCISumCnt,uwSDCISumCnt,uwTDCISumCnt;
INT16U uwRInvRmsCurrSumCnt,uwSInvRmsCurrSumCnt,uwTInvRmsCurrSumCnt;
INT16U uwRSGridRmsVolSumCnt,uwSTGridRmsVolSumCnt,uwTRGridRmsVolSumCnt;
INT16U uwRSInvRmsVolSumCnt,uwSTInvRmsVolSumCnt,uwTRInvRmsVolSumCnt;
INT16U uwRInvRmsVolSumCnt,uwSInvRmsVolSumCnt,uwTInvRmsVolSumCnt;
INT16U uwRInvCurrOffsetSumCnt,uwSInvCurrOffsetSumCnt,uwTInvCurrOffsetSumCnt,uwGFCICurrOffsetSumCnt;
INT16U uwRSGridVolOffsetSumCnt,uwSTGridVolOffsetSumCnt,uwTRGridVolOffsetSumCnt;
INT16U uwRDCICurrOffsetSumCnt,uwSDCICurrOffsetSumCnt,uwTDCICurrOffsetSumCnt;
INT16U uwRInvCurrOffsetHighCnt,uwSInvCurrOffsetHighCnt,uwTInvCurrOffsetHighCnt;
INT16U uwRDCICurrOffsetHighCnt,uwSDCICurrOffsetHighCnt,uwTDCICurrOffsetHighCnt;
INT16U uwRInvRmsPrdCurrSumCnt,uwSInvRmsPrdCurrSumCnt,uwTInvRmsPrdCurrSumCnt;
INT16U uwRSGridRmsPrdVolSumCnt,uwSTGridRmsPrdVolSumCnt,uwTRGridRmsPrdVolSumCnt;
INT16U uwRInvCurrOffsetADCnt,uwSInvCurrOffsetADCnt,uwTInvCurrOffsetADCnt;
INT16U uwRDCICurrOffsetADCnt,uwSDCICurrOffsetADCnt,uwTDCICurrOffsetADCnt;
INT16U uwGFCICurrSumCnt,uwGFCIAvgCurrSumCnt;
INT16U uwRInvAvgVolCnt,uwSInvAvgVolCnt,uwTInvAvgVolCnt;

INT16U fRDCISumClear,fSDCISumClear,fTDCISumClear;
INT16U fRInvRmsCurrSumClear,fSInvRmsCurrSumClear,fTInvRmsCurrSumClear;
INT16U fRSGridRmsVolSumClear,fSTGridRmsVolSumClear,fTRGridRmsVolSumClear;
INT16U fRSInvRmsVolSumClear,fSTInvRmsVolSumClear,fTRInvRmsVolSumClear;
INT16U fRInvRmsVolSumClear,fSInvRmsVolSumClear,fTInvRmsVolSumClear;
INT16U fRInvCurrOffsetSumClear,fSInvCurrOffsetSumClear,fTInvCurrOffsetSumClear,fGFCICurrOffsetSumClear;
INT16U fRSGridVolOffsetSumClear,fSTGridVolOffsetSumClear,fTRGridVolOffsetSumClear;
INT16U fRDCICurrOffsetSumClear,fSDCICurrOffsetSumClear,fTDCICurrOffsetSumClear;
INT16U fRInvRmsPrdCurrSumClear,fSInvRmsPrdCurrSumClear,fTInvRmsPrdCurrSumClear;                                                                 
INT16U fRSGridRmsPrdVolSumClear,fSTGridRmsPrdVolSumClear,fTRGridRmsPrdVolSumClear;
INT16U fGFCICurrSumClear,fGFCIAvgCurrSumClear;
INT16U fRInvAvgVolClear,fSInvAvgVolClear,fTInvAvgVolClear;
INT16S wRSGridVolCalRms,wSTGridVolCalRms,wTRGridVolCalRms;
//------------------------------Temperature Sampling---------------------------
INT16S wEnvTmp,wHeatTmp,wXFMRTmp;
INT16S wEnvTmpAvg,wHeatTmpAvg,wXFMRTmpAvg;
INT16S wEnvFitTemp,wHeatFitTemp,wXFMRFitTemp;

INT32S dwEnvTmpSum,dwHeatTmpSum,dwXFMRTmpSum;
INT16U uwEnvTmpSumCnt,uwHeatTmpSumCnt,uwXFMRTmpSumCnt;
INT16U fEnvTmpSumClear,fHeatTmpSumClear,fXFMRTmpSumClear;
//------------------------------Power Cal--------------------------------------
INT16U uwPowerEff,uwPowerEffAvg;
INT32S dwInstantaneousReactivePower,dwInstantaneousActivePower,dwDCPower;

INT32S dwInvActivePower[4],dwInvReactivePower[4];
INT32U udwApparentPower[4];
INT16S wPowerFactor[4];
INT16S wPowerFactorAvg[4];

INT32U udwPowerEffSum;
INT32S dwPowerFactorSum[4];
INT64S ddwInvActivePowerSum[4],ddwInvReactivePowerSum[4],ddwDCPowerSum;

INT16U uwInvActivePowerSumCnt,uwInvReactivePowerSumCnt,uwDCPowerSumCnt;
INT16U uwPowerEffSumCnt,uwPowerFactorSumCnt[4];

INT16U fInvActivePowerSumClear,fInvReactivePowerSumClear,fDCPowerSumClear;
INT16U fPowerEffSumClear,fPowerFactorSumClear[4];

//------------------------------FVRT-------------------------------------------
INT32U udwGridVolRSSqure[cHalfCycSampNum50Hz480V];
INT32U udwGridVolSTSqure[cHalfCycSampNum50Hz480V];
INT32U udwGridVolTRSqure[cHalfCycSampNum50Hz480V];
INT16U fLVRTEnable;
INT16U fLVRTValue;
INT16U fHVRTValue;
INT16U fGridChkValue;
INT16U fImbalanceLVRT;
INT16U fLVRTStartOrEnd;//1,跌落发生；0，跌落结束
INT16U fLVRTStartOrEndBak;//1,跌落发生；0，跌落结束
INT32U udwHalfCysRVolSqureSum;
INT32U udwHalfCysSVolSqureSum;
INT32U udwHalfCysTVolSqureSum;

INT16U uwHalfCycSampNum;
INT16U uwHalfCysRatedVolSqure;
INT32U udwHalfCysRatedVolSqure0R81;
INT32U udwHalfCysRatedVolSqure1R21;
INT32U udwHalfCysRatedVolSqure0R2;
INT32U udwHalfCysRatedVolSqure0R01;
INT16S wDebugCopy1[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
INT16S wDebugCopy2[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
INT16S wDebugCopy3[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//------------------------------Other------------------------------------------
T_CtrSwitch fADCtrSwitch;
INT16S wSTGridVolBak,wTRGridVolBak;
INT16U fStartOrNormal;  //=0启动自学习，=1正常工作模式
INT16U fInvEnOut,fBiDirDCEnOut;
INT16U fSyncAD;
bool CFCICALCFLAG=false;

//------------------------------Flash to Ram-----------------------------------
#pragma CODE_SECTION(ADCIsr, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalRmsSum, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalSignedSum, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalLVRTSumValue, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalLVRTValue, "AdcIsrRamFuncs")
#pragma CODE_SECTION(swGetRms, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalRmsValue, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalUnSignedSum, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sLineToPhase, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalInvActivePowerSum, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalInvReactivePowerSum, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sCalDCPowerSum, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sRmsClearZero, "AdcIsrRamFuncs")
#pragma CODE_SECTION(sInvShortPowerClearZero, "AdcIsrRamFuncs")
#pragma CODE_SECTION(swGetAvg, "AdcIsrRamFuncs")
//------------------------------Data Safety-----------------------------------
#pragma DATA_SECTION(wGFCICurrRms,"RAMSafetyADProt1"); 
#pragma DATA_SECTION(wGFCIAvgCurr,"RAMSafetyADProt2");
#pragma DATA_SECTION(wBusAvgVol,"RAMSafetyADProt3");
#pragma DATA_SECTION(fBiDirDCEnOut,"RAMSafetyADProt4");
#pragma DATA_SECTION(fInvEnOut,"RAMSafetyADProt5");
#pragma DATA_SECTION(fLVRTValue,"RAMSafetyADProt6");
#pragma DATA_SECTION(fHVRTValue,"RAMSafetyADProt7");
#pragma DATA_SECTION(twInvPhaseRmsVol,"RAMSafetyADProt8");
#pragma DATA_SECTION(twInvPhaseAvgVol,"RAMSafetyADProt9");
#pragma DATA_SECTION(twGridLineRmsVol,"RAMSafetyADProt10");
#pragma DATA_SECTION(wBatteryAvgCurr,"RAMSafetyADProt11");




//------------------------------Data Ram----------------------------------------
#pragma DATA_SECTION(fStartOrNormal,"RamSafetyProt");
#pragma DATA_SECTION(wGFCICurr,"RamSafetyProt");
 
#pragma DATA_SECTION(dwGFCICurrSum,"RamSafetyProt");
#pragma DATA_SECTION(dwGFCIAvgCurrSum,"RamSafetyProt");
#pragma DATA_SECTION(uwGFCICurrSumCnt,"RamSafetyProt");
#pragma DATA_SECTION(uwGFCIAvgCurrSumCnt,"RamSafetyProt");
#pragma DATA_SECTION(fGFCICurrSumClear,"RamSafetyProt");
#pragma DATA_SECTION(fGFCIAvgCurrSumClear,"RamSafetyProt");

#pragma DATA_SECTION(wIsoVol,"RamSafetyProt");
#pragma DATA_SECTION(wIsoAvgVol,"RamSafetyProt");
#pragma DATA_SECTION(dwIsoVolSum,"RamSafetyProt");
#pragma DATA_SECTION(uwIsoVolCnt,"RamSafetyProt");
#pragma DATA_SECTION(fIsoVolClear,"RamSafetyProt");

#pragma DATA_SECTION(wBatteryOutAvgVol,"RamSafetyProt");
#pragma DATA_SECTION(wBatteryInAvgVol,"RamSafetyProt");

#pragma DATA_SECTION(fADCtrSwitch,"RamSafetyProt");

extern void KickDog(void);
extern void DisableDog(void);
/*********************************************************************
Function name:  void InitAdcVar(void)
Description:Initialize AD variable
Calls:          
Called By:  InitVar(void)
Parameters:  void
Return:  void
 *********************************************************************/
void InitAdcVar(void)
{
	//------------------------------DC Sampling------------------------------------
    wPOBusVol = 0;
    wONBusVol = 0;
    wIsoVol = 0;
    wBatteryOutVol = 0;
    wBatteryInVol = 0;
    wBatteryCurr1 = 0;
    wBatteryCurr2 = 0;
    wBatteryCurr3 = 0;
    wBatteryCurr4 = 0;
    wBusAvgVol = 0;
    wPOBusAvgVol = 0;
    wONBusAvgVol = 0;
    wIsoAvgVol = 0;
    wBatteryOutAvgVol = 0;
    wBatteryInAvgVol = 0;
    wBatteryAvgCurr = 0;
    wBatteryAvgCurr1 = 0;
    wBatteryAvgCurr2 = 0;
    wBatteryAvgCurr3 = 0;
    wBatteryAvgCurr4 = 0;
    uwBatCurr1Offset = 0x800;
    uwBatCurr2Offset = 0x800;
    uwBatCurr3Offset = 0x800;
    uwBatCurr4Offset = 0x800;
    dwPOBusVolSum = 0;
    dwONBusVolSum = 0;
    dwIsoVolSum = 0;
    dwBatteryOutVolSum = 0;
    dwBatteryInVolSum = 0;
    dwBatteryCurr1Sum = 0;
    dwBatteryCurr2Sum = 0;
    dwBatteryCurr3Sum = 0;
    dwBatteryCurr4Sum = 0;
    udwBatCurr1OffsetSum = 0;
    udwBatCurr2OffsetSum = 0;
    udwBatCurr3OffsetSum = 0;
    udwBatCurr4OffsetSum = 0;
    uwPOBusVolCnt = 0;
    uwONBusVolCnt = 0;
    uwIsoVolCnt = 0;
    uwBatteryOutVolCnt = 0;
    uwBatteryInVolCnt = 0;
    uwBatteryCurr1Cnt = 0;
    uwBatteryCurr2Cnt = 0;
    uwBatteryCurr3Cnt = 0;
    uwBatteryCurr4Cnt = 0;
    uwBatteryCurr1HighCnt = 0;
    uwBatteryCurr2HighCnt = 0;
    uwBatteryCurr3HighCnt = 0;
    uwBatteryCurr4HighCnt = 0;
    uwBatteryCurr1OKCnt = 0;
    uwBatteryCurr2OKCnt = 0;
    uwBatteryCurr3OKCnt = 0;
    uwBatteryCurr4OKCnt = 0;
    uwBatCurr1OffsetSumCnt = 0;
    uwBatCurr2OffsetSumCnt = 0;
    uwBatCurr3OffsetSumCnt = 0;
    uwBatCurr4OffsetSumCnt = 0;    
    fPOBusVolClear = 0;
    fONBusVolClear = 0;
    fIsoVolClear = 0;
    fBatteryOutVolClear = 0;
    fBatteryInVolClear = 0;
    fBatteryCurr1Clear = 0;
    fBatteryCurr2Clear = 0;
    fBatteryCurr3Clear = 0;
    fBatteryCurr4Clear = 0;
    fBatCurr1OffsetSumClear = 0;
    fBatCurr1OffsetSumClear = 0;
    fBatCurr1OffsetSumClear = 0;
    fBatCurr1OffsetSumClear = 0;
    //------------------------------AC Sampling------------------------------------
    wRInvCurr = 0;        
    wSInvCurr = 0;       
    wTInvCurr = 0; 
    wRInvCurrCalRms = 0;
    wSInvCurrCalRms = 0;
    wTInvCurrCalRms = 0;
    wRInvVol = 0;  
    wSInvVol = 0;
    wTInvVol = 0;
    wRSGridVol = 0;       
    wSTGridVol = 0;       
    wTRGridVol = 0;
    wRSGridVolCalRms = 0;
    wSTGridVolCalRms = 0;
    wTRGridVolCalRms = 0;
    wRGridVol = 0;        
    wSGridVol = 0;        
    wTGridVol = 0; 
    wRDCICurr = 0;   
    wSDCICurr = 0;   
    wTDCICurr = 0;
    wRDCICurrFilt = 0;   
    wSDCICurrFilt = 0;   
    wTDCICurrFilt = 0; 
    wRInvRmsCurr = 0;     
    wSInvRmsCurr = 0;     
    wTInvRmsCurr = 0; 
    wRSGridRmsVol = 0;    
    wSTGridRmsVol = 0;    
    wTRGridRmsVol = 0; 
    wRSGridRmsAvgVol = 0;
    wSTGridRmsAvgVol = 0;
    wTRGridRmsAvgVol = 0;    
    wRInvRmsAvgCurr = 0;
    wSInvRmsAvgCurr = 0;
    wTInvRmsAvgCurr = 0;
    wRSGridRmsFifoAvgVol = 0;
    wSTGridRmsFifoAvgVol = 0;
    wTRGridRmsFifoAvgVol = 0;
    wRInvRmsFifoAvgCurr = 0;
    wSInvRmsFifoAvgCurr = 0;
    wTInvRmsFifoAvgCurr = 0;
    wRDCIAvgCurr = 0;
    wSDCIAvgCurr = 0;
    wTDCIAvgCurr = 0;
    wGFCICurr = 0;
    wGFCICurrRms = 0;
    uwRCurCntDebug = 0;
    uwSCurCntDebug = 0;
    uwTCurCntDebug = 0;
    uwRInvCurrOffset = 0x800;
    uwSInvCurrOffset = 0x800;
    uwTInvCurrOffset = 0x800;
    uwRSGridVolOffset = 0x800;
    uwSTGridVolOffset = 0x800;
    uwTRGridVolOffset = 0x800;
    uwRDCICurrOffset = 0x800;
    uwSDCICurrOffset = 0x800;
    uwTDCICurrOffset = 0x800;
    uwGFCICurrOffset = 0x800;  
    udwRSGridVolFiltoffset = 134217728;
    udwSTGridVolFiltoffset = 134217728;
    udwTRGridVolFiltoffset = 134217728;
    dwRDCICurrSum = 0;  
    dwSDCICurrSum = 0;
    dwTDCICurrSum = 0;
    dwRInvRmsCurrSum = 0;
    dwSInvRmsCurrSum = 0;
    dwTInvRmsCurrSum = 0; 
    dwRSGridRmsVolSum = 0;
    dwSTGridRmsVolSum = 0;
    dwTRGridRmsVolSum = 0;
    dwRSInvRmsVolSum = 0;
    dwSTInvRmsVolSum = 0;
    dwTRInvRmsVolSum = 0;
    dwRInvRmsVolSum = 0;
    dwSInvRmsVolSum = 0;
    dwTInvRmsVolSum = 0;
    dwRInvAvgVolSum = 0;
    dwSInvAvgVolSum = 0;
    dwTInvAvgVolSum = 0;
    udwRInvCurrOffsetSum = 0;
    udwSInvCurrOffsetSum = 0;
    udwTInvCurrOffsetSum = 0;
    udwGFCICurrOffsetSum = 0;
    udwRSGridVolOffsetSum = 0;
    udwSTGridVolOffsetSum = 0;
    udwTRGridVolOffsetSum = 0;
    udwRDCICurrOffsetSum = 0;
    udwSDCICurrOffsetSum = 0;
    udwTDCICurrOffsetSum = 0;
    dwRInvRmsPrdCurrSum = 0;
    dwSInvRmsPrdCurrSum = 0;
    dwTInvRmsPrdCurrSum = 0;
    dwRSGridRmsPrdVolSum = 0;
    dwSTGridRmsPrdVolSum = 0;
    dwTRGridRmsPrdVolSum = 0;
    dwGFCICurrSum = 0;
    dwGFCIAvgCurrSum = 0;
    uwRDCISumCnt = 0;
    uwSDCISumCnt = 0;   
    uwTDCISumCnt = 0; 
    uwRInvRmsCurrSumCnt = 0;
    uwSInvRmsCurrSumCnt = 0;
    uwTInvRmsCurrSumCnt = 0; 
    uwRSGridRmsVolSumCnt = 0;
    uwSTGridRmsVolSumCnt = 0;
    uwTRGridRmsVolSumCnt = 0;
    uwRSInvRmsVolSumCnt = 0;
    uwSTInvRmsVolSumCnt = 0;
    uwTRInvRmsVolSumCnt = 0; 
    uwRInvRmsVolSumCnt = 0;
    uwSInvRmsVolSumCnt = 0;
    uwTInvRmsVolSumCnt = 0;
    uwRInvAvgVolCnt = 0;
    uwSInvAvgVolCnt = 0;
    uwTInvAvgVolCnt = 0;
    uwRInvCurrOffsetSumCnt = 0;
    uwSInvCurrOffsetSumCnt = 0;
    uwTInvCurrOffsetSumCnt = 0;
    uwGFCICurrOffsetSumCnt = 0;
    uwRSGridVolOffsetSumCnt = 0;
    uwSTGridVolOffsetSumCnt = 0;
    uwTRGridVolOffsetSumCnt = 0;
    uwRDCICurrOffsetSumCnt = 0;
    uwSDCICurrOffsetSumCnt = 0;
    uwTDCICurrOffsetSumCnt = 0;
    uwRInvCurrOffsetHighCnt = 0;
    uwSInvCurrOffsetHighCnt = 0;
    uwTInvCurrOffsetHighCnt = 0;
    uwRDCICurrOffsetHighCnt = 0;
    uwSDCICurrOffsetHighCnt = 0;
    uwTDCICurrOffsetHighCnt = 0;
    uwRInvRmsPrdCurrSumCnt = 0;
    uwSInvRmsPrdCurrSumCnt = 0;
    uwTInvRmsPrdCurrSumCnt = 0; 
    uwRSGridRmsPrdVolSumCnt = 0;
    uwSTGridRmsPrdVolSumCnt = 0;
    uwTRGridRmsPrdVolSumCnt = 0; 
    uwRInvCurrOffsetADCnt = 0;
    uwSInvCurrOffsetADCnt = 0;
    uwTInvCurrOffsetADCnt = 0;
    uwRDCICurrOffsetADCnt = 0;
    uwSDCICurrOffsetADCnt = 0;
    uwTDCICurrOffsetADCnt = 0;
    uwGFCICurrSumCnt = 0;
    uwGFCIAvgCurrSumCnt = 0;    
    fRDCISumClear = 0;
    fSDCISumClear = 0;
    fTDCISumClear = 0;
    fRInvRmsCurrSumClear = 0;
    fSInvRmsCurrSumClear = 0;
    fTInvRmsCurrSumClear = 0;
    fRSGridRmsVolSumClear = 0;
    fSTGridRmsVolSumClear = 0;
    fTRGridRmsVolSumClear = 0;
    fRSInvRmsVolSumClear = 0;
    fSTInvRmsVolSumClear = 0;
    fTRInvRmsVolSumClear = 0;
    fRInvRmsVolSumClear = 0;
    fSInvRmsVolSumClear = 0;
    fTInvRmsVolSumClear = 0;
    fRInvAvgVolClear = 0;
    fSInvAvgVolClear = 0;
    fTInvAvgVolClear = 0;
    fRInvCurrOffsetSumClear = 0;
    fSInvCurrOffsetSumClear = 0;
    fTInvCurrOffsetSumClear = 0;
    fGFCICurrOffsetSumClear = 0;
    fRSGridVolOffsetSumClear = 0;
    fSTGridVolOffsetSumClear = 0;
    fTRGridVolOffsetSumClear = 0;
    fRDCICurrOffsetSumClear = 0;
    fSDCICurrOffsetSumClear = 0;
    fTDCICurrOffsetSumClear = 0;
    fRInvRmsPrdCurrSumClear = 0;
    fSInvRmsPrdCurrSumClear = 0;
    fTInvRmsPrdCurrSumClear = 0;
    fRSGridRmsPrdVolSumClear = 0;
    fSTGridRmsPrdVolSumClear = 0;
    fTRGridRmsPrdVolSumClear = 0;
    fGFCICurrSumClear = 0;
    fGFCIAvgCurrSumClear = 0;
    //------------------------------Temperature Sampling---------------------------
    wEnvTmp = 0;
    wHeatTmp = 0;
    wXFMRTmp = 0;
    
    wEnvTmpAvg = 0;
    wHeatTmpAvg = 0;
    wXFMRTmpAvg = 0;
	
	wEnvFitTemp = 0;
	wHeatFitTemp = 0;
	wXFMRFitTemp = 0;
    
    dwEnvTmpSum = 0;
	dwXFMRTmpSum = 0;
	dwHeatTmpSum = 0;
	
	uwEnvTmpSumCnt = 0;
	uwXFMRTmpSumCnt = 0;
	uwHeatTmpSumCnt = 0;
	
	fEnvTmpSumClear = 0;
	fXFMRTmpSumClear = 0;	
	fHeatTmpSumClear= 0;		
    //------------------------------Power Cal--------------------------------------
    uwPowerEff = 0;
    uwPowerEffAvg = 0; 
    dwDCPower = 0; 
    ddwDCPowerSum = 0;
    dwInstantaneousReactivePower = 0;
    dwInstantaneousActivePower = 0; 
    udwPowerEffSum = 0; 
    uwInvActivePowerSumCnt = 0;
	uwInvReactivePowerSumCnt = 0;
	uwDCPowerSumCnt = 0;
	uwPowerEffSumCnt = 0; 
	fInvActivePowerSumClear = 0;
	fInvReactivePowerSumClear = 0;
	fDCPowerSumClear = 0;
	fPowerEffSumClear = 0; 	
	{
    INT16U i;
        for(i=0;i<4;i++)
        {	
            dwInvActivePower[i] = 0;
            dwInvReactivePower[i] = 0;
            udwApparentPower[i] = 0;           
            ddwInvActivePowerSum[i] = 0;
            ddwInvReactivePowerSum[i] = 0;
            dwPowerFactorSum[i] = 0;
            wPowerFactorAvg[i] = 0;
            uwPowerFactorSumCnt[i] = 0;
            fPowerFactorSumClear[i] = 0;
            wPowerFactor[i] = 0;
            wRInvRmsAvgCurrArray[i] = 0;
            wSInvRmsAvgCurrArray[i] = 0;
            wTInvRmsAvgCurrArray[i] = 0; 
            wRSGridRmsAvgVolArray[i] = 0;  
            wSTGridRmsAvgVolArray[i] = 0;
            wTRGridRmsAvgVolArray[i] = 0;             
        }
    }
    //------------------------------FVRT-------------------------------------------
    {
    INT16U i;
	    for(i=0;i<cHalfCycSampNum50Hz480V;i++)
	    {
	        udwGridVolRSSqure[i] = 0;
            udwGridVolSTSqure[i] = 0;
            udwGridVolTRSqure[i] = 0;
	    }
    }
    fLVRTEnable = 0;
    fLVRTValue = 16384;
    fHVRTValue = 16384;
    fGridChkValue = 16384;
    fImbalanceLVRT = 0;
    fLVRTStartOrEnd = 0;
	fLVRTStartOrEndBak = 0;
    udwHalfCysRVolSqureSum = 0;
    udwHalfCysSVolSqureSum = 0;
    udwHalfCysTVolSqureSum = 0;    
    uwHalfCycSampNum = cHalfCycSampNum50Hz480V;
    uwHalfCysRatedVolSqure = cHalfCysRatedVolSqure50Hz480V;
    udwHalfCysRatedVolSqure0R81 = cHalfCysRatedVolSqure0R8150Hz480V;
    udwHalfCysRatedVolSqure1R21 = cHalfCysRatedVolSqure1R2150Hz480V;
    udwHalfCysRatedVolSqure0R2 = cHalfCysRatedVolSqure0R250Hz480V;
    udwHalfCysRatedVolSqure0R01 = cHalfCysRatedVolSqure0R0150Hz480V;
    //------------------------------Other------------------------------------------
    fADCtrSwitch = BiDirDCCtr1And3;
    wSTGridVolBak = 0;
    wTRGridVolBak = 0;
    fStartOrNormal = 0; 
    fInvEnOut = 0;
    fBiDirDCEnOut = 0;//0,disable; 1,Enable 1And2;  2,Enable 3And4;   3,Enable 1,2,3,4;
    fSyncAD = 0;
}

/*********************************************************************
Function name:  void InitAdc(void)
Description:  Initializes the ADC on the F280x
Calls:          
Called By:  main(void)
Parameters:  void
Return:  void
 *********************************************************************/
void InitAdc(void)
{
    AdcRegs.ADCTRL1.bit.RESET = 1;		// Reset the ADC module
    asm(" RPT #22 || NOP");				// Must for ADC reset to take effect

	//--- Call the ADC_cal() function located in the Boot ROM.
    //ADC_cal_func_ptr is a macro defined in the file example_nonBios.h or
    //example_BIOS.h (as may be the case for the example being used).  This
    //macro simply defines ADC_cal_func_ptr to be a function pointer to
    //the correct address in the boot ROM.
	(*ADC_cal_func_ptr)();

    AdcRegs.ADCREFSEL.bit.REF_SEL = 1;	// 0:internal, 1:external

    AdcRegs.ADCTRL3.all = 0x00E4;		// 25M E4
    DelayUs(5000);						// Wait 5ms before using the ADC
    
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;  

    //选择顺序B3/B2/B1/A4/A3/B7/A6/A7/B0/A0/A1/B6/B5/B4/A2/A5 
    AdcRegs.ADCMAXCONV.all = 0x000F;
    AdcRegs.ADCCHSELSEQ1.all = 0x49AB; 
    AdcRegs.ADCCHSELSEQ2.all = 0x76F3; 
    AdcRegs.ADCCHSELSEQ3.all = 0xE108; 
    AdcRegs.ADCCHSELSEQ4.all = 0x52CD;

    sADSelLow();

	//AdcRegs.ADCTRL1.all = 0x0730;
    AdcRegs.ADCTRL1.all = 0x0910; //0x0710

    AdcRegs.ADCTRL2.all = 0x0900;

    /*** Enable the ADC interrupt ***/
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;	// Enable ADCINT in PIE group 1
    IER |= M_INT1;						// Enable INT1 in IER to enable PIE group

} // end AdcInit()

/*********************************************************************
Function name:  INT16U suwGetAvg(volatile INT32U *pSum,volatile INT16U *pSumCnt,INT16U *pFlags)
Description:  计算无攀平均值，计算完毕置标?
Calls:          
Called By:  main(void)
Parameters:  volatile INT32U *pSum,volatile INT16U *pSumCnt,INT16U *pFlags
Return:  INT16U
 *********************************************************************/
INT16U suwGetAvg(volatile INT32U *pSum,volatile INT16U *pSumCnt,INT16U *pFlags)
{
INT32U udwSum=*pSum,i=0;
INT16U uwSumCnt = *pSumCnt;
    while((udwSum != *pSum) || (uwSumCnt != *pSumCnt))
    {
        udwSum = *pSum;
        uwSumCnt = *pSumCnt;
    }     
    *pFlags = 1;
    if (uwSumCnt != 0) i = udwSum/uwSumCnt;
    else i = 0;
    if(i > 0xffff) i = 0xffff; 
    return(i);   
}

/*********************************************************************
Function name:  INT16S swGetAvg(volatile INT32S *pSum,volatile INT16U *pSumCnt,INT16U *pFlags)
Description:  计算有符号数平均值，计算完毕置标志
Calls:          
Called By:  main(void)
Parameters:  volatile INT32S *pSum,volatile INT16U *pSumCnt,INT16U *pFlags
Return:  INT16S
 *********************************************************************/
INT16S swGetAvg(volatile INT32S *pSum,volatile INT16U *pSumCnt,INT16U *pFlags)
{
INT32S dwSum = *pSum,i;
INT16U uwSumCnt = *pSumCnt;
    while((dwSum != *pSum)||(uwSumCnt!=*pSumCnt))
	{
	    dwSum = *pSum;
        uwSumCnt = *pSumCnt;       
	}
	*pFlags = 1;
    if(uwSumCnt != 0) i = dwSum/uwSumCnt;
    else i = 0; 
	if(i > 0x7fff) i = 0x7fff; 
	return(i);   
}
/*********************************************************************
Function name:  INT16U swGetFifoAvg(INT16S wValue, INT16S *pArray, INT16S wArrayNum)
Description:  计算先入先出平均值
Calls:      	 
Called By:  main(void) 
Parameters:  swGetFifoAvg(INT16S wValue, INT16S *pArray, INT16S wArrayNum)
Return:  INT16S      
 *********************************************************************/
INT16S swGetFifoAvg(INT16S wValue, INT16S *pArray, INT16S wArrayNum)
{
INT16S i = 0;
INT32S dwSum = 0;
	if(wArrayNum < 1) 
		wArrayNum = 1;

    for(i=0;i<(wArrayNum-1);i++)
    {
        *(pArray + i) = *(pArray + i + 1);
        dwSum += *(pArray + i);    
    }
    *(pArray + i) = wValue;
    dwSum += *(pArray + i);
    return(dwSum/wArrayNum);    
}
/*********************************************************************
Function name:  void sCalUnSignedSum(INT32U *pSum,INT16U *pData,INT16U *pSumCnt,INT16U *pFlags)
Description:  计算无符号数累加和值，标志为1清除累加值和及计数值,然后标志清0
Calls:          
Called By:  ADCIsr(void)
Parameters:  INT32U *pSum,INT16U *pData,INT16U *pSumCnt,INT16U *pFlags
Return:  void
 *********************************************************************/
void sCalUnSignedSum(INT32U *pSum,INT16U *pData,INT16U *pSumCnt,INT16U *pFlags)
{
    if((*pFlags) != 0)
    {
        *pSum = 0;
        *pSumCnt = 0;
        *pFlags = 0;		
    }
    
    if(*pSumCnt < 0x7ff0) 
    {
        *pSum += *pData;
        (*pSumCnt)++;
    }
}

/*********************************************************************
Function name:  void sCalSignedSum(INT32S *pSum,INT16S *pData,INT16U *pSumCnt,INT16U *pFlags)
Description:  计算有符号数累加和值，标志为1清除累加值和及计数值,然后标志清0
Calls:          
Called By:  ADCIsr(void)
Parameters:  INT32S *pSum,INT16S *pData,INT16U *pSumCnt,INT16U *pFlags
Return:  void
 *********************************************************************/
void sCalSignedSum(INT32S *pSum,INT16S *pData,INT16U *pSumCnt,INT16U *pFlags)
{
    if((*pFlags) != 0)
	{
        *pSum = 0;
        *pSumCnt = 0;
        *pFlags = 0;
	}
    
    if(*pSumCnt < 0x7ff0) 
	{
	    *pSum += *pData;
	    (*pSumCnt)++;
    }
}

/*********************************************************************
Function name:  void sCalSPWMDuty(INT16S *pData,INT16S *pDataPre,INT16U *pCnt,INT16U *pDutyA,INT16U *pDutyB)
Description:  单相开环SPWM，调制波过零触发滤波
Calls:          
Called By:  ADCIsr(void)
Parameters:  INT16S *pData,INT16S *pDataPre,INT16U *pCnt,INT16U *pDutyA,INT16U *pDutyB
Return:  void
 *********************************************************************/
void sCalSPWMDuty(INT16S *pData,INT16S *pDataPre,INT16U *pCnt,INT16U *pDutyA,INT16U *pDutyB)
{
    if(((INT32S)(*pData)*(*pDataPre))<0)  //有变化沿
    {
        *pCnt++;
	    if((*pCnt) < 7)
	    {
	    if((*pDataPre) < 0) //前一次调制波为负的，在uwZeroDelayCnt未达到时，认为当前调制波为正的
		{
		    *pDutyA = abs(*pData);
			*pDutyB = 0;
		}
		else
		{
		    *pDutyA = 0;
			*pDutyB = abs(*pData);
		}
	    }
	    else 
	    {	    
	        if((*pData) < 0) 
	        {
		        *pDutyA = 0;
		        *pDutyB = abs(*pData);
	        }
	        else
	        {   
	            *pDutyA = abs(*pData);
	            *pDutyB = 0;
	        }
		    *pCnt = 0;
	        *pDataPre = *pData;
	    }
    }
    else
    {
        if((*pData) < 0) 
	    {
		    *pDutyA = 0;
		    *pDutyB = abs(*pData);
	    }
	    else
	    {
		    *pDutyA = abs(*pData);
	        *pDutyB = 0;
	    }
    	*pCnt = 0;
	    *pDataPre = *pData;
    }
}
/*********************************************************************
Function name:  void sCalRmsSum(INT32S *pRmsSum,INT16U *pData,INT16U *pRmsSumCnt,INT16U *pRmsFlag)
Description:  计算平方和累加和值
Calls:          
Called By:  ADCIsr(void)
Parameters:  INT32S *pRmsSum,INT16S *pData,INT16U *pRmsSumCnt,INT16U *pRmsFlag
Return:  void
 *********************************************************************/
void sCalRmsSum(INT32S *pRmsSum,INT16S *pData,INT16U *pRmsSumCnt,INT16U *pRmsFlag)
{
INT32S i;
    if((*pRmsFlag) != 0)
    {
        *pRmsSum = 0;
        *pRmsSumCnt = 0;
        *pRmsFlag = 0;		
    }
    
    i = ((INT32S)(*pData)*(*pData))>>4;
    if((*pRmsSum) < 0x7FFFFFFF - i) //防止溢出
    {
        *pRmsSum =*pRmsSum + i;
        (*pRmsSumCnt)++;
    }
}
/*********************************************************************
Function name:  void sCalInvActivePowerSum(INT64S *pVoltMulCurrSum,INT16S *pRVolt,INT16S *pSVolt,INT16S *pTVolt,INT16S *pRCurr, 
                                           INT16S *pSCurr,INT16S *pTCurr,INT16U *pVoltMulCurrSumCnt,INT16U *pVoltMulCurrSumFlag)
Calls:          
Called By:  ADCIsr(void)
Parameters:  INT32S *pRmsSum,INT16S *pData,INT16U *pRmsSumCnt,INT16U *pRmsFlag
Return:  void
 *********************************************************************/
void sCalInvActivePowerSum(INT64S *pVoltMulCurrSum,INT32S *pInstantaneousActivePower,INT16S *pRVolt,INT16S *pSVolt,INT16S *pTVolt,INT16S *pRCurr,
                           INT16S *pSCurr,INT16S *pTCurr,INT16U *pVoltMulCurrSumCnt,INT16U *pVoltMulCurrSumFlag)
{
INT32S dwPowerR,dwPowerS,dwPowerT;    
    if((*pVoltMulCurrSumFlag) != 0)
    {
        *pVoltMulCurrSum = 0;
        *(pVoltMulCurrSum+1) = 0;
        *(pVoltMulCurrSum+2) = 0;
        *(pVoltMulCurrSum+3) = 0;
        *pVoltMulCurrSumCnt = 0;
        *pVoltMulCurrSumFlag = 0;		
    }
    
    dwPowerR = (((INT32S)(*pRVolt))*(*pRCurr))>>8;
    dwPowerS = (((INT32S)(*pSVolt))*(*pSCurr))>>8;
    dwPowerT = (((INT32S)(*pTVolt))*(*pTCurr))>>8;
    
    if((*pVoltMulCurrSumCnt) < 65500)
    {
        *pVoltMulCurrSum = (*pVoltMulCurrSum) + dwPowerR;
        *(pVoltMulCurrSum+1) = (*(pVoltMulCurrSum+1)) + dwPowerS;
        *(pVoltMulCurrSum+2) = (*(pVoltMulCurrSum+2)) + dwPowerT;
        *(pVoltMulCurrSum+3) = (*(pVoltMulCurrSum+3)) + dwPowerR + dwPowerS + dwPowerT;
        
        (*pVoltMulCurrSumCnt)++;
    }
    *pInstantaneousActivePower = dwPowerR + dwPowerS + dwPowerT;
}
/*********************************************************************
Function name:  
Calls:          
Called By:  ADCIsr(void)
Parameters:  
Return:  void
 *********************************************************************/
void sCalInvReactivePowerSum(INT64S *pVoltMulCurrSum,INT32S *pInstantaneousReactivePower,INT16S *pRSVolt,INT16S *pSTVolt,INT16S *pTRVolt,
                             INT16S *pRCurr,INT16S *pSCurr,INT16S *pTCurr,INT16U *pVoltMulCurrSumCnt,INT16U *pVoltMulCurrSumFlag)
{
INT32S dwRSVoltdividesqrt3,dwSTVoltdividesqrt3,dwTRVoltdividesqrt3;
INT32S dwPowerR,dwPowerS,dwPowerT;
    if((*pVoltMulCurrSumFlag) != 0)
    {
        *pVoltMulCurrSum = 0;
        *(pVoltMulCurrSum+1) = 0;
        *(pVoltMulCurrSum+2) = 0;
        *(pVoltMulCurrSum+3) = 0;
        *pVoltMulCurrSumCnt = 0;
        *pVoltMulCurrSumFlag = 0;		
    }
    dwRSVoltdividesqrt3 = ((INT32S)(*pRSVolt))*cSqrt3divide3>>14;
    dwSTVoltdividesqrt3 = ((INT32S)(*pSTVolt))*cSqrt3divide3>>14;
    dwTRVoltdividesqrt3 = ((INT32S)(*pTRVolt))*cSqrt3divide3>>14;
    dwPowerR = dwSTVoltdividesqrt3*(*pRCurr)>>8;
    dwPowerS = dwTRVoltdividesqrt3*(*pSCurr)>>8;
    dwPowerT = dwRSVoltdividesqrt3*(*pTCurr)>>8;

    if((*pVoltMulCurrSumCnt) < 65500)
    {
        *pVoltMulCurrSum = (*pVoltMulCurrSum) + dwPowerR;
        *(pVoltMulCurrSum+1) = (*(pVoltMulCurrSum+1)) + dwPowerS;
        *(pVoltMulCurrSum+2) = (*(pVoltMulCurrSum+2)) + dwPowerT;
        *(pVoltMulCurrSum+3) = (*(pVoltMulCurrSum+3)) + dwPowerR + dwPowerS + dwPowerT;

        (*pVoltMulCurrSumCnt)++;
    }
    *pInstantaneousReactivePower = dwPowerR + dwPowerS + dwPowerT;

	
}         
/*********************************************************************
Function name:  void sCalLVRTSumValue(void)
Calls:          
Called By:  ADCIsr(void)
Parameters:  
Return:  void
 *********************************************************************/
void sCalLVRTSumValue(void)
{
static INT16U i = 0;
INT32S dwTmp;
INT16U j;
static INT16U fPhaseSequBakForFVRT = 0;

    dwTmp = ((INT32S)wRSGridVol * wRSGridVol)>>4;
    udwHalfCysRVolSqureSum = udwHalfCysRVolSqureSum - udwGridVolRSSqure[i] + dwTmp;
    udwGridVolRSSqure[i] = dwTmp;
    dwTmp = ((INT32S)wSTGridVol * wSTGridVol)>>4;
    udwHalfCysSVolSqureSum = udwHalfCysSVolSqureSum - udwGridVolSTSqure[i] + dwTmp;
    udwGridVolSTSqure[i] = dwTmp;
    dwTmp = ((INT32S)wTRGridVol * wTRGridVol)>>4;
    udwHalfCysTVolSqureSum = udwHalfCysTVolSqureSum - udwGridVolTRSqure[i] + dwTmp;
    udwGridVolTRSqure[i] = dwTmp;
    i++;
    
    if((fSysFreChange == 1)||(fPhaseSequBakForFVRT != fPhaseSequ))
    {
    	i = uwHalfCycSampNum + 1;
    }
    
    fPhaseSequBakForFVRT = fPhaseSequ;
    
    if(i == uwHalfCycSampNum)
    {
        i = 0;
        fLVRTEnable = 1;
    }
    else if(i > uwHalfCycSampNum)
    {
        udwHalfCysRVolSqureSum = 0;
        udwHalfCysSVolSqureSum = 0;
        udwHalfCysTVolSqureSum = 0;
        fLVRTValue = 16384;
        fHVRTValue = 16384;
        fGridChkValue = 16384;
        fImbalanceLVRT = 0;
        i = 0;
        fLVRTEnable = 0;
        for(j=0;j<cHalfCycSampNum50Hz480V;j++)   
        {                            
            udwGridVolRSSqure[j] = 0;
            udwGridVolSTSqure[j] = 0;
            udwGridVolTRSqure[j] = 0;
        }                                
    }
}
/*********************************************************************
Function name:  void sCalLVRTValue(void)
Calls:          
Called By:  ADCIsr(void)
Parameters:  
Return:  void
 *********************************************************************/
void sCalLVRTValue(void)
{
INT32U udwtemp;
INT64U uddwMinTemp,uddwMaxTemp;
INT32U udwMaxTemp,udwMinTemp;
    if(fLVRTEnable == 1)
    {
        udwMinTemp = udwHalfCysRVolSqureSum;
        if(udwMinTemp > udwHalfCysSVolSqureSum) 
        {
            udwMinTemp = udwHalfCysSVolSqureSum;
        }
        if(udwMinTemp > udwHalfCysTVolSqureSum)
        {
            udwMinTemp = udwHalfCysTVolSqureSum;
        }
        
        udwMaxTemp = udwHalfCysRVolSqureSum;
        if(udwMaxTemp < udwHalfCysSVolSqureSum) 
        {
            udwMaxTemp = udwHalfCysSVolSqureSum;
        }
        if(udwMaxTemp < udwHalfCysTVolSqureSum)
        {
            udwMaxTemp = udwHalfCysTVolSqureSum;
        }
                
        if((udwMinTemp > udwHalfCysRatedVolSqure0R81)&&(udwMaxTemp < udwHalfCysRatedVolSqure1R21))
        {
            fLVRTValue = 16384;
            fHVRTValue = 16384;
            fGridChkValue = 16384;
            fImbalanceLVRT = 0;
        }
        else if(udwMinTemp <= udwHalfCysRatedVolSqure0R81)//LVRT priotity
        {
            uddwMinTemp = (INT64U)udwMinTemp<<14;
            udwtemp = uddwMinTemp/uwHalfCysRatedVolSqure;//Q28
            fGridChkValue = sqrt(udwtemp);//Q14
                
            if(wEepromLVRTOption == 1)
            {                
                fLVRTValue = fGridChkValue;//sqrt(udwtemp);//Q14
                fHVRTValue = 16384;
                if((fLVRTStartOrEnd == 0)||(fImbalanceLVRT == 1))
                {
                    udwtemp = udwHalfCysRatedVolSqure0R01 + (((INT64U)udwHalfCysRatedVolSqure0R2 * fLVRTValue)>>14);//(fLVRTValue+0.1)^2 - fLVRTValue^2  Q15
                    
                    if((udwMaxTemp - udwMinTemp) > udwtemp)//跌落幅值的最大和最小相差超过10%，认为是不平衡跌落
                    {
                    	fImbalanceLVRT = 1;
                    }
                    else
                    {
                    	fImbalanceLVRT = 0;
                    }
                }
            }
            else
            {
                fLVRTValue = 16384;
                fImbalanceLVRT = 0;
            }
        }
        else if(udwMaxTemp >= udwHalfCysRatedVolSqure1R21)
        {
        	uddwMaxTemp = (INT64U)udwMaxTemp<<14; 
        	udwtemp = uddwMaxTemp/uwHalfCysRatedVolSqure;//Q28
        	fGridChkValue = sqrt(udwtemp);
        	
        	if(wEepromHVRTOption == 1)
        	{
        	    fHVRTValue = fGridChkValue;//sqrt(udwtemp);//Q14 
                fLVRTValue = 16384;
                if((fLVRTStartOrEnd == 0)||(fImbalanceLVRT == 1))
                {
                    udwtemp = (((INT64U)udwHalfCysRatedVolSqure0R2 * fHVRTValue)>>14) - udwHalfCysRatedVolSqure0R01;//fHVRTValue^2 - (fHVRTValue-0.1)^2  Q15
                    
                    if((udwMaxTemp - udwMinTemp) > udwtemp) // Max dec Min over 10%
                    {
                    	fImbalanceLVRT = 1;
                    }
                    else
                    {
                    	fImbalanceLVRT = 0;
                    }
                }
            }
            else
            {
                fHVRTValue = 16384;
                fImbalanceLVRT = 0;
            }
        }
        else
        {
        	fLVRTValue = 16384;
		    fHVRTValue = 16384;
		    fGridChkValue = 16384;
            fImbalanceLVRT = 0;
        }
    }
    else
    {
        fLVRTValue = 16384;
		fHVRTValue = 16384;
		fGridChkValue = 16384;
        fImbalanceLVRT = 0;
    }
}    
/*********************************************************************
Function name:  INT16U suwGetRms(INT32U *pRmsSum,INT16U *pRmsSumCnt,INT16U *pRmsFlag)
Description:  读取平方和及累加次数，并对两者商开根号
Calls:      	 
Called By:  sCalRmsVol(void) 
Parameters:  INT32U *pRmsSum,INT16U *pRmsSumCnt,INT16U *pRmsFlag
Return:  INT16U      
 *********************************************************************/
INT16S swGetRms(volatile INT32S *pRmsSum,volatile INT16U *pRmsSumCnt,INT16U *pRmsFlag)
{
INT32S udwRmsSum = *pRmsSum,i;
INT16U uwRmsSumCnt = *pRmsSumCnt;
    while((udwRmsSum != *pRmsSum)||(uwRmsSumCnt!= *pRmsSumCnt))
	{
	    udwRmsSum = *pRmsSum;
        uwRmsSumCnt = *pRmsSumCnt;       
	}
	*pRmsFlag = 1;
    if(uwRmsSumCnt != 0) 
		i = sqrt(udwRmsSum / uwRmsSumCnt);
    else 
		i = 0;

	i = i<<2;
	if(i > 0x7fff) i = 0x7fff; 
	return(i);	
}
/*********************************************************************
Function name:  INT16U sRmsClearZero(void)
Description:  无过零中断有效值清零
Calls:      	 
Called By:  sCapDeal(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sRmsClearZero(void)  //5ms执行一次
{
	if(fRLstPhase == 1)
	{
		fRInvRmsCurrSumClear = 1;
		wRInvRmsCurr = 0;
		
		fRSGridRmsVolSumClear = 1;
		wRSGridRmsVol = 0;

		fRInvAvgVolClear = 0;
		wRInvAvgVol = 0;
		
		fRInvRmsVolSumClear = 1;
		wRInvRmsVol = 0;
		
		fRInvAvgVolClear = 0;
		
		fTRGridRmsVolSumClear = 1;
		wTRGridRmsVol = 0;
		
		fRSGridRmsPrdVolSumClear = 1;
		wTRGridRmsAvgVol = 0;
		wRSGridRmsAvgVol = 0;
		dwTRGridRmsPrdVolSum = 0;
		dwRSGridRmsPrdVolSum = 0;
		
        fRInvRmsPrdCurrSumClear = 1;
		wRInvRmsAvgCurr = 0;
		dwRInvRmsPrdCurrSum = 0;
		
		fRSGridVolOffsetSumClear = 1;
		fTRGridVolOffsetSumClear = 1;	
		fSTGridVolOffsetSumClear = 1;	
	}
	
	if(fSLstPhase == 1)
	{
		fSInvRmsCurrSumClear = 1;
		wSInvRmsCurr = 0;

		fRSGridRmsVolSumClear = 1;
		wRSGridRmsVol = 0;
		
		fSInvRmsVolSumClear = 1;
		wSInvRmsVol = 0;

		fSInvAvgVolClear = 0;
		wSInvAvgVol = 0;
		
		fSTGridRmsVolSumClear = 1;
		wSTGridRmsVol = 0;

		fSTGridRmsPrdVolSumClear = 1;
		wRSGridRmsAvgVol = 0;
		wSTGridRmsAvgVol = 0;
		dwRSGridRmsPrdVolSum = 0;
		dwSTGridRmsPrdVolSum = 0;
				
		fSInvRmsPrdCurrSumClear = 1;
		wSInvRmsAvgCurr = 0;
		dwSInvRmsPrdCurrSum = 0;
		
		fRSGridVolOffsetSumClear = 1;
		fTRGridVolOffsetSumClear = 1;	
		fSTGridVolOffsetSumClear = 1;
	}
	
	if(fTLstPhase == 1)
	{
		fTInvRmsCurrSumClear = 1;
		wTInvRmsCurr = 0;
		
		fSTGridRmsVolSumClear = 1;
		wSTGridRmsVol = 0;
		
		fTInvRmsVolSumClear = 1;
		wTInvRmsVol = 0;

		fTInvAvgVolClear = 0;
		wTInvAvgVol = 0;
		
		fTRGridRmsVolSumClear = 1;
		wTRGridRmsVol = 0;
		
		fTRGridRmsPrdVolSumClear = 1;
		wSTGridRmsAvgVol = 0;
		wTRGridRmsAvgVol = 0;
		dwSTGridRmsPrdVolSum = 0;
		dwTRGridRmsPrdVolSum = 0;
		
		fTInvRmsPrdCurrSumClear = 1;
		wTInvRmsAvgCurr = 0;
		dwTInvRmsPrdCurrSum = 0;
		
		fRSGridVolOffsetSumClear = 1;
		fTRGridVolOffsetSumClear = 1;	
		fSTGridVolOffsetSumClear = 1;	
	}
}
/*********************************************************************
Function name:  INT16U sRmsClearZero(void)
Description:  clear the variable and set the flag while the capture interrupt didn't occour 
Calls:      	 
Called By:  sCapDeal(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sInvShortPowerClearZero(void)
{
	fInvActivePowerSumClear = 1;
	dwInvActivePower[0] = 0;
	dwInvActivePower[1] = 0;
	dwInvActivePower[2] = 0;
	dwInvActivePower[3] = 0;
	udwApparentPower[0] = 0;
	udwApparentPower[1] = 0;
	udwApparentPower[2] = 0;
	udwApparentPower[3] = 0;
	fInvReactivePowerSumClear = 1;
	dwInvReactivePower[0] = 0;
	dwInvReactivePower[1] = 0;
	dwInvReactivePower[2] = 0;
	dwInvReactivePower[3] = 0;
}
/*********************************************************************
Function name:  INT16U sCalRmsValue(void)
Description:  计算有效值,捕获发生后才可能执行
Calls:      	 
Called By:  CAPIsr(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalRmsValue(void)  //10ms执行一次
{
	wRInvRmsCurr = swGetRms(&dwRInvRmsCurrSum,&uwRInvRmsCurrSumCnt,&fRInvRmsCurrSumClear);
    wSInvRmsCurr = swGetRms(&dwSInvRmsCurrSum,&uwSInvRmsCurrSumCnt,&fSInvRmsCurrSumClear);
    wTInvRmsCurr = swGetRms(&dwTInvRmsCurrSum,&uwTInvRmsCurrSumCnt,&fTInvRmsCurrSumClear);
    
    wRSGridRmsVol = swGetRms(&dwRSGridRmsVolSum,&uwRSGridRmsVolSumCnt,&fRSGridRmsVolSumClear);
    wSTGridRmsVol = swGetRms(&dwSTGridRmsVolSum,&uwSTGridRmsVolSumCnt,&fSTGridRmsVolSumClear);
    wTRGridRmsVol = swGetRms(&dwTRGridRmsVolSum,&uwTRGridRmsVolSumCnt,&fTRGridRmsVolSumClear);
    
    wRInvRmsVol = swGetRms(&dwRInvRmsVolSum,&uwRInvRmsVolSumCnt,&fRInvRmsVolSumClear);
    wSInvRmsVol = swGetRms(&dwSInvRmsVolSum,&uwSInvRmsVolSumCnt,&fSInvRmsVolSumClear);
    wTInvRmsVol = swGetRms(&dwTInvRmsVolSum,&uwTInvRmsVolSumCnt,&fTInvRmsVolSumClear);
    
    wGFCICurrRms = swGetRms(&dwGFCICurrSum,&uwGFCICurrSumCnt,&fGFCICurrSumClear);
    
    wRInvAvgVol = swGetAvg(&dwRInvAvgVolSum,&uwRInvAvgVolCnt,&fRInvAvgVolClear);
    wSInvAvgVol = swGetAvg(&dwSInvAvgVolSum,&uwSInvAvgVolCnt,&fSInvAvgVolClear);
    wTInvAvgVol = swGetAvg(&dwTInvAvgVolSum,&uwTInvAvgVolCnt,&fTInvAvgVolClear);
}
/*********************************************************************
Function name:void sCalInvActivePowerValue(volatile INT64S *pVoltMulCurrSum,volatile INT16U *pVoltMulCurrSumSumCnt,INT16U *pVoltMulCurrSumFlag,INT32U *pudwActivePower)
Description:  calculate the inverter's active power
Calls:      	 
Called By:  CAPIsr(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalInvActivePowerValue(volatile INT64S *pVoltMulCurrSum,volatile INT16U *pVoltMulCurrSumSumCnt,INT16U *pVoltMulCurrSumFlag,INT32S *pdwActivePower)
{
INT64S ddwPowerSum[4],i;
INT16U uwPowerSumCnt,j;

    for(j=0;j<4;j++)
    {
        ddwPowerSum[j] = *(pVoltMulCurrSum + j);
    }
    uwPowerSumCnt = *pVoltMulCurrSumSumCnt;

	while((ddwPowerSum[0] != *pVoltMulCurrSum) || (ddwPowerSum[1] != *(pVoltMulCurrSum+1)) || (ddwPowerSum[2] != *(pVoltMulCurrSum+2)) ||
	      (ddwPowerSum[3] != *(pVoltMulCurrSum+3)) || (uwPowerSumCnt != *pVoltMulCurrSumSumCnt))
	{
	    for(j=0;j<4;j++)
        {
            ddwPowerSum[j] = *(pVoltMulCurrSum + j);
        }
        uwPowerSumCnt = *pVoltMulCurrSumSumCnt;
	}
	
	*pVoltMulCurrSumFlag = 1;   

    for(j=0;j<4;j++)
    {
        if(uwPowerSumCnt != 0) i = ddwPowerSum[j] / uwPowerSumCnt;
        else i = 0;
	    
        *(pdwActivePower+j) = (INT32S)i;
    }
}
/*********************************************************************
Function name:void sCalInvReactivePowerValue(volatile INT64S *pVoltMulCurrSum,volatile INT16U *pVoltMulCurrSumSumCnt,INT16U *pVoltMulCurrSumFlag,INT32U *pudwActivePower)
Description:  calculate the inverter's active power
Calls:      	 
Called By:  CAPIsr(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalInvReactivePowerValue(volatile INT64S *pVoltMulCurrSum,volatile INT16U *pVoltMulCurrSumSumCnt,INT16U *pVoltMulCurrSumFlag,INT32S *pdwReactivePower,INT16S *pRSGridRmsVol,INT16S *pSTGridRmsVol,INT16S *pTRGridRmsVol)
{
INT64S ddwPowerSum[4],i;
INT16U uwPowerSumCnt,j;
INT16S wGridRmsVolSum,wZAtmp;
INT32S dwCapZ;
INT32S Para_Uabc=297,Para_Uabc_Sum=891;
    wGridRmsVolSum = *pRSGridRmsVol + *pSTGridRmsVol + *pTRGridRmsVol;

    for(j=0;j<4;j++)
    {
        ddwPowerSum[j] = *(pVoltMulCurrSum + j);
    }
    uwPowerSumCnt = *pVoltMulCurrSumSumCnt;

	while((ddwPowerSum[0] != *pVoltMulCurrSum) || (ddwPowerSum[1] != *(pVoltMulCurrSum+1)) || (ddwPowerSum[2] != *(pVoltMulCurrSum+2)) ||
	      (ddwPowerSum[3] != *(pVoltMulCurrSum+3)) || (uwPowerSumCnt != *pVoltMulCurrSumSumCnt))
	{
	    for(j=0;j<4;j++)
        {
            ddwPowerSum[j] = *(pVoltMulCurrSum + j);
        }
        uwPowerSumCnt = *pVoltMulCurrSumSumCnt;
	}
	
	*pVoltMulCurrSumFlag = 1;   
	
	wZAtmp = ((INT32S)uwRFreq * 20971)>>16;//Freq pu = 1/256/50*4096*65536
	if(wZAtmp > 6000)
	{
		wZAtmp = 6000;
	}
    else if(wZAtmp < 2500)
    {
    	wZAtmp = 2500;
    }
    
    if(fInvEnOut == 0)//未进行逆变时,不进行补偿
    {
    	wZAtmp = 0;
    }
	
    
	//wDebugTest5=ParaU16_Online;//debug	172
    /*if(dwInvActivePower[3]>0)
    {
		Para_Uabc=297; //309
		Para_Uabc_Sum=891;//927   
	}
	else
	{
		Para_Uabc=297; //297
		Para_Uabc_Sum=891; //891
	}*/
	
    Para_Uabc=297; //297
	Para_Uabc_Sum=891; //891
	
    for(j=0;j<4;j++)
    {
        if(uwPowerSumCnt != 0) 
        {
			if(j ==3 )
            {
            	dwCapZ = ((INT32S)Para_Uabc_Sum* wZAtmp)>>12;//897 3Uab * 3Uab = 27Ua^2, 2*pi*50*300/1000000/27*3 Q20 PCS = 25uF  915   24.1 882
                i = (ddwPowerSum[j] / uwPowerSumCnt) + ((((INT64S)wGridRmsVolSum * wGridRmsVolSum)* dwCapZ)>>24);
            }
            else
            {
            	dwCapZ = ((INT32S)Para_Uabc * wZAtmp)>>12;//299
                i = (ddwPowerSum[j] / uwPowerSumCnt) + ((((INT64S)wGridRmsVolSum * wGridRmsVolSum) * dwCapZ)>>24);
            }
        }
        else 
        {
            i = 0;
        }
        
        *(pdwReactivePower+j) = i;
    }
}
/*********************************************************************
Function name:  void sCalInvApparentPowerValue(INT16S *pRmsVoltA,INT16S *pRmsVoltB,INT16S *pRmsVoltC,INT16S *pRmsCurrA,INT16S *pRmsCurrB,INT16S *pRmsCurrC,INT32U *pdwApparentPower)
Description:  calculate the inverter's active power
Calls:      	 
Called By:  CAPIsr(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalInvApparentPowerValue(INT32S *pActivePower,INT32S *pReactivePower,INT32U *pApparentPower)
{
INT32U udwApparentPower;
INT64U uddwReactivePowerSqu,uddwActivePowerSqu;
INT16U i;
     for(i=0;i<3;i++)
     {
         uddwActivePowerSqu = (INT64U)((INT64S)(*(pActivePower+i)) * (*(pActivePower+i)));
         uddwReactivePowerSqu = (INT64U)((INT64S)(*(pReactivePower+i)) * (*(pReactivePower+i))); 
         udwApparentPower = sqrt(uddwActivePowerSqu + uddwReactivePowerSqu);        
         *(pApparentPower+i) = udwApparentPower;
	 }	 
	 *(pApparentPower+3) = (*pApparentPower) + (*(pApparentPower+1)) + (*(pApparentPower+2));
}
/*********************************************************************
Function name:void sCalDCPowerSum(INT64S *pDCPowerSum,INT16S wDCVol,INT16S wDCCurr,INT16U *pDCPowerSumCnt,INT16U *pDCPowerSumFlag)
Description:  calculate the inverter's active power
Calls:      	 
Called By:  CAPIsr(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalDCPowerSum(INT64S *pDCPowerSum,INT16S wDCVol,INT16S wDCCurr,INT16U *pDCPowerSumCnt,INT16U *pDCPowerSumFlag)
{
INT32S dwDCPowerTmp;
	if((*pDCPowerSumFlag) != 0)
    {
        *pDCPowerSum = 0;
        *pDCPowerSumCnt = 0;
        *pDCPowerSumFlag = 0;		
    }
    
    dwDCPowerTmp = ((INT32S)wDCVol * wDCCurr)>>8;
    
    if((*pDCPowerSumCnt) < 65500)
    {
        *pDCPowerSum = (*pDCPowerSum) + dwDCPowerTmp;   
        (*pDCPowerSumCnt)++;   
    }
}
/*********************************************************************
Function name:void void sCalDCPowerValue(volatile INT64S *pDCPowerSum,volatile INT16U *pDCPowerSumSumCnt,INT16U *pDCPowerFlag,INT32U *pDCPower)
Description:  calculate the inverter's active power
Calls:      	 
Called By:  CAPIsr(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalDCPowerValue(volatile INT64S *pDCPowerSum,volatile INT16U *pDCPowerSumSumCnt,INT16U *pDCPowerFlag,INT32S *pDCPower)
{
INT64S ddwPowerSum,i;
INT16U uwPowerSumCnt;

    ddwPowerSum = *pDCPowerSum;
    uwPowerSumCnt = *pDCPowerSumSumCnt;

	while((ddwPowerSum != *pDCPowerSum) || (uwPowerSumCnt != *pDCPowerSumSumCnt))
	{
	    ddwPowerSum = *pDCPowerSum;
        uwPowerSumCnt = *pDCPowerSumSumCnt;
	}
	
	*pDCPowerFlag = 1;   

    if(uwPowerSumCnt != 0) i = ddwPowerSum / uwPowerSumCnt;
    else i = 0;
    	
    *pDCPower = (INT32S)i;
}
/*********************************************************************
Function name:  void sPowerAdjust(INT32U *pInvActivePower,INT32S *pInvReactivePower)
Description:  
Calls:      	 
Called By:  main(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sPowerAdjust(INT32S *pInvActivePower,INT32S *pInvReactivePower)
{
INT16U j;
INT32S wPTmp,wQTmp; 
    for(j=0;j<4;j++)
    {
    	wPTmp = *(pInvActivePower+j);
    	wQTmp = *(pInvReactivePower+j);
    	/*if(wPTmp < 0)
    	{
			*(pInvReactivePower+j) = (((INT64S)wQTmp * 32767) + ((INT64S)wPTmp * 172))>>15; //172 实验校准系数0.25度	
		    *(pInvActivePower+j) = (((INT64S)wPTmp * 32767) - ((INT64S)wQTmp * 172))>>15; //实验校准系数0.25度
        }
        else
        {
			*(pInvReactivePower+j) = (((INT64S)wQTmp * 32767) -((INT64S)wPTmp * 172))>>15; //实验校准系数0.25度 28
   	   	    *(pInvActivePower+j) = (((INT64S)wPTmp * 32767) + ((INT64S)wQTmp * 172))>>15; //实验校准系数0.25度  28
        }*/
        *(pInvReactivePower+j) = (((INT64S)wQTmp * 32767) + ((INT64S)wPTmp * 172))>>15; //172 实验校准系数0.25度	
		*(pInvActivePower+j) = (((INT64S)wPTmp * 32767) - ((INT64S)wQTmp * 172))>>15; //实验校准系数0.25度
    }
}
/*********************************************************************
Function name:  INT16U sCalRmsPrdValueSum(void)
Description:  计算有效值,捕获发生后才可能执行
Calls:      	 
Called By:  sCapDeal(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalRmsPrdValueSum(void)
{
	sCalSignedSum(&dwRInvRmsPrdCurrSum,&wRInvRmsCurr,&uwRInvRmsPrdCurrSumCnt,&fRInvRmsPrdCurrSumClear);
	sCalSignedSum(&dwSInvRmsPrdCurrSum,&wSInvRmsCurr,&uwSInvRmsPrdCurrSumCnt,&fSInvRmsPrdCurrSumClear);
	sCalSignedSum(&dwTInvRmsPrdCurrSum,&wTInvRmsCurr,&uwTInvRmsPrdCurrSumCnt,&fTInvRmsPrdCurrSumClear);
	
	sCalSignedSum(&dwRSGridRmsPrdVolSum,&wRSGridRmsVol,&uwRSGridRmsPrdVolSumCnt,&fRSGridRmsPrdVolSumClear);
	sCalSignedSum(&dwSTGridRmsPrdVolSum,&wSTGridRmsVol,&uwSTGridRmsPrdVolSumCnt,&fSTGridRmsPrdVolSumClear);
	sCalSignedSum(&dwTRGridRmsPrdVolSum,&wTRGridRmsVol,&uwTRGridRmsPrdVolSumCnt,&fTRGridRmsPrdVolSumClear);	
}
/*********************************************************************
Function name:  INT16U sCalRmsAvgValue(void)
Description:  计算有效值平均值
Calls:      	 
Called By:  main(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalRmsAvgValue(void)
{
    wRInvRmsAvgCurr = swGetAvg(&dwRInvRmsPrdCurrSum,&uwRInvRmsPrdCurrSumCnt,&fRInvRmsPrdCurrSumClear);
    wSInvRmsAvgCurr = swGetAvg(&dwSInvRmsPrdCurrSum,&uwSInvRmsPrdCurrSumCnt,&fSInvRmsPrdCurrSumClear);       
    wTInvRmsAvgCurr = swGetAvg(&dwTInvRmsPrdCurrSum,&uwTInvRmsPrdCurrSumCnt,&fTInvRmsPrdCurrSumClear);            
                                                                                                                      
    wRSGridRmsAvgVol = swGetAvg(&dwRSGridRmsPrdVolSum,&uwRSGridRmsPrdVolSumCnt,&fRSGridRmsPrdVolSumClear);   
    wSTGridRmsAvgVol = swGetAvg(&dwSTGridRmsPrdVolSum,&uwSTGridRmsPrdVolSumCnt,&fSTGridRmsPrdVolSumClear);   
    wTRGridRmsAvgVol = swGetAvg(&dwTRGridRmsPrdVolSum,&uwTRGridRmsPrdVolSumCnt,&fTRGridRmsPrdVolSumClear);	  
}
/*********************************************************************
Function name:  INT16U sCalRmsFifoAvgValue(void)
Description:  计算有效值平均值的滑动平均值
Calls:      	 
Called By:  main(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalRmsFifoAvgValue(void)
{
    wRInvRmsFifoAvgCurr = swGetFifoAvg(wRInvRmsAvgCurr,&wRInvRmsAvgCurrArray[0],4);
    wSInvRmsFifoAvgCurr = swGetFifoAvg(wSInvRmsAvgCurr,&wSInvRmsAvgCurrArray[0],4);       
    wTInvRmsFifoAvgCurr = swGetFifoAvg(wTInvRmsAvgCurr,&wTInvRmsAvgCurrArray[0],4);       
                                                                                                      
    wRSGridRmsFifoAvgVol = swGetFifoAvg(wRSGridRmsAvgVol,&wRSGridRmsAvgVolArray[0],4);    
    wSTGridRmsFifoAvgVol = swGetFifoAvg(wSTGridRmsAvgVol,&wSTGridRmsAvgVolArray[0],4); 
    wTRGridRmsFifoAvgVol = swGetFifoAvg(wTRGridRmsAvgVol,&wTRGridRmsAvgVolArray[0],4);   
}
/*********************************************************************
Function name:  void sCalLongAvg(void)
Description:  计算有效值平均值的滑动平均值
Calls:      	 
Called By:  main(void) 
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalLongAvg(void) 
{
INT16U i;
    for(i=0;i<4;i++)
    {
        sCalSignedSum(&dwPowerFactorSum[i],&wPowerFactor[i],&uwPowerFactorSumCnt[i],&fPowerFactorSumClear[i]);
    }
    sCalUnSignedSum(&udwPowerEffSum,&uwPowerEff,&uwPowerEffSumCnt,&fPowerEffSumClear);
}

/*********************************************************************
Function name:  sCalPowerAvg(INT32U *pPower,INT16U uwAvgTimeCnt)
Description:  
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalPowerAvg(INT32U *pPower,INT16U uwAvgTimeCnt,INT32U *pAvgPower)
{
static INT16U i=0;
static double uddwPowerSum=0;
    i++;
    uddwPowerSum += *pPower;
	if(uwAvgTimeCnt == 0) uwAvgTimeCnt = 1;
    if(i == uwAvgTimeCnt)
    {
        *pAvgPower = uddwPowerSum/uwAvgTimeCnt;
        uddwPowerSum = 0;
        i = 0;
    } 
}
/************************************************************************
Function name:  void sLineToPhase(T_THREE_PHASE_ABC *ptLineAbc, T_THREE_PHASE_ABC *ptPhaseAbc)
Description:  converter ABC line to ABC phase
Calls:        
Called By:   sADCIsr(void)    
Parameters:  T_THREE_PHASE_ABC, T_THREE_PHASE_ABC
Return:  void      
************************************************************************/
void sLineToPhase(T_THREE_PHASE_ABC *ptLineAbc, T_THREE_PHASE_ABC *ptPhaseAbc)
{    
    ptPhaseAbc->a = (INT16S)(((INT32S)((INT32S)ptLineAbc->a * 2 + ptLineAbc->b )*cDivide3)>>14);
    ptPhaseAbc->b = (INT16S)(((INT32S)((INT32S)ptLineAbc->b - ptLineAbc->a)*cDivide3)>>14);
    ptPhaseAbc->c = -(ptPhaseAbc->a + ptPhaseAbc->b);
    
    //ptPhaseAbc->a = (INT16S)(((INT32S)(ptLineAbc->b + 2*ptLineAbc->a)*cDivide3)>>14);
    //ptPhaseAbc->b = (INT16S)(((INT32S)(ptLineAbc->b - ptLineAbc->a)*cDivide3)>>14);
    //ptPhaseAbc->c = -(ptPhaseAbc->a + ptPhaseAbc->b);
}
/*********************************************************************
Function name:  void ADCIsr(void)
Description:  AD中断
Calls:sInvController()      	 
Called By:  
Parameters:  void
Return:  void      
 *********************************************************************/
void ADCIsr(void)
{ 
INT16U uwADData;
INT16S wADTmp = 0;
INT16U uwADTmp = 0;

static INT16U fInvEnOutReal = 0;
static INT16U fDCMode1EnOutReal = 0;
static INT16U fDCMode2EnOutReal = 0;
static INT16U fDCMode3EnOutReal = 0;

    KickDog();

    if(fStartOrNormal == 0)
    {
        if(EPwm1Regs.TBCTR >= (EPwm1Regs.TBPRD - 1000))//1500
        {
        	if(fADCtrSwitch == BiDirDCCtr1And3)
        	{
        		fSyncAD = 0;
        	}
        	else
        	{
        		fSyncAD = 1;       		
        	}
        }
        
        if(fSyncAD == 0)
        {
            if(fADCtrSwitch == BiDirDCCtr1And3)
            {        		
        		if(uwI2CBlockState.bit.fEpRdOnPwStep == 2)
                {
                	//=========================================逆变电流偏置============================================
	                uwADData = AdcRegs.ADCRESULT0>>4;
	                uwRInvCurrOffsetADCnt = uwADData;
	                sOffsetHighChk(&uwRInvCurrOffsetADCnt,&wEepromRInvCurrOffset,&uwRInvCurrOffsetHighCnt,&fRInvCurrOffsetErr,&fRInvCurrOffsetErrEvent); 
                    sCalUnSignedSum(&udwRInvCurrOffsetSum,&uwADData,&uwRInvCurrOffsetSumCnt,&fRInvCurrOffsetSumClear);
		            
		            uwADData = AdcRegs.ADCRESULT1>>4;
		            uwSInvCurrOffsetADCnt = uwADData;
		            sOffsetHighChk(&uwSInvCurrOffsetADCnt,&wEepromSInvCurrOffset,&uwSInvCurrOffsetHighCnt,&fSInvCurrOffsetErr,&fSInvCurrOffsetErrEvent);
                    sCalUnSignedSum(&udwSInvCurrOffsetSum,&uwADData,&uwSInvCurrOffsetSumCnt,&fSInvCurrOffsetSumClear);
		            
		            uwADData = AdcRegs.ADCRESULT2>>4;
		            uwTInvCurrOffsetADCnt = uwADData;
		            sOffsetHighChk(&uwTInvCurrOffsetADCnt,&wEepromTInvCurrOffset,&uwTInvCurrOffsetHighCnt,&fTInvCurrOffsetErr,&fTInvCurrOffsetErrEvent);
                    sCalUnSignedSum(&udwTInvCurrOffsetSum,&uwADData,&uwTInvCurrOffsetSumCnt,&fTInvCurrOffsetSumClear);
                    //===========================================电池电流===============================================
                    uwADData = AdcRegs.ADCRESULT5>>4;
                    sCalUnSignedSum(&udwBatCurr1OffsetSum,&uwADData,&uwBatCurr1OffsetSumCnt,&fBatCurr1OffsetSumClear);
                    
                    uwADData = AdcRegs.ADCRESULT6>>4;
                    sCalUnSignedSum(&udwBatCurr3OffsetSum,&uwADData,&uwBatCurr3OffsetSumCnt,&fBatCurr3OffsetSumClear);
		            //===========================================DCI偏置===============================================
	                uwADData = AdcRegs.ADCRESULT11>>4;
	                uwRDCICurrOffsetADCnt = uwADData;
	                sDCIOffsetHighChk(&uwRDCICurrOffsetADCnt,&wEepromRDCICurrOffset,&uwRDCICurrOffsetHighCnt,&fRDCICurrOffsetErr,&fRDCICurrOffsetErrEvent);
                    sCalUnSignedSum(&udwRDCICurrOffsetSum,&uwADData,&uwRDCICurrOffsetSumCnt,&fRDCICurrOffsetSumClear);
	                
	                uwADData = AdcRegs.ADCRESULT12>>4;
	                uwSDCICurrOffsetADCnt = uwADData; 
	                sDCIOffsetHighChk(&uwSDCICurrOffsetADCnt,&wEepromSDCICurrOffset,&uwSDCICurrOffsetHighCnt,&fSDCICurrOffsetErr,&fSDCICurrOffsetErrEvent); 
                    sCalUnSignedSum(&udwSDCICurrOffsetSum,&uwADData,&uwSDCICurrOffsetSumCnt,&fSDCICurrOffsetSumClear);
                    
                    uwADData = AdcRegs.ADCRESULT13>>4;
                    uwTDCICurrOffsetADCnt = uwADData;
                    sDCIOffsetHighChk(&uwTDCICurrOffsetADCnt,&wEepromTDCICurrOffset,&uwTDCICurrOffsetHighCnt,&fTDCICurrOffsetErr,&fTDCICurrOffsetErrEvent);
                    sCalUnSignedSum(&udwTDCICurrOffsetSum,&uwADData,&uwTDCICurrOffsetSumCnt,&fTDCICurrOffsetSumClear);                    
                }
                
                fADCtrSwitch = InvCtrAndPLL;
        		sSetNextADInt32kHz();
        		sADSelLow();
            }
            else if(fADCtrSwitch == BiDirDCCtr2And4)
            {
                //===========================================电池电流===============================================
                uwADData = AdcRegs.ADCRESULT3>>4;
                sCalUnSignedSum(&udwBatCurr2OffsetSum,&uwADData,&uwBatCurr2OffsetSumCnt,&fBatCurr2OffsetSumClear);
                
                uwADData = AdcRegs.ADCRESULT4>>4;
                sCalUnSignedSum(&udwBatCurr4OffsetSum,&uwADData,&uwBatCurr4OffsetSumCnt,&fBatCurr4OffsetSumClear);
                
                uwADData = AdcRegs.ADCRESULT5>>4;
                sCalUnSignedSum(&udwGFCICurrOffsetSum,&uwADData,&uwGFCICurrOffsetSumCnt,&fGFCICurrOffsetSumClear);
                
            	fADCtrSwitch = BiDirDCCtr1And3;
        		sSetNextADInt32kHz();
        		sADSelHigh();
            }
            else if(fADCtrSwitch == InvCtrAndPLL)
            {
            	fADCtrSwitch = BiDirDCCtr2And4;
        		sSetNextADInt16kHz();
        		sADSelLow();
            }
            else
            {
            	
            }
        } 
        else
        {
            fSyncAD = 0;
            fADCtrSwitch = InvCtrAndPLL;
            sSetNextADInt32kHz();
            sADSelLow();
        }       
        
        sSciTxArray();	    
    }
    else
    {
    	if((fADCtrSwitch == BiDirDCCtr1And3)||(fADCtrSwitch == BiDirDCCtr2And4))//BiDirection DC Converter Control.
    	{
    	    if(fADCtrSwitch == BiDirDCCtr1And3)//GPIO07 = 1
    	    {
    	    	//******** 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    	    	//选择顺序B3/B2/B1/A4/A3/B7/A6/A7/B0/A0/A1/B6/B5/B4/A2/A5
    	    	wHeatTmp = (AdcRegs.ADCRESULT14>>4) - wEepromHeatOffset;
    	    	wEnvTmp = (AdcRegs.ADCRESULT15>>4) - wEepromEnvOffset; 
    	    	    		
    	    	uwADData = AdcRegs.ADCRESULT5>>4;
                wBatteryCurr1 = (((INT32U)uwADData - uwBatCurr1Offset)*wEepromBatteryCurr1Scale)>>12;//wEepromBatteryCurr1Offset
                //if(wBatteryCurr1 < 0)
                //{
                //    wBatteryCurr1 = ((INT32S)wBatteryCurr1 * 4833)>>12;
                //}
                
                uwADData = AdcRegs.ADCRESULT6>>4;
                wBatteryCurr3 = (((INT32U)uwADData - uwBatCurr3Offset)*wEepromBatteryCurr3Scale)>>12;//wEepromBatteryCurr3Offset
                //if(wBatteryCurr3 < 0)
                //{
                //    wBatteryCurr3 = ((INT32S)wBatteryCurr3 * 4833)>>12;
                //}
                
                uwADData = AdcRegs.ADCRESULT7>>4;
                wBatteryInVol = (((INT32U)uwADData - wEepromBatteryInVolOffset)*wEepromBatteryInVolScale)>>12;
                
                if(fPhaseSequ == 1)
	            {          
                    uwADData = AdcRegs.ADCRESULT11>>4;
                    wRDCICurr = (((INT32S)uwADData - uwRDCICurrOffset)*wEepromRDCICurrScale)>>11;//Q3                
		    		        
                    uwADData = AdcRegs.ADCRESULT12>>4;
                    wSDCICurr = (((INT32S)uwADData - uwSDCICurrOffset)*wEepromSDCICurrScale)>>11;
		    		        
                    uwADData = AdcRegs.ADCRESULT13>>4;
                    wTDCICurr = (((INT32S)uwADData - uwTDCICurrOffset)*wEepromTDCICurrScale)>>11;                                              
                }
                else if(fPhaseSequ == 2)
                {
                    uwADData = AdcRegs.ADCRESULT11>>4;	  
                    wRDCICurr = (((INT32S)uwADData - uwRDCICurrOffset)*wEepromRDCICurrScale)>>11;//Q3
            
		    		uwADData = AdcRegs.ADCRESULT12>>4;
                    wTDCICurr = (((INT32S)uwADData - uwSDCICurrOffset)*wEepromSDCICurrScale)>>11;
		    		        
                    uwADData = AdcRegs.ADCRESULT13>>4;
                    wSDCICurr = (((INT32S)uwADData - uwTDCICurrOffset)*wEepromTDCICurrScale)>>11;               
		        }
                else
                {
                    wRDCICurr = 0;
                	wSDCICurr = 0;
                	wTDCICurr = 0;
                	
                	//wRDCICurrFilt = 0;
                	//wSDCICurrFilt = 0; 
                	//wTDCICurrFilt = 0;            	
                }                 
                
                //wRDCICurrFilt = (INT16S)(((INT32S)wRDCICurrFilt * cDCICurrFiltDen + (INT32S)wRDCICurr * cDCICurrFiltNum)>>10);
                //wSDCICurrFilt = (INT16S)(((INT32S)wSDCICurrFilt * cDCICurrFiltDen + (INT32S)wSDCICurr * cDCICurrFiltNum)>>10);
                //wTDCICurrFilt = (INT16S)(((INT32S)wTDCICurrFilt * cDCICurrFiltDen + (INT32S)wTDCICurr * cDCICurrFiltNum)>>10);
    	    	
    	    	//------------------AD Switch-----------------
    	    	fADCtrSwitch = InvCtrAndPLL;
            	sSetNextADInt32kHz();
            	sADSelLow();
            	//------------------Cal Avg-------------------
            	sCalSignedSum(&dwEnvTmpSum,&wEnvTmp,&uwEnvTmpSumCnt,&fEnvTmpSumClear);
            	sCalSignedSum(&dwHeatTmpSum,&wHeatTmp,&uwHeatTmpSumCnt,&fHeatTmpSumClear);
            	sCalSignedSum(&dwBatteryCurr1Sum,&wBatteryCurr1,&uwBatteryCurr1Cnt,&fBatteryCurr1Clear); 
            	sCalSignedSum(&dwBatteryCurr3Sum,&wBatteryCurr3,&uwBatteryCurr3Cnt,&fBatteryCurr3Clear);
                sCalSignedSum(&dwBatteryInVolSum,&wBatteryInVol,&uwBatteryInVolCnt,&fBatteryInVolClear);
                sCalSignedSum(&dwRDCICurrSum,&wRDCICurr,&uwRDCISumCnt,&fRDCISumClear);
                sCalSignedSum(&dwSDCICurrSum,&wSDCICurr,&uwSDCISumCnt,&fSDCISumClear);
                sCalSignedSum(&dwTDCICurrSum,&wTDCICurr,&uwTDCISumCnt,&fTDCISumClear);
                
                sBusOVPChk(&fOVPErr,&fOVPErrEvent);
                sBatteryCurrHighChk(wBatteryCurr1,&uwBatteryCurr1HighCnt,&uwBatteryCurr1OKCnt,&fBatteryCurr1Err,&fBatteryCurr1ErrEvent);
                sBatteryCurrHighChk(wBatteryCurr3,&uwBatteryCurr3HighCnt,&uwBatteryCurr3OKCnt,&fBatteryCurr3Err,&fBatteryCurr3ErrEvent);
            
                sBiDirDCCon1And3Controller(wBatteryInVol,wBatteryCurr1,wBatteryCurr3,fBiDirDCEnOut);           	
    	    }
    	    else// if(fADCtrSwitch == BiDirDCCtr2And4)//GPIO07 = 0
    	    {
    	    	//******** 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    	    	//选择顺序B3/B2/B1/A4/A3/B7/A6/A7/B0/A0/A1/B6/B5/B4/A2/A5    		    		
    	    	uwADData = AdcRegs.ADCRESULT3>>4;
                wBatteryCurr2 = (((INT32U)uwADData - uwBatCurr2Offset)*wEepromBatteryCurr2Scale)>>12;//wEepromBatteryCurr2Offset
                //if(wBatteryCurr2 < 0)
                //{
                //    wBatteryCurr2 = ((INT32S)wBatteryCurr2 * 4833)>>12;
                //}
                
                uwADData = AdcRegs.ADCRESULT4>>4;
                wBatteryCurr4 = (((INT32U)uwADData - uwBatCurr4Offset)*wEepromBatteryCurr4Scale)>>12;//wEepromBatteryCurr4Offset
                //if(wBatteryCurr4 < 0)
                //{
                //    wBatteryCurr4 = ((INT32S)wBatteryCurr4 * 4833)>>12;
                //}
                
                uwADData = AdcRegs.ADCRESULT5>>4;
                wGFCICurr = (((INT32U)uwADData - uwGFCICurrOffset)*wEepromGFCICurrScale)>>12;//wEepromGFCICurrOffset
                
                uwADData = AdcRegs.ADCRESULT6>>4;
                wONBusVol = (((INT32U)uwADData - wEepromONBusVolOffset)*wEepromONBusVolScale)>>12;
                
                uwADData = AdcRegs.ADCRESULT7>>4;
                wPOBusVol = (((INT32U)uwADData - wEepromPOBusVolOffset)*wEepromPOBusVolScale)>>12;
                
                uwADData = AdcRegs.ADCRESULT14>>4;
                wBatteryOutVol = (((INT32U)uwADData - wEepromBatteryOutVolOffset)*wEepromBatteryOutVolScale)>>12;
                
                uwADData = AdcRegs.ADCRESULT15>>4;
                wIsoVol = (((INT32S)uwADData - wEepromIsoVolOffset)*wEepromIsoVolScale)>>12;
                            
                //------------------AD Switch-----------------
                fADCtrSwitch = BiDirDCCtr1And3;
            	sSetNextADInt32kHz();
            	sADSelHigh();
            	//------------------Cal Avg-------------------
                sCalSignedSum(&dwBatteryCurr2Sum,&wBatteryCurr2,&uwBatteryCurr2Cnt,&fBatteryCurr2Clear); 
            	sCalSignedSum(&dwBatteryCurr4Sum,&wBatteryCurr4,&uwBatteryCurr4Cnt,&fBatteryCurr4Clear);
            	sCalSignedSum(&dwGFCIAvgCurrSum,&wGFCICurr,&uwGFCIAvgCurrSumCnt,&fGFCIAvgCurrSumClear);
                sCalRmsSum(&dwGFCICurrSum,&wGFCICurr,&uwGFCICurrSumCnt,&fGFCICurrSumClear);
                sCalSignedSum(&dwPOBusVolSum,&wPOBusVol,&uwPOBusVolCnt,&fPOBusVolClear);
                sCalSignedSum(&dwONBusVolSum,&wONBusVol,&uwONBusVolCnt,&fONBusVolClear);
                sCalSignedSum(&dwBatteryOutVolSum,&wBatteryOutVol,&uwBatteryOutVolCnt,&fBatteryOutVolClear);
                sCalSignedSum(&dwIsoVolSum,&wIsoVol,&uwIsoVolCnt,&fIsoVolClear);
                wBatteryCurr = wBatteryCurr1 + wBatteryCurr2 + wBatteryCurr3 + wBatteryCurr4;
                sCalDCPowerSum(&ddwDCPowerSum,wBatteryInVol,wBatteryCurr,&uwDCPowerSumCnt,&fDCPowerSumClear);               	
                
                sBatteryCurrHighChk(wBatteryCurr2,&uwBatteryCurr2HighCnt,&uwBatteryCurr2OKCnt,&fBatteryCurr2Err,&fBatteryCurr2ErrEvent);
                sBatteryCurrHighChk(wBatteryCurr4,&uwBatteryCurr4HighCnt,&uwBatteryCurr4OKCnt,&fBatteryCurr4Err,&fBatteryCurr4ErrEvent);
                sGridNoUtilityChk(&fNoUtilityErr,&fNoUtilityErrEvent);          
                sMcuFdbChk(&fMcuActDspNoActErr,&fMcuActDspNoActErrEvent);                 
                sBiDirDCCon2And4Controller(wBatteryInVol,wPOBusVol,wONBusVol,wBatteryCurr2,wBatteryCurr4,wBatteryCurr,fBiDirDCEnOut);    	    	
    	    }
    	    
    	    if(fError == 0)
            {   
            	if(fBiDirDCEnOut == 1)
            	{
            		fDCMode2EnOutReal = 0;
            		fDCMode3EnOutReal = 0;
            		
            		sDisBiDirDC3And4CpldOut();
            		if(fDC1And2En == 0)
            		{
            			++fDCMode1EnOutReal;
            			if(fDCMode1EnOutReal>= 3)
            			{
            			    fDCMode1EnOutReal = 3;
            			    sEnBiDirDC1And2CpldOut();
            			}
            		}
            		else
            		{
            			fDCMode1EnOutReal = 0;
            		}
            		EPwm4Regs.CMPA.half.CMPA = wBiDCCtrModu1;
                    EPwm5Regs.CMPA.half.CMPA = wBiDCCtrModu2;
                    EPwm4Regs.CMPB = wBiDCCtrModu1;//for safety
                    EPwm5Regs.CMPB = wBiDCCtrModu2;
                    sEnBiDirDC1PWMOut();
                    sEnBiDirDC2PWMOut();
                    sDisBiDirDC3PWMOut();
                    sDisBiDirDC4PWMOut();
            	}
            	else if(fBiDirDCEnOut == 2)
            	{
            		fDCMode1EnOutReal = 0;
            		fDCMode3EnOutReal = 0;
            		
            		sDisBiDirDC1And2CpldOut();
            		if(fDC3And4En == 0)
            		{
            			++fDCMode2EnOutReal;
            			if(fDCMode2EnOutReal>= 3)
            			{
            			    fDCMode2EnOutReal = 3;
            			    sEnBiDirDC3And4CpldOut();
            			}
            		}
            		else
            		{
            			fDCMode2EnOutReal = 0;
            		}
            		
            		EPwm4Regs.CMPB = wBiDCCtrModu3;
                    EPwm5Regs.CMPB = wBiDCCtrModu4;
                    EPwm4Regs.CMPA.half.CMPA = wBiDCCtrModu3;
                    EPwm5Regs.CMPA.half.CMPA = wBiDCCtrModu4;
                    sEnBiDirDC3PWMOut();
                    sEnBiDirDC4PWMOut();
                    sDisBiDirDC1PWMOut();
                    sDisBiDirDC2PWMOut();            		
            	}
            	else if(fBiDirDCEnOut == 3)
            	{
            		fDCMode1EnOutReal = 0;
            		fDCMode2EnOutReal = 0;
            		
            		++fDCMode3EnOutReal;
            		if(fDCMode3EnOutReal>= 3)
            		{
            		    fDCMode3EnOutReal = 3;
            		    sEnBiDirDC1And2CpldOut();
                        sEnBiDirDC3And4CpldOut();
            		}
            		            		
                    EPwm4Regs.CMPA.half.CMPA = wBiDCCtrModu1;                    	
                    EPwm4Regs.CMPB = wBiDCCtrModu3;
                    EPwm5Regs.CMPA.half.CMPA = wBiDCCtrModu2;
                    EPwm5Regs.CMPB = wBiDCCtrModu4;
                    sEnBiDirDC1PWMOut();
                    sEnBiDirDC2PWMOut();
                    sEnBiDirDC3PWMOut();
                    sEnBiDirDC4PWMOut(); 
            	}
            	else
            	{
            		sDisBiDirDC1And2CpldOut();
                    sDisBiDirDC3And4CpldOut();
                    sDisBiDirDC1PWMOut();
                    sDisBiDirDC3PWMOut();
                    sDisBiDirDC2PWMOut();
                    sDisBiDirDC4PWMOut();
                    fBiDirDCEnOut = 0;    
                    fDCMode1EnOutReal = 0;
                    fDCMode2EnOutReal = 0;
                    fDCMode3EnOutReal = 0;
            	}
            }
            else 
            {
            	//sDCRelayPOpen();     
                sInvRelayOpen();     
                sGridRelayOpen();
                sDCSoftRelayOpen();   
                
                fBiDirDCEnOut = 0;
                fDCMode1EnOutReal = 0;
                fDCMode2EnOutReal = 0;
                fDCMode3EnOutReal = 0;
               
                sDisBiDirDC1And2CpldOut();
                sDisBiDirDC3And4CpldOut();                                
                sDisBiDirDC1PWMOut();
                sDisBiDirDC3PWMOut();
                sDisBiDirDC2PWMOut();
                sDisBiDirDC4PWMOut();
     
                if(fDeCapEnergy !=1)
                {
                	fInvEnOut = 0;
                	fInvEnOutReal = 0;
                	sDisInvCpldOut();
                	sDisInvPWMOut();
                }
            }
    	}
    	else if(fADCtrSwitch == InvCtrAndPLL)//GPIO07 = 0
    	{
    		if(fDCIReguRefurbish == 1)
            {           	
                wRefurbishDCIReguR = wDCIReguR;
                wRefurbishDCIReguS = wDCIReguS;
                wRefurbishDCIReguT = wDCIReguT;
                fDCIReguRefurbish = 0;
            }
            
            //******** 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    		//选择顺序B3/B2/B1/A4/A3/B7/A6/A7/B0/A0/A1/B6/B5/B4/A2/A5   		
            if(fPhaseSequ == 2)
            {
                //===============inv cur===============
                uwADData = AdcRegs.ADCRESULT0>>4;
                wRInvCurrCalRms = (((INT32S)uwADData - uwRInvCurrOffset)*wEepromRInvCurrScale)>>11; 
                wADTmp = (INT16S)(uwADData<<1) + wRefurbishDCIReguR;                              
                wRInvCurr = (((INT32S)wADTmp - (uwRInvCurrOffset<<1))*wEepromRInvCurrScale)>>12; 
                                           
                uwADData = AdcRegs.ADCRESULT1>>4;
                wTInvCurrCalRms = (((INT32S)uwADData - uwSInvCurrOffset)*wEepromSInvCurrScale)>>11; 
                wADTmp = (INT16S)(uwADData<<1) + wRefurbishDCIReguT;
                wTInvCurr = (((INT32S)wADTmp - (uwSInvCurrOffset<<1))*wEepromSInvCurrScale)>>12;
                             
                uwADData = AdcRegs.ADCRESULT2>>4;
                wSInvCurrCalRms = (((INT32S)uwADData - uwTInvCurrOffset)*wEepromTInvCurrScale)>>11; 
                wADTmp = (INT16S)(uwADData<<1) + wRefurbishDCIReguS;
                wSInvCurr = (((INT32S)wADTmp - (uwTInvCurrOffset<<1))*wEepromTInvCurrScale)>>12; 
                //================grid vol===============
                uwADData = AdcRegs.ADCRESULT8>>4;
                udwRSGridVolFiltoffset = ((INT64U)udwRSGridVolFiltoffset*cGridOffsetFiltDen + ((INT64U)uwADData<<16)*cGridOffsetFiltNum)>>15;			
		    	uwADTmp = udwRSGridVolFiltoffset>>16;
                sCalUnSignedSum(&udwRSGridVolOffsetSum,&uwADTmp,&uwRSGridVolOffsetSumCnt,&fRSGridVolOffsetSumClear);
                wTRGridVol = (((INT32S)uwADData - uwRSGridVolOffset)*wEepromRSGridVolScale)>>11;
                wRSGridVolCalRms = wTRGridVol;
                wTRGridVol = -wTRGridVol;
                
                uwADData = AdcRegs.ADCRESULT9>>4;
                udwSTGridVolFiltoffset = ((INT64U)udwSTGridVolFiltoffset*cGridOffsetFiltDen + ((INT64U)uwADData<<16)*cGridOffsetFiltNum)>>15;			
		    	uwADTmp = udwSTGridVolFiltoffset>>16;
                sCalUnSignedSum(&udwSTGridVolOffsetSum,&uwADTmp,&uwSTGridVolOffsetSumCnt,&fSTGridVolOffsetSumClear);
                wSTGridVol = (((INT32S)uwADData - uwSTGridVolOffset)*wEepromSTGridVolScale)>>11;
                wSTGridVolCalRms = wSTGridVol;
                wSTGridVol = -wSTGridVol;
                
                uwADData = AdcRegs.ADCRESULT10>>4;
                udwTRGridVolFiltoffset = ((INT64U)udwTRGridVolFiltoffset*cGridOffsetFiltDen + ((INT64U)uwADData<<16)*cGridOffsetFiltNum)>>15;			
		    	uwADTmp = udwTRGridVolFiltoffset>>16;
                sCalUnSignedSum(&udwTRGridVolOffsetSum,&uwADTmp,&uwTRGridVolOffsetSumCnt,&fTRGridVolOffsetSumClear);
                wRSGridVol = (((INT32S)uwADData - uwTRGridVolOffset)*wEepromTRGridVolScale)>>11;
                wTRGridVolCalRms = wRSGridVol;
                wRSGridVol = -wRSGridVol;               
                //================inv vol================
                uwADData = AdcRegs.ADCRESULT11>>4;
                wRInvVol = (((INT32S)uwADData - wEepromRInvVolOffset)*wEepromRInvVolScale)>>11;
                uwADData = AdcRegs.ADCRESULT12>>4;
                wTInvVol = (((INT32S)uwADData - wEepromSInvVolOffset)*wEepromSInvVolScale)>>11;
                uwADData = AdcRegs.ADCRESULT13>>4;
                wSInvVol = (((INT32S)uwADData - wEepromTInvVolOffset)*wEepromTInvVolScale)>>11;
            }
            else
            {
            	//=================inv cur===============
		    	uwADData = AdcRegs.ADCRESULT0>>4;
		    	wRInvCurrCalRms = (((INT32S)uwADData - uwRInvCurrOffset)*wEepromRInvCurrScale)>>11; 
                wADTmp = (INT16S)(uwADData<<1) + wRefurbishDCIReguR;
                wRInvCurr = (((INT32S)wADTmp - (uwRInvCurrOffset<<1))*wEepromRInvCurrScale)>>12; 
                                           
                uwADData = AdcRegs.ADCRESULT1>>4;
                wSInvCurrCalRms = (((INT32S)uwADData - uwSInvCurrOffset)*wEepromSInvCurrScale)>>11; 
                wADTmp = (INT16S)(uwADData<<1) + wRefurbishDCIReguS;
                wSInvCurr = (((INT32S)wADTmp - (uwSInvCurrOffset<<1))*wEepromSInvCurrScale)>>12;
                             
                uwADData = AdcRegs.ADCRESULT2>>4;
                wTInvCurrCalRms = (((INT32S)uwADData - uwTInvCurrOffset)*wEepromTInvCurrScale)>>11; 
                wADTmp = (INT16S)(uwADData<<1) + wRefurbishDCIReguT;
                wTInvCurr = (((INT32S)wADTmp - (uwTInvCurrOffset<<1))*wEepromTInvCurrScale)>>12;   
                //=================grid vol==============            
                uwADData = AdcRegs.ADCRESULT8>>4;           
                udwRSGridVolFiltoffset = ((INT64U)udwRSGridVolFiltoffset*cGridOffsetFiltDen + ((INT64U)uwADData<<16)*cGridOffsetFiltNum)>>15;			
		    	uwADTmp = udwRSGridVolFiltoffset>>16;
                sCalUnSignedSum(&udwRSGridVolOffsetSum,&uwADTmp,&uwRSGridVolOffsetSumCnt,&fRSGridVolOffsetSumClear);
                wRSGridVol = (((INT32S)uwADData - uwRSGridVolOffset)*wEepromRSGridVolScale)>>11;
                wRSGridVolCalRms = wRSGridVol;
                
                uwADData = AdcRegs.ADCRESULT9>>4;
                udwSTGridVolFiltoffset = ((INT64U)udwSTGridVolFiltoffset*cGridOffsetFiltDen + ((INT64U)uwADData<<16)*cGridOffsetFiltNum)>>15;			
		    	uwADTmp = udwSTGridVolFiltoffset>>16;
                sCalUnSignedSum(&udwSTGridVolOffsetSum,&uwADTmp,&uwSTGridVolOffsetSumCnt,&fSTGridVolOffsetSumClear);
                wSTGridVol = (((INT32S)uwADData - uwSTGridVolOffset)*wEepromSTGridVolScale)>>11;
                wSTGridVolCalRms = wSTGridVol;
                
                uwADData = AdcRegs.ADCRESULT10>>4;
                udwTRGridVolFiltoffset = ((INT64U)udwTRGridVolFiltoffset*cGridOffsetFiltDen + ((INT64U)uwADData<<16)*cGridOffsetFiltNum)>>15;			
		    	uwADTmp = udwTRGridVolFiltoffset>>16;
                sCalUnSignedSum(&udwTRGridVolOffsetSum,&uwADTmp,&uwTRGridVolOffsetSumCnt,&fTRGridVolOffsetSumClear);
		    	wTRGridVol = (((INT32S)uwADData - uwTRGridVolOffset)*wEepromTRGridVolScale)>>11;
		    	wTRGridVolCalRms = wTRGridVol;		    	
                
                //=================inv vol===============
                uwADData = AdcRegs.ADCRESULT11>>4;
                wRInvVol = (((INT32S)uwADData - wEepromRInvVolOffset)*wEepromRInvVolScale)>>11;
                uwADData = AdcRegs.ADCRESULT12>>4;
                wSInvVol = (((INT32S)uwADData - wEepromSInvVolOffset)*wEepromSInvVolScale)>>11;
                uwADData = AdcRegs.ADCRESULT13>>4;
                wTInvVol = (((INT32S)uwADData - wEepromTInvVolOffset)*wEepromTInvVolScale)>>11;
            }            
            
            fADCtrSwitch = BiDirDCCtr2And4;
        	sSetNextADInt16kHz();
        	sADSelLow();
            
            sLineToPhase(&twGridLineVol, &twGridPhaseVol);
            //================== 逆变器输出电流 =====================
            sCalRmsSum(&dwRInvRmsCurrSum,&wRInvCurrCalRms,&uwRInvRmsCurrSumCnt,&fRInvRmsCurrSumClear); //wRInvCurr
            sCalRmsSum(&dwSInvRmsCurrSum,&wSInvCurrCalRms,&uwSInvRmsCurrSumCnt,&fSInvRmsCurrSumClear); //wSInvCurr
            sCalRmsSum(&dwTInvRmsCurrSum,&wTInvCurrCalRms,&uwTInvRmsCurrSumCnt,&fTInvRmsCurrSumClear); //wTInvCurr  
            //================== 电网侧线电压 =======================
            sCalRmsSum(&dwRSGridRmsVolSum,&wRSGridVolCalRms,&uwRSGridRmsVolSumCnt,&fRSGridRmsVolSumClear);
            sCalRmsSum(&dwSTGridRmsVolSum,&wSTGridVolCalRms,&uwSTGridRmsVolSumCnt,&fSTGridRmsVolSumClear);
            sCalRmsSum(&dwTRGridRmsVolSum,&wTRGridVolCalRms,&uwTRGridRmsVolSumCnt,&fTRGridRmsVolSumClear);
            //================== 逆变侧线电压 =======================
            sCalRmsSum(&dwRInvRmsVolSum,&wRInvVol,&uwRInvRmsVolSumCnt,&fRInvRmsVolSumClear);
            sCalRmsSum(&dwSInvRmsVolSum,&wSInvVol,&uwSInvRmsVolSumCnt,&fSInvRmsVolSumClear);
            sCalRmsSum(&dwTInvRmsVolSum,&wTInvVol,&uwTInvRmsVolSumCnt,&fTInvRmsVolSumClear);
            
            sCalSignedSum(&dwRInvAvgVolSum,&wRInvVol,&uwRInvAvgVolCnt,&fRInvAvgVolClear);
            sCalSignedSum(&dwSInvAvgVolSum,&wSInvVol,&uwSInvAvgVolCnt,&fSInvAvgVolClear);
            sCalSignedSum(&dwTInvAvgVolSum,&wTInvVol,&uwTInvAvgVolCnt,&fTInvAvgVolClear);
            
            sInvCurrHighChk(&wRInvCurrCalRms,&wSInvCurrCalRms,&wTInvCurrCalRms,&fInvCurrHighErr,&fInvCurrHighErrEvent);
            sInvAndGridVolDeal(&twInvPhaseVol,&twGridPhaseVol);
            //sGridVolAndInvVolDiffChk(twGridPhaseVol,twInvPhaseVol,&fGridAndInvVolDiffErr,&fGridAndInvVolDiffErrEvent);
            sGridVolAndInvVolDiffChk(twGridLineVol,twInvPhaseVol,&fGridAndInvVolDiffErr,&fGridAndInvVolDiffErrEvent);
            //=============== 计算交流侧逆变有功功率 ================
            //wRInvCurrCalRms = wRInvCurrCalRms - (((INT32S)wGFCICurr * 262)>>14);
            //wSInvCurrCalRms = wSInvCurrCalRms - (((INT32S)wGFCICurr * 262)>>14);
            //wTInvCurrCalRms = wTInvCurrCalRms - (((INT32S)wGFCICurr * 262)>>14);
            sCalInvActivePowerSum(&ddwInvActivePowerSum[0],&dwInstantaneousActivePower,&wRGridVol,&wSGridVol,&wTGridVol,&wRInvCurrCalRms,
                                  &wSInvCurrCalRms,&wTInvCurrCalRms,&uwInvActivePowerSumCnt,&fInvActivePowerSumClear);
            sCalInvReactivePowerSum(&ddwInvReactivePowerSum[0],&dwInstantaneousReactivePower,&wRSGridVol,&wSTGridVol,&wTRGridVol,
                                    &wRInvCurrCalRms,&wSInvCurrCalRms,&wTInvCurrCalRms,&uwInvReactivePowerSumCnt,&fInvReactivePowerSumClear); 
                                
            sCalLVRTSumValue();
            sInvController(wPOBusVol,wONBusVol,twInvCurr,twGridPhaseVol,wBatteryCurr,fInvEnOut); 
            
            if((fError == 0)||(fDeCapEnergy ==1))
            {           
                if(fInvEnOut == 1)
                {
                	fInvEnOutReal++;
                    if(fInvEnOutReal >= 2)
                    {   
                        fInvEnOutReal = 2;
                        sEnInvCpldOut();
                    }
            
                    if(tInvModuAbc.A.word0 > PWM_MAX_Duty) tInvModuAbc.A.word0 = PWM_MAX_Duty;
                    if(tInvModuAbc.A.word1 > PWM_MAX_Duty) tInvModuAbc.A.word1 = PWM_MAX_Duty;
                    if(tInvModuAbc.B.word0 > PWM_MAX_Duty) tInvModuAbc.B.word0 = PWM_MAX_Duty;
                    if(tInvModuAbc.B.word1 > PWM_MAX_Duty) tInvModuAbc.B.word1 = PWM_MAX_Duty;
                    if(tInvModuAbc.C.word0 > PWM_MAX_Duty) tInvModuAbc.C.word0 = PWM_MAX_Duty;
                    if(tInvModuAbc.C.word1 > PWM_MAX_Duty) tInvModuAbc.C.word1 = PWM_MAX_Duty;  
                    
                    if(tInvModuAbc.A.word0 < 1) tInvModuAbc.A.word0 = 1;
                    if(tInvModuAbc.A.word1 < 1) tInvModuAbc.A.word1 = 1;
                    if(tInvModuAbc.B.word0 < 1) tInvModuAbc.B.word0 = 1;
                    if(tInvModuAbc.B.word1 < 1) tInvModuAbc.B.word1 = 1;
                    if(tInvModuAbc.C.word0 < 1) tInvModuAbc.C.word0 = 1;
                    if(tInvModuAbc.C.word1 < 1) tInvModuAbc.C.word1 = 1; 	
                    
                    if(fPhaseSequ == 1) 
                    {  
                        EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD - tInvModuAbc.A.word0;
                        EPwm1Regs.CMPB = tInvModuAbc.A.word1;
                        
                        EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.TBPRD - tInvModuAbc.B.word0;
                        EPwm2Regs.CMPB = tInvModuAbc.B.word1;
                        
                        EPwm3Regs.CMPA.half.CMPA = EPwm3Regs.TBPRD - tInvModuAbc.C.word0;
                        EPwm3Regs.CMPB = tInvModuAbc.C.word1;                   
		    	    }
                    else if(fPhaseSequ == 2) 
                    {
                        EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD - tInvModuAbc.A.word0;
                        EPwm1Regs.CMPB = tInvModuAbc.A.word1;
                        
                        EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.TBPRD - tInvModuAbc.C.word0;
                        EPwm2Regs.CMPB = tInvModuAbc.C.word1;
                        
                        EPwm3Regs.CMPA.half.CMPA = EPwm3Regs.TBPRD - tInvModuAbc.B.word0;
                        EPwm3Regs.CMPB = tInvModuAbc.B.word1;
                    }
                    else //0% duty
                    {
                        EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD;
                        EPwm1Regs.CMPB = 0;//EPwm1Regs.TBPRD
                        
                        EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.TBPRD;
                        EPwm2Regs.CMPB = 0;//EPwm2Regs.TBPRD
                        
                        EPwm3Regs.CMPA.half.CMPA = EPwm3Regs.TBPRD;
                        EPwm3Regs.CMPB = 0;//EPwm3Regs.TBPRD
                    }                 				
                    sEnInvPWMOut();
                }
                else
                {
                    sDisInvCpldOut();
                    sDisInvPWMOut();
                    fInvEnOutReal = 0;
                    fInvEnOut = 0;
                }
            }
            else 
            {
            	//sDCRelayPOpen();     
                sInvRelayOpen();     
                sGridRelayOpen();
                sDCSoftRelayOpen();                 
                fBiDirDCEnOut = 0;
                fDCMode1EnOutReal = 0;
                fDCMode2EnOutReal = 0;
                fDCMode3EnOutReal = 0;               
                fInvEnOut = 0;
                fInvEnOutReal = 0;
                sDisBiDirDC1And2CpldOut();
                sDisBiDirDC3And4CpldOut();
                sDisInvCpldOut();                
                sDisBiDirDC1PWMOut();
                sDisBiDirDC3PWMOut();
                sDisBiDirDC2PWMOut();
                sDisBiDirDC4PWMOut();
                sDisInvPWMOut();
            }
    		
    	}
    	else
    	{
    		
    	}
    	
    	EPwm6Regs.CMPA.half.CMPA = EPwm6Regs.TBPRD - uwFanDuty;
    	        
        #if (!SCI_SCOPE)
            if(fADCtrSwitch == InvCtrAndPLL)
            {
            //static INT16U fCopyStart = 0;
            //static INT32U udwDelay10s = 40000;
            //static INT16U i = 0;
            //static INT16U uwDelay = 0;
            //    
            //    if(++uwDelay >= 4)
            //    {
            //        uwDelay = 0;
            //        if(fCopyStart == 0)
            //        {
            //            i = 0;
            //            if((wActivePowerPer <= 1638)&&(++udwDelay10s >= 40000))
            //            {
            //                udwDelay10s = 40000;
            //                fCopyStart = 1;
            //            }
            //        }
            //        
            //        if(fCopyStart == 1)
            //        {
            //            wDebugCopy1[i] = tInvCurrDqoRef.d;
            //            wDebugCopy2[i] = tInvCurrDqo.d;
            //            wDebugCopy3[i] = tInvCurrDqo.q;
            //            
            //            i++;
            //            if(i >= 100)
            //            {
            //                i = 0;
            //                fCopyStart = 0;
            //                udwDelay10s = 0;
            //            }
            //        }
                                    
                    sSciTxArray(); 
            //    }           
            }
        #endif
        #if SCI_SCOPE       
            SciaTxDeal(); 
        #endif       	
    }
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;			// Reset SEQ1 to CONV00 state
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;			// Clear ADC SEQ1 interrupt flag
} //end ADCIsr
/*** end of file *****************************************************/

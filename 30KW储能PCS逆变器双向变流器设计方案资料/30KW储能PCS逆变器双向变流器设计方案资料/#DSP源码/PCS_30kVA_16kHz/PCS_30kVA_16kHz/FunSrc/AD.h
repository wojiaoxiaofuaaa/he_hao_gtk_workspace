/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef AD_H
#define AD_H
#include "..\FunSrc\SysDefine.h"

//---------------------------------------------------------------------------
// Function Prototypes
//
typedef enum  
{
	BiDirDCCtr1And3,
    BiDirDCCtr2And4,
    InvCtrAndPLL    
}T_CtrSwitch;

extern void InitAdc(void);
extern void InitAdcVar(void);
extern void ADCIsr(void);
extern INT16U suwGetAvg(volatile INT32U *pSum,volatile INT16U *pSumCnt,INT16U *pFlags);
extern INT16S swGetAvg(volatile INT32S *pSum,volatile INT16U *pSumCnt,INT16U *pFlags);
extern void sCalPowerAvg(INT32U *pPower,INT16U uwAvgTimeCnt,INT32U *pAvgPower);
extern void sCalInvActivePowerValue(volatile INT64S *pVoltMulCurrSum,volatile INT16U *pVoltMulCurrSumSumCnt,INT16U *pVoltMulCurrSumFlag,INT32S *pudwActivePower);
extern void sCalDCPowerValue(volatile INT64S *pDCPowerSum,volatile INT16U *pDCPowerSumSumCnt,INT16U *pDCPowerFlag,INT32S *pDCPower);
extern void sCalInvReactivePowerValue(volatile INT64S *pVoltMulCurrSum,volatile INT16U *pVoltMulCurrSumSumCnt,INT16U *pVoltMulCurrSumFlag,INT32S *pdwReactivePower,INT16S *pRSGridRmsVol,INT16S *pSTGridRmsVol,INT16S *pTRGridRmsVol);
extern void sCalInvApparentPowerValue(INT32S *pActivePower,INT32S *pReactivePower,INT32U *pApparentPower);
extern void sCalRmsValue(void);
extern INT16S swGetFifoAvg(INT16S wValue, INT16S *pArray, INT16S wArrayNum);
extern void sCalRmsPrdValueSum(void);
extern void sCalRmsAvgValue(void);
extern void sCalRmsFifoAvgValue(void);
extern void sRmsClearZero(void);
extern void sInvShortPowerClearZero(void);
extern void sCalLongAvg(void); 
extern void sCalLVRTValue(void);
extern void sPowerAdjust(INT32S *pInvActivePower,INT32S *pInvReactivePower);
extern void sCalSignedSum(INT32S *pSum,INT16S *pData,INT16U *pSumCnt,INT16U *pFlags);
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
//------------------------------DC Sampling------------------------------------
extern INT16S wPOBusVol,wONBusVol,wIsoVol,wBatteryOutVol,wBatteryInVol;
extern INT16S wBatteryCurr,wBatteryCurr1,wBatteryCurr2,wBatteryCurr3,wBatteryCurr4;

extern INT16S wBusAvgVol,wPOBusAvgVol,wONBusAvgVol,wIsoAvgVol,wBatteryOutAvgVol,wBatteryInAvgVol;
extern INT16S wBatteryAvgCurr,wBatteryAvgCurr1,wBatteryAvgCurr2,wBatteryAvgCurr3,wBatteryAvgCurr4;

extern INT16U uwBatCurr1Offset,uwBatCurr2Offset,uwBatCurr3Offset,uwBatCurr4Offset;

extern INT32S dwPOBusVolSum,dwONBusVolSum,dwIsoVolSum,dwBatteryOutVolSum,dwBatteryInVolSum;
extern INT32S dwBatteryCurr1Sum,dwBatteryCurr2Sum,dwBatteryCurr3Sum,dwBatteryCurr4Sum;
extern INT32U udwBatCurr1OffsetSum,udwBatCurr2OffsetSum,udwBatCurr3OffsetSum,udwBatCurr4OffsetSum;

extern INT16U uwPOBusVolCnt,uwONBusVolCnt,uwIsoVolCnt,uwBatteryOutVolCnt,uwBatteryInVolCnt;
extern INT16U uwBatteryCurr1Cnt,uwBatteryCurr2Cnt,uwBatteryCurr3Cnt,uwBatteryCurr4Cnt;
extern INT16U uwBatteryCurr1HighCnt,uwBatteryCurr2HighCnt,uwBatteryCurr3HighCnt,uwBatteryCurr4HighCnt;
extern INT16U uwBatteryCurr1OKCnt,uwBatteryCurr2OKCnt,uwBatteryCurr3OKCnt,uwBatteryCurr4OKCnt;
extern INT16U uwBatCurr1OffsetSumCnt,uwBatCurr2OffsetSumCnt,uwBatCurr3OffsetSumCnt,uwBatCurr4OffsetSumCnt;

extern INT16U fPOBusVolClear,fONBusVolClear,fIsoVolClear,fBatteryOutVolClear,fBatteryInVolClear;
extern INT16U fBatteryCurr1Clear,fBatteryCurr2Clear,fBatteryCurr3Clear,fBatteryCurr4Clear;
extern INT16U fBatCurr1OffsetSumClear,fBatCurr2OffsetSumClear,fBatCurr3OffsetSumClear,fBatCurr4OffsetSumClear;
//------------------------------AC Sampling------------------------------------
extern T_THREE_PHASE_ABC twInvCurr,twInvCurrCalRms;
extern T_THREE_PHASE_ABC wDCIAvgCurr;
extern T_THREE_PHASE_ABC twInvPhaseVol;
extern T_THREE_PHASE_ABC twGridLineVol;
extern T_THREE_PHASE_ABC twGridPhaseVol;
extern T_THREE_PHASE_ABC twDCICurr;
extern T_THREE_PHASE_ABC twDCICurrFilt;
extern T_THREE_PHASE_ABC twInvRmsCurr;      
extern T_THREE_PHASE_ABC twGridLineRmsVol;
extern T_THREE_PHASE_ABC twInvPhaseRmsVol;
extern T_THREE_PHASE_ABC twInvPhaseAvgVol; 
extern T_THREE_PHASE_ABC twInvRmsAvgCurr;      
extern T_THREE_PHASE_ABC twGridLineRmsAvgVol; 
extern T_THREE_PHASE_ABC twInvRmsFifoAvgCurr;
extern T_THREE_PHASE_ABC twGridLineRmsFifoAvgVol;
extern INT16S wGFCICurr,wGFCICurrRms,wGFCIAvgCurr; 

extern INT16U uwRInvCurrOffset,uwSInvCurrOffset,uwTInvCurrOffset;
extern INT16U uwRSGridVolOffset,uwSTGridVolOffset,uwTRGridVolOffset;
extern INT16U uwRDCICurrOffset,uwSDCICurrOffset,uwTDCICurrOffset,uwGFCICurrOffset;
extern INT32U udwRSGridVolFiltoffset,udwSTGridVolFiltoffset,udwTRGridVolFiltoffset;
extern INT16S wRInvRmsAvgCurrArray[4],wSInvRmsAvgCurrArray[4],wTInvRmsAvgCurrArray[4];  
extern INT16S wRSGridRmsAvgVolArray[4],wSTGridRmsAvgVolArray[4],wTRGridRmsAvgVolArray[4]; 

extern INT32S dwRDCICurrSum,dwSDCICurrSum,dwTDCICurrSum;
extern INT32S dwRInvRmsCurrSum,dwSInvRmsCurrSum,dwTInvRmsCurrSum;
extern INT32S dwRSGridRmsVolSum,dwSTGridRmsVolSum,dwTRGridRmsVolSum;
extern INT32S dwRSInvRmsVolSum,dwSTInvRmsVolSum,dwTRInvRmsVolSum;
extern INT32S dwRInvRmsVolSum,dwSInvRmsVolSum,dwTInvRmsVolSum;
extern INT32U udwRInvCurrOffsetSum,udwSInvCurrOffsetSum,udwTInvCurrOffsetSum,udwGFCICurrOffsetSum;
extern INT32U udwRSGridVolOffsetSum,udwSTGridVolOffsetSum,udwTRGridVolOffsetSum;
extern INT32U udwRDCICurrOffsetSum,udwSDCICurrOffsetSum,udwTDCICurrOffsetSum;
extern INT32S dwRInvRmsPrdCurrSum,dwSInvRmsPrdCurrSum,dwTInvRmsPrdCurrSum;
extern INT32S dwRSGridRmsPrdVolSum,dwSTGridRmsPrdVolSum,dwTRGridRmsPrdVolSum;
extern INT32S dwGFCICurrSum,dwGFCIAvgCurrSum;
extern INT32S dwRInvAvgVolSum,dwSInvAvgVolSum,dwTInvAvgVolSum;

extern INT16U uwRDCISumCnt,uwSDCISumCnt,uwTDCISumCnt;
extern INT16U uwRInvRmsCurrSumCnt,uwSInvRmsCurrSumCnt,uwTInvRmsCurrSumCnt;
extern INT16U uwRSGridRmsVolSumCnt,uwSTGridRmsVolSumCnt,uwTRGridRmsVolSumCnt;
extern INT16U uwRSInvRmsVolSumCnt,uwSTInvRmsVolSumCnt,uwTRInvRmsVolSumCnt;
extern INT16U uwRInvRmsVolSumCnt,uwSInvRmsVolSumCnt,uwTInvRmsVolSumCnt;
extern INT16U uwRInvCurrOffsetSumCnt,uwSInvCurrOffsetSumCnt,uwTInvCurrOffsetSumCnt,uwGFCICurrOffsetSumCnt;
extern INT16U uwRSGridVolOffsetSumCnt,uwSTGridVolOffsetSumCnt,uwTRGridVolOffsetSumCnt;
extern INT16U uwRDCICurrOffsetSumCnt,uwSDCICurrOffsetSumCnt,uwTDCICurrOffsetSumCnt;
extern INT16U uwRInvRmsPrdCurrSumCnt,uwSInvRmsPrdCurrSumCnt,uwTInvRmsPrdCurrSumCnt;
extern INT16U uwRSGridRmsPrdVolSumCnt,uwSTGridRmsPrdVolSumCnt,uwTRGridRmsPrdVolSumCnt;
extern INT16U uwGFCICurrSumCnt,uwGFCIAvgCurrSumCnt;
extern INT16U uwRInvAvgVolCnt,uwSInvAvgVolCnt,uwTInvAvgVolCnt;

extern INT16U fRDCISumClear,fSDCISumClear,fTDCISumClear;
extern INT16U fRInvRmsCurrSumClear,fSInvRmsCurrSumClear,fTInvRmsCurrSumClear;
extern INT16U fRSGridRmsVolSumClear,fSTGridRmsVolSumClear,fTRGridRmsVolSumClear;
extern INT16U fRSInvRmsVolSumClear,fSTInvRmsVolSumClear,fTRInvRmsVolSumClear;
extern INT16U fRInvRmsVolSumClear,fSInvRmsVolSumClear,fTInvRmsVolSumClear;
extern INT16U fRInvCurrOffsetSumClear,fSInvCurrOffsetSumClear,fTInvCurrOffsetSumClear,fGFCICurrOffsetSumClear;
extern INT16U fRSGridVolOffsetSumClear,fSTGridVolOffsetSumClear,fTRGridVolOffsetSumClear;
extern INT16U fRDCICurrOffsetSumClear,fSDCICurrOffsetSumClear,fTDCICurrOffsetSumClear;
extern INT16U fRInvRmsPrdCurrSumClear,fSInvRmsPrdCurrSumClear,fTInvRmsPrdCurrSumClear;                                                                 
extern INT16U fRSGridRmsPrdVolSumClear,fSTGridRmsPrdVolSumClear,fTRGridRmsPrdVolSumClear;  
extern INT16U fGFCICurrSumClear,fGFCIAvgCurrSumClear;
extern INT16U fRInvAvgVolClear,fSInvAvgVolClear,fTInvAvgVolClear;
//------------------------------Temperature Sampling---------------------------
extern INT16S wEnvTmp,wHeatTmp,wXFMRTmp;
extern INT16S wEnvTmpAvg,wHeatTmpAvg,wXFMRTmpAvg;
extern INT16S wEnvFitTemp,wHeatFitTemp,wXFMRFitTemp;

extern INT32S dwEnvTmpSum,dwHeatTmpSum,dwXFMRTmpSum;
extern INT16U uwEnvTmpSumCnt,uwHeatTmpSumCnt,uwXFMRTmpSumCnt;
extern INT16U fEnvTmpSumClear,fHeatTmpSumClear,fXFMRTmpSumClear;
//------------------------------Power Cal--------------------------------------
extern INT16U uwPowerEff,uwPowerEffAvg;
extern INT32S dwInstantaneousReactivePower,dwInstantaneousActivePower,dwDCPower;

extern INT32S dwInvActivePower[4],dwInvReactivePower[4];
extern INT32U udwApparentPower[4];
extern INT16S wPowerFactor[4];
extern INT16S wPowerFactorAvg[4];

extern INT32U udwPowerEffSum;
extern INT32S dwPowerFactorSum[4];
extern INT64S ddwInvActivePowerSum[4],ddwInvReactivePowerSum[4],ddwDCPowerSum;

extern INT16U uwInvActivePowerSumCnt,uwInvReactivePowerSumCnt,uwDCPowerSumCnt;
extern INT16U uwPowerEffSumCnt,uwPowerFactorSumCnt[4];

extern INT16U fInvActivePowerSumClear,fInvReactivePowerSumClear,fDCPowerSumClear;
extern INT16U fPowerEffSumClear,fPowerFactorSumClear[4];

//------------------------------FVRT-------------------------------------------
extern INT16U fLVRTValue;
extern INT16U fHVRTValue;
extern INT16U fImbalanceLVRT;
extern INT16U fLVRTStartOrEnd;
extern INT16U fLVRTStartOrEndBak;
extern INT32U udwHalfCysRVolSqureSum;
extern INT32U udwHalfCysSVolSqureSum;
extern INT32U udwHalfCysTVolSqureSum;
extern INT16U uwHalfCycSampNum;
extern INT16U uwHalfCysRateVolSqure;
extern INT32U udwHalfCysRateVolSqure0R81;
extern INT32U udwHalfCysRateVolSqure1R21;
extern INT32U udwHalfCysRateVolSqure0R2;
extern INT32U udwHalfCysRateVolSqure0R01;
//------------------------------Other------------------------------------------
extern T_CtrSwitch fADCtrSwitch;
extern INT16S wSTGridVolBak,wTRGridVolBak;
extern INT16U fStartOrNormal;
extern INT16U fInvEnOut,fBiDirDCEnOut;
extern INT16U fSyncAD;
//---------------------------------------------------------------------------
// Global symbols defined in source files
#define sADSelHigh()                {GpioDataRegs.GPASET.bit.GPIO17 = 1;}
#define sADSelLow()                 {GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;} 

#define wRInvCurr                       twInvCurr.a       
#define wSInvCurr                       twInvCurr.b       
#define wTInvCurr                       twInvCurr.c    

#define wRInvCurrCalRms                 twInvCurrCalRms.a       
#define wSInvCurrCalRms                 twInvCurrCalRms.b       
#define wTInvCurrCalRms                 twInvCurrCalRms.c  
                                        
#define wRInvVol                        twInvPhaseVol.a    
#define wSInvVol                        twInvPhaseVol.b    
#define wTInvVol                        twInvPhaseVol.c    
                                        
#define wRSGridVol                      twGridLineVol.a   
#define wSTGridVol                      twGridLineVol.b   
#define wTRGridVol                      twGridLineVol.c  
                                        
#define wRGridVol                       twGridPhaseVol.a   
#define wSGridVol                       twGridPhaseVol.b   
#define wTGridVol                       twGridPhaseVol.c 
                                        
#define wRDCICurr                       twDCICurr.a  
#define wSDCICurr                       twDCICurr.b  
#define wTDCICurr                       twDCICurr.c 

#define wRDCICurrFilt                   twDCICurrFilt.a  
#define wSDCICurrFilt                   twDCICurrFilt.b  
#define wTDCICurrFilt                   twDCICurrFilt.c   
                                        
#define wRInvRmsCurr                    twInvRmsCurr.a     
#define wSInvRmsCurr                    twInvRmsCurr.b     
#define wTInvRmsCurr                    twInvRmsCurr.c   
                                                        
#define wRSGridRmsVol                   twGridLineRmsVol.a 
#define wSTGridRmsVol                   twGridLineRmsVol.b 
#define wTRGridRmsVol                   twGridLineRmsVol.c 

#define wRInvRmsVol                     twInvPhaseRmsVol.a 
#define wSInvRmsVol                     twInvPhaseRmsVol.b 
#define wTInvRmsVol                     twInvPhaseRmsVol.c 

#define wRInvAvgVol                     twInvPhaseAvgVol.a 
#define wSInvAvgVol                     twInvPhaseAvgVol.b 
#define wTInvAvgVol                     twInvPhaseAvgVol.c 
                                        
#define wRInvRmsAvgCurr                 twInvRmsAvgCurr.a
#define wSInvRmsAvgCurr                 twInvRmsAvgCurr.b
#define wTInvRmsAvgCurr                 twInvRmsAvgCurr.c 
          
#define wRSGridRmsAvgVol                twGridLineRmsAvgVol.a
#define wSTGridRmsAvgVol                twGridLineRmsAvgVol.b
#define wTRGridRmsAvgVol                twGridLineRmsAvgVol.c       

#define wRInvRmsFifoAvgCurr             twInvRmsFifoAvgCurr.a
#define wSInvRmsFifoAvgCurr             twInvRmsFifoAvgCurr.b
#define wTInvRmsFifoAvgCurr             twInvRmsFifoAvgCurr.c      
     
#define wRSGridRmsFifoAvgVol            twGridLineRmsFifoAvgVol.a
#define wSTGridRmsFifoAvgVol            twGridLineRmsFifoAvgVol.b
#define wTRGridRmsFifoAvgVol            twGridLineRmsFifoAvgVol.c  

#define wRDCIAvgCurr                    wDCIAvgCurr.a
#define wSDCIAvgCurr                    wDCIAvgCurr.b
#define wTDCIAvgCurr                    wDCIAvgCurr.c

#define cHalfCycSampNum50Hz480V             80
#define cHalfCysRateVolSqure50Hz480V        18000 
#define cHalfCysRateVolSqure0R8150Hz480V    238878720 
#define cHalfCysRateVolSqure1R2150Hz480V    356843520
#define cHalfCysRateVolSqure0R250Hz480V     58982400
#define cHalfCysRateVolSqure0R0150Hz480V    6635520

#define cHalfCycSampNum60Hz480V             67
#define cHalfCysRateVolSqure60Hz480V        15000
#define cHalfCysRateVolSqure0R8160Hz480V    199065600
#define cHalfCysRateVolSqure1R2160Hz480V    297369600
#define cHalfCysRateVolSqure0R260Hz480V     49152000
#define cHalfCysRateVolSqure0R0160Hz480V    5529600

#define cHalfCycSampNum50Hz400V             80
#define cHalfCysRateVolSqure50Hz400V        12500 
#define cHalfCysRateVolSqure0R8150Hz400V    165888000
#define cHalfCysRateVolSqure1R2150Hz400V    247808000
#define cHalfCysRateVolSqure0R250Hz400V     40960000
#define cHalfCysRateVolSqure0R0150Hz400V    4608000

#define cHalfCycSampNum60Hz400V             67
#define cHalfCysRateVolSqure60Hz400V        10417
#define cHalfCysRateVolSqure0R8160Hz400V    138240000
#define cHalfCysRateVolSqure1R2160Hz400V    206506667
#define cHalfCysRateVolSqure0R260Hz400V     34133333
#define cHalfCysRateVolSqure0R0160Hz400V    3840000
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

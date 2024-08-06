/**********************************************************************
Copyright 2009, Chint Power
File Name: I2C.h
Description:
Version: V1.0
Author: 
Data: 2009-10-27
**********************************************************************/

#ifndef I2C_H
#define I2C_H

#include "..\FunSrc\SysDefine.h"
//---------------------------------------------------------------------------
typedef union
{
    INT16S      word;
    struct
    {
        INT16U  fProt          :4;
        INT16U  fAdc           :4;
        INT16U  fHis           :4;
        INT16U  fProtSuppl     :4;
    }bits;
}T_I2CBlockMap;

typedef union
{
    INT16S      word;
    struct
    {
        INT16U  fEpImptErr          :1;
        INT16U  fEpProtErr          :1;
        INT16U  fEpAdcErr           :1;
        INT16U  fEpHisErr           :1;
        INT16U  fEpErr              :1;
        INT16U  fFrameFinished      :1;
        INT16U  fEpRdOnPwStep       :2;
        INT16U  fProtSumErr         :1;
	    INT16U  fProtRangeErr       :1;
        INT16U  fAdcSumErr          :1;
        INT16U  fAdcRangeErr        :1;
        INT16U  fHisSumErr          :1;
        INT16U  fNoStandardStatus   :1;
        INT16U  fProtSupplSumErr    :1;
        INT16U  fProtSupplRangeErr  :1;
    }bit;
}T_EepromStatus;


typedef union 
{
    INT16U      all;
    struct
    {
        INT16U  GroupNO         :4;
        INT16U  InverterNum     :4;
        INT16U  MachineVersion  :8;
    }bit;
}U_MachineCoeff;

typedef union 
{
    INT16U      all;
    struct
    {
        INT16U  RatedFre        :2;         
        INT16U  RatedVol        :6;
        INT16U  RatedPower      :4; 
        INT16U  CurrOVEn        :2;
        INT16U  PowerOVEn       :2;
    }bit;
}U_RatedCoeff;

typedef union 
{
    INT16U      all;
    struct
    {        
        INT16U  LVRTEnable     :1;
        INT16U  HVRTEnable     :1; 
        INT16U  Reserver1      :2;
        INT16U  OvFWEnable     :1;
        INT16U  UnFWEnable     :1;
        INT16U  Reserver2      :2;        
        INT16U  OvVWEnable     :1; 
        INT16U  UnVWEnable     :1; 
        INT16U  Reserver3      :2;
        INT16U  AIEnable       :1;
        INT16U  Reserver4      :3;  
    }bit;
}U_SysCtrSignal;

extern void InitI2c(void);
extern void InitI2cVar(void);
extern INT16U sfI2cTx(INT16U uwAdress,INT16S *Data);
extern INT16U sfI2cRx(INT16U uwAdress,INT16S *Data);
extern INT8U sfEepromReadOnPower(void);
extern INT8U sfEepromRunDataChange(void);
extern void sEepromProtectDataSetDefaultVaule(INT16U uwDataNum);
extern void sEepromwAdcAdjustDataSetDefaultVaule(INT16U uwDataNum);
extern void sEepromProtectSupplDataSetDefaultVaule(INT16U uwDataNum);
extern void sEepromwHisDataSetDefaultVaule(INT16U uwDataNum);
extern void sInitLCDImptData(void);
extern void sEepromHandle(void);
extern void sEEPROMProRangeChk(void);
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern INT16S wI2cRxData;
extern INT16S wI2cTxData;

extern INT16S wEepromImptData[];
extern INT16S wEepromProtectData[];
extern INT16S wEepromAdcAdjustData[];
extern INT16S wEepromProtectSupplData[]; 
extern INT16S wEepromHisData[];
extern const INT16S wEepromDefaultHisDataArray[];

extern const INT16S wEepromDefaultImptMaxArray[];
extern const INT16S wEepromDefaultImptMinArray[];


extern const INT16S wEepromDefaultProtMaxArrayIEEE1547[];
extern const INT16S wEepromDefaultProtMinArrayIEEE1547[];
extern const INT16S wEepromDefaultProtectArrayIEEE1547[];

extern const INT16S wEepromDefaultProtMaxArrayRule21[];
extern const INT16S wEepromDefaultProtMinArrayRule21[];
extern const INT16S wEepromDefaultProtectArrayRule21[];

extern const INT16S wEepromDefaultProtMaxSupplArray[];
extern const INT16S wEepromDefaultProtMinSupplArray[];
extern const INT16S wEepromDefaultProtectSupplArray[];

extern T_EepromStatus uwI2CBlockState;

extern INT16S wLCDImptData[];
extern T_FLAGOBJECT uwLCDRxImptDataFlag;

extern INT16U fEepromRdWrHalt;
extern INT16U fEepromOperateRequire;
extern INT16U fSciStandChanged;
extern INT16U fEepromChkError;
extern U_MachineCoeff fMachineCoeff;
extern U_RatedCoeff fRatedCoeff;
extern U_SysCtrSignal fSysCtrSignal;
extern INT16U fEepromProtSumErrNum;
extern INT16U fEepromProtSuplSumErrNum;
//---------------------------------------------------------------------------
// Global symbols defined in source files
//
//#define cProtNum 84
//#define cProtSupplNum 70
#define wEepromBlockMap                  wEepromImptData[0]
#define wEepromMachineConfig             wEepromImptData[1]
#define wEepromRatedConfig               wEepromImptData[2]
#define wEepromSerialNumberH             wEepromImptData[3]
#define wEepromSerialNumberM             wEepromImptData[4]                                                             
#define wEepromSerialNumberL             wEepromImptData[5]
#define wEepromStandardValue             wEepromImptData[6]


#define wEepromGridVolMax_1              wEepromProtectData[0]//170
#define wEepromVolMaxTripTime_1          wEepromProtectData[1]
#define wEepromGridVolMin_1              wEepromProtectData[2]
#define wEepromVolMinTripTime_1          wEepromProtectData[3]
#define wEepromGridVolMax_2              wEepromProtectData[4]            
#define wEepromVolMaxTripTime_2          wEepromProtectData[5] 
#define wEepromSum00                     wEepromProtectData[6]
 
#define wEepromGridVolMin_2              wEepromProtectData[7]
#define wEepromVolMinTripTime_2          wEepromProtectData[8]
#define wEepromGridVolMax_3              wEepromProtectData[9]
#define wEepromVolMaxTripTime_3          wEepromProtectData[10]
#define wEepromGridVolMin_3              wEepromProtectData[11]
#define wEepromVolMinTripTime_3          wEepromProtectData[12]
#define wEepromSum01                     wEepromProtectData[13]

#define wEepromGridVolRecover_Max        wEepromProtectData[14] 
#define wEepromGridVolRecover_Min        wEepromProtectData[15]
#define wEepromGridVolRecover_Time       wEepromProtectData[16]
#define wEepromGridVoltBalanceMax        wEepromProtectData[17]
#define wEepromGridFaultPowerStep        wEepromProtectData[18]
#define wEepromPowerOnWaitTime           wEepromProtectData[19]
#define wEepromSum02                     wEepromProtectData[20]

#define wEepromGridFreqMax_1             wEepromProtectData[21]
#define wEepromFreqMaxTripTime_1         wEepromProtectData[22]
#define wEepromGridFreqMin_1             wEepromProtectData[23]
#define wEepromFreqMinTripTime_1         wEepromProtectData[24]
#define wEepromGridFreqMax_2             wEepromProtectData[25]
#define wEepromFreqMaxTripTime_2         wEepromProtectData[26]
#define wEepromSum03                     wEepromProtectData[27]

#define wEepromGridFreqMin_2             wEepromProtectData[28]
#define wEepromFreqMinTripTime_2         wEepromProtectData[29]
#define wEepromGridFreqRecover_Max       wEepromProtectData[30]
#define wEepromGridFreqRecover_Min       wEepromProtectData[31]
#define wEepromGridFreqRecover_Time      wEepromProtectData[32]
#define wEepromSysCtrSignal              wEepromProtectData[33]
#define wEepromSum04                     wEepromProtectData[34]

#define wEepromLVRTTripVol               wEepromProtectData[35]
#define wEepromHVRTTripVol               wEepromProtectData[36]
#define wEepromFVRTPosCurrK              wEepromProtectData[37]
#define wEepromFVRTNegCurrK              wEepromProtectData[38]
#define wEepromFVRTPowerStep             wEepromProtectData[39]
#define wEepromOvFWTripFreq              wEepromProtectData[40]
#define wEepromSum05                     wEepromProtectData[41]

#define wEepromOvFWRatio                 wEepromProtectData[42]
#define wEepromOvFWExitFreq              wEepromProtectData[43]
#define wEepromUnFWTripFreq              wEepromProtectData[44] 
#define wEepromUnFWRatio                 wEepromProtectData[45]
#define wEepromUnFWExitFreq              wEepromProtectData[46]
#define wEepromFWRecPowerStep            wEepromProtectData[47]
#define wEepromSum06                     wEepromProtectData[48]

#define wEepromFWActTime                 wEepromProtectData[49] 
#define wEepromVWActTime                 wEepromProtectData[50]
#define wEepromPF_PCurveActPw1           wEepromProtectData[51]
#define wEepromPF_PCurvePF1              wEepromProtectData[52]
#define wEepromPF_PCurveActPw2           wEepromProtectData[53]
#define wEepromPF_PCurvePF2              wEepromProtectData[54]
#define wEepromSum07                     wEepromProtectData[55]

#define wEepromPF_PCurveLockInV          wEepromProtectData[56]
#define wEepromPF_PCurveLockOutV         wEepromProtectData[57]
#define wEepromQ_UCurveVolt1s            wEepromProtectData[58]
#define wEepromQ_UCurveReactPw1s         wEepromProtectData[59]
#define wEepromQ_UCurveVolt2s            wEepromProtectData[60]
#define wEepromQ_UCurveReactPw2s         wEepromProtectData[61]
#define wEepromSum08                     wEepromProtectData[62]

#define wEepromQ_UCurveVolt1i            wEepromProtectData[63]
#define wEepromQ_UCurveReactPw1i         wEepromProtectData[64]
#define wEepromQ_UCurveVolt2i            wEepromProtectData[65]
#define wEepromQ_UCurveReactPw2i         wEepromProtectData[66]
#define wEepromQ_UCurveLockInP           wEepromProtectData[67]
#define wEepromQ_UCurveLockOutP          wEepromProtectData[68]
#define wEepromSum09                     wEepromProtectData[69]

#define wEepromIsoImpMin                 wEepromProtectData[70]
#define wEepromGFCIMax                   wEepromProtectData[71]
#define wEepromDCIMax                    wEepromProtectData[72]
#define wEepromOvVWStartVolt             wEepromProtectData[73]
#define wEepromOvVWStopVolt              wEepromProtectData[74]
#define wEepromOvVWRatio                 wEepromProtectData[75]
#define wEepromSum10                     wEepromProtectData[76]

#define wEepromUnVWStartVolt             wEepromProtectData[77]
#define wEepromUnVWStopVolt              wEepromProtectData[78]
#define wEepromUnVWRatio                 wEepromProtectData[79]
#define wEepromQUActTime                 wEepromProtectData[80]
#define wEepromIsoRes                    wEepromProtectData[81]
#define wEepromUserSave1                 wEepromProtectData[82]
#define wEepromSum11                     wEepromProtectData[83]

#define wEepromRatedFreqSel              fRatedCoeff.bit.RatedFre
#define wEepromRatedVolSel               fRatedCoeff.bit.RatedVol
#define wEepromRatedPowerSel             fRatedCoeff.bit.RatedPower
#define wEepromCurrOVEn                  fRatedCoeff.bit.CurrOVEn
#define wEepromPowerOVEn                 fRatedCoeff.bit.PowerOVEn
#define wEepromMachineVersion            fMachineCoeff.bit.MachineVersion
#define wEepromGroupNO                   fMachineCoeff.bit.GroupNO
#define wEepromInverterNum               fMachineCoeff.bit.InverterNum

#define wEepromAIEnable                  fSysCtrSignal.bit.AIEnable                 
#define wEepromLVRTOption                fSysCtrSignal.bit.LVRTEnable
#define wEepromHVRTOption                fSysCtrSignal.bit.HVRTEnable
#define wEepromOvFWEn                    fSysCtrSignal.bit.OvFWEnable
#define wEepromUnFWEn                    fSysCtrSignal.bit.UnFWEnable               
#define wEerpomOvVWEnable                fSysCtrSignal.bit.OvVWEnable
#define wEerpomUnVWEnable                fSysCtrSignal.bit.UnVWEnable

#define wEepromChargeVoltLimt            wEepromProtectSupplData[0]//26
#define wEepromDisChargeVoltLimt         wEepromProtectSupplData[1]
#define wEepromChargeCurrLimt            wEepromProtectSupplData[2]
#define wEepromDisChargeCurrLimt         wEepromProtectSupplData[3]
#define wEepromFloatChargeVolt           wEepromProtectSupplData[4]
#define wEepromSToFChargeCurr            wEepromProtectSupplData[5]//恒流转浮充电流
#define wEepromSupplSum01                wEepromProtectSupplData[6]

#define wEepromBatteryVoltMax            wEepromProtectSupplData[7]//电池电压最上限，保护点
#define wEepromBatteryVoltMin            wEepromProtectSupplData[8]
#define wEepromBatChargeCurrMax          wEepromProtectSupplData[9]//电池充电电流最上限，保护点
#define wEepromBatDisChargeCurrMax       wEepromProtectSupplData[10]//电池放电电流最下限，保护点
#define wEepromDCRelayVoltDiff           wEepromProtectSupplData[11]//直流Relay允许的电压差
#define wEepromBatPreChargeCurrMax       wEepromProtectSupplData[12]
#define wEepromSupplSum02                wEepromProtectSupplData[13]

#define wEepromRemoteSel                 wEepromProtectSupplData[14]
#define wEepromCSVSSwitch                wEepromProtectSupplData[15]
#define wEepromAutoCSVSSwitchEn          wEepromProtectSupplData[16]
#define wEepromVSCtrMode                 wEepromProtectSupplData[17]//电压源控制模式
#define wEepromVSVoltSet                 wEepromProtectSupplData[18]
#define wEepromVSFreqSet                 wEepromProtectSupplData[19]
#define wEepromSupplSum03                wEepromProtectSupplData[20]

#define wEepromVSPSet                    wEepromProtectSupplData[21]
#define wEepromVSQSet                    wEepromProtectSupplData[22]
#define wEepromVSDroopEn                 wEepromProtectSupplData[23]
#define wEepromVSFreqDroopKByP           wEepromProtectSupplData[24] //VS 0.01Hz 100% P <==> Hz , 100 mean 100%P droop 1Hz
#define wEepromVSVoltDroopKByQ           wEepromProtectSupplData[25] //VS 0.1%  100% Q <==> x% Voltage, 100 mean 100% Q droop 10% rated voltage
#define wEepromCSPCtrModeSel             wEepromProtectSupplData[26]
#define wEepromSupplSum04                wEepromProtectSupplData[27]

#define wEepromCSPPer                    wEepromProtectSupplData[28]
#define wEepromCSCurrSet                 wEepromProtectSupplData[29]
#define wEepromPActStep                  wEepromProtectSupplData[30]
#define wEepromCSQCtrModeSel             wEepromProtectSupplData[31]
#define wEepromCSPFSet                   wEepromProtectSupplData[32]
#define wEepromCSQPer                    wEepromProtectSupplData[33]
#define wEepromSupplSum05                wEepromProtectSupplData[34]

#define wEepromQActStep                  wEepromProtectSupplData[35]
#define wEepromSoftPowerStep             wEepromProtectSupplData[36]
#define wEepromSoftStopEn                wEepromProtectSupplData[37]
#define wEepromStopPowerStep             wEepromProtectSupplData[38]
#define wEepromPVStartUpVol              wEepromProtectSupplData[39]
#define wEepromPVFitFactor               wEepromProtectSupplData[40]
#define wEepromSupplSum06                wEepromProtectSupplData[41]

#define wEepromISOEnable                 wEepromProtectSupplData[42] 
#define wEepromGFCICTChkEn               wEepromProtectSupplData[43]
#define wEepromAPFOption                 wEepromProtectSupplData[44]
#define wEepromArcingEn                  wEepromProtectSupplData[45]
#define wEepromFullRangeMpptEnable       wEepromProtectSupplData[46]
#define wEepromFullRangeMpptCycle        wEepromProtectSupplData[47]
#define wEepromSupplSum07                wEepromProtectSupplData[48]

#define wEepromInvCurrMax                wEepromProtectSupplData[49]  //逆变电流上限 
#define wEepromDCIOffsetDiffMax          wEepromProtectSupplData[50] //DCI偏置上限 
#define wEepromOffsetDiffMax             wEepromProtectSupplData[51] //逆变电流偏置上限
#define wEepromEnvironmentTmpMax         wEepromProtectSupplData[52] //环境温度上限 
#define wEepromPowerTmpMax               wEepromProtectSupplData[53] //模块温度上限 
#define wEepromXFMRTmpMax                wEepromProtectSupplData[54] //电感温度上限
#define wEepromSupplSum08                wEepromProtectSupplData[55]

#define wEepromTransformerTmpMax         wEepromProtectSupplData[56] //变压器温度上限
#define wEepromBusVolMax                 wEepromProtectSupplData[57]
#define wEepromCapVolMax                 wEepromProtectSupplData[58]
#define wEepromDCRelayActProt            wEepromProtectSupplData[59]
#define wEepromBiDirDCSwitch             wEepromProtectSupplData[60]
#define wEepromBatteryType               wEepromProtectSupplData[61]
#define wEepromSupplSum09                wEepromProtectSupplData[62]

#define wEepromClientSave4               wEepromProtectSupplData[63]
#define wEepromClientSave5               wEepromProtectSupplData[64]
#define wEepromClientSave6               wEepromProtectSupplData[65]
#define wEepromClientSave7               wEepromProtectSupplData[66]
#define wEepromClientSave8               wEepromProtectSupplData[67]
#define wEepromClientSave9               wEepromProtectSupplData[68]
#define wEepromSupplSum10                wEepromProtectSupplData[69]


#define wEepromPOBusVolScale             wEepromAdcAdjustData[0] //100    
#define wEepromPOBusVolOffset            wEepromAdcAdjustData[1]     
#define wEepromONBusVolScale             wEepromAdcAdjustData[2]     
#define wEepromONBusVolOffset            wEepromAdcAdjustData[3]  
#define wEepromBatteryCurr1Scale         wEepromAdcAdjustData[4]     
#define wEepromBatteryCurr1Offset        wEepromAdcAdjustData[5]                                        
#define wEepromADCofeeSum00              wEepromAdcAdjustData[6]  
#define wEepromBatteryCurr2Scale         wEepromAdcAdjustData[7]  
#define wEepromBatteryCurr2Offset        wEepromAdcAdjustData[8]  
#define wEepromBatteryCurr3Scale         wEepromAdcAdjustData[9]  
#define wEepromBatteryCurr3Offset        wEepromAdcAdjustData[10]
#define wEepromBatteryCurr4Scale         wEepromAdcAdjustData[11]
#define wEepromBatteryCurr4Offset        wEepromAdcAdjustData[12]
#define wEepromADCofeeSum01              wEepromAdcAdjustData[13]
#define wEepromRSGridVolScale            wEepromAdcAdjustData[14]   
#define wEepromRSGridVolOffset           wEepromAdcAdjustData[15]   
#define wEepromSTGridVolScale            wEepromAdcAdjustData[16]   
#define wEepromSTGridVolOffset           wEepromAdcAdjustData[17]   
#define wEepromTRGridVolScale            wEepromAdcAdjustData[18]   
#define wEepromTRGridVolOffset           wEepromAdcAdjustData[19]   
#define wEepromADCofeeSum02              wEepromAdcAdjustData[20]   
#define wEepromRInvVolScale              wEepromAdcAdjustData[21]   
#define wEepromRInvVolOffset             wEepromAdcAdjustData[22]   
#define wEepromSInvVolScale              wEepromAdcAdjustData[23]   
#define wEepromSInvVolOffset             wEepromAdcAdjustData[24]  
#define wEepromTInvVolScale              wEepromAdcAdjustData[25]  
#define wEepromTInvVolOffset             wEepromAdcAdjustData[26]  
#define wEepromADCofeeSum03              wEepromAdcAdjustData[27]  
#define wEepromRDCICurrScale             wEepromAdcAdjustData[28]  
#define wEepromRDCICurrOffset            wEepromAdcAdjustData[29]  
#define wEepromSDCICurrScale             wEepromAdcAdjustData[30]  
#define wEepromSDCICurrOffset            wEepromAdcAdjustData[31]  
#define wEepromTDCICurrScale             wEepromAdcAdjustData[32]  
#define wEepromTDCICurrOffset            wEepromAdcAdjustData[33] 
#define wEepromADCofeeSum04              wEepromAdcAdjustData[34]  
#define wEepromRInvCurrScale             wEepromAdcAdjustData[35]  
#define wEepromRInvCurrOffset            wEepromAdcAdjustData[36]  
#define wEepromSInvCurrScale             wEepromAdcAdjustData[37]  
#define wEepromSInvCurrOffset            wEepromAdcAdjustData[38] 
#define wEepromTInvCurrScale             wEepromAdcAdjustData[39]  
#define wEepromTInvCurrOffset            wEepromAdcAdjustData[40] 
#define wEepromADCofeeSum05              wEepromAdcAdjustData[41] 
#define wEepromIsoVolScale               wEepromAdcAdjustData[42]     
#define wEepromIsoVolOffset              wEepromAdcAdjustData[43]
#define wEepromBatteryOutVolScale        wEepromAdcAdjustData[44] 
#define wEepromBatteryOutVolOffset       wEepromAdcAdjustData[45]     
#define wEepromBatteryInVolScale         wEepromAdcAdjustData[46]     
#define wEepromBatteryInVolOffset        wEepromAdcAdjustData[47]           
#define wEepromADCofeeSum06              wEepromAdcAdjustData[48] 
#define wEepromGFCICurrScale             wEepromAdcAdjustData[49]       
#define wEepromGFCICurrOffset            wEepromAdcAdjustData[50]
#define wEepromEnvOffset                 wEepromAdcAdjustData[51]
#define wEepromHeatOffset                wEepromAdcAdjustData[52]
#define wEepromXFMROffset                wEepromAdcAdjustData[53]    
#define wEepromTfTmpOffset               wEepromAdcAdjustData[54]   
#define wEepromADCofeeSum07              wEepromAdcAdjustData[55]
#define wEepromSave1                     wEepromAdcAdjustData[56]
#define wEepromSave2                     wEepromAdcAdjustData[57]
#define wEepromSave3                     wEepromAdcAdjustData[58]
#define wEepromSave4                     wEepromAdcAdjustData[59]
#define wEepromSave5                     wEepromAdcAdjustData[60]
#define wEepromSave6                     wEepromAdcAdjustData[61]
#define wEepromADCofeeSum08              wEepromAdcAdjustData[62]  
#define wEepromSave7                     wEepromAdcAdjustData[63]
#define wEepromSave8                     wEepromAdcAdjustData[64]
#define wEepromSave9                     wEepromAdcAdjustData[65]
#define wEepromSave10                    wEepromAdcAdjustData[66]
#define wEepromSave11                    wEepromAdcAdjustData[67]
#define wEepromSave12                    wEepromAdcAdjustData[68]
#define wEepromADCofeeSum09              wEepromAdcAdjustData[69]  

#define wEepromHisPVLinkStatus           wEepromHisData[0]      // 历史PV连接方式  =0,连接状态未知；=0x1111端口设置为并联状态；=0x2222端口设置为独立连接

// 历史ArcBoard状态，=0xaaaa,ArcBoard OK，=0xbbbb ArcBoard Error，其余值代表Eeprom未被操作，认为ArcBoard OK
#define wEepromHisArcBoardStatus	     wEepromHisData[1]
// 历史Arcing状态，=0xaaaa => No Arcing occur，=0xbbbb => Arcing Error and don't clear it; =0xcccc => Arcing occur and have Cleared it(其余值代表Eeprom未被操作，认为No Arcing occur)
#define wEepromHisArcingStatus	         wEepromHisData[2]
#define wEepromHisSave1     	         wEepromHisData[3]      // 历史保留数据
#define wEepromHisSave2	                 wEepromHisData[4]      // 历史保留数据
#define wEepromHisSave3	                 wEepromHisData[5]      // 历史保留数据
#define wEempromHisSum00	             wEepromHisData[6]      // 校验和0                                
#define sI2CSwRst()                         {I2caRegs.I2CMDR.bit.IRS = 0;DelayUs(100);I2caRegs.I2CMDR.bit.IRS = 1;}
#endif  // end of I2C_H definition

/*** end of file *****************************************************/

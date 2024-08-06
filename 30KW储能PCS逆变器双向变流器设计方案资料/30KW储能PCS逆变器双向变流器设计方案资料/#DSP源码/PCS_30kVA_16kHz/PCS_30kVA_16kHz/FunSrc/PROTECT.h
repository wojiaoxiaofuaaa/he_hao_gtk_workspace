/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef PROTECT_H
#define PROTECT_H
#include "..\FunSrc\SysDefine.h"

typedef struct
{
    INT16U    Flag0;
    INT16U    Flag1;
    INT16U    Flag2;
    INT16U    Flag3;
    INT16U    Flag4;
    INT16U    Flag5;
    INT16U    Flag6;
    INT16U    Flag7;
    INT16U    Flag8;
    INT16U    Flag9;
    INT16U    Flag10;
    INT16U    Flag11;
    INT16U    Flag12;
    INT16U    Flag13;
    INT16U    Flag14;
    INT16U    Flag15;
    INT16U    Flag16;
    INT16U    Flag17;
    INT16U    Flag18;
    INT16U    Flag19;
    INT16U    Flag20;
    INT16U    Flag21;
    INT16U    Flag22;
    INT16U    Flag23;
    INT16U    Flag24;
    INT16U    Flag25;
    INT16U    Flag26;
    INT16U    Flag27;
    INT16U    Flag28;
    INT16U    Flag29;
    INT16U    Flag30;
    INT16U    Flag31;
    INT16U    Flag32;
    INT16U    Flag33;
    INT16U    Flag34;
	INT16U    Flag35;
	INT16U    Flag36;
	INT16U    Flag37;
	INT16U    Flag38;
	INT16U    Flag39;
	INT16U    Flag40;
	INT16U    Flag41;
	INT16U    Flag42;
	INT16U    Flag43;
	INT16U    Flag44;
	INT16U    Flag45;
	INT16U    Flag46;
	INT16U    Flag47;
	INT16U    Flag48;
	INT16U    Flag49;
	INT16U    Flag50;
	INT16U    Flag51;
	INT16U    Flag52;
	INT16U    Flag53;
	INT16U    Flag54;
	INT16U    Flag55;
	INT16U    Flag56;
	INT16U    Flag57;
	INT16U    Flag58;
	INT16U    Flag59;
	INT16U    Flag60;
	INT16U    Flag61;
	INT16U    Flag62;
	INT16U    Flag63;
	INT16U    Flag64;
}T_PROTECTOBJECT;

//---------------------------------------------------------------------------
// Function Prototypes
//
extern void InitProtectVar(void);
extern void sErroringDeal(void);
extern void sSpiChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sBusVolHighChk(INT16S *pBusVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sBatVolHighChk(INT16S *pBatOutVol,INT16S *pBatInVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sVCapHighChk(INT16S *pPOVol,INT16S *pONVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sVCapDiffChk(INT16S *pPOVol,INT16S *pONVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sLineRmsVolChk(INT16S *pRSRmsVol,INT16S *pSTRmsVol,INT16S *pTRRmsVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sPhaseRmsCurChk(INT16S *pRRmsCur,INT16S *pSRmsCur,INT16S *pTRmsCur,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sInvCurrHighChk(INT16S *pRInvCurr,INT16S *pSInvCurr,INT16S *pTInvCurr,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sBatteryCurrHighChk(INT16S wDCCurr,INT16U *pHighCnt,INT16U *pOKCnt,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sFreHighChk(INT16U *pRFreq,INT16U *pSFreq,INT16U *pTFreq,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sFreLowChk(INT16U *pRFreq,INT16U *pSFreq,INT16U *pTFreq,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sAIFreqChk(INT16U *pRFreq,INT16U *pSFreq,INT16U *pTFreq,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sGridCurrOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCIHighChk(INT16S *pRDCI,INT16S *pSDCI,INT16S *pTDCI,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCIOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sGFCISteadyHighChk(INT16S *pGFCIRmsCurr,INT16S *pGFCIAvgCurr,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sGFCIDynamicHighChk(INT16S wGFCCurrRms,INT16S wGFCCurrAvg,INT16U *pErrFlag,INT16U *pEventFlag);
extern INT16S swGFCICoeffCal(INT16S wCurr);
extern void sTmpHighChk(INT16S *pHeatTmp,INT16S *pEnvTmp,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sEepromChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sGridNoUtilityChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern INT16U sfRelayChk(INT16U *pGridErrFlag,INT16U *pGridEventFlag,INT16U *pInvErrFlag,INT16U *pInvEventFlag);
extern INT16U sfRelayInitStateChk(INT16U *pGridErrFlag,INT16U *pGridEventFlag,INT16U *pInvErrFlag,INT16U *pInvEventFlag);
extern INT16U sfIsoChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern INT16U suwGetFaultGrade(void);
extern void sErrFaultDeal(void);
extern void sTZStateClear(void);
extern void sGridVoltBalanceDegreeChk(INT16S *pBalanceDegree,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sIGBTHaltStateChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sMcuFdbChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCRelayOpenAct(void);
extern void sFanFdbChk(INT16U *pFanStatus2,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sPowerDisMatchChk(INT32S *pPowerDCSide,INT32S *pPowerACSide,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sTmpLoseChk(INT16S *pHeatTemp,INT16S *pEnvTemp,INT16U *pErrFlag,INT16U *pEventFlag);
extern INT16U sfInvVolNormalChk(T_THREE_PHASE_ABC tInvPhaseAvgVol,T_THREE_PHASE_ABC tInvPhaseRmsVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sBusOVPChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sACOCPStateChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCOCPStateChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCRelayChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sSpdChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sCtrPowerChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sFuseStateChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sAuxiRelayChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sBatRevConnectChk(INT16S *pBatVolt,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sGridVolAndInvVolDiffChk(T_THREE_PHASE_ABC tGridPhaseVol,T_THREE_PHASE_ABC tInvPhaseVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sCANActiveChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern INT16U sfGFCICTChk(INT16S wGFCICTCurr,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDecCapEnergyChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCPWMChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCSoftRelayChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sRamChk(void);
extern void sFlashChk(void);
extern void sBatteryCoeffChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCCurrBalanceChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sCPLDOscilChk(INT16U *pErrFlag,INT16U *pEventFlag);
extern void sDCSwitchChk(INT16S wBatInVol,INT16U *pErrFlag,INT16U *pEventFlag);
extern void sBusDiffBatVolChk(INT16S *pBusVol,INT16S *pBatInVol,INT16U *pErrFlag,INT16U *pEventFlag);
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern T_PROTECTOBJECT tErrFlag,tEventFlag;
extern INT16U uwWarrningCode,uwFaultCode[],uwPFCode;
extern INT16U uwFaultGrade;
extern INT16U uwfIsoChkFinished,uwfIsoChkOk;
extern INT16U fRelayCheckStep;
extern INT16U uwInvRelayOKCnt;
extern INT16U uwInvRelayBadCnt;
extern INT16U uwGridRelayOKCnt;
extern INT16U uwGridRelayBadCnt;
extern INT16U uwAuxiRelayOKCnt;
extern INT16U uwAuxiRelayBadCnt;
extern INT16U fGridRelayFaultEvent,fOCPFaultEvent,fInvCurrHighFaultEvent,fPhaseRmsCurFaultEvent,fInvRelayFaultEvent;
extern INT16U fRunInvAhdFaultEvent,fDCIHighFaultEvent,fDampResistanceFaultEvent;
extern INT16U fTempdisable;
extern INT16U uwInvVolNormalChkOk,uwGridAndInvVolChkOk;
extern INT16U fDCIDelayClear;
extern INT16U fRemoteDCBreakerCtrEn;
extern INT16U uwIsoChkStep;
extern INT16S wIsoImp;
extern INT16U fFanFaultStatus;
extern INT16U fDCRelayBurn;
extern INT16U fDCPWMLost;
extern INT16U fDCSoftRelayLost;
extern INT16S wDebugFlashSum;
extern INT32S dwGFCIAvgActSum;
extern INT32S dwGFCIRmsActSum;
//---------------------------------------------------------------------------

// Global symbols defined in source files
//
#define fFanErr                                tErrFlag.Flag0    // FAN1错误      
#define fFanErrEvent                           tEventFlag.Flag0  // FAN1错误事件  

#define fGridVoltBalanceDegreeErr              tErrFlag.Flag1    // 电网不平衡故障                                       
#define fGridVoltBalanceDegreeErrEvent         tEventFlag.Flag1  // 电网不平衡故障事?
  
#define fGFCISteadyErr                         tErrFlag.Flag2    // 稳态漏电流错误 
#define fGFCISteadyErrEvent                    tEventFlag.Flag2  // 稳态漏缌鞔砦笫录?
 
#define fEepromErr                             tErrFlag.Flag3    // Eeprom错误     
#define fEepromErrEvent                        tEventFlag.Flag3  // Eeprom错误事件  
                                               
#define fTempSensorErr                         tErrFlag.Flag4     // 温度传感器异常       
#define fTempSensorErrEvent                    tEventFlag.Flag4   // 温度传感器异常事件   

#define fRunInvAhdErr                          tErrFlag.Flag5     // 开机逆变自检错误  
#define fRunInvAhdErrEvent                     tEventFlag.Flag5   // 开机逆湓检错误事件  

#define fPowerDisMatchErr                      tErrFlag.Flag6     // owerIn!=PowerOut错误  
#define fPowerDisMatchEvent                    tEventFlag.Flag6   // PowerIn!=PowerOut错误事件  

#define fBatteryCoeffErr                       tErrFlag.Flag7     // 电池参数设置错误
#define fBatteryCoeffErrEvent                  tEventFlag.Flag7   // 电池参数设置错误事件
 
#define fBusHighErr                            tErrFlag.Flag8     // PV电压过高错误       
#define fBusHighErrEvent                       tEventFlag.Flag8   // PV电压过高错误事件  
 
#define fBatteryCurr1Err                       tErrFlag.Flag9     // 第1路双向DC/DC过流    
#define fBatteryCurr1ErrEvent                  tEventFlag.Flag9   // 第1路双向DC/DC过流 事件

#define fLineGridRmsVolErr                     tErrFlag.Flag10    // 电网线电压有效值错误        
#define fLineGridRmsVolErrEvent                tEventFlag.Flag10  // 电网线电压有效值错误事件  
  
#define fVCapHighErr                           tErrFlag.Flag11    // 半BUS电压高     
#define fVCapHighErrEvent                      tEventFlag.Flag11  // 半BUS电压高事件 
 
#define fInvCurrHighErr                        tErrFlag.Flag12    // 相电流瞬时值过高错误       
#define fInvCurrHighErrEvent                   tEventFlag.Flag12  // 相电流瞬时值过高错误事件  
 
#define fFreHighErr                            tErrFlag.Flag13    // 相频率过高错误       
#define fFreHighErrEvent                       tEventFlag.Flag13  // 相频率过高错误事件   

#define fFreLowErr                             tErrFlag.Flag14    // 相频率过低错误       
#define fFreLowErrEvent                        tEventFlag.Flag14  // 相频率过低错误事件  
 
#define fNoUtilityErr                          tErrFlag.Flag15    // 电网掉电错误        
#define fNoUtilityErrEvent                     tEventFlag.Flag15  // 电网掉电错误事件  
  
#define fGridRelayErr                          tErrFlag.Flag16    // Relay错误     
#define fGridRelayErrEvent                     tEventFlag.Flag16  // Relay错误事件 

#define fOTErr                                 tErrFlag.Flag17    // 过温错误       
#define fOTErrEvent                            tEventFlag.Flag17  // 过温错误事件 

#define fGFCICurrOffsetErr                     tErrFlag.Flag18    // 漏电流偏置过高错误        
#define fGFCICurrOffsetErrEvent                tEventFlag.Flag18  // 漏电流偏置过高错误事件 

#define fRInvCurrOffsetErr                     tErrFlag.Flag19    // R相电流偏置超限错误       
#define fRInvCurrOffsetErrEvent                tEventFlag.Flag19  // R相电流偏置超限错误事件 
  
#define fSInvCurrOffsetErr                     tErrFlag.Flag20    // S相电流偏置超限错误        
#define fSInvCurrOffsetErrEvent                tEventFlag.Flag20  // S相电流偏置超限错误事件 
  
#define fTInvCurrOffsetErr                     tErrFlag.Flag21    // T相电流偏置超限错误     
#define fTInvCurrOffsetErrEvent                tEventFlag.Flag21  // T相电流偏置超限错误事件 

#define fInvRelayErr                           tErrFlag.Flag22    // 逆变侧Relay错误      
#define fInvRelayErrEvent                      tEventFlag.Flag22  // 逆变侧Relay错误事件 
 
#define fBatteryCurr2Err                       tErrFlag.Flag23    // 第2路双向DC/DC过流      
#define fBatteryCurr2ErrEvent                  tEventFlag.Flag23  // 第2路双向DC/DC过流事件 
   
#define fBatteryCurr3Err                       tErrFlag.Flag24    // 第3路双向DC/DC过流      
#define fBatteryCurr3ErrEvent                  tEventFlag.Flag24  // 第3路双向DC/DC过流事件  

#define fRDCICurrOffsetErr                     tErrFlag.Flag25    // RS线逆变电压偏置超限错误       
#define fRDCICurrOffsetErrEvent                tEventFlag.Flag25  // RS线逆变电压偏置超限错误事件 
  
#define fSDCICurrOffsetErr                     tErrFlag.Flag26    // ST线逆变电压偏置超限错误        
#define fSDCICurrOffsetErrEvent                tEventFlag.Flag26  // ST线逆变电压偏置超限错误事件  
  
#define fTDCICurrOffsetErr                     tErrFlag.Flag27    // TR线逆变电压偏置超限错误     
#define fTDCICurrOffsetErrEvent                tEventFlag.Flag27  // TR线逆变电压偏置超限错误事件 
   
#define fDCIHighErr                            tErrFlag.Flag28    // 直流分量过高错误        
#define fDCIHighErrEvent                       tEventFlag.Flag28  // 直流分量过高错误事件 
   
#define fIsoLowErr                             tErrFlag.Flag29    // 绝缘阻抗过低错误     
#define fIsoLowErrEvent                        tEventFlag.Flag29  // 绝缘阻抗过低错误事件 

#define fGFCIHighErr                           tErrFlag.Flag30    // 漏电流过高错误       
#define fGFCIHighErrEvent                      tEventFlag.Flag30  // 漏电流过高错误事件 

#define fAuxiRelayErr                          tErrFlag.Flag31    // 系统频率检测值错误  
#define fAuxiRelayErrEvent                     tEventFlag.Flag31  // 系统频率检测值错误事件

#define fInvCurrOffsetErr                      tErrFlag.Flag32    //(fRInvCurrOffsetErr | fSInvCurrOffsetErr | fTInvCurrOffsetErr)
#define fInvCurrOffsetErrEvent                 tEventFlag.Flag32  //(fRInvCurrOffsetErrEvent | fSInvCurrOffsetErrEvent | fTInvCurrOffsetErrEvent)

#define fDCICurrOffsetErr                      tErrFlag.Flag33    //(fRDCICurrOffsetErr | fSDCICurrOffsetErr | fTDCICurrOffsetErr)
#define fDCICurrOffsetErrEvent                 tEventFlag.Flag33  //(fRDCICurrOffsetErrEvent | fSDCICurrOffsetErrEvent | fTDCICurrOffsetErrEvent)

#define fGridCurrOffsetErr                     tErrFlag.Flag34    //(fRInvCurrOffsetErr | fSInvCurrOffsetErr | fTInvCurrOffsetErr)
#define fGridCurrOffsetErrEvent                tEventFlag.Flag34 

#define fDCRelayErr                            tErrFlag.Flag35    // DC Breaker跳出保护
#define fDCRelayErrEvent                       tEventFlag.Flag35  // DC Breaker跳出保护事件

#define fGFCICTErr                             tErrFlag.Flag36    // PV启动电压不足
#define fGFCICTErrEvent                        tEventFlag.Flag36  // PV启动电压不足事件

#define fBattRevConnectErr                     tErrFlag.Flag37    // PV反接错误  
#define fBattRevConnectErrEvent                tEventFlag.Flag37  // PV反接错误事件 

#define fAIFreqErr                             tErrFlag.Flag38    // 孤岛频率保护 
#define fAIFreqErrEvent                        tEventFlag.Flag38  // 孤岛频率保护事件

#define fPhaseRmsCurErr                        tErrFlag.Flag39    // 逆变电流不平衡错误     
#define fPhaseRmsCurErrEvent                   tEventFlag.Flag39  // 逆变电流不平衡错误事件 

#define fIGBTHaltErr                           tErrFlag.Flag40    // IGBTHalt1     
#define fIGBTHaltErrEvent                      tEventFlag.Flag40  // IGBTHalt1事件

#define fMcuActDspNoActErr                     tErrFlag.Flag41    // MCU动作DSP没动作错误   
#define fMcuActDspNoActErrEvent                tEventFlag.Flag41  // MCU动作DSP没动作错误事件

#define fGridAndInvVolDiffErr                  tErrFlag.Flag42    // 网侧逆变器侧电压差错误   
#define fGridAndInvVolDiffErrEvent             tEventFlag.Flag42  // 网侧逆变器侧电压差错误事件

#define fOVPErr                                tErrFlag.Flag43    // 硬件过压保护   
#define fOVPErrEvent                           tEventFlag.Flag43  // 硬件过压保护事件

#define fOCPErr                                tErrFlag.Flag44    // 硬件过流保护
#define fOCPErrEvent                           tEventFlag.Flag44  // 硬件过流保护事?

#define fDecCapEnergyErr                       tErrFlag.Flag45    // 执行快速放电时产生故障
#define fDecCapEnergyErrEvent                  tEventFlag.Flag45  // 执行快速放电时产生故障 

#define fIGBTHalt3Err                          tErrFlag.Flag46    // IGBTHalt3 
#define fIGBTHalt3ErrEvent                     tEventFlag.Flag46  // IGBTHalt3事件      

#define fRGridCurrOffsetErr                    tErrFlag.Flag47    // R相电流偏置超限错误       
#define fRGridCurrOffsetErrEvent               tEventFlag.Flag47  // R相电流偏置超限错误事件   

#define fSGridCurrOffsetErr                    tErrFlag.Flag48    // S相电流偏置超限错误        
#define fSGridCurrOffsetErrEvent               tEventFlag.Flag48  // S相电流偏置超限错误   

#define fTGridCurrOffsetErr                    tErrFlag.Flag49    // T相电流贸薮砦?    
#define fTGridCurrOffsetErrEvent               tEventFlag.Flag49  // T相电流偏置超限错误事件                                                

#define fVCapDiffErr                           tErrFlag.Flag50    // BUS差过大
#define fVCapDiffErrEvent                      tEventFlag.Flag50  // BUS差过大事件

#define fSpdErr                                tErrFlag.Flag51    // 输入输出防雷器异常
#define fSpdErrEvent                           tEventFlag.Flag51  // 输入输出防雷器异常事件

#define fDCOCPErr                              tErrFlag.Flag52    //IGBTPowerHalt
#define fDCOCPErrEvent                         tEventFlag.Flag52  //IGBTPowerHalt事件

#define fBatVolHighErr                         tErrFlag.Flag53    //系统48V电源异常
#define fBatVolHighErrEvent                    tEventFlag.Flag53  //系统48V电源异常事件

#define fCtrPowerErr                           tErrFlag.Flag54    //控制15V电源异常
#define fCtrPowerErrEvent                      tEventFlag.Flag54  //控制15V电源异常事件

#define fFuseErr                               tErrFlag.Flag55    //输入熔断器异常
#define fFuseErrEvent                          tEventFlag.Flag55  //输入熔断器异常事件

#define fDampResistanceErr                     tErrFlag.Flag56    //谐振电阻烧毁
#define fDampResistanceErrEvent                tEventFlag.Flag56  //谐振电阻烧毁异常事件

#define fCANNoAcitveErr                        tErrFlag.Flag57    //与LCD通信CAN异常
#define fCANNoAcitveErrEvent                   tEventFlag.Flag57  //与LCD通信CAN异常事件

#define fBatteryCurr4Err                       tErrFlag.Flag58    //第4路双向DC/DC过流
#define fBatteryCurr4ErrEvent                  tEventFlag.Flag58  //第4路双向DC/DC过流事件

#define fDCPWMErr                              tErrFlag.Flag59    //直流驱动丢失
#define fDCPWMErrEvent                         tEventFlag.Flag59  //直流驱动丢失事件 

#define fDCSoftRelayErr                        tErrFlag.Flag60    //直流软起电路异常
#define fDCSoftRelayErrEvent                   tEventFlag.Flag60  //直流软起电路异常事件 

#define fDCCurrBalanceErr                      tErrFlag.Flag61    //直流电流不平衡
#define fDCCurrBalanceErrEvent                 tEventFlag.Flag61  //直流电流不平衡事件

#define fCPLDOsciErr                           tErrFlag.Flag62    //CPLDOscLost
#define fCPLDOsciErrEvent                      tEventFlag.Flag62  //CPLDOscLost事件

#define fDCSwitchErr                           tErrFlag.Flag63    //DCSwitch断开
#define fDCSwitchErrEvent                      tEventFlag.Flag63  //DCSwitch断开事件

#define fBusDiffBatVolErr                      tErrFlag.Flag64    //BUS电压低于内部电池电压
#define fBusDiffBatVolErrEvent                 tEventFlag.Flag64  //BUS电压低于内部电池电压事件
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

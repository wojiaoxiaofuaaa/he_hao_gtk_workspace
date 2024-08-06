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
#define fFanErr                                tErrFlag.Flag0    // FAN1����      
#define fFanErrEvent                           tEventFlag.Flag0  // FAN1�����¼�  

#define fGridVoltBalanceDegreeErr              tErrFlag.Flag1    // ������ƽ�����                                       
#define fGridVoltBalanceDegreeErrEvent         tEventFlag.Flag1  // ������ƽ�������?
  
#define fGFCISteadyErr                         tErrFlag.Flag2    // ��̬©�������� 
#define fGFCISteadyErrEvent                    tEventFlag.Flag2  // ��̬©��������¼?
 
#define fEepromErr                             tErrFlag.Flag3    // Eeprom����     
#define fEepromErrEvent                        tEventFlag.Flag3  // Eeprom�����¼�  
                                               
#define fTempSensorErr                         tErrFlag.Flag4     // �¶ȴ������쳣       
#define fTempSensorErrEvent                    tEventFlag.Flag4   // �¶ȴ������쳣�¼�   

#define fRunInvAhdErr                          tErrFlag.Flag5     // ��������Լ����  
#define fRunInvAhdErrEvent                     tEventFlag.Flag5   // �������Լ�����¼�  

#define fPowerDisMatchErr                      tErrFlag.Flag6     // owerIn!=PowerOut����  
#define fPowerDisMatchEvent                    tEventFlag.Flag6   // PowerIn!=PowerOut�����¼�  

#define fBatteryCoeffErr                       tErrFlag.Flag7     // ��ز������ô���
#define fBatteryCoeffErrEvent                  tEventFlag.Flag7   // ��ز������ô����¼�
 
#define fBusHighErr                            tErrFlag.Flag8     // PV��ѹ���ߴ���       
#define fBusHighErrEvent                       tEventFlag.Flag8   // PV��ѹ���ߴ����¼�  
 
#define fBatteryCurr1Err                       tErrFlag.Flag9     // ��1·˫��DC/DC����    
#define fBatteryCurr1ErrEvent                  tEventFlag.Flag9   // ��1·˫��DC/DC���� �¼�

#define fLineGridRmsVolErr                     tErrFlag.Flag10    // �����ߵ�ѹ��Чֵ����        
#define fLineGridRmsVolErrEvent                tEventFlag.Flag10  // �����ߵ�ѹ��Чֵ�����¼�  
  
#define fVCapHighErr                           tErrFlag.Flag11    // ��BUS��ѹ��     
#define fVCapHighErrEvent                      tEventFlag.Flag11  // ��BUS��ѹ���¼� 
 
#define fInvCurrHighErr                        tErrFlag.Flag12    // �����˲ʱֵ���ߴ���       
#define fInvCurrHighErrEvent                   tEventFlag.Flag12  // �����˲ʱֵ���ߴ����¼�  
 
#define fFreHighErr                            tErrFlag.Flag13    // ��Ƶ�ʹ��ߴ���       
#define fFreHighErrEvent                       tEventFlag.Flag13  // ��Ƶ�ʹ��ߴ����¼�   

#define fFreLowErr                             tErrFlag.Flag14    // ��Ƶ�ʹ��ʹ���       
#define fFreLowErrEvent                        tEventFlag.Flag14  // ��Ƶ�ʹ��ʹ����¼�  
 
#define fNoUtilityErr                          tErrFlag.Flag15    // �����������        
#define fNoUtilityErrEvent                     tEventFlag.Flag15  // ������������¼�  
  
#define fGridRelayErr                          tErrFlag.Flag16    // Relay����     
#define fGridRelayErrEvent                     tEventFlag.Flag16  // Relay�����¼� 

#define fOTErr                                 tErrFlag.Flag17    // ���´���       
#define fOTErrEvent                            tEventFlag.Flag17  // ���´����¼� 

#define fGFCICurrOffsetErr                     tErrFlag.Flag18    // ©����ƫ�ù��ߴ���        
#define fGFCICurrOffsetErrEvent                tEventFlag.Flag18  // ©����ƫ�ù��ߴ����¼� 

#define fRInvCurrOffsetErr                     tErrFlag.Flag19    // R�����ƫ�ó��޴���       
#define fRInvCurrOffsetErrEvent                tEventFlag.Flag19  // R�����ƫ�ó��޴����¼� 
  
#define fSInvCurrOffsetErr                     tErrFlag.Flag20    // S�����ƫ�ó��޴���        
#define fSInvCurrOffsetErrEvent                tEventFlag.Flag20  // S�����ƫ�ó��޴����¼� 
  
#define fTInvCurrOffsetErr                     tErrFlag.Flag21    // T�����ƫ�ó��޴���     
#define fTInvCurrOffsetErrEvent                tEventFlag.Flag21  // T�����ƫ�ó��޴����¼� 

#define fInvRelayErr                           tErrFlag.Flag22    // ����Relay����      
#define fInvRelayErrEvent                      tEventFlag.Flag22  // ����Relay�����¼� 
 
#define fBatteryCurr2Err                       tErrFlag.Flag23    // ��2·˫��DC/DC����      
#define fBatteryCurr2ErrEvent                  tEventFlag.Flag23  // ��2·˫��DC/DC�����¼� 
   
#define fBatteryCurr3Err                       tErrFlag.Flag24    // ��3·˫��DC/DC����      
#define fBatteryCurr3ErrEvent                  tEventFlag.Flag24  // ��3·˫��DC/DC�����¼�  

#define fRDCICurrOffsetErr                     tErrFlag.Flag25    // RS������ѹƫ�ó��޴���       
#define fRDCICurrOffsetErrEvent                tEventFlag.Flag25  // RS������ѹƫ�ó��޴����¼� 
  
#define fSDCICurrOffsetErr                     tErrFlag.Flag26    // ST������ѹƫ�ó��޴���        
#define fSDCICurrOffsetErrEvent                tEventFlag.Flag26  // ST������ѹƫ�ó��޴����¼�  
  
#define fTDCICurrOffsetErr                     tErrFlag.Flag27    // TR������ѹƫ�ó��޴���     
#define fTDCICurrOffsetErrEvent                tEventFlag.Flag27  // TR������ѹƫ�ó��޴����¼� 
   
#define fDCIHighErr                            tErrFlag.Flag28    // ֱ���������ߴ���        
#define fDCIHighErrEvent                       tEventFlag.Flag28  // ֱ���������ߴ����¼� 
   
#define fIsoLowErr                             tErrFlag.Flag29    // ��Ե�迹���ʹ���     
#define fIsoLowErrEvent                        tEventFlag.Flag29  // ��Ե�迹���ʹ����¼� 

#define fGFCIHighErr                           tErrFlag.Flag30    // ©�������ߴ���       
#define fGFCIHighErrEvent                      tEventFlag.Flag30  // ©�������ߴ����¼� 

#define fAuxiRelayErr                          tErrFlag.Flag31    // ϵͳƵ�ʼ��ֵ����  
#define fAuxiRelayErrEvent                     tEventFlag.Flag31  // ϵͳƵ�ʼ��ֵ�����¼�

#define fInvCurrOffsetErr                      tErrFlag.Flag32    //(fRInvCurrOffsetErr | fSInvCurrOffsetErr | fTInvCurrOffsetErr)
#define fInvCurrOffsetErrEvent                 tEventFlag.Flag32  //(fRInvCurrOffsetErrEvent | fSInvCurrOffsetErrEvent | fTInvCurrOffsetErrEvent)

#define fDCICurrOffsetErr                      tErrFlag.Flag33    //(fRDCICurrOffsetErr | fSDCICurrOffsetErr | fTDCICurrOffsetErr)
#define fDCICurrOffsetErrEvent                 tEventFlag.Flag33  //(fRDCICurrOffsetErrEvent | fSDCICurrOffsetErrEvent | fTDCICurrOffsetErrEvent)

#define fGridCurrOffsetErr                     tErrFlag.Flag34    //(fRInvCurrOffsetErr | fSInvCurrOffsetErr | fTInvCurrOffsetErr)
#define fGridCurrOffsetErrEvent                tEventFlag.Flag34 

#define fDCRelayErr                            tErrFlag.Flag35    // DC Breaker��������
#define fDCRelayErrEvent                       tEventFlag.Flag35  // DC Breaker���������¼�

#define fGFCICTErr                             tErrFlag.Flag36    // PV������ѹ����
#define fGFCICTErrEvent                        tEventFlag.Flag36  // PV������ѹ�����¼�

#define fBattRevConnectErr                     tErrFlag.Flag37    // PV���Ӵ���  
#define fBattRevConnectErrEvent                tEventFlag.Flag37  // PV���Ӵ����¼� 

#define fAIFreqErr                             tErrFlag.Flag38    // �µ�Ƶ�ʱ��� 
#define fAIFreqErrEvent                        tEventFlag.Flag38  // �µ�Ƶ�ʱ����¼�

#define fPhaseRmsCurErr                        tErrFlag.Flag39    // ��������ƽ�����     
#define fPhaseRmsCurErrEvent                   tEventFlag.Flag39  // ��������ƽ������¼� 

#define fIGBTHaltErr                           tErrFlag.Flag40    // IGBTHalt1     
#define fIGBTHaltErrEvent                      tEventFlag.Flag40  // IGBTHalt1�¼�

#define fMcuActDspNoActErr                     tErrFlag.Flag41    // MCU����DSPû��������   
#define fMcuActDspNoActErrEvent                tEventFlag.Flag41  // MCU����DSPû���������¼�

#define fGridAndInvVolDiffErr                  tErrFlag.Flag42    // ������������ѹ�����   
#define fGridAndInvVolDiffErrEvent             tEventFlag.Flag42  // ������������ѹ������¼�

#define fOVPErr                                tErrFlag.Flag43    // Ӳ����ѹ����   
#define fOVPErrEvent                           tEventFlag.Flag43  // Ӳ����ѹ�����¼�

#define fOCPErr                                tErrFlag.Flag44    // Ӳ����������
#define fOCPErrEvent                           tEventFlag.Flag44  // Ӳ������������?

#define fDecCapEnergyErr                       tErrFlag.Flag45    // ִ�п��ٷŵ�ʱ��������
#define fDecCapEnergyErrEvent                  tEventFlag.Flag45  // ִ�п��ٷŵ�ʱ�������� 

#define fIGBTHalt3Err                          tErrFlag.Flag46    // IGBTHalt3 
#define fIGBTHalt3ErrEvent                     tEventFlag.Flag46  // IGBTHalt3�¼�      

#define fRGridCurrOffsetErr                    tErrFlag.Flag47    // R�����ƫ�ó��޴���       
#define fRGridCurrOffsetErrEvent               tEventFlag.Flag47  // R�����ƫ�ó��޴����¼�   

#define fSGridCurrOffsetErr                    tErrFlag.Flag48    // S�����ƫ�ó��޴���        
#define fSGridCurrOffsetErrEvent               tEventFlag.Flag48  // S�����ƫ�ó��޴���   

#define fTGridCurrOffsetErr                    tErrFlag.Flag49    // T�������ó��޴��?    
#define fTGridCurrOffsetErrEvent               tEventFlag.Flag49  // T�����ƫ�ó��޴����¼�                                                

#define fVCapDiffErr                           tErrFlag.Flag50    // BUS�����
#define fVCapDiffErrEvent                      tEventFlag.Flag50  // BUS������¼�

#define fSpdErr                                tErrFlag.Flag51    // ��������������쳣
#define fSpdErrEvent                           tEventFlag.Flag51  // ��������������쳣�¼�

#define fDCOCPErr                              tErrFlag.Flag52    //IGBTPowerHalt
#define fDCOCPErrEvent                         tEventFlag.Flag52  //IGBTPowerHalt�¼�

#define fBatVolHighErr                         tErrFlag.Flag53    //ϵͳ48V��Դ�쳣
#define fBatVolHighErrEvent                    tEventFlag.Flag53  //ϵͳ48V��Դ�쳣�¼�

#define fCtrPowerErr                           tErrFlag.Flag54    //����15V��Դ�쳣
#define fCtrPowerErrEvent                      tEventFlag.Flag54  //����15V��Դ�쳣�¼�

#define fFuseErr                               tErrFlag.Flag55    //�����۶����쳣
#define fFuseErrEvent                          tEventFlag.Flag55  //�����۶����쳣�¼�

#define fDampResistanceErr                     tErrFlag.Flag56    //г������ջ�
#define fDampResistanceErrEvent                tEventFlag.Flag56  //г������ջ��쳣�¼�

#define fCANNoAcitveErr                        tErrFlag.Flag57    //��LCDͨ��CAN�쳣
#define fCANNoAcitveErrEvent                   tEventFlag.Flag57  //��LCDͨ��CAN�쳣�¼�

#define fBatteryCurr4Err                       tErrFlag.Flag58    //��4·˫��DC/DC����
#define fBatteryCurr4ErrEvent                  tEventFlag.Flag58  //��4·˫��DC/DC�����¼�

#define fDCPWMErr                              tErrFlag.Flag59    //ֱ��������ʧ
#define fDCPWMErrEvent                         tEventFlag.Flag59  //ֱ��������ʧ�¼� 

#define fDCSoftRelayErr                        tErrFlag.Flag60    //ֱ�������·�쳣
#define fDCSoftRelayErrEvent                   tEventFlag.Flag60  //ֱ�������·�쳣�¼� 

#define fDCCurrBalanceErr                      tErrFlag.Flag61    //ֱ��������ƽ��
#define fDCCurrBalanceErrEvent                 tEventFlag.Flag61  //ֱ��������ƽ���¼�

#define fCPLDOsciErr                           tErrFlag.Flag62    //CPLDOscLost
#define fCPLDOsciErrEvent                      tEventFlag.Flag62  //CPLDOscLost�¼�

#define fDCSwitchErr                           tErrFlag.Flag63    //DCSwitch�Ͽ�
#define fDCSwitchErrEvent                      tEventFlag.Flag63  //DCSwitch�Ͽ��¼�

#define fBusDiffBatVolErr                      tErrFlag.Flag64    //BUS��ѹ�����ڲ���ص�ѹ
#define fBusDiffBatVolErrEvent                 tEventFlag.Flag64  //BUS��ѹ�����ڲ���ص�ѹ�¼�
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

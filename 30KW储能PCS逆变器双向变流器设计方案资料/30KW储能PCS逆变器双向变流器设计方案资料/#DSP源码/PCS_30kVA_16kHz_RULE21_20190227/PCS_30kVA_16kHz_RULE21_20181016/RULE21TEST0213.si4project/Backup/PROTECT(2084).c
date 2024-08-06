/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  PROTECT.c 
Description:  保护和故障类型判断功能函数 
Version :  V1.0
Author:Zhou Xu	 
Date:2009-11-19
*********************************************************************/
#include "..\FunSrc\Funheader.h"

#define cDivide3                 5461
#define cBusN20V                -320
#define cVolt3V                  48
#define cVolt10V                 160     //10V*(270/400)*Q4
#define cVolt20V                 320
#define cVolt30V                 480
#define cVolt40V                 640     //40V*Q4
#define cVolt50V                 800
#define cVolt60V                 960     //40V*Q4
#define cVolt70V                 1120
#define cVolt80V                 1280     //80V*Q4
#define cVolt100V                1600 
#define cVolt980V                15680 
#define cVolt990V                15840
#define cVolt500V                8000
#define cVolt510V                8160
#define cTemp5C                  80  
#define cTemp15C                 240  
#define cTemp70C                 1120
#define cTemp90C                 1440
#define cTemp120C                1920
#define cTemptN30C              -480
#define cTemptN35C              -560
#define cCurrRms3A               768
#define cCurrRms5A               1280
#define cCurrN1A                -256
#define cCurr8R75A               2240

#define cTestResImp              166//300   //测试电阻R0阻抗
#define cInvSelfResImp           123 //107 //98      //逆变器自身并联阻抗

#define cGFCI20mA                360    //Q4
#define cGFCI35mA                560    //Q4
#define cGFCI65mA                1040   //Q4
#define cGFCI70mA                1120   //Q4
#define cGFCI100mA               1600   //Q4
#define cGFCI120mA               1920   //Q4
#define cGFCI130mA               2080   //Q4
#define cGFCI140mA               2240   //Q4
#define cGFCI150mA               2400   //Q4                                

#define cBatteryCurrMax          4608//Q8 
#define cInvCurrMax              24320//Q8 95A

INT16U uwFaultGrade;
INT16U uwWarrningCode,uwFaultCode[5],uwPFCode; 
INT16U fOCPEventBak;
INT16U fGridRelayErrEventBak;
INT16U fInvRelayErrEventBak;
INT16U fInvCurrHighErrEventBak;
INT16U fDCIErrHighEventBak;
INT16U fPhaseRmsCurErrEventBak;
INT16U fRunInvAhdErrEventBak;
INT16U fDampResistanceErrEventBak;


INT32U udwRelayNFTimerCnt;
INT32U udwInvRelayNFTimerCnt;
INT32U udwOCPNFTimerCnt;
INT32U udwInvCurrHighNFTimerCnt;
INT32U udwDCIHighNFTimerCnt,udwPhaseRmsCurNFTimerCnt;
INT32U udwRunInvAhdNFTimerCnt;
INT32U udwDampResistanceNFTimerCnt;

INT16U fGridRelayFaultEvent;
INT16U fInvRelayFaultEvent;
INT16U fOCPFaultEvent;
INT16U fInvCurrHighFaultEvent;
INT16U fDCICurrOffsetFaultEvent;
INT16U fDCIHighFaultEvent,fPhaseRmsCurFaultEvent;
INT16U fRunInvAhdFaultEvent;
INT16U fDampResistanceFaultEvent;

INT16U uwRelayFaultCnt;
INT16U uwInvRelayFaultCnt;
INT16U uwOCPFaultCnt;
INT16U uwInvCurrHighFaultCnt;
INT16U uwDCIHighFaultCnt,uwPhaseRmsCurFaultCnt;
INT16U uwRunInvAhdFaultCnt;
INT16U uwDampResistanceFaultCnt;

INT16U uwRelayHoldFaultCnt;
INT16U uwInvRelayHoldFaultCnt;
INT16U uwOCPHoldFaultCnt;
INT16U uwInvCurrHighHoldFaultCnt;
INT16U uwDCIHighHoldFaultCnt,uwPhaseRmsCurHoldFaultCnt;
INT16U uwDCIChkDelay5sCnt;
INT16U uwRunInvAhdHoldFaultCnt;
INT16U uwDampResistanceHoldFaultCnt;

INT16U fTempdisable = 0;
INT16U uwInvVolNormalChkOk,uwGridAndInvVolChkOk;
INT16U fDCIDelayClear = 0;
INT16U fRemoteDCBreakerCtrEn = 0;
INT16U fDCRelayBurn = 0;
INT16U fDCPWMLost = 0;
INT16U fDCSoftRelayLost = 0;
INT16S wDebugFlashSum = 0;

INT16U fDCPWMLostPreChk=0;
INT16U fACPWMLostPreChk=0;
INT16U uwDCRelayOpenCnt = 0;





INT16S wRamSafetyForCheck = 0;
T_PROTECTOBJECT tErrFlag;
T_PROTECTOBJECT tEventFlag;

INT16S wGFCIAvgInitArray[8] = {0,0,0,0,0,0,0,0};
INT16S wGFCIRmsInitArray[8] = {0,0,0,0,0,0,0,0};
INT16S wGFCIAvgActArray[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
INT16S wGFCIRmsActArray[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
INT16S wDeltaGFCIAvgArray[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
INT16S wDeltaGFCIRmsArray[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
INT16U uwGFCIInitArrayIndex = 0;
INT16U uwGFCIActArrayIndex = 0;
INT32S dwGFCIAvgInitSum = 0;
INT32S dwGFCIRmsInitSum = 0;
INT32S dwGFCIAvgActSum = 0;
INT32S dwGFCIRmsActSum = 0;
INT16U uwGFCIHighCnt = 0;
INT16U uwDeltaGFCIChkDelay100Prd = 0;
INT16U uwSteadyErrClearDelay10s = 0;
INT16U uwDeltaGFCIErrClearDelay10s = 0;
INT16U uwADSampleErrCnt=0;


INT16U uwfIsoChkFinished;
INT16U uwfIsoChkOk;
INT16U uwIsoChkCounter = 0;                                                                     
INT32S dwIsoRlyOpenVngSum = 0;  //when ISO relay open, Neg to PE voltage                          
INT32S dwIsoRlyCloseVngSum = 0; //when ISo relay close, Neg to PE voltage  
INT16S wIsoRlyOpenVngAvgTemp = 0;
INT16S wIsoRlyCloseVngAvgTemp = 0;
INT16U uwIsoSumCnt = 0;                                                        
INT16U uwIsoTimer20msCnt = 0; 
INT16U uwIsoChkStep = 0;
INT16S wIsoImp = 0;

INT16U fRelayCheckStep;
INT16U uwInvRelayOKCnt = 0;
INT16U uwInvRelayBadCnt = 0;
INT16U uwGridRelayOKCnt = 0;
INT16U uwGridRelayBadCnt = 0;
INT16U uwAuxiRelayOKCnt = 0;
INT16U uwAuxiRelayBadCnt = 0;

//GFCI CT check
INT16U fGFCICTStatue;   
INT16U uwfGFCICTChkFinished;
INT16U uwfGFCICTChkOk;
INT16U uwGFCICTChkStep = 0; 

INT16U fFanFaultStatus = 0;
INT16U UabTemp=0;
INT16U UbcTemp=0;
INT16U UcaTemp=0;

//-------------------------Code--------------------------------
#pragma CODE_SECTION(sGridNoUtilityChk, "ProtectRamFuncs")
#pragma CODE_SECTION(sMcuFdbChk, "ProtectRamFuncs")
#pragma CODE_SECTION(sBatteryCurrHighChk, "ProtectRamFuncs")
#pragma CODE_SECTION(sInvCurrHighChk, "ProtectRamFuncs")
#pragma CODE_SECTION(sGridVolAndInvVolDiffChk, "ProtectRamFuncs")
#pragma CODE_SECTION(sBusOVPChk, "ProtectRamFuncs")

//safety flash area
#pragma DATA_SECTION(wFlashSafetyCheckCRC,"FlashSafetyCRC");
const INT16U wFlashSafetyCheckCRC = 0x5AF0;//wDebugFlashSum

#pragma CODE_SECTION(sfIsoChk,"FlashSafety");
#pragma CODE_SECTION(sfRelayChk,"FlashSafety");
#pragma CODE_SECTION(sGFCIDynamicHighChk,"FlashSafety");
#pragma CODE_SECTION(sGFCISteadyHighChk,"FlashSafety");
#pragma CODE_SECTION(swGFCICoeffCal,"FlashSafety");
#pragma CODE_SECTION(sErroringDeal,"FlashSafety");

//#pragma CODE_SECTION(LL$$DIV,"FPUmathTables");

//-------------------------Data Safety--------------------------------
#pragma DATA_SECTION(tErrFlag,"RAMSafetytFlagProt0");
#pragma DATA_SECTION(tEventFlag,"RAMSafetytFlagProt1");
#pragma DATA_SECTION(wGFCIAvgInitArray,"RAMSafetyProtectProt3");
#pragma DATA_SECTION(wGFCIRmsInitArray,"RAMSafetyProtectProt4");
#pragma DATA_SECTION(wGFCIAvgActArray,"RAMSafetyProtectProt5");

#pragma DATA_SECTION(wGFCIRmsActArray,"RAMSafetyProtectProt6");
#pragma DATA_SECTION(wDeltaGFCIAvgArray,"RAMSafetyProtectProt7");
#pragma DATA_SECTION(wDeltaGFCIRmsArray,"RAMSafetyProtectProt8");
#pragma DATA_SECTION(uwGFCIInitArrayIndex,"RAMSafetyProtectProt9");
#pragma DATA_SECTION(uwGFCIActArrayIndex,"RAMSafetyProtectProt10");

#pragma DATA_SECTION(dwGFCIAvgInitSum,"RAMSafetyProtectProt11");
#pragma DATA_SECTION(dwGFCIRmsInitSum,"RAMSafetyProtectProt12");
#pragma DATA_SECTION(dwGFCIAvgActSum,"RAMSafetyProtectProt13");
#pragma DATA_SECTION(dwGFCIRmsActSum,"RAMSafetyProtectProt14");

#pragma DATA_SECTION(uwGFCIHighCnt,"RAMSafetyProtectProt15");
#pragma DATA_SECTION(uwDeltaGFCIChkDelay100Prd,"RAMSafetyProtectProt16");
#pragma DATA_SECTION(uwSteadyErrClearDelay10s,"RAMSafetyProtectProt17");
#pragma DATA_SECTION(uwDeltaGFCIErrClearDelay10s,"RAMSafetyProtectProt18");
#pragma DATA_SECTION(uwfIsoChkFinished,"RAMSafetyProtectProt19");
#pragma DATA_SECTION(uwfIsoChkOk,"RAMSafetyProtectProt20");
#pragma DATA_SECTION(uwIsoChkCounter,"RAMSafetyProtectProt21");

#pragma DATA_SECTION(dwIsoRlyOpenVngSum,"RAMSafetyProtectProt22");
#pragma DATA_SECTION(dwIsoRlyCloseVngSum,"RAMSafetyProtectProt23");

#pragma DATA_SECTION(wIsoRlyOpenVngAvgTemp,"RAMSafetyProtectProt24");
#pragma DATA_SECTION(wIsoRlyCloseVngAvgTemp,"RAMSafetyProtectProt25");
#pragma DATA_SECTION(uwIsoSumCnt,"RAMSafetyProtectProt26");
#pragma DATA_SECTION(uwIsoTimer20msCnt,"RAMSafetyProtectProt27");
#pragma DATA_SECTION(wIsoImp,"RAMSafetyProtectProt28");
#pragma DATA_SECTION(fRelayCheckStep,"RAMSafetyProtectProt29");
#pragma DATA_SECTION(uwInvRelayOKCnt,"RAMSafetyProtectProt30");
#pragma DATA_SECTION(uwInvRelayBadCnt,"RAMSafetyProtectProt31");
#pragma DATA_SECTION(uwGridRelayOKCnt,"RAMSafetyProtectProt32");
#pragma DATA_SECTION(uwGridRelayBadCnt,"RAMSafetyProtectProt33");
#pragma DATA_SECTION(uwAuxiRelayOKCnt,"RAMSafetyProtectProt34");
#pragma DATA_SECTION(uwAuxiRelayBadCnt,"RAMSafetyProtectProt35");
#pragma DATA_SECTION(uwIsoChkStep,"RAMSafetyProtectProt36");
#pragma DATA_SECTION(uwDCRelayOpenCnt,"RAMSafetyProtectProt37");




//-------------------------Data--------------------------------
#pragma DATA_SECTION(wRamSafetyForCheck,"RamSafetyProt");

#pragma DATA_SECTION(uwFaultCode,"RamSafetyProt");


#pragma DATA_SECTION(fDCRelayBurn,"RamSafetyProt");
#pragma DATA_SECTION(fDCPWMLost,"RamSafetyProt");
#pragma DATA_SECTION(fDCPWMLostPreChk,"RamSafetyProt");
#pragma DATA_SECTION(fACPWMLostPreChk,"RamSafetyProt");



#pragma DATA_SECTION(fGFCICTStatue,"RamSafetyProt");
#pragma DATA_SECTION(uwfGFCICTChkFinished,"RamSafetyProt");
#pragma DATA_SECTION(uwfGFCICTChkOk,"RamSafetyProt");
#pragma DATA_SECTION(uwGFCICTChkStep,"RamSafetyProt");
/*********************************************************************
Function name:  void InitProtectVar(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/    
void InitProtectVar(void)
{
	uwWarrningCode = 0;
	uwFaultCode[0] = 0;
	uwFaultCode[1] = 0;
	uwFaultCode[2] = 0;
	uwFaultCode[3] = 0;
	uwFaultCode[4] = 0;
	uwPFCode = 0;
	uwFaultGrade = 0;
	fFanErr = 0;                
	fFanErrEvent = 0;           
	fGFCISteadyErr = 0;                 
	fGFCISteadyErrEvent = 0;            
	fEepromErr = 0;              
	fEepromErrEvent = 0;         
		                        
	fTempSensorErr = 0;            
	fTempSensorErrEvent = 0;       
	fRunInvAhdErr = 0;             
	fRunInvAhdErrEvent = 0;        
	fPowerDisMatchErr = 0;            
	fPowerDisMatchEvent = 0;              
	fBatteryCoeffErr = 0;       
	fBatteryCoeffErrEvent = 0;   
	//fBusSoftStaErr = 0;     
	//fBusSoftStaErrEvent = 0;
	fBusHighErr = 0;              
	fBusHighErrEvent = 0;         
	fBatteryCurr1Err = 0;         
	fBatteryCurr1ErrEvent = 0;  
	fLineGridRmsVolErr = 0;
	fLineGridRmsVolErrEvent = 0;
	fVCapHighErr = 0;
	fVCapHighErrEvent = 0;
	fBatteryCurr4Err = 0;
	fBatteryCurr4ErrEvent = 0;
	fDCPWMErr = 0;
	fDCPWMErrEvent = 0;
	fDCSoftRelayErr = 0;
	fDCSoftRelayErrEvent = 0;
	fDCCurrBalanceErr = 0;
	fDCCurrBalanceErrEvent = 0;
	fCPLDOsciErr = 0;
	fCPLDOsciErrEvent = 0;
    fDCSwitchErr = 0;
	fDCSwitchErrEvent = 0;
	fBusDiffBatVolErr = 0;
	fBusDiffBatVolErrEvent = 0;
	fInvCurrHighErr = 0;        
	fInvCurrHighErrEvent = 0; 	  	
	fFreHighErr = 0;            
	fFreHighErrEvent = 0;       
	fFreLowErr = 0;             
	fFreLowErrEvent = 0;        	   
	fNoUtilityErr = 0;           
	fNoUtilityErrEvent = 0;      
	fGridRelayErr = 0;              
	fGridRelayErrEvent = 0; 
	fInvRelayErr = 0;
	fInvRelayErrEvent = 0;         
	fOTErr = 0;               
	fOTErrEvent = 0;     
	fGFCICurrOffsetErr = 0;  //Update @2010.08.11 By Wdd
	fGFCICurrOffsetErrEvent = 0;              
	fRInvCurrOffsetErr = 0;                   
	fRInvCurrOffsetErrEvent = 0;              
	fSInvCurrOffsetErr = 0;                   
	fSInvCurrOffsetErrEvent = 0;              
	fTInvCurrOffsetErr = 0;                   
	fTInvCurrOffsetErrEvent = 0;          
	fBatteryCurr2Err = 0;            
	fBatteryCurr2ErrEvent = 0;       
	fBatteryCurr3Err = 0;            
	fBatteryCurr3ErrEvent = 0;       
	fRDCICurrOffsetErr = 0;       
	fRDCICurrOffsetErrEvent = 0;  
	fSDCICurrOffsetErr = 0;       
	fSDCICurrOffsetErrEvent = 0;  
	fTDCICurrOffsetErr = 0;       
	fTDCICurrOffsetErrEvent = 0;  	           
	fDCIHighErr = 0;             
	fDCIHighErrEvent = 0;        
	fIsoLowErr = 0;              
	fIsoLowErrEvent = 0;         
	fGFCIHighErr = 0;            
	fGFCIHighErrEvent = 0; 	
	fAuxiRelayErr = 0;
	fAuxiRelayErrEvent = 0;	
	fDecCapEnergyErr = 0;
	fDecCapEnergyErrEvent = 0;	 
	fInvCurrOffsetErr = 0;          
	fInvCurrOffsetErrEvent = 0;                 
	fDCICurrOffsetErr = 0;     
	fDCICurrOffsetErrEvent = 0; 	    
	fDCRelayErr = 0;                           
	fDCRelayErrEvent = 0;                       
	fGFCICTErr = 0;                      
	fGFCICTErrEvent = 0;                      
	fBattRevConnectErr = 0;     
	fBattRevConnectErrEvent = 0;
    fAIFreqErr = 0;
    fAIFreqErrEvent = 0;	
	fVCapDiffErr = 0;
	fVCapDiffErrEvent = 0;
	fGridVoltBalanceDegreeErr = 0;
    fGridVoltBalanceDegreeErrEvent = 0;
    fPhaseRmsCurErr = 0;
    fPhaseRmsCurErrEvent = 0;
    fIGBTHaltErr = 0;
    fIGBTHaltErrEvent = 0;
  	fMcuActDspNoActErr = 0;
	fMcuActDspNoActErrEvent = 0;
	fGridAndInvVolDiffErr = 0;
	fGridAndInvVolDiffErrEvent = 0;
	fOVPErr = 0;
	fOVPErrEvent = 0;
	fOCPErr = 0;
	fOCPErrEvent = 0;	    
    fSpdErr = 0; 
    fSpdErrEvent = 0;

	fDCOCPErr = 0;
    fDCOCPErrEvent = 0;

	fBatVolHighErr = 0;
	fBatVolHighErrEvent = 0;

	fCtrPowerErr = 0;
	fCtrPowerErrEvent = 0;
	fFuseErr = 0;
	fFuseErrEvent = 0;
	fDampResistanceErr = 0;
	fDampResistanceErrEvent = 0;

	fDCPWMPreChkErr=0;
	fDCPWMPreChkErrEvent=0;
	fACPWMPreChkErr=0;
	fACPWMPreChkErrEvent=0;
	
	fGFCICTStatue = 0;
    uwfGFCICTChkFinished = 0;
    uwfGFCICTChkOk = 0;
    uwGFCICTChkStep = 0; 

	fCANNoAcitveErr = 0;
	fCANNoAcitveErrEvent = 0;
  
	fOCPEventBak = 0;
	fGridRelayErrEventBak = 0;
	fInvRelayErrEventBak = 0;
	fInvCurrHighErrEventBak = 0;
    fDCIErrHighEventBak = 0;                                  
    fPhaseRmsCurErrEventBak = 0; 
	fDampResistanceErrEventBak = 0;
    fRunInvAhdErrEventBak = 0;
    uwRelayFaultCnt = 0;
	uwInvRelayFaultCnt = 0;
	uwOCPFaultCnt = 0;
	uwInvCurrHighFaultCnt = 0;
    uwDCIHighFaultCnt = 0;   
    uwPhaseRmsCurFaultCnt = 0; 
    uwRunInvAhdFaultCnt = 0;
	uwDampResistanceFaultCnt = 0;
    uwRelayHoldFaultCnt = 0;  
	uwInvRelayHoldFaultCnt = 0;
	uwOCPHoldFaultCnt = 0;
	uwInvCurrHighHoldFaultCnt = 0;
    uwDCIHighHoldFaultCnt = 0;   
    uwPhaseRmsCurHoldFaultCnt = 0;                               
    uwDCIChkDelay5sCnt = 0;
    uwRunInvAhdHoldFaultCnt = 0;
	uwDampResistanceHoldFaultCnt = 0;
    udwRelayNFTimerCnt = 0;
    udwInvRelayNFTimerCnt = 0;   
	udwOCPNFTimerCnt = 0;
	udwInvCurrHighNFTimerCnt = 0;
    udwDCIHighNFTimerCnt = 0;   
    udwPhaseRmsCurNFTimerCnt = 0;   
    udwRunInvAhdNFTimerCnt = 0;
	udwDampResistanceNFTimerCnt = 0;
    fGridRelayFaultEvent = 0;
	fInvRelayFaultEvent = 0;
	fOCPFaultEvent= 0;
	fInvCurrHighFaultEvent = 0;
    fDCICurrOffsetFaultEvent = 0;       
    fDCIHighFaultEvent = 0;                                     
    fPhaseRmsCurFaultEvent = 0;
    fRunInvAhdFaultEvent = 0;
	fDampResistanceFaultEvent = 0;
    uwfIsoChkFinished = 0;
    uwfIsoChkOk = 0;
	fRelayCheckStep = 0;
    uwInvRelayOKCnt = 0;
    uwInvRelayBadCnt = 0;
    uwGridRelayOKCnt = 0;
    uwGridRelayBadCnt = 0;
    uwAuxiRelayOKCnt = 0;
    uwAuxiRelayBadCnt = 0;
	fTempdisable = 0;
	uwInvVolNormalChkOk = 0;
	uwGridAndInvVolChkOk = 0;
	fDCIDelayClear = 0;
	fRemoteDCBreakerCtrEn = 0;
	uwIsoChkStep = 0;
	wIsoImp = 0;
	fFanFaultStatus = 0;
	fDCRelayBurn = 0;  
	fDCPWMLost = 0; 
	fDCSoftRelayLost = 0;
	fDCPWMLostPreChk=0;
	fACPWMLostPreChk=0;
}

/*********************************************************************
Function name:  void sErroringDeal(INT16S *pBusVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  故障发生时紧急处理
Calls:        	 
Called By:  所有故障检测程序和故障模式下
Parameters:  void
Return:  void      
**********************************************************************/
void sErroringDeal()
{   
	fError = 1;       
    fInvEnOut = 0;     
    fBiDirDCEnOut = 0;
    fDCModeSwitchEn = 0;
    
    sDisBiDirDC1And2CpldOut();
    sDisBiDirDC3And4CpldOut();
    sDisBiDirDC1PWMOut();
    sDisBiDirDC2PWMOut();
    sDisBiDirDC3PWMOut();
    sDisBiDirDC4PWMOut();               
    sDisInvCpldOut();            
    sDisInvPWMOut(); 
    if((uwSysMode == InvRunMode)||(uwSysMode == PreCheckMode))
    {
        sDCOCPDisable();
    }
 	sGridRelayOpen();
 	sInvRelayOpen();
 	sAuxiRelayOpen();
    sIsoRelayOpen();    
    sPERelayOpen();
	sDCSoftRelayOpen();
    
    if(wEepromDCRelayActProt == 1)
    {
    	//sDCRelayOpenAct();//sDCRelayPOpen();
		 if(wBatteryAvgCurr > cCurrN1A)//-1A
    	{
	    	if(++uwDCRelayOpenCnt > 2)
	    	{
	            uwDCRelayOpenCnt = 0;
	            sDCRelayPOpen();
	        }
    	}
    }
    fMpptEnable = 0;	
    //fLoadDerating = 0;    */
}
/*********************************************************************
Function name:void sBusVolHighChk(INT16S *pBusVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sBusVolHighChk(INT16S *pBusVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwPVHighOtherModeCnt = 0;
static INT16U uwPVRecOtherModeCnt = 0;	
static INT16U uwPVHighInvModeCnt = 0;
static INT16U uwPVRecInvModeCnt = 0;
INT16S wBusVolMax;
    
    wBusVolMax = ((INT32S)wEepromBusVolMax * 26214)>>14;	

	if(uwSysMode == InvRunMode)
	{
		if(*pBusVol > wBusVolMax)//wBusVolMax 14240
		{
			if((++uwPVHighInvModeCnt) >= 2)  //10ms=2*5ms
			{
				uwPVHighInvModeCnt = 2;
				uwPVRecInvModeCnt = 0;
				*pErrFlag = 1;
			
				if((*pEventFlag) == 0)
				{					
					sErroringDeal(); 
					*pEventFlag = 1;
				}
				
            }	
		}
		else
		{
			uwPVHighInvModeCnt= 0;
			if (++uwPVRecInvModeCnt > 12000)  //60s
			{
				uwPVRecInvModeCnt = 12000;				
				*pErrFlag = 0; 
			}
		} 
		uwPVHighOtherModeCnt = 0;
		uwPVRecOtherModeCnt = 0;
	}
	else   
	{
		if(*pBusVol > cVolt980V)//14400
		{
			if(++uwPVHighOtherModeCnt >= 4)//200
			{
				uwPVHighOtherModeCnt = 4;
				uwPVRecOtherModeCnt = 0;
				*pErrFlag = 1;
			
				if((*pEventFlag) == 0)
				{ 					
					sErroringDeal();
					*pEventFlag = 1;
                }
			}	
		}
		else
		{
			uwPVHighOtherModeCnt = 0;
			if (++uwPVRecOtherModeCnt > 12000) //60s
			{
				uwPVRecOtherModeCnt = 12000;				
				*pErrFlag = 0; 
			}
		} 
		uwPVHighInvModeCnt = 0;
		uwPVRecInvModeCnt = 0;		
	}
}
/*********************************************************************
Function name:void sBatVolHighChk(INT16S *pBatOutVol,INT16S *pBatInVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sBatVolHighChk(INT16S *pBatOutVol,INT16S *pBatInVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFaultDelay = 0;
static INT16U uwOKDelay = 0;
INT16S wBatVolMax;
    
    wBatVolMax = ((INT32S)wEepromBatteryVoltMax * 26214)>>14;	

	if((*pBatOutVol > wBatVolMax)||(*pBatInVol > wBatVolMax))
	{
		uwOKDelay = 0;
		
		if((++uwFaultDelay) >= 2)
		{
			uwFaultDelay = 2;
			*pErrFlag = 1;
		
			if((*pEventFlag) == 0)
			{					
				sErroringDeal(); 
				*pEventFlag = 1;
			}
			
			if(fDCRelayPCtr == 1)//DC Relay is Close
			{
				sDCRelayOpenAct();
			}			
        }	
	}
	else
	{
		uwFaultDelay = 0;
		if (++uwOKDelay > 3000)  //60s
		{
			uwOKDelay = 3000;				
			*pErrFlag = 0; 
		}
	} 
}
/*********************************************************************
Function name:void sBusDiffBatVolChk(INT16S *pBusVol,INT16S *pBatInVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void) 20ms
Parameters:  void
Return:  void      
**********************************************************************/
void sBusDiffBatVolChk(INT16S *pBusVol,INT16S *pBatInVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrDelay = 0;
static INT16U uwClcDelay = 0;

    if(*pBusVol < (*pBatInVol - cVolt60V))
    {
        uwClcDelay = 0;
        
        if(++uwErrDelay >= 3)
        {
            uwErrDelay = 3;			
			*pErrFlag = 1;
			
			if((*pEventFlag) == 0)
			{					
				sErroringDeal(); 
				*pEventFlag = 1;
			}
        }
    }
    else if(*pBusVol > (*pBatInVol - cVolt50V))
    {
        uwErrDelay = 0;
	    if (++uwClcDelay > 3000)  //60s
		{
			uwClcDelay = 3000;
			*pErrFlag = 0; 
		}
    }
}
/*********************************************************************
Function name:void sVCapHighChk(INT16S *pPOVol,INT16S *pONVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sVCapHighChk(INT16S *pPOVol,INT16S *pONVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrDelay1 = 0;
static INT16U uwErrDelay2 = 0;
static INT16U uwClcDelay1 = 0;
static INT16U uwClcDelay2 = 0;
INT16S wCapVolMax;

    wCapVolMax = ((INT32S)wEepromCapVolMax * 26214)>>14;

    if(uwSysMode == InvRunMode)
    {
    	if((*pPOVol > wCapVolMax)||(*pONVol > wCapVolMax))//wCapVolMax 7200
	    {
	    	if(++uwErrDelay1 > 2)
	    	{
	    		uwErrDelay1 = 2;
				uwClcDelay1 = 0;
				*pErrFlag = 1;
			
				if((*pEventFlag) == 0)
				{					
					sErroringDeal(); 
					*pEventFlag = 1;
				}
	    	}
	    }
	    else
	    {
	    	uwErrDelay1 = 0;
	    	if (++uwClcDelay1 > 12000)  //60s
			{
				uwClcDelay1 = 12000;
				*pErrFlag = 0; 
			}
	    }
	    
	    uwErrDelay2 = 0;
	    uwClcDelay2 = 0;
    }
	else
	{
		if((*pPOVol > cVolt500V)||(*pONVol > cVolt500V))
		{
			if(++uwErrDelay2 >= 10)
			{
				uwErrDelay2 = 10;
				uwClcDelay2 = 0;
				*pErrFlag = 1;
			
				if((*pEventFlag) == 0)
				{ 					
					sErroringDeal();
					*pEventFlag = 1;
                }
			}	
		}
		else
		{
			uwErrDelay2 = 0;
			if (++uwClcDelay2 > 12000) //60s
			{
				uwClcDelay2 = 12000;				
				*pErrFlag = 0; 
			}
		} 
		uwErrDelay1 = 0;
	    uwClcDelay1 = 0;
	}
}
/*********************************************************************
Function name:void sVCapDiffChk(INT16S *pPOVol,INT16S *pONVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sVCapDiffChk(INT16S *pPOVol,INT16S *pONVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrDelay1 = 0;
static INT16U uwErrDelay2 = 0;
static INT16U uwClcDelay1 = 0;
static INT16U uwClcDelay2 = 0;
INT16S wVCapDiffVol;

    wVCapDiffVol = (*pPOVol) - (*pONVol);
    if(uwSysMode == InvRunMode)
    {
    	if(abs(wVCapDiffVol) > cVolt60V)
	    {
	    	if(++uwErrDelay1 > 5)
	    	{
	    		uwErrDelay1 = 5;
				uwClcDelay1 = 0;
				*pErrFlag = 1;
			
				if((*pEventFlag) == 0)
				{					
					sErroringDeal(); 
					*pEventFlag = 1;
				}
	    	}
	    }
	    else
	    {
	    	uwErrDelay1 = 0;
	    	if (++uwClcDelay1 > 3000)  //60s  12000
			{
				uwClcDelay1 = 3000;
				*pErrFlag = 0; 
			}
	    }
	    
	    uwErrDelay2 = 0;
	    uwClcDelay2 = 0;
    }
	else
	{
		if(abs(wVCapDiffVol) > cVolt30V)
		{
			if(++uwErrDelay2 >= 100)
			{
				uwErrDelay2 = 100;
				uwClcDelay2 = 0;
				*pErrFlag = 1;
			
				if((*pEventFlag) == 0)
				{ 					
					sErroringDeal();
					*pEventFlag = 1;
                }
			}	
		}
		else
		{
			uwErrDelay2 = 0;
			if (++uwClcDelay2 > 3000) //60s 12000
			{
				uwClcDelay2 = 3000;				
				*pErrFlag = 0; 
			}
		} 
		uwErrDelay1 = 0;
	    uwClcDelay1 = 0;
	}
}
/*********************************************************************
Function name:  void sLineRmsVolChk(INT16S *pRSRmsVol,INT16S *pSTRmsVol,INT16S *pTRRmsVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sLineRmsVolChk(INT16S *pRSRmsVol,INT16S *pSTRmsVol,INT16S *pTRRmsVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwLineRmsHighErrCnt1 = 0; 
static INT16U uwLineRmsHighErrCnt2 = 0;
static INT16U uwLineRmsHighErrCnt3 = 0; 
static INT16U uwLineRmsLowErrCnt1 = 0;
static INT16U uwLineRmsLowErrCnt2 = 0;
static INT16U uwLineRmsLowErrCnt3 = 0;
static INT16U uwErrClearDelay = 0;
INT16U uwLowVolTripTime1,uwLowVolTripTime2,uwLowVolTripTime3,uwHighVolTripTime1,uwHighVolTripTime2,uwHighVolTripTime3,uwGirdVolExitTime;
INT16S wGridVolMax_1,wGridVolMin_1,wGridVolMax_2,wGridVolMin_2,wGridVolMax_3,wGridVolMin_3,wGridVolRecover_Max,wGridVolRecover_Min;

    if(wEepromRatedVolSel == 0)
	{
        wGridVolMax_1 = ((INT32S)wEepromGridVolMax_1 * 13107)>>11;
        wGridVolMin_1 = ((INT32S)wEepromGridVolMin_1 * 13107)>>11;
        wGridVolMax_2 = ((INT32S)wEepromGridVolMax_2 * 13107)>>11;
        wGridVolMin_2 = ((INT32S)wEepromGridVolMin_2 * 13107)>>11;
        wGridVolMax_3 = ((INT32S)wEepromGridVolMax_3 * 13107)>>11;
        wGridVolMin_3 = ((INT32S)wEepromGridVolMin_3 * 13107)>>11;
        wGridVolRecover_Max = ((INT32S)wEepromGridVolRecover_Max * 13107)>>11;
        wGridVolRecover_Min = ((INT32S)wEepromGridVolRecover_Min * 13107)>>11;
	}
	else
    {
    	wGridVolMax_1 = ((INT32S)wEepromGridVolMax_1 * 15729)>>11;
        wGridVolMin_1 = ((INT32S)wEepromGridVolMin_1 * 15729)>>11;
        wGridVolMax_2 = ((INT32S)wEepromGridVolMax_2 * 15729)>>11;
        wGridVolMin_2 = ((INT32S)wEepromGridVolMin_2 * 15729)>>11;
        wGridVolMax_3 = ((INT32S)wEepromGridVolMax_3 * 15729)>>11;
        wGridVolMin_3 = ((INT32S)wEepromGridVolMin_3 * 15729)>>11;
        wGridVolRecover_Max = ((INT32S)wEepromGridVolRecover_Max * 15729)>>11;
        wGridVolRecover_Min = ((INT32S)wEepromGridVolRecover_Min * 15729)>>11;
	}

    if((INT16U)wEepromVolMaxTripTime_3 > 3)
    {
    	uwHighVolTripTime3 = (INT16U)wEepromVolMaxTripTime_3 - 3;
    }
    else
    {
    	uwHighVolTripTime3 = 1;
    }
    
    if((INT16U)wEepromVolMaxTripTime_2 > 3)
    {
    	uwHighVolTripTime2 = (INT16U)wEepromVolMaxTripTime_2 - 3;
    }
    else
    {
    	uwHighVolTripTime2 = 1;
    }
    
    if((INT16U)wEepromVolMaxTripTime_1 > 3)
    {
    	uwHighVolTripTime1 = (INT16U)wEepromVolMaxTripTime_1 - 3;
    }
    else
    {
    	uwHighVolTripTime1 = 1;
    }
    
    if((INT16U)wEepromVolMinTripTime_3 > 3)
    {
    	uwLowVolTripTime3 = (INT16U)wEepromVolMinTripTime_3 - 3;
    }
    else
    {
    	uwLowVolTripTime3 = 1;
    }
    
    if((INT16U)wEepromVolMinTripTime_2 > 3)
    {
    	uwLowVolTripTime2 = (INT16U)wEepromVolMinTripTime_2 - 3;
    }
    else
    {
    	uwLowVolTripTime2 = 1;
    }
    
    if((INT16U)wEepromVolMinTripTime_1 > 3)
    {
    	uwLowVolTripTime1 = (INT16U)wEepromVolMinTripTime_1 - 3;
    }
    else
    {
    	uwLowVolTripTime1 = 1;
    }
    
    if(fFactoryFastStart == 1)
    {
        uwGirdVolExitTime = 100;
    }
    else
    {
        uwGirdVolExitTime = wEepromGridVolRecover_Time;
    }
    
    if(fCSVSSwitch == 0)
    {     
        if(wEepromLVRTOption == 1)
        {
        	if(fLVRTValue < 1474)
        	{
        		uwLowVolTripTime1 = 225;
        		uwLowVolTripTime2 = 225;
        		uwLowVolTripTime3 = 225;
        	}
        }
        
        if(((*pRSRmsVol) >= wGridVolMax_3) || ((*pSTRmsVol) >= wGridVolMax_3) || ((*pTRRmsVol) >= wGridVolMax_3))
        {
        	uwLineRmsHighErrCnt3++;
        	if(uwLineRmsHighErrCnt3 >= uwHighVolTripTime3)
        	{
        		uwLineRmsHighErrCnt3 = uwHighVolTripTime3;
        	}    	
        }
        
        if(((*pRSRmsVol) <= wGridVolMin_3) || ((*pSTRmsVol) <= wGridVolMin_3) || ((*pTRRmsVol) <= wGridVolMin_3))
        {
        	uwLineRmsLowErrCnt3++;
        	if(uwLineRmsLowErrCnt3 >= uwLowVolTripTime3)
        	{
        		uwLineRmsLowErrCnt3 = uwLowVolTripTime3;
        	}
        }
        else
        {
        	uwLineRmsLowErrCnt3 = 0;
        }
                
        if(((*pRSRmsVol) >= wGridVolMax_2) || ((*pSTRmsVol) >= wGridVolMax_2) || ((*pTRRmsVol) >= wGridVolMax_2))
        {
        	uwLineRmsHighErrCnt2++;
        	if(uwLineRmsHighErrCnt2 >= uwHighVolTripTime2)
        	{
        		uwLineRmsHighErrCnt2 = uwHighVolTripTime2;
        	}    	
        }
                
        if(((*pRSRmsVol) <= wGridVolMin_2) || ((*pSTRmsVol) <= wGridVolMin_2) || ((*pTRRmsVol) <= wGridVolMin_2))
        {
        	uwLineRmsLowErrCnt2++;
        	if(uwLineRmsLowErrCnt2 >= uwLowVolTripTime2)
        	{
        		uwLineRmsLowErrCnt2 = uwLowVolTripTime2;
        	}
        }
        else
        {
        	uwLineRmsLowErrCnt2 = 0;
        }
        
        if(((*pRSRmsVol) >= wGridVolMax_1) || ((*pSTRmsVol) >= wGridVolMax_1) || ((*pTRRmsVol) >= wGridVolMax_1))
        {
        	uwLineRmsHighErrCnt1++;
        	if(uwLineRmsHighErrCnt1 >= uwHighVolTripTime1)
        	{
        		uwLineRmsHighErrCnt1 = uwHighVolTripTime1;
        	}   	
        }
        else
        {
        	uwLineRmsHighErrCnt1 = 0;
        }
        
        if(((*pRSRmsVol) <= wGridVolMin_1) || ((*pSTRmsVol) <= wGridVolMin_1) || ((*pTRRmsVol) <= wGridVolMin_1))
        {
        	uwLineRmsLowErrCnt1++;
        	if(uwLineRmsLowErrCnt1 >= uwLowVolTripTime1)
        	{
        		uwLineRmsLowErrCnt1 = uwLowVolTripTime1;
        	}    	
        }
        else
        {
        	uwLineRmsLowErrCnt1 = 0;
        }
        
        if(((*pRSRmsVol) <= wGridVolRecover_Max) && ((*pSTRmsVol) <= wGridVolRecover_Max) && ((*pTRRmsVol) <= wGridVolRecover_Max) &&   
           ((*pRSRmsVol) >= wGridVolRecover_Min) && ((*pSTRmsVol) >= wGridVolRecover_Min) && ((*pTRRmsVol) >= wGridVolRecover_Min))
        {
            if(uwLineRmsHighErrCnt3 > 0)
        	{
        	    uwLineRmsHighErrCnt3--;
        	}
        	
        	if(uwLineRmsHighErrCnt2 > 0)
        	{
        	    uwLineRmsHighErrCnt2--;
        	}
        	uwLineRmsHighErrCnt1 = 0;
        	uwLineRmsLowErrCnt3 = 0; 
        	uwLineRmsLowErrCnt2 = 0;
        	uwLineRmsLowErrCnt1 = 0;
        	uwErrClearDelay++;    	
        }
        else
        {
        	uwErrClearDelay = 0;
        }
        
        if((uwLineRmsHighErrCnt3 >= uwHighVolTripTime3)||(uwLineRmsLowErrCnt3 >= uwLowVolTripTime3)||
           (uwLineRmsHighErrCnt2 >= uwHighVolTripTime2)||(uwLineRmsLowErrCnt2 >= uwLowVolTripTime2)||
           (uwLineRmsHighErrCnt1 >= uwHighVolTripTime1)||(uwLineRmsLowErrCnt1 >= uwLowVolTripTime1))
        {
        	*pErrFlag = 1;
        	fGridPretect = 1;
            
            if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;            
            }
        }
        else
        {
        	if(uwErrClearDelay >= uwGirdVolExitTime)
        	{
        	    uwErrClearDelay = uwGirdVolExitTime;
        	    *pErrFlag = 0;
        	}
        }
    }
    else
    {
        uwHighVolTripTime3 = 300;
        uwHighVolTripTime2 = 300;
        uwHighVolTripTime1 = 300;
        
        if(((*pRSRmsVol) >= wGridVolMax_3) || ((*pSTRmsVol) >= wGridVolMax_3) || ((*pTRRmsVol) >= wGridVolMax_3))
        {
        	uwLineRmsHighErrCnt3++;
        	if(uwLineRmsHighErrCnt3 >= uwHighVolTripTime3)
        	{
        		uwLineRmsHighErrCnt3 = uwHighVolTripTime3;
        	}    	
        }
        else
        {
            uwLineRmsHighErrCnt3 = 0;
        }
        
        if(((*pRSRmsVol) >= wGridVolMax_2) || ((*pSTRmsVol) >= wGridVolMax_2) || ((*pTRRmsVol) >= wGridVolMax_2))
        {
        	uwLineRmsHighErrCnt2++;
        	if(uwLineRmsHighErrCnt2 >= uwHighVolTripTime2)
        	{
        		uwLineRmsHighErrCnt2 = uwHighVolTripTime2;
        	}    	
        }
        else
        {
            uwLineRmsHighErrCnt2 = 0;
        }
        
        if(((*pRSRmsVol) >= wGridVolMax_1) || ((*pSTRmsVol) >= wGridVolMax_1) || ((*pTRRmsVol) >= wGridVolMax_1))
        {
        	uwLineRmsHighErrCnt1++;
        	if(uwLineRmsHighErrCnt1 >= uwHighVolTripTime1)
        	{
        		uwLineRmsHighErrCnt1 = uwHighVolTripTime1;
        	}   	
        }
        else
        {
        	uwLineRmsHighErrCnt1 = 0;
        }
  
        if(((*pRSRmsVol) <= wGridVolRecover_Max) && ((*pSTRmsVol) <= wGridVolRecover_Max) && ((*pTRRmsVol) <= wGridVolRecover_Max))
        {
        	uwLineRmsHighErrCnt3 = 0;
        	uwLineRmsHighErrCnt2 = 0;
        	uwLineRmsHighErrCnt1 = 0;
        	uwErrClearDelay++;    	
        }
        else
        {
        	uwErrClearDelay = 0;
        }
        
        if(((uwLineRmsHighErrCnt3 >= uwHighVolTripTime3)||(uwLineRmsHighErrCnt2 >= uwHighVolTripTime2)||(uwLineRmsHighErrCnt1 >= uwHighVolTripTime1))&&(fVSVoltCheckEn == 1))
        {
        	*pErrFlag = 1;
            
            if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;            
            }
        }
        else
        {
            if(fVSVoltCheckEn == 0)
            {
                uwErrClearDelay = uwGirdVolExitTime;
        	    *pErrFlag = 0;
            }
            else
            {
        	    if(uwErrClearDelay >= uwGirdVolExitTime)
        	    {
        	        uwErrClearDelay = uwGirdVolExitTime;
        	        *pErrFlag = 0;
        	    }
        	}
        }
    }
}
/*********************************************************************
Function name:  void sInvCurrHighChk(INT16S *pRInvCurr,INT16S *pSInvCurr,INT16S *pTInvCurr,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sInvCurrHighChk(INT16S *pRInvCurr,INT16S *pSInvCurr,INT16S *pTInvCurr,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwInvCurrHighCnt = 0;
static INT16U uwErrClearDelay2s = 0;
    if((abs(*pRInvCurr) > cInvCurrMax) || (abs(*pSInvCurr) > cInvCurrMax) || (abs(*pTInvCurr) > cInvCurrMax))
    {
    	if(++uwInvCurrHighCnt > 4)//200 4
		{
	        uwInvCurrHighCnt = 4; 
	        uwErrClearDelay2s = 0;
	        *pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal();   // emergency shutdown
                *pEventFlag = 1; 
            }
	    }
    }
    else
    {
    	if(uwInvCurrHighCnt > 0)
		{
		    uwInvCurrHighCnt--;
		}
		else
		{
		    if(++uwErrClearDelay2s > 24000)
		    {
		        uwErrClearDelay2s = 24000;
		        *pErrFlag = 0; 
		    }
		}   
    }   
}
/*********************************************************************
Function name: void sBatteryCurrHighChk(INT16S wDCCurr,INT16U *pHighCnt,INT16U *pOKCnt,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sBatteryCurrHighChk(INT16S wDCCurr,INT16U *pHighCnt,INT16U *pOKCnt,INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16S wBatChargeCurrMax;
INT16S wBatDisChargeCurrMax;
    
    if(fBattDarkStart == 1)
    {
    	wBatChargeCurrMax = ((INT32S)wEepromBatPreChargeCurrMax * 10486)>>14;
    }
    else
    {
        wBatChargeCurrMax = ((INT32S)wEepromBatChargeCurrMax * 10486)>>14;//10486 = 256/100 Q12,But this is single protect
    }
    wBatDisChargeCurrMax = ((INT32S)wEepromBatDisChargeCurrMax * 10486)>>14;
    
    if((wDCCurr < wBatChargeCurrMax)||(wDCCurr > wBatDisChargeCurrMax))
    {
    	if(++(*pHighCnt) > 4)
		{
	        *pHighCnt = 4; 
	        *pOKCnt = 0;
	        *pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal();
                *pEventFlag = 1; 
            }
	    }
    }
    else
    {
    	if((*pHighCnt) > 0)
		{
		    (*pHighCnt)--;
		}
		else
		{
		    if(++(*pOKCnt) > 24000)
		    {
		        *pOKCnt = 24000;
		        *pErrFlag = 0; 
		    }
		}   
    }   
}
/*********************************************************************
Function name:  void sFreHighChk(INT16U *pRFre,INT16U *pSFre,INT16U *pTFre,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sFreHighChk(INT16U *pRFreq,INT16U *pSFreq,INT16U *pTFreq,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFreHighCnt1 = 0;
static INT16U uwFreHighCnt2 = 0;
static INT16U uwFreRecCnt = 0;
static INT16U fFVRTDelay = 0;
INT16U uwHighFreTripTime1,uwHighFreTripTime2,uwHighFreExitTime;
INT16S wGridFreqMax_1,wGridFreqMax_2,wGridFreqRecover_Max;

    if(fFactoryFastStart == 1)
    {
        uwHighFreExitTime = 50;
    }
    else
    {
        uwHighFreExitTime = wEepromGridFreqRecover_Time;
    }
         
    if(fCSVSSwitch == 0)
    {
        if(wEepromRatedFreqSel == 1)
        {
            wGridFreqMax_1 = ((INT32S)(wEepromGridFreqMax_1 + 6000) * 10486)>>12;
            wGridFreqMax_2 = ((INT32S)(wEepromGridFreqMax_2 + 6000) * 10486)>>12;		
	        wGridFreqRecover_Max = ((INT32S)(wEepromGridFreqRecover_Max + 6000) * 10486)>>12;
	    }
	    else
	    {
	        wGridFreqMax_1 = ((INT32S)(wEepromGridFreqMax_1 + 5000) * 10486)>>12;
            wGridFreqMax_2 = ((INT32S)(wEepromGridFreqMax_2 + 5000) * 10486)>>12;		
	        wGridFreqRecover_Max = ((INT32S)(wEepromGridFreqRecover_Max + 5000) * 10486)>>12;
	    }
           
        if((INT16U)wEepromFreqMaxTripTime_2 > 3)
        {
        	uwHighFreTripTime2 = (INT16U)wEepromFreqMaxTripTime_2 - 3;
        }
        else
        {
        	uwHighFreTripTime2 = 1;
        }
        
        if((INT16U)wEepromFreqMaxTripTime_1 > 3)
        {
        	uwHighFreTripTime1 = (INT16U)wEepromFreqMaxTripTime_1 - 3;
        }
        else
        {
        	uwHighFreTripTime1 = 1;
        }

        if((fLVRTValue < uwLVRTActiveValue)||(fHVRTValue > uwHVRTActiveValue))
        { 
        	fFVRTDelay = 50;    	
        }
        else
        {
        	if(fFVRTDelay > 0)
        	{
        		fFVRTDelay--;
        	}
        	else
        	{
        		fFVRTDelay = 0;
        	}
        }
        
        if((((*pRFreq) > wGridFreqMax_2) || ((*pSFreq) > wGridFreqMax_2) || ((*pTFreq) > wGridFreqMax_2))
        	&&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue)&&(fFVRTDelay == 0))
        {
        	uwFreHighCnt2++;
        	if(uwFreHighCnt2 >= uwHighFreTripTime2)
        	{
        		uwFreHighCnt2 = uwHighFreTripTime2;
        	}
        }
        else
        {
        	uwFreHighCnt2 = 0;
        }
        
        if((((*pRFreq) > wGridFreqMax_1) || ((*pSFreq) > wGridFreqMax_1) || ((*pTFreq) > wGridFreqMax_1))
        	&&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue)&&(fFVRTDelay == 0))
        {
        	uwFreHighCnt1++;
        	if(uwFreHighCnt1 >= uwHighFreTripTime1)
        	{
        		uwFreHighCnt1 = uwHighFreTripTime1;
        	}
        }
        else
        {
        	uwFreHighCnt1 = 0;
        }
      
        if(((*pRFreq) < wGridFreqRecover_Max) && ((*pSFreq) < wGridFreqRecover_Max) && ((*pTFreq) < wGridFreqRecover_Max))
        {
        	uwFreHighCnt1 = 0;
        	uwFreHighCnt2 = 0;
        	uwFreRecCnt++;
        }
        else
        {
        	uwFreRecCnt = 0;
        }
        
        if((uwFreHighCnt2 >= uwHighFreTripTime2)||(uwFreHighCnt1 >= uwHighFreTripTime1))
        {
        	*pErrFlag = 1;
        	fGridPretect = 1;
            
            if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;
            }
        }
        else
        {
        	if(uwFreRecCnt >= uwHighFreExitTime)
        	{
        		uwFreRecCnt = uwHighFreExitTime;
        		*pErrFlag = 0;
        	}
        }
    }
    else
    {
        uwFreHighCnt2 = 0;
        
        if(wEepromRemoteSel == 1)
        {
            wGridFreqMax_1 = ((INT32S)wRemoteVSFreqSet * 10486)>>12;
            wGridFreqRecover_Max = wGridFreqMax_1 + 2048;
            wGridFreqMax_1 = wGridFreqMax_1 + 2560;		        
	    }
	    else
	    {
	        wGridFreqMax_1 = ((INT32S)wEepromVSFreqSet * 10486)>>12;
            wGridFreqRecover_Max = wGridFreqMax_1 + 2048;
            wGridFreqMax_1 = wGridFreqMax_1 + 2560;
	    }
        
        if((INT16U)wEepromFreqMaxTripTime_1 > 3)
        {
        	uwHighFreTripTime1 = (INT16U)wEepromFreqMaxTripTime_1 - 3;
        }
        else
        {
        	uwHighFreTripTime1 = 1;
        }
        
        if(((*pRFreq) > wGridFreqMax_1) || ((*pSFreq) > wGridFreqMax_1) || ((*pTFreq) > wGridFreqMax_1))
        {
        	uwFreHighCnt1++;
        	if(uwFreHighCnt1 >= uwHighFreTripTime1)
        	{
        		uwFreHighCnt1 = uwHighFreTripTime1;
        	}
        }
        else
        {
        	uwFreHighCnt1 = 0;
        }

        if(((*pRFreq) < wGridFreqRecover_Max) && ((*pSFreq) < wGridFreqRecover_Max) && ((*pTFreq) < wGridFreqRecover_Max))
        {
        	uwFreHighCnt1 = 0;        	
        	uwFreRecCnt++;
        }
        else
        {
        	uwFreRecCnt = 0;
        }
        
        if((uwFreHighCnt1 >= uwHighFreTripTime1)&&(fVSFreqCheckEn == 1))
        {
        	*pErrFlag = 1;
        	fGridPretect = 1;
            
            if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;
            }
        }
        else
        {
            if(fVSFreqCheckEn == 0)
            {
                uwFreRecCnt = uwHighFreExitTime;
        		*pErrFlag = 0;
            }
            else
            {
        	    if(uwFreRecCnt >= uwHighFreExitTime)
        	    {
        	    	uwFreRecCnt = uwHighFreExitTime;
        	    	*pErrFlag = 0;
        	    }
        	}
        }
    }
}
/*********************************************************************
Function name:  void sFreLowChk(INT16S *pFre,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sFreLowChk(INT16U *pRFreq,INT16U *pSFreq,INT16U *pTFreq,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFreLowCnt1 = 0;
static INT16U uwFreLowCnt2 = 0;
static INT16U uwFreLowRecCnt = 0;
static INT16U fFVRTDelay = 0;
INT16U uwLowFreTripTime1,uwLowFreTripTime2,uwLowFreExitTime;
INT16S wGridFreqMin_1,wGridFreqMin_2,wGridFreqRecover_Min;

    if(fFactoryFastStart == 1)
    {
        uwLowFreExitTime = 50;
    }
    else
    {
        uwLowFreExitTime = wEepromGridFreqRecover_Time;
    }
        
    if(fCSVSSwitch == 0)
    {
        if(wEepromRatedFreqSel == 1)
        {
            wGridFreqMin_1 = ((INT32S)(wEepromGridFreqMin_1 + 6000) * 10486)>>12;
            wGridFreqMin_2 = ((INT32S)(wEepromGridFreqMin_2 + 6000) * 10486)>>12;		
	        wGridFreqRecover_Min = ((INT32S)(wEepromGridFreqRecover_Min + 6000) * 10486)>>12;
	    }
	    else
	    {
	        wGridFreqMin_1 = ((INT32S)(wEepromGridFreqMin_1 + 5000) * 10486)>>12;
            wGridFreqMin_2 = ((INT32S)(wEepromGridFreqMin_2 + 5000) * 10486)>>12;		
	        wGridFreqRecover_Min = ((INT32S)(wEepromGridFreqRecover_Min + 5000) * 10486)>>12;
	    }
        
        if((INT16U)wEepromFreqMinTripTime_2 > 3)
        {
        	uwLowFreTripTime2 = (INT16U)wEepromFreqMinTripTime_2 - 3;
        }
        else
        {
        	uwLowFreTripTime2 = 1;
        }
        
        if((INT16U)wEepromFreqMinTripTime_1 > 3)
        {
        	uwLowFreTripTime1 = (INT16U)wEepromFreqMinTripTime_1 - 3;
        }
        else
        {
        	uwLowFreTripTime1 = 1;
        } 

        if((fLVRTValue < uwLVRTActiveValue)||(fHVRTValue > uwHVRTActiveValue))
        { 
        	fFVRTDelay = 50;    	
        }
        else
        {
        	if(fFVRTDelay > 0)
        	{
        		fFVRTDelay--;
        	}
        	else
        	{
        		fFVRTDelay = 0;
        	}
        }
        
        if((((*pRFreq) < wGridFreqMin_2) || ((*pSFreq) < wGridFreqMin_2) || ((*pTFreq) < wGridFreqMin_2))
          &&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue)&&(fFVRTDelay == 0))
        {
        	uwFreLowCnt2++;
        	if(uwFreLowCnt2 >= uwLowFreTripTime2)
        	{
        		uwFreLowCnt2 = uwLowFreTripTime2;
        	}
        }
        else
        {
        	uwFreLowCnt2 = 0;
        }
        
        if((((*pRFreq) < wGridFreqMin_1) || ((*pSFreq) < wGridFreqMin_1) || ((*pTFreq) < wGridFreqMin_1))
        	&&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue)&&(fFVRTDelay == 0))
        {
        	uwFreLowCnt1++;
        	if(uwFreLowCnt1 >= uwLowFreTripTime1)
        	{
        		uwFreLowCnt1 = uwLowFreTripTime1;
        	}
        }
        else
        {
        	uwFreLowCnt1 = 0;
        }
        
        if(((*pRFreq) > wGridFreqRecover_Min) && ((*pSFreq) > wGridFreqRecover_Min) && ((*pTFreq) > wGridFreqRecover_Min))
        {
        	uwFreLowCnt1 = 0;
        	uwFreLowCnt2 = 0;
        	uwFreLowRecCnt++;
        }
        else
        {
        	uwFreLowRecCnt = 0;
        }
        
        if((uwFreLowCnt2 >= uwLowFreTripTime2)||(uwFreLowCnt1 >= uwLowFreTripTime1))
        {
        	*pErrFlag = 1;
            fGridPretect = 1;
            
            if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;
            }
        }
        else
        {
        	if(uwFreLowRecCnt >= uwLowFreExitTime)
        	{
        		uwFreLowRecCnt = uwLowFreExitTime;
        		*pErrFlag = 0;
        	}
        }
    }
    else
    {
        uwFreLowCnt2 = 0;
        
	    if(wEepromRemoteSel == 1)
        {
            wGridFreqMin_1 = ((INT32S)wRemoteVSFreqSet * 10486)>>12;
            wGridFreqRecover_Min = wGridFreqMin_1 - 2048;
            wGridFreqMin_1 = wGridFreqMin_1 - 2560;		        
	    }
	    else
	    {
            wGridFreqMin_1 = ((INT32S)wEepromVSFreqSet * 10486)>>12;
            wGridFreqRecover_Min = wGridFreqMin_1 - 2048;
            wGridFreqMin_1 = wGridFreqMin_1 - 2560;
	    }
        
        if((INT16U)wEepromFreqMinTripTime_1 > 3)
        {
        	uwLowFreTripTime1 = (INT16U)wEepromFreqMinTripTime_1 - 3;
        }
        else
        {
        	uwLowFreTripTime1 = 1;
        }
        
        if((((*pRFreq) < wGridFreqMin_1) || ((*pSFreq) < wGridFreqMin_1) || ((*pTFreq) < wGridFreqMin_1))
        	&&(uwSysMode == InvRunMode))
        {
        	uwFreLowCnt1++;
        	if(uwFreLowCnt1 >= uwLowFreTripTime1)
        	{
        		uwFreLowCnt1 = uwLowFreTripTime1;
        	}
        }
        else
        {
        	uwFreLowCnt1 = 0;
        }
        
        if((((*pRFreq) > wGridFreqRecover_Min) && ((*pSFreq) > wGridFreqRecover_Min) && ((*pTFreq) > wGridFreqRecover_Min))||(uwSysMode != InvRunMode))
        {
        	uwFreLowCnt1 = 0;
        	uwFreLowRecCnt++;
        }
        else
        {
        	uwFreLowRecCnt = 0;
        }
        
        if((uwFreLowCnt1 >= uwLowFreTripTime1)&&(fVSFreqCheckEn == 1))
        {
        	*pErrFlag = 1;
            fGridPretect = 1;
            
            if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;
            }
        }
        else
        {
            if(fVSFreqCheckEn == 0)
            {
                uwFreLowRecCnt = uwLowFreExitTime;
        	    *pErrFlag = 0;
            }
            else
            {
        	    if(uwFreLowRecCnt >= uwLowFreExitTime)
        	    {
        	    	uwFreLowRecCnt = uwLowFreExitTime;
        	    	*pErrFlag = 0;
        	    }
        	}
        }
    }
} 
/*********************************************************************
Function name:void sAIFreqChk(INT16U *pRFreq,INT16U *pSFreq,INT16U *pTFreq,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sAIFreqChk(INT16U *pRFreq,INT16U *pSFreq,INT16U *pTFreq,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrCnt = 0;
static INT16U uwOKCnt = 0;
static INT16U fFVRTDelay = 0;

    if((fLVRTValue < uwLVRTActiveValue)||(fHVRTValue > uwHVRTActiveValue))
    { 
    	fFVRTDelay = 50;    	
    }
    else
    {
    	if(fFVRTDelay > 0)
    	{
    		fFVRTDelay--;
    	}
    	else
    	{
    		fFVRTDelay = 0;
    	}
    }
    
    if((wEepromAIEnable == 1)&&(fFVRTDelay == 0))
    {
    	if((abs((INT16S)(*pRFreq) - (INT16S)(*pSFreq)) > 256) || (abs((INT16S)(*pSFreq) - (INT16S)(*pTFreq)) > 256) || (abs((INT16S)(*pTFreq) - (INT16S)(*pRFreq)) > 256))
        {
        	uwOKCnt = 0;
        	
        	if(++uwErrCnt >= 2)
        	{
        		*pErrFlag = 1;
                
                if((*pEventFlag) == 0)
                {
                    sErroringDeal();     
                    *pEventFlag = 1;
                }
        	}
        }
        else
        {
        	uwErrCnt = 0;
        	if(++uwOKCnt >= 500)
        	{
        		uwOKCnt = 500;
        		*pErrFlag = 0;
        	}
        }
    }
    else
    {
    	uwErrCnt = 0;
        if(++uwOKCnt >= 500)
        {
        	uwOKCnt = 500;
        	*pErrFlag = 0;
        }
    }    
} 
/*********************************************************************
Function name: sTmpHighChk(INT16S *pBstTmp,INT16S *pInvTmp,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sTmpHighChk(INT16S *pHeatTmp,INT16S *pEnvTmp,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwTmpHighCnt = 0,uwTmpRecCnt = 0;
static INT16U uwEnvTempHighCnt = 0;
static INT16U uwHeatTempHighCnt = 0;
INT16S wHeatTempMax,wEnvironmentTmpMax;
INT16S wHeatTemp;
INT16S wEnvTemp;
    
    wHeatTemp = *pHeatTmp;
    wEnvTemp = *pEnvTmp;
    
    wHeatTempMax = ((INT32S)wEepromPowerTmpMax * 26214)>>14;
    wEnvironmentTmpMax = ((INT32S)wEepromEnvironmentTmpMax * 26214)>>14;
    
    if(checkbit(fTempdisable,0) != 0)
    {
    	wHeatTemp = wHeatTempMax - cTemp15C - 16;
    }
    
    if(checkbit(fTempdisable,1) != 0)
    {
    	wEnvTemp = wEnvironmentTmpMax - cTemp5C - 16;
    }
   
    if(wHeatTemp >= cTemp120C)
	{
		  if((++uwHeatTempHighCnt)>1800)  //15min
		  {
		  	  fTempdisable |= 1;  
		      uwHeatTempHighCnt = 0;
		  }
	}
	  
	if(wEnvTemp >= cTemp90C)
	{
		  if((++uwEnvTempHighCnt)>1800)   //15min
		  {
		  	  fTempdisable |= 2;  
		      uwEnvTempHighCnt = 0;
		  }
	}
	
	if((wHeatTemp > wHeatTempMax) || (wEnvTemp > wEnvironmentTmpMax))
	{
		if(++uwTmpHighCnt > 5)//5*500=2.5s
		{
			uwTmpHighCnt = 5;
			uwTmpRecCnt = 0;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal();                
                *pEventFlag = 1;
            }
			
		}
		
	}
	else
	{
	    if((wHeatTemp < (wHeatTempMax - cTemp15C)) && (wEnvTemp < (wEnvironmentTmpMax - cTemp5C)))
	    {
	        if(++uwTmpRecCnt > 120)
	        {
	            uwTmpRecCnt = 120;
	            uwTmpHighCnt = 0;
	            *pErrFlag = 0; 
	        }   
	    }
	}
}
/*********************************************************************
Function name: void sTmpLoseChk(INT16S *pEnvTemp,INT16S *pXFMRTmp,INT16S *TempErr,INT16S *TempErrEvent)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sTmpLoseChk(INT16S *pHeatTemp,INT16S *pEnvTemp,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwTmpLoseCnt = 0;
static INT16U uwErrClearDelay5s = 0;
    
    if(*pHeatTemp < cTemptN30C)
	{
		fTempdisable |= 1;  
	}
	else if(((*pHeatTemp) > (cTemptN30C + 80))&&((*pHeatTemp) < (cTemp120C - 160)))
	{
	    fTempdisable &= 0xFFFE;
	}
	   
	if(*pEnvTemp < cTemptN35C)
	{
		fTempdisable |= 2;  
	}
	else if(((*pEnvTemp) > (cTemptN35C + 80))&&((*pEnvTemp) < (cTemp90C - 160)))
	{
	    fTempdisable &= 0xFFFD; 
	}
	
    if(fTempdisable > 0)
    {
    	if(++uwTmpLoseCnt > 4)
    	{
    		uwTmpLoseCnt = 4;
    		uwErrClearDelay5s = 0;
    		*pErrFlag = 1;
            if((*pEventFlag) == 0)
            {
                *pEventFlag = 1;    //Warning!
            }
    	}
    }
    else
    {
        if(uwTmpLoseCnt > 0)
		{
		    uwTmpLoseCnt--;
		}
	    else
	    {
		    if(++uwErrClearDelay5s > 10)
		    {    
		        uwErrClearDelay5s = 10;
		        *pErrFlag = 0;
		    }     
	    }   
    }
}
/*********************************************************************
Function name:  void sOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16U i;
static INT16U uwErrClearDelay2s = 0;
    if((*pOffset) > (*pDefaultOffset)) i = (*pOffset) - (*pDefaultOffset);
    else i = (*pDefaultOffset) - (*pOffset);
    	 	
	if(i > wEepromOffsetDiffMax)  //0.05V/1.5*2048=68  wEepromOffsetDiffMax
	{
		if(++(*pOffsetHighCnt) > 10) 
		{
			*pOffsetHighCnt = 10;
			uwErrClearDelay2s = 0;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal();                                                                           
                *pEventFlag = 1;
            }
		}	
	}
	else
	{
		if((*pOffsetHighCnt) > 0)
		{
			(*pOffsetHighCnt)--;
		}
	    else
	    {
		    if(++uwErrClearDelay2s > 10000)
		    {    
		        uwErrClearDelay2s = 10000;
		        *pErrFlag = 0;
		    }     
	    }   	
	}		
}
/*********************************************************************
Function name:  void sGridCurrOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sGridCurrOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16U i;
static INT16U uwErrClearDelay2s = 0;
    if((*pOffset) > (*pDefaultOffset)) i = (*pOffset) - (*pDefaultOffset);
    else i = (*pDefaultOffset) - (*pOffset);
    	 	
	if(i > (wEepromOffsetDiffMax<<1))  //0.05V/1.5*2048=68  wEepromOffsetDiffMax
	{
		if(++(*pOffsetHighCnt) > 10) 
		{
			*pOffsetHighCnt = 10;
			uwErrClearDelay2s = 0;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal();                                                                           
                *pEventFlag = 1;
            }
		}	
	}
	else
	{
		if((*pOffsetHighCnt) > 0)
		{
			(*pOffsetHighCnt)--;
		}
	    else
	    {
		    if(++uwErrClearDelay2s > 10000)
		    {    
		        uwErrClearDelay2s = 10000;
		        *pErrFlag = 0;
		    }     
	    }   	
	}		
}
/*********************************************************************
Function name:  void sDCIOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sDCIOffsetHighChk(INT16U *pOffset,INT16S *pDefaultOffset,INT16U *pOffsetHighCnt,INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16U i;
static INT16U uwErrClearDelay2s = 0;
    if((*pOffset) > (*pDefaultOffset))
    {
        i = (*pOffset) - (*pDefaultOffset);
    }
    else
    { 
        i = (*pDefaultOffset) - (*pOffset);
    }
    		
	if(i > wEepromDCIOffsetDiffMax)  //10%误差 300
	{
		if(++(*pOffsetHighCnt) > 10)  
		{
			*pOffsetHighCnt = 10;
			uwErrClearDelay2s = 0;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal();                                                                           
                *pEventFlag = 1;
            }
		}	
	}
	else
	{
		if((*pOffsetHighCnt) > 0)
		{
			(*pOffsetHighCnt)--;
		}
	    else
	    {
		    if(++uwErrClearDelay2s > 10000)
		    {    
		        uwErrClearDelay2s = 10000;
		        *pErrFlag = 0;
		    }     
	    }   	
	}		
}

/*********************************************************************
Function name:  void sDCIHighChk(INT16S *pRDCI,INT16S *pSDCI,INT16S *pTDCI,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sDCIHighChk(INT16S *pRDCI,INT16S *pSDCI,INT16S *pTDCI,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwDCIHighCnt = 0;
static INT16U uwErrClearDelay60s = 0;
INT16S wDCIMax;
static INT16U uwDCIPrtTime = 50;
static INT16U uwDelay3s = 150;
static INT16U fChargeBak=0;

    wDCIMax = ((INT32S)wEepromDCIMax * 26214)>>15;
    
    if(fUqUnconv == 1)
    {
        uwDCIPrtTime = 500;
        uwDelay3s = 0;
    }
    else
    {
        if(++uwDelay3s >= 150)
        {
            uwDelay3s = 150;
            uwDCIPrtTime = 250;
        }
    }
    
    if((fInvEnOut == 1) && (uwSysMode == InvRunMode) && (fCSVSSwitch == 0)) //逆变运行时DCI故障标志又清又置，其他情况标志只清不置
    {
    	if(fDCIDelayClear == 0)
    	{
    	    uwDCIChkDelay5sCnt = 0;
    	    fDCIDelayClear = 2;
    	}
    	else if((fDCIDelayClear == 1)||(fChargeBak!=fCharge))
    	{
    	    uwDCIChkDelay5sCnt = 1500;
    	    fDCIDelayClear = 2;
			uwDCIHighCnt=0;
			//wDebugTest5=fDCIDelayClear;
    	}
    	if(++uwDCIChkDelay5sCnt >= 2000)
    	{
    		uwDCIChkDelay5sCnt = 2000;
                                                         		
    		if((abs(*pRDCI) > wDCIMax) || (abs(*pSDCI) > wDCIMax) || (abs(*pTDCI) > wDCIMax))                                                                               
    		{
    	        if(++uwDCIHighCnt > uwDCIPrtTime)                                                    
    			{                                                                         
    				uwDCIHighCnt = uwDCIPrtTime;
    				uwErrClearDelay60s = 0;                                                     
    				*pErrFlag = 1;                                                        
    		                                                                              
    		        if((*pEventFlag) == 0)                                                
    		        {                                                                     
    		            sErroringDeal();                                                  
    		            *pEventFlag = 1;                                                  
    		        }                                                                       				                                                                      
    			}	         
    	    
    	    }
    		else                                                                          
    		{                                                                             
    			uwDCIHighCnt = 0;
    			if(++uwErrClearDelay60s > 3000)
    			{
    			    uwErrClearDelay60s = 3000;
    			    *pErrFlag = 0;         
    			}                                                   			   	                                                                  
    		}    	                                                                       		
    	}	
    }
    else
    {  	
    	uwDCIChkDelay5sCnt = 0;
    	
    	if(((abs(*pRDCI) < (wDCIMax - 80)) && (abs(*pSDCI) < (wDCIMax - 80)) && (abs(*pTDCI) < (wDCIMax - 80)))||(fCSVSSwitch == 1))
    	{
    		uwDCIHighCnt= 0;                                                                       
    		if(++uwErrClearDelay60s > 3000)
    		{
    		    uwErrClearDelay60s = 3000;
    		    *pErrFlag = 0;         
    		}                                                 	        
    	}      
    }	
	fChargeBak=fCharge;
}
/*********************************************************************
Function name:void sGFCIDynamicHighChk(INT16S wGFCIRmsCurr,INT16S wGFCIAvgCurr,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  CAPIsr(void) cap interupt
Parameters:  void
Return:  void      
**********************************************************************/
/********************************************************************
安规要求
30mA   300ms
60mA   150ms
150mA  40ms
实际动作
x:30mA   y:240ms   
x:60mA   y:100ms
x:150mA  y:10ms
即：
30mA  24
60mA  10
150mA 1   可设定150mA为阀值。
则30mA 累加到150mA 需要 5次，而实际是24次，因此系数为0.208333
则60mA 累加到150mA 需要 2.5次，而实际是10次，因此系数为0.25
这里采用2阶拟合y = 0.000057870*X^2 - 0.003819444*X + 0.270833333
********************************************************************/ 
void sGFCIDynamicHighChk(INT16S wGFCCurrRms,INT16S wGFCCurrAvg,INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16S wGFCIAvgInitValue,wGFCIRmsInitValue;
INT16S wDeltaGFCIAvgNew,wDeltaGFCIRmsNew;
INT16S wDeltaGFCIAvgCoeffNew,wDeltaGFCIRmsCoeffNew;
   
    wGFCIAvgInitValue = dwGFCIAvgInitSum>>3;
    wGFCIRmsInitValue = dwGFCIRmsInitSum>>3;
    
    wDeltaGFCIAvgNew = abs(wGFCCurrAvg - wGFCIAvgInitValue);
    wDeltaGFCIRmsNew = wGFCCurrRms - wGFCIRmsInitValue;//abs(wGFCCurrRms - wGFCIRmsInitValue);
        
    wDeltaGFCIAvgCoeffNew = swGFCICoeffCal(wDeltaGFCIAvgNew);
    wDeltaGFCIRmsCoeffNew = swGFCICoeffCal(wDeltaGFCIRmsNew);
    
    wDeltaGFCIAvgNew = ((INT32S)wDeltaGFCIAvgNew * wDeltaGFCIAvgCoeffNew)>>14;
    wDeltaGFCIRmsNew = ((INT32S)wDeltaGFCIRmsNew * wDeltaGFCIRmsCoeffNew)>>14;
    
    dwGFCIAvgActSum = dwGFCIAvgActSum - wDeltaGFCIAvgArray[uwGFCIActArrayIndex] + wDeltaGFCIAvgNew;
    dwGFCIRmsActSum = dwGFCIRmsActSum - wDeltaGFCIRmsArray[uwGFCIActArrayIndex] + wDeltaGFCIRmsNew;
    
    dwGFCIAvgInitSum = dwGFCIAvgInitSum - wGFCIAvgInitArray[uwGFCIInitArrayIndex] + wGFCIAvgActArray[uwGFCIActArrayIndex];
    dwGFCIRmsInitSum = dwGFCIRmsInitSum - wGFCIRmsInitArray[uwGFCIInitArrayIndex] + wGFCIRmsActArray[uwGFCIActArrayIndex];
    
    wDeltaGFCIAvgArray[uwGFCIActArrayIndex] = wDeltaGFCIAvgNew;
    wDeltaGFCIRmsArray[uwGFCIActArrayIndex] = wDeltaGFCIRmsNew;
    
    wGFCIAvgInitArray[uwGFCIInitArrayIndex] = wGFCIAvgActArray[uwGFCIActArrayIndex];
    wGFCIRmsInitArray[uwGFCIInitArrayIndex] = wGFCIRmsActArray[uwGFCIActArrayIndex];
    
    wGFCIAvgActArray[uwGFCIActArrayIndex] = wGFCCurrAvg;
    wGFCIRmsActArray[uwGFCIActArrayIndex] = wGFCCurrRms;
    
    uwGFCIInitArrayIndex++;
    if(uwGFCIInitArrayIndex >= 8)
    {
    	uwGFCIInitArrayIndex = 0;
    }
    
    uwGFCIActArrayIndex++;
    if(uwGFCIActArrayIndex >= 30)
    {
    	uwGFCIActArrayIndex = 0;
    }
        	
	if((uwSysMode > PreCheckMode)&&(fLVRTValue > uwSoftLVRTValue)&&(fHVRTValue < uwSoftHVRTValue))
	{
	    if((fGridRelayCtrBak != fGridRelayCtr)||(fDCRelayCtrBak != fDCRelayPCtr)||(fAuxiRelayCtrBak != fAuxiRelayCtr)
	      ||(fInvEnOutBak != fInvEnOut)||((fUqUnconvBak == 0)&&(fUqUnconv == 1)))
	    {
	    	uwDeltaGFCIChkDelay100Prd = 0;
	    }
	}
	else
	{
		uwDeltaGFCIChkDelay100Prd = 0;
	}
		
	fGridRelayCtrBak = fGridRelayCtr;
	fDCRelayCtrBak = fDCRelayPCtr;
	fAuxiRelayCtrBak = fAuxiRelayCtr;
	fInvEnOutBak = fInvEnOut;
	fUqUnconvBak = fUqUnconv;
	
	if(++uwDeltaGFCIChkDelay100Prd >= 100)
	{
		uwDeltaGFCIChkDelay100Prd = 100;
	}
  	
	if(((dwGFCIAvgActSum > cGFCI150mA) || (dwGFCIRmsActSum > cGFCI150mA)) && (uwDeltaGFCIChkDelay100Prd >= 100))
	{
		uwDeltaGFCIErrClearDelay10s = 0;
		*pErrFlag = 1;
        
        if((*pEventFlag) == 0)
        {
            sErroringDeal();            
            *pEventFlag = 1;
        }
	}
	else
	{
		if(++uwDeltaGFCIErrClearDelay10s > 1000)
		{
		    uwDeltaGFCIErrClearDelay10s = 1000; 
		    *pErrFlag = 0;
		}
	} 	
}
/*********************************************************************
Function name:INT16S swGFCICoeffCal(INT16S wGFCICurr)
Description:  
Calls:        	 
Called By: 
Parameters:  INT16S
Return:  INT16S  
y = 0.000057870*X^2 - 0.003819444*X + 0.270833333    
**********************************************************************/
INT16S swGFCICoeffCal(INT16S wCurr)
{
INT16S wCoeff;
	
	if(wCurr > cGFCI150mA)
    {
    	wCoeff = 16384;
    }
    else if(wCurr < cGFCI20mA)
    {
    	wCoeff = 0;
    }
    else
    {
    	wCoeff = 4437 - (((INT32S)wCurr * 4005)>>10) + (((INT64S)((INT32S)wCurr * wCurr) * 15534)>>22);
    }
    
    return(wCoeff);
}
/*********************************************************************
Function name:void sGFCISteadyHighChk(INT16S *pGFCIRmsCurr,INT16S *pGFCIAvgCurr,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  CAPIsr(void) cap interupt
Parameters:  void
Return:  void      
**********************************************************************/
void sGFCISteadyHighChk(INT16S *pGFCIRmsCurr,INT16S *pGFCIAvgCurr,INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16S wGFCIHighTemp;

	wGFCIHighTemp = ((INT32S)wEepromGFCIMax * 26214)>>14;
	
	if(((*pGFCIRmsCurr) > wGFCIHighTemp)||(abs(*pGFCIAvgCurr) > wGFCIHighTemp))
	{
	    if (++uwGFCIHighCnt > 20)
	    {
	        uwGFCIHighCnt = 20;
	        uwSteadyErrClearDelay10s = 0;
		    *pErrFlag = 1;
            
            if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;
            }
	    }
	}
	else
	{
	    if(uwGFCIHighCnt > 0)
		{   
			uwGFCIHighCnt--;
		}
		else
		{
		    if(++uwSteadyErrClearDelay10s >= 1000)
		    {
		        uwSteadyErrClearDelay10s = 1000; 
		        *pErrFlag = 0;
		    }
	    }
	}
}
/*********************************************************************
Function name:  void sEepromChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sEepromChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrClearDelay2s = 0;
    if(uwI2CBlockState.bit.fEpErr == 1) //fEepromChkError
    {
    	uwErrClearDelay2s = 0;
    	*pErrFlag = 1;
    	
    	if((*pEventFlag) == 0)
    	{
    		*pEventFlag = 1;
      }	
    }
    else
    {
    	if(++uwErrClearDelay2s > 100)
    	{
    	    uwErrClearDelay2s = 100;
    	    *pErrFlag = 0;
        } 
    }
}
/*********************************************************************
Function name:  void sGridNoUtilityChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  ADCIsr(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sGridNoUtilityChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16U i = 0,j = 0;
static INT16U uwErrClearCnt = 0;
static INT32U udwNoUtilityCnt = 0;
static INT16U uwDetectLstPhase5msCnt = 0;
static INT16U uwDetectRecPhase5msCnt = 0;
INT16U uwExitTimeSet;
	
	if(fFactoryFastStart == 1)
	{
	    uwExitTimeSet = 8000;
	}
	else
	{
	    uwExitTimeSet = 64000;
	}
	
	if(fCSVSSwitch == 0)
	{
        if((fRLstPhase == 1) || (fSLstPhase == 1) || (fTLstPhase == 1))
	    {		
	        if(++uwDetectLstPhase5msCnt > 16) //50
	        {
	            i = 1;
	            uwDetectLstPhase5msCnt = 16;	
	            sRmsClearZero(); //有效值清零 		
	        }
	        uwDetectRecPhase5msCnt = 0;
        }
        else
	    {
	        if(++uwDetectRecPhase5msCnt > 16)
	        {
	            i = 0;
	            uwDetectRecPhase5msCnt = 16;
	        }
	        uwDetectLstPhase5msCnt = 0;
        }
        
        if((uwSysMode == PowerOnMode) || (uwSysMode == FaultMode) || (uwSysMode == ShutMode))
        {
            if(((abs(wRSGridVol) < cVolt10V) && (abs(wSTGridVol) < cVolt10V)) || ((abs(wSTGridVol) < cVolt10V) && (abs(wTRGridVol) < cVolt10V)) ||
	           ((abs(wTRGridVol) < cVolt10V) && (abs(wRSGridVol) < cVolt10V)) || ((abs(wRGridVol) < cVolt10V) && (abs(wSGridVol) < cVolt10V)) ||
	           ((abs(wSGridVol) < cVolt10V) && (abs(wTGridVol) < cVolt10V)) || ((abs(wTGridVol) < cVolt10V) && (abs(wRGridVol) < cVolt10V)))
            {
	            j = 1;
            }
            else 
            {
	            j = 0;
            }
        }
        else
        {
            j = 0;
        }
        
	    if((j == 1) || (i == 1))
	    {
	        if(++udwNoUtilityCnt > 320000)  // for  wRSGridVol && wRGridVol
	        {
	            udwNoUtilityCnt = 320000;
	            *pErrFlag = 1; 	
	            fGridPretect = 1;	    
	    	    if((*pEventFlag) == 0)
                {
                    sErroringDeal();            
                    *pEventFlag = 1;
                }
            }
	    	uwErrClearCnt = 0;  
	    }
	    else if((j == 0) && (i == 0))
	    {
	        if(++uwErrClearCnt > uwExitTimeSet)  //0.2ms*10000=2s
	        {    
	    		uwErrClearCnt = uwExitTimeSet;
	            *pErrFlag = 0;
	        }
	    	udwNoUtilityCnt= 0;
	    } 
	}
	else
	{
	    udwNoUtilityCnt = 0; 
		uwDetectLstPhase5msCnt = 0;  
		uwDetectRecPhase5msCnt = 0;  
	    uwErrClearCnt = uwExitTimeSet;
	    *pErrFlag = 0;
	}
}
/********************************************************************* 
Function name:INT16U sfRelayChk(INT16U *pGridErrFlag,INT16U *pGridEventFlag,INT16U *pInvErrFlag,INT16U *pInvEventFlag)                        
Description:  check the Output Relay (inverter relay and grid relay), if it is normally working             
Calls:        	                                                       
Called By:  in PreChk Mode, and Maybe in "Protect.c" file, 20ms period time                                        
Parameters:  void                                                      
Return:  INT16U type flag                                                       
 *********************************************************************/
INT16U sfRelayChk(INT16U *pGridErrFlag,INT16U *pGridEventFlag,INT16U *pInvErrFlag,INT16U *pInvEventFlag)  
{   
INT16U fRelayChkOk = 0;
INT16S wRGridRmsVol,wSGridRmsVol,wTGridRmsVol;
INT16S wRInvRmsVolReal,wSInvRmsVolReal,wTInvRmsVolReal;

    wRGridRmsVol = ((INT32S)wRSGridRmsVol * 9459)>>14;
    wSGridRmsVol = ((INT32S)wSTGridRmsVol * 9459)>>14;
    wTGridRmsVol = ((INT32S)wTRGridRmsVol * 9459)>>14;
    
    wRGridRmsVol = wRGridRmsVol - cVolt80V;
    wSGridRmsVol = wSGridRmsVol - cVolt80V;
    wTGridRmsVol = wTGridRmsVol - cVolt80V; 
    
    if(wRGridRmsVol < cVolt50V)
    {
        wRGridRmsVol = cVolt50V;
    }
    
    if(wSGridRmsVol < cVolt50V)
    {
        wSGridRmsVol = cVolt50V;
    }
    
    if(wTGridRmsVol < cVolt50V)
    {
        wTGridRmsVol = cVolt50V;
    }
    
    wRInvRmsVolReal = wRInvRmsVol - abs(wRInvAvgVol);//VS Mode,No gird,The grid voltage,inv voltage and invavgvol is 0
    wSInvRmsVolReal = wSInvRmsVol - abs(wSInvAvgVol);
    wTInvRmsVolReal = wTInvRmsVol - abs(wTInvAvgVol);

    switch(fRelayCheckStep)
    {
    	case 0:
    		uwInvRelayOKCnt = 0;
            uwInvRelayBadCnt = 0;
            uwGridRelayOKCnt = 0;
            uwGridRelayBadCnt = 0;
            uwAuxiRelayOKCnt = 0;
            uwAuxiRelayBadCnt = 0;
            sInvRelayOpen();
            sGridRelayOpen();
            sAuxiRelayOpen();
            fRelayCheckStep++;
            break;
            
        case 1:
        	if((wRInvRmsVolReal > wRGridRmsVol)||(wSInvRmsVolReal > wSGridRmsVol)||(wTInvRmsVolReal > wTGridRmsVol))
            {
            	uwInvRelayBadCnt++;
            	if(uwInvRelayBadCnt >= 100)
            	{
            		uwInvRelayBadCnt = 0;
            		*pGridErrFlag = SET;
            		*pInvErrFlag = SET;
                	if(((*pGridEventFlag) == 0)||((*pInvEventFlag) == 0))
                	{
                	    sErroringDeal();
                		*pGridEventFlag = 1;
                		*pInvEventFlag = 1;
                	}
            	}
            }
            else
            {
            	if(uwInvRelayBadCnt > 0)
            	{
            		uwInvRelayBadCnt--;
            	}
            	else
            	{
            		uwInvRelayOKCnt++;
            		if(uwInvRelayOKCnt >= 100)
            		{
            			uwInvRelayOKCnt = 0;
            			fRelayCheckStep++;            			
            		}
            	}
            }	
        	break;
        	
        case 2:
        	if(wBusAvgVol < (wStdBusVolRef - cVolt70V - cVolt70V))
        	{
        		sAuxiRelayClose();
        		
        		if(++uwAuxiRelayBadCnt >= 1000)
        		{
        			uwAuxiRelayBadCnt = 1000;
        			fAuxiRelayLost = 1;
        		}        			
        	}
        	else
        	{
        		if(++uwAuxiRelayOKCnt >= 200)
        		{
        			uwAuxiRelayOKCnt = 200;
        			sAuxiRelayOpen();
        		    uwAuxiRelayBadCnt = 0;
        		    fRelayCheckStep++; 
        		}        		       			
        	}
        	break;
        	
        case 3:        	    		
        	sGridRelayClose();
        	sAuxiRelayOpen();
        	uwInvRelayOKCnt = 0;
        	uwInvRelayBadCnt = 0;
        	uwGridRelayOKCnt = 0;
            uwGridRelayBadCnt = 0;
            uwAuxiRelayOKCnt = 0;
            uwAuxiRelayBadCnt = 0;
            fRelayCheckStep++;
            break;
        	       	
        case 4: 
        	uwGridRelayOKCnt = 0;
            uwGridRelayBadCnt = 0;
                   	
        	if((wRInvRmsVolReal > wRGridRmsVol)||(wSInvRmsVolReal > wSGridRmsVol)||(wTInvRmsVolReal > wTGridRmsVol))
        	{
        		if(++uwInvRelayBadCnt >= 50)
        		{
        			uwInvRelayBadCnt = 50;
        			*pInvErrFlag = 1;
        			
        			if(*pInvEventFlag == 0)
        			{
        				sErroringDeal();
        				*pInvEventFlag = 1;
        			}
        		}
        	}
        	else
        	{
        		if(uwInvRelayBadCnt > 0)
            	{
            		uwInvRelayBadCnt--;
            	}
            	else
            	{            		
        		    if(++uwInvRelayOKCnt >= 100)
        		    {
        		    	uwInvRelayOKCnt = 0;
        		    	sGridRelayOpen();
        		    	fRelayCheckStep++;
        		    }
        		}
        	}        	
			break;
		
		case 5:
			if(++uwInvRelayOKCnt >= 50)
        	{ 
        		uwInvRelayOKCnt = 0;    	
        	    sInvRelayClose();
        	    fRelayCheckStep++;
        	}      	
        	break;
        	
        case 6:
        	uwInvRelayOKCnt = 0;
            uwInvRelayBadCnt = 0;
            uwAuxiRelayOKCnt = 0;
            uwAuxiRelayBadCnt = 0;
                   	
        	if((wRInvRmsVolReal > wRGridRmsVol)||(wSInvRmsVolReal > wSGridRmsVol)||(wTInvRmsVolReal > wTGridRmsVol))
        	{
        		if(++uwGridRelayBadCnt >= 50)
        		{
        			uwGridRelayBadCnt = 50;
        			*pGridErrFlag = 1;
        			
        			if(*pGridEventFlag == 0)
        			{
        				sErroringDeal();
        				*pGridEventFlag = 1;
        			}
        		}
        	}
        	else
        	{
        		if(uwGridRelayBadCnt > 0)
            	{
            		uwGridRelayBadCnt--;
            	}
            	else
            	{
        		    if(++uwGridRelayOKCnt >= 100)
        		    {
        		    	uwGridRelayOKCnt = 0;
        		    	fRelayCheckStep++;
        		    }
        		}
        	}        	
			break;		
			
		case 7:
			uwInvRelayOKCnt = 0;
            uwInvRelayBadCnt = 0;
            uwGridRelayOKCnt = 0;
            uwGridRelayBadCnt = 0;
            uwAuxiRelayOKCnt = 0;
            uwAuxiRelayBadCnt = 0;
            fRelayChkOk = 1;
            break;
            
        default:
        	uwInvRelayOKCnt = 0;
            uwInvRelayBadCnt = 0;
            uwGridRelayOKCnt = 0;
            uwGridRelayBadCnt = 0;
            sInvRelayOpen();
            sGridRelayOpen();
            sAuxiRelayOpen();
            break;        	     	
    }
    
 	return(fRelayChkOk);
}
/*********************************************************************                                
Function name:  INT16U sfIsoChk(INT16U *pErrFlag,INT16U *pEventFlag)                                 
Description:  check the PV isolation if is normal                                                     
Calls:        	                                                                                      
Called By:  in PreChk Mode, and Maybe in "Protect.c" file, 20ms period time                           
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/                               
INT16U sfIsoChk(INT16U *pErrFlag,INT16U *pEventFlag)                                                    
{                                                                                                                                                      
INT16U wIsoRestemp;
    wIsoRestemp=wEepromIsoRes;
	//start check   
	     
    uwIsoTimer20msCnt++;
    if(uwIsoTimer20msCnt >= 2)
    {	                                                                             
    	uwIsoTimer20msCnt = 0;
    	
    	if(uwfIsoChkFinished == 2)   //=2表示检测进行中，=0表示未检测，=1表示已检测                                                                     
     	{                                                                                                  	                                                                                                                           
     		switch(uwIsoChkStep)                                                                          
     		{                                                                                             
     			case 0:// wait for about 1s ,prepare for calculating Iso average voltage                                                                                                                                                                           	       	                                           
     	       	    uwIsoChkCounter = 0;                                                         
     	       	    uwIsoSumCnt = 0;  
     	       	    wIsoRlyOpenVngAvgTemp = 0;  
     	       	    wIsoRlyCloseVngAvgTemp = 0;                                                                                                                  
                    dwIsoRlyOpenVngSum = 0; 
                    dwIsoRlyCloseVngSum = 0; 
                    sACPowerDisable();                                                          
                    sPERelayClose();          //PE Relay close
                    sIsoRelayOpen();  
                    uwIsoChkStep = 1; //into next step ,1                                                                                                                                                                                           
     	       	    break; 
     	       	                                                                               
     	       	case 1:                                                                                                                                                        
     	       	   	uwIsoChkCounter++;                                                                    
     	       		if (uwIsoChkCounter >= 800)   //1*20ms ,wait for Relay close                           
                    {                                                                                     
                    	uwIsoChkCounter = 0;  
                    	uwIsoChkStep = 2; //into next step,2                                                                   	                                                            
                    }                                                                                    
                    break;  
                                                                                                  
     			case 2:// calculate the average Vng                                                                                  
     				uwIsoSumCnt++;                                                                        
     				dwIsoRlyOpenVngSum = dwIsoRlyOpenVngSum + wIsoAvgVol;  //sum  
     				if (uwIsoSumCnt >=128)                                                                 
     				{                                                                                     
     				    wIsoRlyOpenVngAvgTemp = (INT16S)(dwIsoRlyOpenVngSum>>7);                               
     				    dwIsoRlyOpenVngSum = 0;                                                          
     				    uwIsoSumCnt = 0;                                                                 				                                        
     				    sIsoRelayClose(); //Iso Relay close  
     				    uwIsoChkStep = 3;	//into next step,3                                                     
     				}                                                                                   
     				break;   
     				                                                                             
     			case 3: 
     			    uwIsoChkCounter++;                                                                    
     	       	    if (uwIsoChkCounter >= 200)   //5*20ms ,wait for Relay open                            
                    {                                                                                     
                  	    uwIsoChkCounter = 0;   
                  	    uwIsoSumCnt = 0;
                  	    dwIsoRlyCloseVngSum = 0;                                                
                  	    uwIsoChkStep = 4; //into next step,4                                                
                    }                                                                                     
     	       	    break;  
     	       	                                                                                  
     	        case 4:                                                                               
     	       		uwIsoSumCnt++;                                                                        
     				dwIsoRlyCloseVngSum = dwIsoRlyCloseVngSum + wIsoAvgVol;  //sum 
     				
     				if(uwIsoSumCnt >= 128)                                                                 
     				{                                                                                     
     				    wIsoRlyCloseVngAvgTemp = (INT16S)(dwIsoRlyCloseVngSum>>128); 
						
     				    dwIsoRlyCloseVngSum = 0;                                                          
     				    uwIsoSumCnt = 0;                                                                      				                                        
     				    sIsoRelayOpen(); //Iso Relay close 
     				    sPERelayOpen(); //Iso Relay close  
     				    uwIsoChkStep = 5;	//into next step,5                                                     
     				}                                                                                   
     	       		break;	 
     	       		                                                                                 			                                                                      
     			case 5: //calculate the uwIsoRp and uwIsoRn
     			    { 
     			        INT32S dwIsoImpTemp,dwVoSubVc,dwDualVcSubVo; 
     			        dwVoSubVc = (INT32S)wIsoRlyOpenVngAvgTemp - wIsoRlyCloseVngAvgTemp;
     			        dwDualVcSubVo = (INT32S)((INT32S)wIsoRlyCloseVngAvgTemp<<1) - wIsoRlyOpenVngAvgTemp;
     			         			        
     			        if(((dwVoSubVc > 0)&&(dwDualVcSubVo > 0))||((dwVoSubVc < 0)&&(dwDualVcSubVo < 0)))
     			        {
     			            dwIsoImpTemp = (INT32S)(((INT64S)dwVoSubVc * cTestResImp)/dwDualVcSubVo);
							if(wIsoRestemp<=1)wIsoRestemp=1;
							if(dwIsoImpTemp >=wIsoRestemp)
				    	    {
				    	        dwIsoImpTemp = (wIsoRestemp-1);
				    	    }
 			                
 			                dwIsoImpTemp = ((INT64S)dwIsoImpTemp * wIsoRestemp)/(wIsoRestemp - dwIsoImpTemp);
 			                
 			                if(dwIsoImpTemp > 2000)
				    	    {
				    	        wIsoImp = 2000;
				    	    }
				    	    else
				    	    {
				    	        wIsoImp = dwIsoImpTemp;
				    	    } 
     			        }
     			        else
     			        {
     			            wIsoImp = 1;
     			        }
     			                          
     				    if(wIsoImp >= wEepromIsoImpMin)    //Iso ok                  
     				    {                                                                                     
     				    	uwfIsoChkFinished =1;                                                                 
     				        uwfIsoChkOk = 1;                                                
     				    }                                                                                               
     				    else                                                                                  
     				    {                                                                                     
     				    	*pErrFlag = 1;                                                                
                            if((*pEventFlag) == 0)                                                        
                            {                                                                             
                                sErroringDeal();                                                               
                                *pEventFlag = 1;                                                          
                            }                                                                             
    				    	uwfIsoChkFinished =1;                                                         
    				    	uwfIsoChkOk = 0;                                                             
     				    }
     				}                                                                                     
     				break;		                                                                          
     			                                                                          
     		   default:                                                                                   
     		        break;	                                                                              
     		}                                                                                             
     	}                                                                                                 
     	
     	if((uwfIsoChkFinished == 1) || (uwfIsoChkFinished == 0))                                                                                              
     	{                                                                                                 	
     	   uwIsoChkCounter = 0; 
     	   uwIsoChkStep = 0;    
     	   dwIsoRlyOpenVngSum = 0;
     	   dwIsoRlyCloseVngSum = 0; 
     	   wIsoRlyOpenVngAvgTemp = 0; 
     	   wIsoRlyCloseVngAvgTemp = 0;  
     	   //sIsoRelayOpen();    
     	   sPERelayOpen(); 
     	   uwIsoSumCnt = 0; 
     	     
                    
     	   if(uwfIsoChkFinished == 0) uwfIsoChkFinished = 2;                                                                        
     	}                                                                                              
    }   
    return(uwfIsoChkOk);                                                                            
}                                                                                                    
/*********************************************************************                                
Function name:  void sBatRevConnectChk(INT16S *pBatVolt,INT16U *pErrFlag,INT16U *pEventFlag)                                
Description:  check the PV connect                                                    
Calls:        	                                                                                      
Called By:  in PreChk Mode, and Maybe in "Protect.c" file, 20ms period time                           
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/ 
void sBatRevConnectChk(INT16S *pBatVolt,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwPVRevConnectCnt = 0;
static INT16U uwErrClearDelay2s = 0;

    if((fDCRelayPCtr == 0)&&(*pBatVolt < cBusN20V))
    {
    	uwErrClearDelay2s = 0;
    	if(++uwPVRevConnectCnt > 10)
		{
	       uwPVRevConnectCnt = 10; 	       
	       *pErrFlag = 1;
           
            if((*pEventFlag) == 0)
            {
                sErroringDeal();                                      
                *pEventFlag = 1;
            }
        }
    }
    else
    {
    	if(uwPVRevConnectCnt > 0)
		{
			 uwPVRevConnectCnt--;
		}
		else
		{
			if(++uwErrClearDelay2s > 500)
			{
			    uwErrClearDelay2s = 500;
			    *pErrFlag = 0;
			} 
		}
    }
}
/*********************************************************************                                
Function name:  void sGridVoltBalanceDegreeChk(INT16S *pBalanceDegree,INT16U *pErrFlag,INT16U *pEventFlag)                                 
Description:  check the PV connect                                                    
Calls:        	                                                                                      
Called By:  in main, and Maybe in "Protect.c" file, 500ms period time                           
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/ 
void sGridVoltBalanceDegreeChk(INT16S *pBalanceDegree,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwBalanceDegreeHighCnt = 0;
static INT16U uwBalanceDegreeDoubleHighCnt = 0;
static INT16U uwErrClearDelay1m = 0;
INT16U uwExitTimeSet;

    if(fFactoryFastStart == 1)
    {
        uwExitTimeSet = 5;
    }
    else
    {
        uwExitTimeSet = 300;
    }

    if(fCSVSSwitch == 0)
    {
        if((*pBalanceDegree) > ((wEepromGridVoltBalanceMax*3)>>1))
        {
            if(++uwBalanceDegreeDoubleHighCnt > 65)
            {
                uwBalanceDegreeDoubleHighCnt = 65;
                uwErrClearDelay1m = 0;
	            *pErrFlag = 1;
	            fGridPretect = 1;
                
                if((*pEventFlag) == 0)
                {
                    sErroringDeal();                                      
                    *pEventFlag = 1;
                }
            }
        }
        else if((*pBalanceDegree) > wEepromGridVoltBalanceMax) 
        {
            if(++uwBalanceDegreeHighCnt > 80)   //30s
            {                                  
                uwBalanceDegreeHighCnt = 80;     
                uwErrClearDelay1m = 0;          
                *pErrFlag = 1;
                fGridPretect = 1;                  
                                                
                if((*pEventFlag) == 0)         
                {                              
                    sErroringDeal();           
                    *pEventFlag = 1;          
                }                              
            }                                  
        }
        else
        {
            uwBalanceDegreeDoubleHighCnt = 0;
            uwBalanceDegreeHighCnt = 0;
            
	        if(++uwErrClearDelay1m > uwExitTimeSet) //1min
	        {
	            uwErrClearDelay1m = uwExitTimeSet;
                *pErrFlag = 0;
	        } 
        }
    }
    else
    {
        uwBalanceDegreeDoubleHighCnt = 0;
        uwBalanceDegreeHighCnt = 0;
        
	    if(++uwErrClearDelay1m > uwExitTimeSet) //1min
	    {
	        uwErrClearDelay1m = uwExitTimeSet;
            *pErrFlag = 0;
	    }
    }
} 
/*********************************************************************
Function name:  void sWarrningDeal()
Description:  判断是否有警告类型故障
Calls:        	 
Called By:  sErrFaultDeal(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sWarrningDeal() //5ms
{
static INT16U uwWarrningCodeHoldCnt = 0; //uwWarrningCode维持1s，保证上位机接受到
	if(fFanErrEvent||fBatteryCoeffErrEvent||fEepromErrEvent||fTempSensorErrEvent||fSpdErrEvent)
	{
		uwWarrningCodeHoldCnt = 0;
		
		uwWarrningCode = fFanErrEvent + 0*2 + 0*4 + fEepromErrEvent*8 + fSpdErrEvent*16 + fTempSensorErrEvent*32 \
		                  + fBatteryCoeffErrEvent*64 + fDCCurrBalanceErrEvent * 256;
		
		fFanErrEvent = fFanErr;          //若无故障，则清除故障事件  
		fBatteryCoeffErrEvent = fBatteryCoeffErr;
		fEepromErrEvent = fEepromErr;
        fTempSensorErrEvent = fTempSensorErr;		
		fSpdErrEvent = fSpdErr;		
		fDCCurrBalanceErrEvent = fDCCurrBalanceErr;
		
	    if(uwFaultGrade == 0)
	    {
	    	uwFaultGrade = 1;
	    }	
    }
    else
    {
    	if(uwFaultGrade == 1) uwFaultGrade = 0; //清除警告性故障等级
    		
    	if(uwWarrningCode != 0)
    	{    	    
    	    uwWarrningCodeHoldCnt++;
    	    if(uwWarrningCodeHoldCnt >= 200)
    	    {
    	    	uwWarrningCode = 0;
    	    	uwWarrningCodeHoldCnt = 200;
    	    }   		
    	}
    	else 
    	{ 
			uwWarrningCodeHoldCnt = 0;
        }
    }
}
/*********************************************************************
Function name:  void sFaultDeal()
Description:  判断是否有可恢复类型故障
Calls:        	 
Called By:  sErrFaultDeal(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sFaultDeal(void) //5ms
{
static INT16U uwFaultCodeHoldCnt = 0;
    fInvCurrOffsetErrEvent = (fRInvCurrOffsetErrEvent | fSInvCurrOffsetErrEvent | fTInvCurrOffsetErrEvent);
    fDCICurrOffsetErrEvent = (fRDCICurrOffsetErrEvent | fSDCICurrOffsetErrEvent | fTDCICurrOffsetErrEvent);    

	if(fInvCurrHighErrEvent||fGridRelayErrEvent||fIGBTHaltErrEvent||fDCIHighErrEvent||fInvCurrOffsetErrEvent||fDCOCPErrEvent\
	   ||fIsoLowErrEvent||fFreHighErrEvent||fFreLowErrEvent||fOTErrEvent||fBattRevConnectErrEvent||fGFCIHighErrEvent||fBatteryCurr3ErrEvent \
	   ||fOCPErrEvent||fBusHighErrEvent||fNoUtilityErrEvent||fDCICurrOffsetErrEvent||fLineGridRmsVolErrEvent \
	   ||fGridVoltBalanceDegreeErrEvent||fPhaseRmsCurErrEvent||fMcuActDspNoActErrEvent||fGridAndInvVolDiffErrEvent \
	   ||fBatteryCurr1ErrEvent||fOVPErrEvent||fPowerDisMatchEvent||fRunInvAhdErrEvent||fDCRelayErrEvent||fVCapDiffErrEvent \
	   ||fBatVolHighErrEvent||fCtrPowerErrEvent||fFuseErrEvent||fGFCISteadyErrEvent||fDampResistanceErrEvent||fBatteryCurr2ErrEvent \
	   ||fVCapHighErrEvent||fBatteryCurr4ErrEvent||fInvRelayErrEvent||fAuxiRelayErrEvent||fGFCICTErrEvent||fAIFreqErrEvent \
	   ||fDecCapEnergyErrEvent||fDCPWMErrEvent||fDCSoftRelayErrEvent||fCPLDOsciErrEvent||fDCSwitchErrEvent||fBusDiffBatVolErrEvent \
	   ||fCANNoAcitveErrEvent||fDCPWMPreChkErrEvent||fACPWMPreChkErrEvent)
	{
   	    uwFaultCodeHoldCnt = 0;
   	    if(fStartOrNormal == 1)
		{
			uwFaultCode[0] = fDCRelayErrEvent + fIGBTHaltErrEvent*2 + fDCOCPErrEvent*4 + fBatteryCurr3ErrEvent*8 \
			                 + fBatVolHighErrEvent*16 + fBusHighErrEvent*32 + fAIFreqErrEvent*64 + fLineGridRmsVolErrEvent*128 \
			                 + 0*256 + fInvCurrHighErrEvent*512 + fFreHighErrEvent*1024 + fFreLowErrEvent*2048 \
			                 + fNoUtilityErrEvent*4096 + fGridRelayErrEvent*8192 + fOTErrEvent*16384 + fInvCurrOffsetErrEvent*32768; 
			
			uwFaultCode[1] = fCtrPowerErrEvent + fDCICurrOffsetErrEvent*2 + fDCIHighErrEvent*4 + fIsoLowErrEvent*8 + fGFCIHighErrEvent*16   
	   	                     + fInvRelayErrEvent*32 + fBattRevConnectErrEvent*64 + fMcuActDspNoActErrEvent*128 + fOCPErrEvent*256
	   	                     + fGridVoltBalanceDegreeErrEvent*512 + fVCapDiffErrEvent*1024 + fPhaseRmsCurErrEvent*2048
	   	                     + fGFCICTErrEvent*4096 + fGridAndInvVolDiffErrEvent*8192+fOVPErrEvent*16384+fRunInvAhdErrEvent*32768;
			
			uwFaultCode[2] = fBatteryCurr1ErrEvent + fFuseErrEvent*2 + fPowerDisMatchEvent*4 + fAuxiRelayErrEvent*16 + fBatteryCurr2ErrEvent*64
			                 + fVCapHighErrEvent*128 + 0*256 + 0*512 + fBatteryCurr4ErrEvent*8192 + fGFCISteadyErrEvent*16384 + 0*32768;
			
			uwFaultCode[3] = fCANNoAcitveErrEvent*2 + fDCPWMErrEvent*4 + fDCSoftRelayErrEvent*8 + fCPLDOsciErrEvent*16 + fDCSwitchErrEvent*32 + fBusDiffBatVolErrEvent*64+fDCPWMPreChkErrEvent*128+fACPWMPreChkErrEvent*256
			                 + fDecCapEnergyErrEvent*16384;
			                 
		    uwFaultCode[4] = 0;
   	    }
		else
		{
        	uwFaultCode[0] = fInvCurrOffsetErrEvent*32768; 
   	        uwFaultCode[1] = fDCICurrOffsetErrEvent*2;
            uwFaultCode[2] = 0;
            uwFaultCode[3] = 0;
            uwFaultCode[4] = 0;
		}

        fBusHighErrEvent = fBusHighErr;
        fVCapHighErrEvent = fVCapHighErr;
        fBatteryCurr1ErrEvent = fBatteryCurr1Err;
        fLineGridRmsVolErrEvent = fLineGridRmsVolErr;
        //fInvCurrHighErrEvent = fInvCurrHighErr;        // PF
        fFreHighErrEvent = fFreHighErr;//5
        fAuxiRelayErrEvent = fAuxiRelayErr;
        fDecCapEnergyErrEvent = fDecCapEnergyErr;
        fDCPWMErrEvent = fDCPWMErr;
        fDCSoftRelayErrEvent = fDCSoftRelayErr;
        fFreLowErrEvent = fFreLowErr;      
        fNoUtilityErrEvent = fNoUtilityErr;
        fOTErrEvent = fOTErr;            
        fIsoLowErrEvent = fIsoLowErr;
        fGFCIHighErrEvent = fGFCIHighErr; //10
        fCANNoAcitveErrEvent = fCANNoAcitveErr;
        fBattRevConnectErrEvent = fBattRevConnectErr;
		fAIFreqErrEvent = fAIFreqErr;

		fGridVoltBalanceDegreeErrEvent = fGridVoltBalanceDegreeErr;
        fIGBTHaltErrEvent = fIGBTHaltErr;
        
        fGridAndInvVolDiffErrEvent = fGridAndInvVolDiffErr;
        fOVPErrEvent = fOVPErr;
		fOCPErrEvent = fOCPErr;
        fPowerDisMatchEvent = fPowerDisMatchErr;      
		fDCRelayErrEvent = fDCRelayErr;
		fCPLDOsciErrEvent = fCPLDOsciErr;
		fDCSwitchErrEvent = fDCSwitchErr;
		fBusDiffBatVolErrEvent = fBusDiffBatVolErr;
		fDCOCPErrEvent = fDCOCPErr;
		fGFCICTErrEvent = fGFCICTErr;
		
		fBatVolHighErrEvent = fBatVolHighErr;
        fCtrPowerErrEvent = fCtrPowerErr;		
		fVCapDiffErrEvent = fVCapDiffErr;
		fBatteryCurr4ErrEvent = fBatteryCurr4Err;

		fMcuActDspNoActErrEvent = fMcuActDspNoActErr;//26
		fFuseErrEvent = fFuseErr;
		fGFCISteadyErrEvent = fGFCISteadyErr;

		fBatteryCurr2ErrEvent = fBatteryCurr2Err;
		fBatteryCurr3ErrEvent = fBatteryCurr3Err;
		
		fRGridCurrOffsetErrEvent = fRGridCurrOffsetErr;
		fSGridCurrOffsetErrEvent = fSGridCurrOffsetErr;
		fTGridCurrOffsetErrEvent = fTGridCurrOffsetErr;

		fDCPWMPreChkErrEvent=fDCPWMPreChkErr;
		fACPWMPreChkErrEvent=fACPWMPreChkErr;
		
        if(uwFaultGrade < 2)
	    {
	    	uwFaultGrade = 2;
	    }	       
    }
    else
    {        	
    	if((uwFaultCode[0] != 0)||(uwFaultCode[1] != 0)||(uwFaultCode[2] != 0)||(uwFaultCode[3] != 0)||(uwFaultCode[4] != 0))
    	{    	        	    
    	    uwFaultCodeHoldCnt++;
    	    if(uwFaultCodeHoldCnt >= 200)
    	    {
    	    	uwFaultCode[0] = 0;
    	    	uwFaultCode[1] = 0;
    	    	uwFaultCode[2] = 0;
				uwFaultCode[3] = 0;
				uwFaultCode[4] = 0;
    	    	uwFaultCodeHoldCnt = 0;
    	    	if(uwFaultGrade == 2) uwFaultGrade = 0; //清除苫指葱怨收系燃?   	
    	    }   		
    	}
    	else 
    	{
            uwFaultCodeHoldCnt = 0;
    	}
    }
}
/*********************************************************************
Function name:  void sPFChk(INT16U *pErrEventFlag,INT16U *pErrEventBakFlag,INT16U *pErrFlag,INT16U *pFaultCnt,
                            INT16U *pHoldFaultCnt,INT32U *pNoFaultTimerCnt,INT16U *pFaultEventFlag)
Description:  判断对应的可恢复性故障是否转化为永久性故障
Calls:    
Called By:  sPFDeal(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sPFChk(INT16U *pErrEventFlag,INT16U *pErrEventBakFlag,INT16U *pErrFlag,INT16U *pFaultCnt,
            INT16U *pHoldFaultCnt,INT32U *pNoFaultTimerCnt,INT16U *pFaultEventFlag, INT16U uRestartCnt)
{
	if((*pErrEventFlag) == 1)
	{
		(*pHoldFaultCnt)++;
		
        *pNoFaultTimerCnt = 0;  //连续无故障计时
        if((*pErrEventBakFlag == 0) || (*pHoldFaultCnt > uRestartCnt))  //*pHoldFaultCnt要修改5分钟，考虑根据故障类型调整时间
        {      	
        	*pHoldFaultCnt = 0;
        	
        	if(*pFaultCnt > 2)   // 初始故障就记录一次，所以重启2次则需计数到3
        	{        		
        		*pFaultEventFlag = 1;
        	}
        	else
        	{
        		(*pFaultCnt)++;  //故障连续发生计数
        	}
        }       
    }
    else
    {
        *pHoldFaultCnt = 0; //故障持续时间
        
        (*pNoFaultTimerCnt)++;
        if(*pNoFaultTimerCnt > 720000) //1小时无连续故障发生,清一次计数
        {
            *pNoFaultTimerCnt = 0;
            if(*pFaultCnt > 0)
            {
                *pFaultCnt = 0;  //(*pFaultCnt)--;
            }
	    }
	    if(fQuitShutModeOrder == 1) 
	    {  	
	        *pFaultCnt = 0;
	    }
    }
      
    *pErrEventBakFlag = *pErrEventFlag;
    *pErrEventFlag = *pErrFlag;  //若无故障，则清除收鲜录?  
}

/*********************************************************************
Function name:  void sPFDeal()
Description:  判断是否有永久性类型故障
Calls:  void sPFChk(INT16U *pErrEventFlag, INT16U *pErrEventBakFlag, INT16U *pErrFlag, INT32U *pNoFaultTimerCnt,  INT16U *pEventFlag)	 
Called By:  sPFDeal(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sPFDeal(void) //5ms
{
	static INT16U uwPFCodeHoldCnt = 0;
	
	fInvCurrOffsetErr = (fRInvCurrOffsetErr | fSInvCurrOffsetErr | fTInvCurrOffsetErr);
    fDCICurrOffsetErr = (fRDCICurrOffsetErr | fSDCICurrOffsetErr | fTDCICurrOffsetErr);
	fGridCurrOffsetErr = (fRGridCurrOffsetErr | fSGridCurrOffsetErr | fTGridCurrOffsetErr);
	
    sPFChk(&fGridRelayErrEvent,&fGridRelayErrEventBak,&fGridRelayErr,&uwRelayFaultCnt,
           &uwRelayHoldFaultCnt,&udwRelayNFTimerCnt,&fGridRelayFaultEvent,60000); 
           
    sPFChk(&fInvRelayErrEvent,&fInvRelayErrEventBak,&fInvRelayErr,&uwInvRelayFaultCnt,
           &uwInvRelayHoldFaultCnt,&udwInvRelayNFTimerCnt,&fInvRelayFaultEvent,60000);
             
    sPFChk(&fDCIHighErrEvent,&fDCIErrHighEventBak,&fDCIHighErr,&uwDCIHighFaultCnt,
           &uwDCIHighHoldFaultCnt,&udwDCIHighNFTimerCnt,&fDCIHighFaultEvent,60000);
	sPFChk(&fOCPErrEvent,&fOCPEventBak,&fOCPErr,&uwOCPFaultCnt,
           &uwOCPHoldFaultCnt,&udwOCPNFTimerCnt,&fOCPFaultEvent,60000);
    sPFChk(&fInvCurrHighErrEvent,&fInvCurrHighErrEventBak,&fInvCurrHighErr,&uwInvCurrHighFaultCnt,
           &uwInvCurrHighHoldFaultCnt,&udwInvCurrHighNFTimerCnt,&fInvCurrHighFaultEvent,60000);
    sPFChk(&fPhaseRmsCurErrEvent,&fPhaseRmsCurErrEventBak,&fPhaseRmsCurErr,&uwPhaseRmsCurFaultCnt,
           &uwPhaseRmsCurHoldFaultCnt,&udwPhaseRmsCurNFTimerCnt,&fPhaseRmsCurFaultEvent,60000);         
    sPFChk(&fRunInvAhdErrEvent,&fRunInvAhdErrEventBak,&fRunInvAhdErr,&uwRunInvAhdFaultCnt,
           &uwRunInvAhdHoldFaultCnt,&udwRunInvAhdNFTimerCnt,&fRunInvAhdFaultEvent,60000);
    sPFChk(&fDampResistanceErrEvent,&fDampResistanceErrEventBak,&fDampResistanceErr,&uwDampResistanceFaultCnt,
           &uwDampResistanceHoldFaultCnt,&udwDampResistanceNFTimerCnt,&fDampResistanceFaultEvent,60000);
           
    //判断是否清除故障事件
    fRInvCurrOffsetErrEvent = fRInvCurrOffsetErr;  
    fSInvCurrOffsetErrEvent = fSInvCurrOffsetErr;
    fTInvCurrOffsetErrEvent = fTInvCurrOffsetErr;             
    fRDCICurrOffsetErrEvent = fRDCICurrOffsetErr; 
    fSDCICurrOffsetErrEvent = fSDCICurrOffsetErr; 
    fTDCICurrOffsetErrEvent = fTDCICurrOffsetErr; 
    //fRInvCurrAvgOffsetErrEvent = fRInvCurrAvgOffsetErr;  
    //fSInvCurrAvgOffsetErrEvent = fSInvCurrAvgOffsetErr;
    //fTInvCurrAvgOffsetErrEvent = fTInvCurrAvgOffsetErr;   
	
 
    if(fGridRelayFaultEvent || fDCIHighFaultEvent || fOCPFaultEvent	|| fInvCurrHighFaultEvent || fPhaseRmsCurFaultEvent 
    	||fDampResistanceFaultEvent|| fRunInvAhdFaultEvent||fInvRelayFaultEvent)
    {        
		if (fStartOrNormal == 1)
		{
			uwPFCode = fDampResistanceFaultEvent + fGridRelayFaultEvent*8 + fInvRelayFaultEvent*16 + 0*32 + fDCIHighFaultEvent*64 + 0*128 + fOCPFaultEvent*256 \
			           + fInvCurrHighFaultEvent*512 + fPhaseRmsCurFaultEvent*1024 + 0*2048 + fRunInvAhdFaultEvent*4096;
		 
		}
		else 
		{
			uwPFCode = fDCICurrOffsetFaultEvent*32;
		}
		
        fGridRelayFaultEvent = fGridRelayErrEvent;
		fInvRelayFaultEvent = fInvRelayErrEvent;
        fDCIHighFaultEvent = fDCIHighErrEvent;
	  	fOCPFaultEvent = fOCPErrEvent;
        fRunInvAhdFaultEvent = fRunInvAhdErrEvent;
        fInvCurrHighFaultEvent = fInvCurrHighErrEvent;
        fPhaseRmsCurFaultEvent = fPhaseRmsCurErrEvent;
        fDampResistanceFaultEvent = fDampResistanceErrEvent;

		if(uwPFCode != 0) 
		{
			uwFaultGrade = 3; 
	    } 

    }
	else
	{
    	if(uwPFCode != 0)
    	{    	        	    
    	    uwPFCodeHoldCnt++;
    	    if(uwPFCodeHoldCnt >= 200)
    	    {
    	    	uwPFCodeHoldCnt = 200;
    	    	if(fQuitShutModeOrder == 1)  //判断是否有清除永久性故障命令
    	    	{
    	    	    uwPFCode = 0;   	    	
    	    	    uwPFCodeHoldCnt = 0;    	    	
    	    	    //if(uwFaultGrade == 3)
    	    	    //{
    	    	    //    uwFaultGrade = 0; //清除可恢复性故障等级 
                    //}	
                }   		
            }
        }
    	else
    	{
    	    uwPFCodeHoldCnt = 0;
    	}
	}	
}
/*********************************************************************
Function name:  void suwGetFaultGrade(void)
Description:  
Calls:      	 
Called By:  sSysMode(void)
Parameters:  void
Return:  INT16U      
**********************************************************************/
INT16U suwGetFaultGrade(void)
{
	return(uwFaultGrade);
}
/*********************************************************************
Function name:  void sErrFaultDeal(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:   sWarrningDeal(),sFaultDeal(),sPFDeal()     	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sErrFaultDeal(void)
{
//===accord alarm level to decide how to deal with
	sWarrningDeal();  // fan and eeprom error
	sFaultDeal();    //
	sPFDeal();       //
    
    if(uwFaultGrade == 3) //永久性故障等级
    {
    	uwSysModePrepare = ShutMode;
    }
    else if(uwFaultGrade == 2) //常规故障等级
    {
    	uwSysModePrepare = FaultMode;
    }
    else //系统模式维持不变
    {
    	uwSysModePrepare = uwSysModePrepare;
    }
}
/*********************************************************************
Function name:  void sFanFdbChk(INT16U *pFanStatus,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sFanFdbChk(INT16U *pFanStatus,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFanFdbOutCnt = 0;
static INT16U uwFanStopStatusOutCnt = 0;
static INT16U uwErrClearDelay2s = 0;
static INT16U ErrOutStatus = 0;

static INT16U uwFanStartCheckCnt = 0;
static INT16U uwFanStartCheckEN = 0;
INT16U uwFaultTimeSet;

    if(fFactoryFastStart == 0)
    {
        uwFaultTimeSet = 250;
        if(wHeatFitTemp > cTemp70C)
        {
            uwFanStartCheckCnt++;
            if(uwFanStartCheckCnt >= 100)
            {
                uwFanStartCheckCnt = 100;
                uwFanStartCheckEN = 1;
            }
        }
        else
        {
            if(uwFanStartCheckCnt > 0)
            {
                uwFanStartCheckCnt--;
            }
            else
            {
                uwFanStartCheckCnt = 0;
                uwFanStartCheckEN = 0;
            }
        }
    }
    else
    {
        uwFanStartCheckEN = 1;
        uwFaultTimeSet = 60;        
    }

    if((*pFanStatus) == 1)
    {
        uwFanStopStatusOutCnt = 0;

        if((fFanState == 0)&&(uwFanStartCheckEN == 1))
        {
            if(++uwFanFdbOutCnt > uwFaultTimeSet)
            {
                uwFanFdbOutCnt = uwFaultTimeSet;
                uwErrClearDelay2s = 0;
                *pErrFlag = 1;
                *pEventFlag = 1;
                ErrOutStatus = 1;
                fFanFaultStatus = 1;
            }
        }
        else
        {
            uwFanFdbOutCnt = 0;

            if(++uwErrClearDelay2s > 100)
            {
                uwErrClearDelay2s = 100;
                if(ErrOutStatus==1)
                {
                    *pEventFlag = 0;
                    *pErrFlag = 0;
                    ErrOutStatus=0;
                }
            }
        }
    }
    else
    {
        uwFanFdbOutCnt = 0;

        if((fFanState == 1)&&(uwFanStartCheckEN == 1))
        {
            if(++uwFanStopStatusOutCnt > uwFaultTimeSet)
            {
                uwFanStopStatusOutCnt = uwFaultTimeSet;
                uwErrClearDelay2s = 0;
                *pErrFlag = 1;
                *pEventFlag = 1;
                ErrOutStatus=2;
                fFanFaultStatus=0;
            }
        }
        else
        {
            uwFanStopStatusOutCnt = 0;
            if(++uwErrClearDelay2s > 100)
            {
                uwErrClearDelay2s = 100;
                if(ErrOutStatus==2)
                {
                    *pEventFlag = 0;
                    *pErrFlag = 0;
                    ErrOutStatus=0;
                }
            }
        }
    }
}
/*********************************************************************
Function name:  void sBusOVPChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sBusOVPChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwOVPFaultCnt = 0, uwOVPNormalCnt = 0;
	if(fBUSOVPState == 0) 
	{
		if(++uwOVPFaultCnt > 2)  // 5ms*2=20ms
		{
			uwOVPFaultCnt = 2;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal(); 
                *pEventFlag = 1;
            }			
		}	
		uwOVPNormalCnt = 0;
	}
	else
	{
		uwOVPFaultCnt = 0;
			
		if(++uwOVPNormalCnt > 2000)
		{
		    uwOVPNormalCnt = 2000;
		    *pErrFlag = 0;
		}  
	} 
}
/*********************************************************************
Function name:  void sACOCPStateChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sACOCPStateChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwOCPFaultCnt = 0, uwOCPNormalCnt = 0;
	if(fACOCPState == 1) 
	{
		if(++uwOCPFaultCnt > 2)  // 5ms*4=20ms
		{
			uwOCPFaultCnt = 2;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal(); 
                *pEventFlag = 1;
            }			
		}	
		uwOCPNormalCnt = 0;
	}
	else
	{
		uwOCPFaultCnt = 0;
			
		if(++uwOCPNormalCnt > 2000)
		{
		    uwOCPNormalCnt = 2000;
		    *pErrFlag = 0;
		}  
	} 
}
/*********************************************************************
Function name:  void sDCOCPStateChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sDCOCPStateChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwOCPFaultCnt = 0, uwOCPNormalCnt = 0;
	if(fDCOCPState == 0) 
	{
		uwOCPNormalCnt = 0;
		
		if(++uwOCPFaultCnt > 2)  // 5ms*4=20ms
		{
			uwOCPFaultCnt = 2;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal(); 
                *pEventFlag = 1;
            }
            
            if(fDCRelayPCtr == 1)//DC Relay is Close
			{
				sDCRelayOpenAct();
			}			
		}			
	}
	else
	{
		uwOCPFaultCnt = 0;
			
		if(++uwOCPNormalCnt > 2000)
		{
		    uwOCPNormalCnt = 2000;
		    *pErrFlag = 0;
		}  
	} 
}
/*********************************************************************
Function name:  void sFuseStateChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sFuseStateChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFuseFaultCnt = 0, uwFuseNormalCnt = 0;
	if(fFuseState == 0) 
	{
		if(++uwFuseFaultCnt > 10)
		{
			uwFuseFaultCnt = 10;
			*pErrFlag = 1;
        
            if((*pEventFlag) == 0)
            {
                sErroringDeal(); 
                *pEventFlag = 1;
            }			
		}	
		uwFuseNormalCnt = 0;
	}
	else
	{
		uwFuseFaultCnt = 0;
			
		if(++uwFuseNormalCnt > 4)
		{
		    uwFuseNormalCnt = 4;
		    *pErrFlag = 0;
		}  
	} 
}
/*********************************************************************
Function name:  void sTZStateClear(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sTZStateClear(void)
{
static INT16U uwTZClcDelay = 0;

    if((EPwm1Regs.TZFLG.bit.OST == 1)||(EPwm2Regs.TZFLG.bit.OST == 1)||(EPwm3Regs.TZFLG.bit.OST == 1)||(fOVPErrEvent == 1))//||(fCPLDOsciErrEvent == 1))
	{
        if(++uwTZClcDelay > 400)
        {
            uwTZClcDelay = 400;
            sPWMTZClr();
        }   
    }
    else
    {
        uwTZClcDelay = 0;
    }  	
}
/*********************************************************************
Function name:  void sIGBTHaltStateChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sIGBTHaltStateChk(INT16U *pErrFlag,INT16U *pEventFlag)  //for ovp, ocp and emergency act
{
static INT16U uwIGBTHalt1FaultCnt = 0, uwIGBTHalt1NormalCnt = 0;
    if(fIGBTHalt == 0)    // Low level is fault
	{
		if (++uwIGBTHalt1FaultCnt > 2)
		{
			uwIGBTHalt1FaultCnt = 2;
			
			*pErrFlag = 1;
			if((*pEventFlag) == 0)
			{
				sErroringDeal(); 
				*pEventFlag = 1;
			}		   
		}
		uwIGBTHalt1NormalCnt = 0;
	}
	else                       
	{
		uwIGBTHalt1FaultCnt = 0;
		
		if (++uwIGBTHalt1NormalCnt > 2000)
		{
			uwIGBTHalt1NormalCnt = 2000;
			*pErrFlag = 0;
		}
	} 	
}
/*********************************************************************
Function name:  void sDCPWMChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sDCPWMChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFaultDelay = 0;
static INT16U uwOKDelay = 0;

    if(fDCPWMLost == 1)
	{
	    uwOKDelay = 0;
	    
	    if(++uwFaultDelay >= 1)
	    {
	    	uwFaultDelay = 1;
	    	*pErrFlag = 1;
		    if((*pEventFlag) == 0)
		    {
		    	sErroringDeal(); 
		    	*pEventFlag = 1;
		    }
	    }				   
	}
	else                       
	{
		uwFaultDelay = 0;
		
		if(++uwOKDelay > 2000)
		{
			uwOKDelay = 2000;
			*pErrFlag = 0;
		}
	} 	
}
/*********************************************************************
Function name:  void sDCPWMChkPreChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sDCPWMChkPreChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFaultDelay = 0;
static INT16U uwOKDelay = 0;
    if(uwSysMode==InvRunMode)
    {
		*pErrFlag=0;
		*pEventFlag=0;
		 uwFaultDelay=0;
		 uwOKDelay=0;
		 return;
	}
	
    if(fDCPWMLostPreChk == 1)
	{
	    uwOKDelay = 0;
	    
	    if(++uwFaultDelay >= 1)
	    {
	    	uwFaultDelay = 1;
	    	*pErrFlag = 1;
		    if((*pEventFlag) == 0)
		    {
		    	sErroringDeal(); 
		    	*pEventFlag = 1;
		    }
	    }				   
	}
	else                       
	{
		uwFaultDelay = 0;
		
		if(++uwOKDelay > 2000)
		{
			uwOKDelay = 2000;
			*pErrFlag = 0;
		}
	} 	
}
/*********************************************************************
Function name:  void sACPWMChkPreChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sACPWMChkPreChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFaultDelay = 0;
static INT16U uwOKDelay = 0;
    if(uwSysMode==InvRunMode)
    {
		*pErrFlag=0;
		*pEventFlag=0;
		 uwFaultDelay=0;
		 uwOKDelay=0;
		 return;
	}
	
    if(fACPWMLostPreChk == 1)
	{
	    uwOKDelay = 0;
	    
	    if(++uwFaultDelay >= 1)
	    {
	    	uwFaultDelay = 1;
	    	*pErrFlag = 1;
		    if((*pEventFlag) == 0)
		    {
		    	sErroringDeal(); 
		    	*pEventFlag = 1;
		    }
	    }				   
	}
	else                       
	{
		uwFaultDelay = 0;
		
		if(++uwOKDelay > 2000)
		{
			uwOKDelay = 2000;
			*pErrFlag = 0;
		}
	} 	
}

/*********************************************************************
Function name:  void sDCPWMChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sDCCurrBalanceChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFaultDelay = 0;
static INT16U uwOKDelay = 0;

    if(fBiDirDCEnOut == 1)
    {
    	if(abs(wBatteryAvgCurr1 - wBatteryAvgCurr2) >= cCurr8R75A)
    	{
    		uwOKDelay = 0;
    		
    		if(++uwFaultDelay >= 100)
    		{
    			uwFaultDelay = 100;
	    	    *pErrFlag = 1;
	    	    *pEventFlag = 1;
    		}
    	}
    	else
    	{
    		uwFaultDelay = 0;
    		
    		if(++uwOKDelay > 1000)
		    {
		    	uwOKDelay = 1000;
		    	*pErrFlag = 0;
		    }
    	}
    }
    else if(fBiDirDCEnOut == 2)
    {
    	if(abs(wBatteryAvgCurr3 - wBatteryAvgCurr4) >= cCurr8R75A)
    	{
    		uwOKDelay = 0;
    		
    		if(++uwFaultDelay >= 100)
    		{
    			uwFaultDelay = 100;
	    	    *pErrFlag = 1;
	    	    *pEventFlag = 1;
    		}
    	}
    	else
    	{
    		uwFaultDelay = 0;
    		
    		if(++uwOKDelay > 1000)
		    {
		    	uwOKDelay = 1000;
		    	*pErrFlag = 0;
		    }
    	}
    }
    else if(fBiDirDCEnOut == 3)
    {
    	if((abs(wBatteryAvgCurr1 - wBatteryAvgCurr2) >= cCurr8R75A)||(abs(wBatteryAvgCurr1 - wBatteryAvgCurr3) >= cCurr8R75A)||
    	   (abs(wBatteryAvgCurr1 - wBatteryAvgCurr4) >= cCurr8R75A)||(abs(wBatteryAvgCurr2 - wBatteryAvgCurr3) >= cCurr8R75A)||
    	   (abs(wBatteryAvgCurr2 - wBatteryAvgCurr4) >= cCurr8R75A)||(abs(wBatteryAvgCurr3 - wBatteryAvgCurr4) >= cCurr8R75A))
    	{
    		uwOKDelay = 0;
    		
    		if(++uwFaultDelay >= 100)
    		{
    			uwFaultDelay = 100;
	    	    *pErrFlag = 1;
	    	    *pEventFlag = 1;
    		}
    	}
    	else
    	{
    		uwFaultDelay = 0;
    		
    		if(++uwOKDelay > 1000)
		    {
		    	uwOKDelay = 1000;
		    	*pErrFlag = 0;
		    }
    	}
    }
    else
    {
    	uwFaultDelay = 0;
    	
    	if(++uwOKDelay > 1000)
	    {
	    	uwOKDelay = 1000;
	    	*pErrFlag = 0;
	    }
    }
}
/*********************************************************************
Function name:void sBatteryCoeffChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sBatteryCoeffChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFault1Delay = 0;
static INT16U uwFault2Delay = 0;
static INT16U uwOKDelay = 0;
INT16S wBatChargeVoltMax,wBatDisChargeVoltMin;

    wBatChargeVoltMax = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
    wBatDisChargeVoltMin = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
    
    if(wBatFloatChargeVolt > wBatChargeVoltLimt)
    {
        uwOKDelay = 0;
        uwFault2Delay = 0;
        
        if(++uwFault1Delay >= 3)
    	{
    		uwFault1Delay = 3;
	        *pErrFlag = 1;
	        *pEventFlag = 1;
    	}
    }
    else if(((wBatteryInAvgVol > (wBatChargeVoltMax + cVolt3V))||(wBatteryInAvgVol < (wBatDisChargeVoltMin - cVolt3V)))&&(uwSysMode >= PreCheckMode)&&(fDCRelayPCtr == 1))
    {
        uwOKDelay = 0;
        uwFault1Delay = 0;
        
        if(++uwFault2Delay >= 100)
    	{
    		uwFault2Delay = 100;
	        *pErrFlag = 1;
	        *pEventFlag = 1;
    	}
    }
    else
    {
        uwFault1Delay = 0;
        uwFault2Delay = 0;
        
        if(++uwOKDelay > 50)
		{
			uwOKDelay = 50;
			*pErrFlag = 0;
		}
    }
}
/*********************************************************************
Function name:void sDCSoftRelayChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sDCSoftRelayChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFaultDelay = 0;
static INT16U uwOKDelay = 0;

    if(fDCSoftRelayLost == 1)
	{
	    uwOKDelay = 0;
	    
	    if(++uwFaultDelay >= 1)
	    {
	    	uwFaultDelay = 1;
	    	*pErrFlag = 1;
		    if((*pEventFlag) == 0)
		    {
		    	sErroringDeal(); 
		    	*pEventFlag = 1;
		    }
	    }				   
	}
	else                       
	{
		uwFaultDelay = 0;
		
		if (++uwOKDelay > 2000)
		{
			uwOKDelay = 2000;
			*pErrFlag = 0;
		}
	} 	
}
/*********************************************************************
Function name:  void sPhaseRmsCurChk(INT16S *pRRmsCur,INT16S *pSRmsCur,INT16S *pTRmsCur,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sPhaseRmsCurChk(INT16S *pRRmsCur,INT16S *pSRmsCur,INT16S *pTRmsCur,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwPhaseRmsErrCnt = 0; 
static INT16U uwErrClearDelay60s = 0; 
INT16S wRmsCurAvg;
       
    wRmsCurAvg = (INT16S)(((INT32S)((INT32S)(*pRRmsCur) + (*pSRmsCur) + (*pTRmsCur))*cDivide3)>>14);
    
    if(fCSVSSwitch == 0)
    {
        if(((abs(*pRRmsCur - wRmsCurAvg)> cCurrRms3A)||(abs(*pSRmsCur - wRmsCurAvg)> cCurrRms3A)||(abs(*pTRmsCur - wRmsCurAvg)> cCurrRms3A))
          &&(fLVRTValue > 14745)&&(fHVRTValue < 18022))
        {
            if((abs(*pRRmsCur - wRmsCurAvg)> cCurrRms5A)||(abs(*pSRmsCur - wRmsCurAvg)> cCurrRms5A)||(abs(*pTRmsCur - wRmsCurAvg)> cCurrRms5A))
            {
                uwPhaseRmsErrCnt += 5;
            }
            else
            {
                uwPhaseRmsErrCnt++;
            }
        	if(uwPhaseRmsErrCnt >= 1000)  //1000*10ms=10s Ensure PowerAvg has been updated.
        	{
        	    uwPhaseRmsErrCnt = 1000;
        	    uwErrClearDelay60s = 0;
        	    *pErrFlag = 1;
                if((*pEventFlag) == 0)
                {
                    sErroringDeal();            
                    *pEventFlag = 1;
                }
            }
        }
        else 
        {
            if(uwPhaseRmsErrCnt > 0)
	    	{
	    	    uwPhaseRmsErrCnt = uwPhaseRmsErrCnt>>1;
	    	}
	    	else
	    	{
	    	    if(++uwErrClearDelay60s > 6000) //60s
	    	    {
	    	        uwErrClearDelay60s = 6000;
	    	        *pErrFlag = 0; 
	    	    }
	    	}
        }
    }
    else
    {
        uwPhaseRmsErrCnt = 0;
	    if(++uwErrClearDelay60s > 6000) //60s
	    {
	        uwErrClearDelay60s = 6000;
	        *pErrFlag = 0; 
	    }
    }
}
/*********************************************************************                                
Function name:  void sMcuFdbChk(INT16U *pErrFlag,INT16U *pEventFlag)                                       
Description:  check the feedback Signal of the Mcu                                                  
Calls:        	                                                                                      
Called By:  boost period interrupt,6kHz                           
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/ 
void sMcuFdbChk(INT16U *pErrFlag,INT16U *pEventFlag)//11.03.07修改
{     
static INT32U udwErrClearDelay1Min = 0;
	if((fMCUActState == 0) && ((fError == 0)||(fDeCapEnergy >= 1)))
	{
		udwErrClearDelay1Min = 0;     
		*pErrFlag = 1;
		
        if((*pEventFlag) == 0)
        {
            sErroringDeal();                                      
            *pEventFlag = 1;       
		}	
	}
	else
	{
		if(fMCUActState == 1)
		{
		    if(++udwErrClearDelay1Min > 160000)
		    {
		        udwErrClearDelay1Min = 160000;
		        *pErrFlag = 0;
		    }
	    }
	    else 
	    {
            udwErrClearDelay1Min = 0; 
        }
    }
}
/*********************************************************************                                
Function name:  void sDCRelayOpenAct(void);                                 
Description:                                                      
Calls:        	                                                                                      
Called By:                            
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/ 
void sDCRelayChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwCLoseFaultDelay = 0;
static INT16U uwCloseOKDelay = 0;
static INT16U uwOpenFaultDelay = 0;
static INT16U uwOpenOKDelay = 0;

    if(fDCRelayPCtr == 1)
    {
    	uwOpenFaultDelay = 0;
    	uwOpenOKDelay = 0;
    	   	
    	if(abs(wBatteryInAvgVol - wBatteryOutAvgVol) > cVolt50V)
    	{
    		uwCloseOKDelay = 0;
		
		    if(++uwCLoseFaultDelay >= 10)
		    {
		    	uwCLoseFaultDelay = 10;
		    	*pErrFlag = 1;
            
                if((*pEventFlag) == 0)
                {
                    sErroringDeal(); 
                    *pEventFlag = 1;
                }
                
                if(fDCRelayPCtr == 1)//DC Relay is Close
			    {
			    	sDCRelayOpenAct();
			    }
		    }
    	}
    	else
	    {
	    	uwCLoseFaultDelay = 0;
	    	if(++uwCloseOKDelay >= 1500)
	    	{
	    		uwCloseOKDelay = 1500;
	    	    *pErrFlag = 0;
	    	}
	    }
    }
    else
    {   
    	uwCLoseFaultDelay = 0;
    	uwCloseOKDelay = 0;
    	   		
    	if(((abs(wBatteryInAvgVol - wBatteryOutAvgVol) <= cVolt50V)&&(uwSysMode <= PowerOnMode)&&(wBatteryOutAvgVol > 3200))||(fDCRelayBurn == 1))
    	{
    		uwOpenOKDelay = 0;
    		
    		if(++uwOpenFaultDelay >= 500)
    		{
    			uwOpenFaultDelay = 500;
			    *pErrFlag = 1;
                
                if((*pEventFlag) == 0)
                {
                    sErroringDeal(); 
                    *pEventFlag = 1;
                }
    		}
    	}
    	else
    	{
    		uwOpenFaultDelay = 0;
    		if(++uwOpenOKDelay >= 1500)
	    	{
	    		uwOpenOKDelay = 1500;
	    	    *pErrFlag = 0;
	    	}
    	}
    }
}
/*********************************************************************                                
Function name:  void sDCRelayOpenAct(void);                                 
Description:                                                      
Calls:        	                                                                                      
Called By:                            
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/ 
void sDCRelayOpenAct(void)
{
//static INT16U uwDCRelayOpenCnt = 0;
     
    if(wBatteryAvgCurr > cCurrN1A)//-1A
    {
    	if(++uwDCRelayOpenCnt > 2)
    	{
            uwDCRelayOpenCnt = 0;
            sDCRelayPOpen();
        }
    }
}
/*********************************************************************                                
Function name: void sPowerDisMatchChk(INT32S *pPowerDCSide,INT32S *pPowerACSide,INT16U *pErrFlag,INT16U *pEventFlag);                                
Description:  check the PV current                                                    
Calls:        	                                                                                      
Called By:  AD.C                          
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/
void sPowerDisMatchChk(INT32S *pPowerDCSide,INT32S *pPowerACSide,INT16U *pErrFlag,INT16U *pEventFlag)				  
{
static INT16U uwPowerDisMatErrCnt = 0; 
static INT16U uwErrClearDelay60s = 0; 
INT32S dwDCPowerMax;
INT32S dwDCPowerMin;

    dwDCPowerMax = (*pPowerACSide) + 96000;//+-6000W
    dwDCPowerMin = (*pPowerACSide) - 96000;
    
    if((fInvEnOut == 1) || ((*pEventFlag)==1))
    { 
        if((((*pPowerDCSide) > dwDCPowerMax)||((*pPowerDCSide) < dwDCPowerMin))&&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))
        {                        
    	    if(++uwPowerDisMatErrCnt > 50)  
    	    {
    	        uwPowerDisMatErrCnt = 50;
    	        uwErrClearDelay60s = 0;
    	        *pErrFlag = 1;
                if((*pEventFlag) == 0)
                {
                    sErroringDeal();            
                    *pEventFlag = 1;
                }
            }
        }
        else 
        {
            if(uwPowerDisMatErrCnt > 0)
		    {
			    uwPowerDisMatErrCnt--;
		    }
		    else
		    {
			    if(++uwErrClearDelay60s > 3000) 
			    {
			        uwErrClearDelay60s = 3000;
			        *pErrFlag = 0; 
			    }
		    }
        }	
	}
}
/*********************************************************************                                
Function name:INT16U sfInvVolNormalChk(T_THREE_PHASE_ABC tInvPhaseAvgVol,T_THREE_PHASE_ABC tInvPhaseRmsVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  check the PV connect                                                    
Calls:        	                                                                                      
Called By:  in main, and Maybe in "Protect.c" file, 500ms period time                           
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/ 
INT16U sfInvVolNormalChk(T_THREE_PHASE_ABC tInvPhaseAvgVol,T_THREE_PHASE_ABC tInvPhaseRmsVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
INT16S sDeltaRSVolRms = 0,sDeltaSTVolRms = 0,sDeltaTRVolRms = 0;
INT16S wDeltaRSVolAvg = 0,wDeltaSTVolAvg = 0,wDeltaTRVolAvg = 0; 
static INT16U uwInvVolErrCnt = 0; 
static INT16U uwErrClearDelay2s = 0;

    sDeltaRSVolRms = 0;//abs(tInvPhaseRmsVol.a - abs(tInvPhaseAvgVol.a) - tInvPhaseRmsVol.b + abs(tInvPhaseAvgVol.b));//abs(tInvPhaseRmsVol.a - tInvPhaseRmsVol.b);
    sDeltaSTVolRms = 0;//abs(tInvPhaseRmsVol.b - abs(tInvPhaseAvgVol.b) - tInvPhaseRmsVol.c + abs(tInvPhaseAvgVol.c));
    sDeltaTRVolRms = 0;//abs(tInvPhaseRmsVol.c - abs(tInvPhaseAvgVol.c) - tInvPhaseRmsVol.a + abs(tInvPhaseAvgVol.a));
    
    wDeltaRSVolAvg = abs(tInvPhaseAvgVol.a - tInvPhaseAvgVol.b);
    wDeltaSTVolAvg = abs(tInvPhaseAvgVol.b - tInvPhaseAvgVol.c);
    wDeltaTRVolAvg = abs(tInvPhaseAvgVol.c - tInvPhaseAvgVol.a); 
    
    if(fInvEnOut == 1)
    {
        if((sDeltaRSVolRms > 480)||(sDeltaSTVolRms > 480)||(sDeltaTRVolRms > 480)||
           (wDeltaRSVolAvg > 320)||(wDeltaSTVolAvg > 320)||(wDeltaTRVolAvg > 320))
        {
   	        if(++uwInvVolErrCnt > 10)    //20*5ms=100ms
		    {
	            uwInvVolErrCnt = 10; 
	            uwErrClearDelay2s = 0;
	            *pErrFlag = 1;
            
                if((*pEventFlag) == 0)
                {
                    sErroringDeal();                                      
                    *pEventFlag = 1;
                }
                uwInvVolNormalChkOk = 0;
                uwModuSoftIncCnt = 0;
            }	
        }
        else
        {
            if(uwInvVolErrCnt > 0)
		    {
		        uwInvVolErrCnt--;
		    }
		    else
		    {
		        if(++uwErrClearDelay2s > 400)
		        {
		            uwErrClearDelay2s = 400;
		            *pErrFlag = 0; 
		        } 
		        uwInvVolNormalChkOk = 1;
		    }
	    }
    }
    else
    {
        uwInvVolErrCnt = 0;
    }
	return(uwInvVolNormalChkOk);	
}
/*********************************************************************                                
Function name:void sSpdChk(INT16U *pErrFlag,INT16U *pEventFlag)                                                                                   
Calls:        	                                                                                      
Called By:   main(void)                           
Parameters:  void                                                                                     
Return:  void                                                                           
 *********************************************************************/ 
void sSpdChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrCnt = 0;
static INT16U uwErrClearDelay2s = 0;
	if(fSpdState == 0)
	{  
		if(++uwErrCnt > 10)
		{
	       uwErrCnt = 10;
	       *pErrFlag = 1;                        
           *pEventFlag = 1;
        }
        uwErrClearDelay2s = 0;
	}	
    else
    {        
	  	uwErrCnt = 0;

	  	if(++uwErrClearDelay2s > 4)
	  	{
	  	    uwErrClearDelay2s = 4;
	  	    *pErrFlag = 0;
	  	}  
    }	
}
/*********************************************************************                                
Function name:void sCtrPowerChk(INT16U *pErrFlag,INT16U *pEventFlag)                                                                                   
Calls:        	                                                                                      
Called By:   main(void)                           
Parameters:  void                                                                                     
Return:  void                                                                           
 *********************************************************************/ 
void sCtrPowerChk(INT16U *pErrFlag,INT16U *pEventFlag) 
{
static INT16U uwErrCnt = 0;
static INT16U uwErrClearDelay10s = 0;
	if(fCtrPowerState == 0) 
	{  
		uwErrClearDelay10s = 0;
		if(++uwErrCnt > 2)
		{
	        uwErrCnt = 2;
	        *pErrFlag = 1;                        
            if((*pEventFlag) == 0)                                                
    		{                                                                     
    		    sErroringDeal();                                                  
    		    *pEventFlag = 1;                                                 
    		}
        }
	}	
    else
    {
        uwErrCnt = 0;
        
	    if(++uwErrClearDelay10s > 2000)
	    {
	        uwErrClearDelay10s = 2000;
	        *pErrFlag = 0;
	    }   
    }	
}
/*********************************************************************
Function name:void sGridVolAndInvVolDiffChk(T_THREE_PHASE_ABC tGridPhaseVol,T_THREE_PHASE_ABC tInvPhaseVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sGridVolAndInvVolDiffChk(T_THREE_PHASE_ABC tGridLineVol,T_THREE_PHASE_ABC tInvPhaseVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrClearDelay2s = 0;
static INT16U uwVolDiffCnt = 0;
static INT16U uwLVRTDelay = 0;
static INT16U uwStartDelay = 0;
INT16U fErrEnable = 1;
INT16S wUab,wUbc,wUca;

   
    if((fLVRTValue <= uwLVRTActiveValue)||(fHVRTValue >= uwHVRTActiveValue))
    {
        fErrEnable = 0;
        uwLVRTDelay = 0;
    }
    else
    {
        if(++uwLVRTDelay >= 8000)  
        {
            uwLVRTDelay = 8000;  
            fErrEnable = 1;
        }
        else
        {
            fErrEnable = 0; 
        }
    }
              
    if((fInvEnOut == 1)&&(fRunInvAhdChkRequire == 1))
    {
    	++uwStartDelay;
    	if(uwStartDelay >= 240)
    	{
    		uwStartDelay = 240;
    	}
		wUab=tInvPhaseVol.a-tInvPhaseVol.b;
		wUbc=tInvPhaseVol.b-tInvPhaseVol.c;
		wUca=tInvPhaseVol.c-tInvPhaseVol.a;
	    if(((abs(tGridLineVol.a - wUab) > cVolt100V) || (abs(tGridLineVol.b - wUbc) > cVolt100V)
	    || (abs(tGridLineVol.c - wUca) > cVolt100V))&&(fErrEnable == 1)&&(uwStartDelay >= 240))
	    {
	        if(++uwVolDiffCnt > 16) //2ms
	        {
	            uwVolDiffCnt = 16;
	            uwErrClearDelay2s = 0;       
	            *pErrFlag = 1;
                if((*pEventFlag) == 0)
                {
                    sErroringDeal();            
                    *pEventFlag = 1;                                           
                }              
            }
	    }
	    else
	    {
            if(uwVolDiffCnt > 0)                                                      
        	{                                                                         
        		uwVolDiffCnt = uwVolDiffCnt>>1;                                                       
        	}                                                                         
        	else 
            {		
	    	    if(++uwErrClearDelay2s > 64000)
	    	    {    
	    	        uwErrClearDelay2s = 64000;
	    	        *pErrFlag = 0;
	    	    }
	    	}
    
	    }
	}
	else
	{
		uwStartDelay = 0;
		if(uwVolDiffCnt > 0)                                                      
        {                                                                         
        	uwVolDiffCnt = uwVolDiffCnt>>1;                                                       
        }                                                                         
        else 
        {		
	        if(++uwErrClearDelay2s > 64000)
	        {    
	           uwErrClearDelay2s = 64000;
	           *pErrFlag = 0;
	        }           
        }
	}
}
/*********************************************************************                                
Function name:  void sCANActiveChk(INT16U *pErrFlag,INT16U *pEventFlag)                                       
Description:  check the feedback Signal of the Mcu                                                  
Calls:        	                                                                                      
Called By:  boost period interrupt,6kHz                           
Parameters:  void                                                                                     
Return:  INT16U type flag                                                                             
 *********************************************************************/ 
void sCANActiveChk(INT16U *pErrFlag,INT16U *pEventFlag)
{     
static INT16U uwErrCnt = 0;
static INT16U uwResetDelay = 0;
	if(fCANActive == 0)
	{
		if(++uwErrCnt >= 100)
		{  
		    uwErrCnt = 100;
		    *pErrFlag = 1;
		    
		    if((*pEventFlag) == 0)
			{					
				sErroringDeal(); 
				*pEventFlag = 1;
				uwResetDelay = 1000; 
			}
		}	
	}
	else
	{
		fCANActive = 0;
		uwErrCnt = 0;
		//uwResetDelay = 0;
		*pErrFlag = 0;	
    }
    
    if((*pEventFlag) == 1)
    {
    	if(++uwResetDelay > 1000) 
    	{
    		sResetECana();
    		sCANConfigID();
    		uwResetDelay = 0; 
    	}
    }     
}
/*********************************************************************
Function name:  INT16U sGFCICTChk(INT16S *pGFCIRmsCurr,INT16S *pGFCIAvgCurr,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  
Parameters:  void
Return:  void      
********************************************************************/ 
INT16U sfGFCICTChk(INT16S wGFCICTCurr,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwGFCICTErrNum = 0;
static INT16U uwGFCICTChkCounter = 0;
static INT16S wGFCICTInitValue = 0;
static INT32S dwGFCICTInitValueSum = 0;
static INT16U uwGFCICTErrTestNum = 0;  //重复检查三次，减少误判
  
    if (uwfGFCICTChkFinished == 2)  //=2表示检测进行中，=0表示未检测，=1表示已检测
    {    
        switch(uwGFCICTChkStep)
        {
          	case 0:        		
			dwGFCICTInitValueSum = dwGFCICTInitValueSum + wGFCICTCurr;
			if(++uwGFCICTChkCounter > 100)
			{
           		wGFCICTInitValue = dwGFCICTInitValueSum/uwGFCICTChkCounter;
           		uwGFCICTChkStep++;
				uwGFCICTChkCounter = 0;  
				dwGFCICTInitValueSum = 0;					
			}
           	uwfGFCICTChkOk = 0;            
           	break;

           	case 1:
			sGFCIRelayClose(); 
			if(++uwGFCICTChkCounter > 100)
			{
			    uwGFCICTChkStep++;
			    uwGFCICTChkCounter = 0;
			}	
			uwfGFCICTChkOk = 0;
			break;

			case 2:
			if(++uwGFCICTChkCounter < 200)
            {
			    if((abs(wGFCICTCurr - wGFCICTInitValue) < cGFCI35mA) || (abs(wGFCICTCurr - wGFCICTInitValue) > cGFCI65mA))
				{
				    uwGFCICTErrNum++;				    
				}
			}
			else
			{				    
                if(uwGFCICTErrNum > 20)
				{
					uwGFCICTErrTestNum++;
				    if(uwGFCICTErrTestNum > 3)
				    {
				        uwGFCICTErrTestNum = 0;
				        
				        *pErrFlag = 1;
                        if((*pEventFlag) == 0)
                        {
                            sErroringDeal();            
                            *pEventFlag = 1;
                        } 
                        uwfGFCICTChkFinished = 1;
                    }
                    else
                    {
                        uwGFCICTChkStep = 4;
                    }
				}
				else
				{
					uwGFCICTChkStep = 3;	
					uwGFCICTErrTestNum = 0;				
				}
                uwGFCICTChkCounter = 0;
                uwGFCICTErrNum = 0;
                sGFCIRelayOpen();
			}
            
			uwfGFCICTChkOk = 0;
            break;
            
            case 3:					
     	    if(++uwGFCICTChkCounter >= 20)  //5*20ms ,wait for inverter relay close
     	    {
     	    	uwGFCICTChkCounter = 0;
     	    	uwfGFCICTChkFinished = 1; 
                uwfGFCICTChkOk = 1; 
     	    }                    
            break;	
            
            case 4:					
     	    if(++uwGFCICTChkCounter >= 20)  //5*20ms,test faile,need repeat again
     	    {
     	    	uwGFCICTChkCounter = 0;
     	    	uwfGFCICTChkFinished = 2; 
                uwfGFCICTChkOk = 0; 
                uwGFCICTErrNum = 0;
                dwGFCICTInitValueSum = 0;
                uwGFCICTChkStep = 0;
     	    }                    
            break;	
                               
     	    default: 
     	    uwfGFCICTChkFinished = 1;
    		uwfGFCICTChkOk = 0;
			uwGFCICTChkCounter = 0;
    		break;
		}		    
    }
        
	if((uwfGFCICTChkFinished == 1) || (uwfGFCICTChkFinished == 0)) 
    {                             
    	uwGFCICTChkStep = 0;      
     	uwGFCICTChkCounter = 0;       
     	uwGFCICTErrNum = 0;  
     	dwGFCICTInitValueSum = 0;
     	uwGFCICTErrTestNum = 0;
     	if(uwfGFCICTChkFinished == 0) uwfGFCICTChkFinished = 2;   
    }  
           
    return(uwfGFCICTChkOk);   	       	            
}
/*********************************************************************
Function name:void sAuxiRelayChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls: 20ms  	 
Called By:  
Parameters:  void
Return:  void      
********************************************************************/ 
void sAuxiRelayChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrDelay = 0;
static INT16U uwOKDelay = 0;
	if(fAuxiRelayLost == 1)
	{
		uwOKDelay = 0;
		
		if(++uwErrDelay >= 2)
		{
			uwErrDelay = 2;
			*pErrFlag = 1;
			if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;
            } 
		}
	}
	else
	{
		uwErrDelay = 0;
		if(++uwOKDelay > 3000)
		{
			uwOKDelay = 3000;
			*pErrFlag = 0;			
		}
	}
}
/*********************************************************************
Function name:void sDCSwitchChk(INT16S wBatInVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls: 100ms  	 
Called By:  
Parameters:  void
Return:  void      
********************************************************************/ 
void sDCSwitchChk(INT16S wBatInVol,INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwFaultDelay = 0;
static INT16U uwOKDelay = 0;
INT16S wBatPortVolReal;

    wBatPortVolReal = ((INT32S)wBatteryPortVolt * 26214)>>14; 

    if((abs(wBatPortVolReal - wBatInVol) > cVolt30V)&&(fBMSActive == 1)&&(fDCRelayPCtr == 1)&&(uwSysMode == InvRunMode))
    {
        uwOKDelay = 0;
        if((++uwFaultDelay) >= 5)
        {
            uwFaultDelay = 5;                                                     
    		*pErrFlag = 1;                                                        
    		                                                                      
    		if((*pEventFlag) == 0)                                                
    		{                                                                     
    		    sErroringDeal();                                                  
    		    *pEventFlag = 1;                                                 
    		}
        }
    }
    else
    {
        uwFaultDelay = 0;

    	if(++uwOKDelay > 300)
    	{
    	    uwOKDelay = 300;
    	    *pErrFlag = 0;         
    	}
    }
}
/*********************************************************************
Function name:void sDecCapEnergyChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls: 10ms  	 
Called By:  
Parameters:  void
Return:  void      
********************************************************************/ 
void sDecCapEnergyChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwErrDelay = 0;
static INT16U uwOKDelay = 0;
	if((fPowerDanger == 1)&&(fDeCapEnergy == 1))
	{
		uwOKDelay = 0;
		
		if(++uwErrDelay >= 2)
		{
			uwErrDelay = 2;
			*pErrFlag = 1;
			if((*pEventFlag) == 0)
            {
                sErroringDeal();            
                *pEventFlag = 1;
            } 
		}
	}
	else if(fPowerDanger == 1)
	{
		fPowerDanger = 0;
	}
	else
	{
		uwErrDelay = 0;
		if(++uwOKDelay > 1000)
		{
			uwOKDelay = 1000;
			*pErrFlag = 0;			
		}
	}
}
/*********************************************************************
Function name:sSumCheck
Description:  
Calls:
Called By:
Parameters: 
Return: 
*********************************************************************/
INT16U sSumCheck(INT16U *Ptr,INT16U length)
{
INT16U uwRegSum = 0;

	while(length--)
	{
		uwRegSum += *Ptr;
		*Ptr++;			
	}
	return uwRegSum;
}
/*********************************************************************
Function name:sRamChk
Description:  
Calls: ADISR
Called By:
Parameters: 
Return: 
*********************************************************************/
void sRamChk(void)
{
INT16U i=0;
INT16S * pPtr; 	

    pPtr = (INT16S *)((INT32U)0xf000);
    for(i=0;i<0x1000;i++)
    {
        *(pPtr + i) = 0;
    }
    
    for(i=0;i<0x1000;i++)
    {
        if(*(pPtr + i) != 0) sChipReset();
        	
        *(pPtr + i) = 0x5555;
        if(*(pPtr + i) != 0x5555) sChipReset();
        	
        *(pPtr + i) = 0x3333;
        if(*(pPtr + i) != 0x3333) sChipReset();
        	
        *(pPtr + i) = 0x0f0f;
        if(*(pPtr + i) != 0x0f0f) sChipReset();
        	
        *(pPtr + i) = 0x00ff;
        if(*(pPtr + i) != 0x00ff) sChipReset();
        	
        *(pPtr + i) = 0xffff;
        if(*(pPtr + i) != 0xffff) sChipReset();
    }
    
    for(i=0;i<0x1000;i++)
    {
        if(*(pPtr + 0xfff - i) != 0xffff) sChipReset();
        	
        *(pPtr + 0xfff - i) = 0x00ff;
        if(*(pPtr + 0xfff - i) != 0x00ff) sChipReset();
        	
        *(pPtr + 0xfff - i) = 0x0f0f;
        if(*(pPtr + 0xfff - i) != 0x0f0f) sChipReset();
        	
        *(pPtr + 0xfff - i) = 0x3333;
        if(*(pPtr + 0xfff - i) != 0x3333) sChipReset();
        	
        *(pPtr + 0xfff - i) = 0x5555;
        if(*(pPtr + 0xfff - i) != 0x5555) sChipReset();
        	
        *(pPtr + 0xfff - i) = 0;
        if(*(pPtr + 0xfff - i) != 0) sChipReset();
    }
}
/*********************************************************************
Function name:sFlashChk
Description:  
Calls:
Called By:
Parameters: 
Return: 
*********************************************************************/
void sFlashChk(void)
{
static INT16U uwChkSum = 0;
static INT32U dwFlashAddr = 0x338001;
static INT16U uwLen = 0x80;
    
    uwChkSum = uwChkSum + sSumCheck((INT16U *)(dwFlashAddr),uwLen);
    
    dwFlashAddr = dwFlashAddr + uwLen;
    
    if(dwFlashAddr >= (0x338001 + 0x3afd))
    {
        uwLen = 0x80;
        dwFlashAddr = 0x338001;
		
		wDebugFlashSum = uwChkSum;
		
        if(uwChkSum != wFlashSafetyCheckCRC) 
        {
          // sChipReset();
        }
		uwChkSum = 0;
    }
    if((dwFlashAddr + uwLen) > (0x338001 + 0x3ffd)) 
    {
        uwLen = ((INT32U)0x338001 + 0x3ffd) - dwFlashAddr;
    }       
}
/*********************************************************************
Function name:  void sCPLDOscilChk(INT16U *pErrFlag,INT16U *pEventFlag)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sCPLDOscilChk(INT16U *pErrFlag,INT16U *pEventFlag)
{
static INT16U uwOscilFaultCnt = 0, uwOscilNormalCnt = 0;

    if((EPwm1Regs.TZFLG.bit.OST == 1)||(EPwm2Regs.TZFLG.bit.OST == 1)||(EPwm3Regs.TZFLG.bit.OST == 1))
    {
        if((fMCUActState == 1)&&(fACOCPState == 0)&&(fCtrPowerState == 1)&&(fIGBTHalt == 1)&&(fDCOCPState == 1)&&(fBUSOVPState == 1))
        {
            uwOscilNormalCnt = 0;
		
		    if(++uwOscilFaultCnt > 2)
		    {
		    	uwOscilFaultCnt = 2;
		    	*pErrFlag = 1;
            
                if((*pEventFlag) == 0)
                {
                    sErroringDeal(); 
                    *pEventFlag = 1;
                }
                
                if(fDCRelayPCtr == 1)//DC Relay is Close
		    	{
		    		sDCRelayOpenAct();
		    	}			
		    }
        }
        else
        {
            uwOscilFaultCnt = 0;
			
		    if(++uwOscilNormalCnt > 2000)
		    {
		        uwOscilNormalCnt = 2000;
		        *pErrFlag = 0;
		    }
        }
    }
    else
    {
        uwOscilFaultCnt = 0;
			
		if(++uwOscilNormalCnt > 2000)
		{
		    uwOscilNormalCnt = 2000;
		    *pErrFlag = 0;
		} 
    }
}
/*********************************************************************
Function name:  void sADSamplChk(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/

void sADSamplChk(void)
{
	INT16U UabRms=0,UbcRms=0,UcaRms=0;

	//if(fScibTxEn==1||uwSysMode!=InvRunMode)return;
	if(fScibTxEn==1)return;
	UabRms=tScibRxData[2].word;
	UbcRms=tScibRxData[3].word;
	UcaRms=tScibRxData[4].word;

    UabTemp=(INT16U)(((INT32U)UabRms*28377)>>10);
	UbcTemp=(INT16U)(((INT32U)UbcRms*28377)>>10);
	UcaTemp=(INT16U)(((INT32U)UcaRms*28377)>>10);
	
	if((abs(wRSGridRmsVol-UabTemp)>cVolt100V)||(abs(wSTGridRmsVol-UbcTemp)>cVolt100V)
		||(abs(wTRGridRmsVol-UcaTemp)>cVolt100V))
		{
			uwADSampleErrCnt++;
				if(uwADSampleErrCnt > 400)
				{
					uwADSampleErrCnt = 400;
					sChipReset();
				}
		}
		else uwADSampleErrCnt=0;
		
}

/*** end of file *****************************************************/



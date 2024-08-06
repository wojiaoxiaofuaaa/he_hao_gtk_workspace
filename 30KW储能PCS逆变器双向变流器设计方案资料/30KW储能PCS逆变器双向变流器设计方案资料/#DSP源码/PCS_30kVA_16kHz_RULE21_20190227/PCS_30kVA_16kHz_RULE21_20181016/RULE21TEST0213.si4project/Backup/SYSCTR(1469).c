/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  SYSTR.c 
Description:  œµÕ≥øÿ÷∆π¶ƒ‹∫Ø ˝ 
Version :  V1.0
Author:  Zhouxu	 
Date:2011-12-02
*********************************************************************/
#include "..\FunSrc\Funheader.h"

#define cFreq60Hz                     15360   //Q8
#define cFreq50Hz                     12800

#define c30Secs                       3000
#define c10Secs                       1000
#define c1Secs                        100

#define cDectoHex                     16777  //16384/1000 Q10

#define cDisQPFCtr                    0x00
#define cQSetCtr                      0x01
#define cPFSetCtr                     0x02
#define cPFPCurveCtr                  0x03
#define cQUCurveCtr                   0x04

#define cVolt5V                       80   //Q4
#define cVolt10V                      160
#define cVolt15V                      240
#define cVolt18V                      288
#define cVolt20V                      320
#define cVolt30V                      480
#define cVolt40V                      640
#define cVolt50V                      800
#define cVolt60V                      960
#define cVolt70V                      1120
#define cVolt100V                     1600
#define cVolt200V                     3200
#define cVolt300V                     4800
#define cVolt660V                     10560
#define cVolt800V                     12800
#define cVolt810V                     12960
#define cVolt910V                     14560

#define cCurr25A                      6400 //Q8
#define cCurr18A                      4608

#define cBusVoltFiltGainDen           3392
#define cBusVoltFiltGainNum           704
#define cInvPwmKLimtLow               956   //14189/950V/Q4*Q10
#define cInvPwmKLimtHigh              1297  //14189/700V/Q4*Q10
#define cSqrt3PwmK                    14189

//macro definition for temperature calculation
#define cEnvironmentTempN20AD         186  //2.5*4020./(51+4020+69529.6)./3*4096=287   Q0
#define cEnvironmentTemp0AD           434  //2.5*4020./(51+4020+27549.5)./3*4096=1144  Q0
#define cEnvironmentTemp20AD          848  //2.5*4020./(51+4020+12105.3)./3*4096=1733 Q0
#define cEnvironmentTemp40AD          1388 //2.5*4020./(51+4020+5816.1)./3*4096=2137  Q0
#define cEnvironmentTemp60AD          1936 //2.5*4020./(51+4020+3014.8)./3*4096=2469  Q0

#define cEnvironmentKLessN20          687 //0.191   Q12
#define cEnvironmentKLess0            329 //0.04589 Q12
#define cEnvironmentKLess20           197 //0.03394 Q12
#define cEnvironmentKLess40           151 //0.03704 Q12         
#define cEnvironmentKLess60           149 //0.04513 Q12
#define cEnvironmentKLess85           186 //0.07171 Q12         
                         
#define cEnvironmenty0LessN20         -800  //-50     Q4
#define cEnvironmenty0Less0           -543  //-33.95  Q4
#define cEnvironmenty0Less20          -323  //-20.19  Q4
#define cEnvironmenty0Less40          -177  //11.07   Q4
#define cEnvironmenty0Less60          -169  //-10.56  Q4
#define cEnvironmenty0Less85          -459  //-28.67  Q4

//macro definition for temperature calculation
#define cHeatTemp_40AD                16
#define cHeatTemp_30AD                28
#define cHeatTemp_20AD                47
#define cHeatTemp_10AD                76
#define cHeatTemp0AD                  119
#define cHeatTemp10AD                 179
#define cHeatTemp25AD                 305
#define cHeatTemp40AD                 502
#define cHeatTemp70AD                 1060
#define cHeatTemp125AD                2252
//macro definition for Full load limit value based on the temperature 
#define cTemp20C                      320
#define cTemp30C                      480
#define cTemp40C                      640 
#define cTemp45C                      720
#define cTemp55C                      880
#define cTemp58C                      928
#define cTemp73C                      1168
#define cTemp75C                      1200 
#define cTemp78C                      1248
#define cTemp85C                      1360
#define cTemp93C                      1488

#define cPF0R80                       800  
#define cPF1R00                       1000   
#define cPFThousandSquare             268435456 

#define cPower15kW                    240000//15kW,Q4  
#define cPower10kW                    160000//10kW,Q4 

#define cI2TValueMax                  301842450 //0.5s¿€ª˝“ª¥Œ 1.1±∂π˝‘ÿŒ¨≥÷60∑÷÷”

#define cAdjustPFLimitHigh            1280
#define cAdjustPFLimitLow            -1280

#define cEnvDecPowercoef              328      //0.02
#define cHeatDecPowercoef             983      //0.06
#define cLPFilterNum                  8834
#define cLPFilterDen                  23934   

#define cPowerEff                     4915   //1.20 Q12
#define cPowerEffI                    5120   //1/0.8 Q12

#define cFanDutyKt                    3072//Q12
#define cFanDutyTt                    7864//Q20
#define cFanDutyBt                    7578//Q14
#define cFanDutyKp                    2458//Q12
#define cFanDutyTp                    6291//Q20
#define cFanDutyBp                    4751//Q14
    
T_SYSMODE uwSysMode,uwSysModePrepare;
INT16U fRunOrder;
INT16U fCallDefaultValueOrder;
INT16U fQuitShutModeOrder;
INT16U fError;

INT16U uwRFre,uwSFre,uwTFre;
INT16U uwRFreq,uwSFreq,uwTFreq,uwGridFreq;
INT16S wIqRefBakToLVRT;
INT16S wpuRmsCurrBakToLVRT;

INT16U fPhaseSequ;
INT16U fPhaseSequBak;
INT16U fIsoStatue;
INT16U fRelayChkRequire;
INT16U fInvSoftStaRequire;
INT16U fBatteryInitiStateOK;
INT16U fMpptEnable;
INT32U udwPowerOnWaitTime;

INT16U fFanStatus;
INT16S wGridVolBalanceDegree;
INT16S wGridVolAvgBalanceDegree;
INT16U fRunInvAhdChkRequire,fInvLittleVolOk,fPvPowEnough,fPLLStaOk,fRunInvAhdChkRestart;   
INT16U fDeCapEnergy = 0;
INT16U uwDECapEnergyStep = 0;
INT16U fClearBusCtr = 0;
INT16U uwLVRTActiveValue = 14745;
INT16U uwHVRTActiveValue = 18022;
INT32S dwI2TValue = 0;
INT16U fCANRunOrder = 1;
INT16S wReactiveCurrErr;
INT16S wReactiveCurrRefPreSat;
INT16S wAdjustPF;
INT16S wPFSetHex;
INT16U uwCurrMax = 16384;//6940;
INT16S wGridFreMaxArray[4] = {cFreq60Hz,cFreq60Hz,cFreq60Hz,cFreq60Hz};
INT16S wGridFreMinArray[4] = {cFreq60Hz,cFreq60Hz,cFreq60Hz,cFreq60Hz};
INT32S dwGridFreMaxSum = 61440;
INT32S dwGridFreMinSum = 61440;
INT32S dwFreFiltMaxPrev = 3932160;
INT32S dwFreFiltMinPrev = 3932160;
INT16S wFreMaxBak = cFreq60Hz;
INT16S wFreMinBak = cFreq60Hz;
INT16U fAuxiRelayLost = 0;
INT16U fPowerDanger = 0;
INT16S wReactivePowerPer = 0;
INT16S wPFSet = 16384;
INT16S fReactiveMode = 0;
INT16S fActiveMode = 0;
INT16S wActivePowerPer = 16384;
INT16U fDerating = 0;
INT32S dwReactivePowerAim = 0;
INT16S wPowerDeratingIdmax = 13062;
INT16S wAdjustStep = 4;
INT16S wDebugIqStep;
INT16U fSysFreChange = 0;
INT16U uwpuApparentPowerMax;
INT16U uwpuReactivePowerMax;
INT16U fGridPretect = 0;
INT16U fQCtrEnDelay2s = 0;

INT32S dwIqRef = 0;
INT16U wDebugRunInvChkStep = 0;
INT16S wDebugTest1 = 0;
INT16S wDebugTest2 = 0;
INT16S wDebugTest3 = 0;
INT16S wDebugTest4 = 0;
INT16S wDebugTest5 = 0;
INT16S wDebugTest6 = 0;

INT32S dwRatedPower = 30000;  //30kW
INT16S wRatedInvCurrPeak = 13064;   //Q8 480 30kW
INT16S wRatedInvCurrInv = 10274;   //16384*32/wRatedInvCurrPeak
INT16S wInvCurrPeakMax = 13064;   //Q8 480 30kW
INT16S wInvCurrResolving = 512;
INT16S wRatedGridLineVol = 7680;
INT16S wpuGridLineVol = 16384;
INT16S wRatedRmsCurr = 9238;    //30KW/480/1.732 Q8  
INT32S dwSoftStopIdLimMax;
INT16U fFVRTMaxCurrDelay2s = 4;
INT16S wStdBusVolRef = 11520;
INT16U fDC1And2En = 0;
INT16U fDC3And4En = 0;
INT16U fStartByDCOrAC = 0;//0:By DC   1:By AC
INT16U fGridRelayCloseDeal = 0;
INT16S wIdRefBak = 0;
INT16S wIqRefBak = 0;
INT16U fCharge = 0;
INT16U fChargeToDisCharge = 0;
INT16U fDisChargeToCharge = 0;
INT16U fDCChargeToDisCharge = 0;
INT16U fDCDisChargeToCharge = 0;

INT16U uwDCSoftStep = 0;
INT16U fDCCtrBusSoftSta = 0;
INT16U fBattDarkStart = 0;
INT32S dwIdLimitHigh;
INT32S dwIdLimitLow;
INT16U uwPFSetMax;
INT16S wDCCurrSet;
INT32S dwDCCurrSetIng;
INT16S wDCCurrAim;
INT16S wDCCurrAimBak;
INT16S wInvMaxRmsCurr;
INT32S dwActivePowerAim;
INT16U fCSVSSwitch;
INT16U fAdjustQEn;
INT16U fHotBackup;
INT16U fLocalPCSComdOnOff;
INT16U fLocalPCSConnect;
INT16U fGridRelayCtr;
INT16U fDCRelayPCtr;
INT16U fQFMRelayCtr;
INT16U fDCSoftRelayCtr;
INT16U fAuxiRelayCtr;
INT16U fGridRelayCtrBak;
INT16U fDCRelayCtrBak;
INT16U fAuxiRelayCtrBak;
INT16U fInvEnOutBak;
INT16U fFloatCharge = 0;//0,≥ı ºªØ  1,∫„≥‰   2£¨∏°≥‰
INT16U uwAuxiRelayDelay = 0;
INT16U uwDCSoftRelayDelay = 0;
INT16U uwFVRTCurrSoftDelay = 300;
INT16S wBatChargeVoltLimt = 0;
INT16S wBatDisChargeVoltLimt = 0;
INT16S wBatChargeCurrLimt = 0;
INT16S wBatDisChargeCurrLimt = 0;
INT16S wBatFloatChargeVolt = 0;
INT16S wBatSToFChargeCurr = 0;
INT16U fACPowerCtr = 1;
INT16U fDCOCPEnable = 1;
INT16U fBMSActive = 0;
INT16U fVSRunByGrid = 0;
INT16U fFreOvStartOrEnd = 0;
INT16U fFreUnStartOrEnd = 0;
INT16U fVoltOvStartOrEnd = 0;
INT16U fVoltUnStartOrEnd = 0;
INT16U fVSFreqCheckEn = 0;
INT16U fVSVoltCheckEn = 0;
INT16S wVSVoltSet = 6271;
INT16S wVSFreqSet = 6032;
INT16S wVSFreqSetRev = 11126;
INT32S dwVSPSet = 0;
INT32S dwVSQSet = 0;

INT32S dwregister0 = 0;
INT32S dwregister1 = 0;
INT32S dwregister2 = 0;
INT16S wregister0 = 0;
INT16S wregister1 = 0;
INT16S wRegistercheckFlag = 0;
INT16U fATSStateBak = 1;
INT16U fQFMRelayAct = 0;
INT16U fATSForceCS2VS = 320;
INT16U fATSForceStop = 0;
INT16U fPStepSwitch = 0;
INT16U fQStepSwitch = 0;
INT16U fPQPriority = 1;//0,P priority  1, Q priority
INT16U  DisChargeMaxPower=16384;
INT16U  ChargeMaxPower=16384;


fPCS_State fPCSState;
//-------------------Data Safety------------------
#pragma DATA_SECTION(fError,"RAMSafetySYSCTRProt1");
#pragma DATA_SECTION(fGridRelayCtr,"RAMSafetySYSCTRProt2");
#pragma DATA_SECTION(fDCRelayPCtr,"RAMSafetySYSCTRProt3");
#pragma DATA_SECTION(fDCSoftRelayCtr,"RAMSafetySYSCTRProt4");
#pragma DATA_SECTION(fAuxiRelayCtr,"RAMSafetySYSCTRProt5");
#pragma DATA_SECTION(fGridRelayCtrBak,"RAMSafetySYSCTRProt6");
#pragma DATA_SECTION(fDCRelayCtrBak,"RAMSafetySYSCTRProt7");
#pragma DATA_SECTION(fAuxiRelayCtrBak,"RAMSafetySYSCTRProt8");
#pragma DATA_SECTION(uwSysMode,"RAMSafetySYSCTRProt9");


#pragma DATA_SECTION(fAuxiRelayLost,"RAMSafetySYSCTRProt10");
#pragma DATA_SECTION(wStdBusVolRef,"RAMSafetySYSCTRProt11");
#pragma DATA_SECTION(fDC1And2En,"RAMSafetySYSCTRProt12");
#pragma DATA_SECTION(fDC3And4En,"RAMSafetySYSCTRProt13");
#pragma DATA_SECTION(fInvEnOutBak,"RAMSafetySYSCTRProt14");
#pragma DATA_SECTION(fACPowerCtr,"RAMSafetySYSCTRProt15");
#pragma DATA_SECTION(fDCOCPEnable,"RAMSafetySYSCTRProt16");

#pragma DATA_SECTION(dwregister0,"RAMSafetySYSCTRProt17");
#pragma DATA_SECTION(dwregister1,"RAMSafetySYSCTRProt18");
#pragma DATA_SECTION(dwregister2,"RAMSafetySYSCTRProt19");

#pragma DATA_SECTION(wregister0,"RAMSafetySYSCTRProt20");
#pragma DATA_SECTION(wregister1,"RAMSafetySYSCTRProt21");
#pragma DATA_SECTION(wRegistercheckFlag,"RAMSafetySYSCTRProt22");
#pragma DATA_SECTION(fMpptEnable,"RAMSafetySYSCTRProt23");






//-------------------Data ------------------

#pragma DATA_SECTION(fQFMRelayCtr,"RamSafetyProt");

/*********************************************************************
Function name:  void sRestartVarInit(void)
Description:  ÷ÿ–¬∆Ù∂Ø ±œ‡πÿ±‰¡ø≥ı ºªØ
Calls:        	 
Called By:  sSysMode(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sRestartVarInit(void)
{
    sDisInvCpldOut();
    sDisInvPWMOut(); 
    sDisBiDirDC1And2CpldOut();
    sDisBiDirDC3And4CpldOut();
    sDisBiDirDC1PWMOut();
    sDisBiDirDC2PWMOut();
    sDisBiDirDC3PWMOut();
    sDisBiDirDC4PWMOut();
    
	fInvEnOut = 0;
    fBiDirDCEnOut = 0;
    fDCModeSwitchEn = 0;
	fDCModeSwitch = 1;
	sBiDirDCCtrParamInit();
 
    DelayUs(50);
    //============add 100KW code =====2010-01-21
    fBatteryInitiStateOK = 0;
    //===============================
    fFreDetect = 0;
    fPhaseSequ = 0;
    fIsoStatue = 0;
    fRelayChkRequire = 0;
    uwfIsoChkFinished = 0;
	fRelayCheckStep = 0; // reset initial flag
	uwInvRelayOKCnt = 0;
    uwInvRelayBadCnt = 0;
    uwGridRelayOKCnt = 0;
    uwGridRelayBadCnt = 0;
    uwAuxiRelayOKCnt = 0;
    uwAuxiRelayBadCnt = 0;
    uwfIsoChkOk = 0;
    fInvSoftStaRequire = 0;
    uwfPhaseSequencChkOk = 0;
    fMpptEnable = 0;
    sInvParamInit();
    //sMPPTParamInit();
    fRunInvAhdChkRequire = 0;// 0;
	fRunInvAhdChkRestart = 0;
	
	fGFCICTStatue = 0;
    uwfGFCICTChkFinished = 0; 
    uwfGFCICTChkOk = 0; 
    uwGFCICTChkStep = 0; 
    
    fInvLittleVolOk = 0;
    fPvPowEnough = 0;
	fPLLStaOk = 0;
    uwModuSoftIncCnt = 0;
    uwInvVolNormalChkOk = 0;
    uwGridAndInvVolChkOk = 0;
	fDCIDelayClear = 0;	
	uwDECapEnergyStep = 0;
	fClearBusCtr = 0;
	dwI2TValue = 0;
	wReactiveCurrErr =0;
	wReactiveCurrRefPreSat = 0;
    fLVRTStartOrEnd = 0;
	fLVRTStartOrEndBak = 0;
	wAdjustPF = 0;
	//fMpptUpdate = 0;
	uwCurrMax = 16384;//6940;
    wReactivePowerPer = 0;
    wPFSet = 16384;
    fReactiveMode = 0;
	fActiveMode = 0;
    wActivePowerPer = 16384;
    fDerating = 0;
    dwReactivePowerAim = 0;
    wPowerDeratingIdmax = 13062;
    wAdjustStep = 4;
    uwpuApparentPowerMax = 16384;
    uwpuReactivePowerMax = 9830;
    wInvDsCurrLimitHigh = 8709;
	wInvDsCurrLimitLow = -13064;
	wInvDsCurrLimitHighBak = 8709;
	wInvDsCurrLimitLowBak = -13064;
	dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
    dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
    fQCtrEnDelay2s = 0;
    fECANRunOrder = 0;
    
    fStartByDCOrAC = 0;
    fGridRelayCloseDeal = 0;
    wIdRefBak = 0;
    wIqRefBak = 0;
    uwDCSoftStep = 0;
    fDCCtrBusSoftSta = 0;
    fChargeToDisCharge = 0;
    fDisChargeToCharge = 0;
	fDCChargeToDisCharge = 0;
    fDCDisChargeToCharge = 0;
    
    wDCCurrSet = 15360;
    wDCCurrAim = 15360;
    wDCCurrAimBak = 15360;
	dwDCCurrSetIng = 3932160;
    uwIsoChkStep = 0;
    dwActivePowerAim = 0;
    //fCSVSSwitch = 0;
    fAdjustQEn = 0;
    fHotBackup = 0;
    fLocalPCSComdOnOff = 0;
    fLocalPCSConnect = 0;
    dwDeltaDCCurr = 0;
    fFloatCharge = 0;
    fVSRunByGrid = 0;
    uwFVRTCurrSoftDelay = 300;
    udwPowerOnWaitTime = 200;
}
/*********************************************************************
Function name:  void InitSysCtrVar(void)
Description:  œµÕ≥øÿ÷∆±‰¡ø≥ı ºªØ
Calls:        	 
Called By:  sSysMode(void)
Parameters:  void
Return:  void   
 *********************************************************************/
void InitSysCtrVar(void)
{
    fError = 0;
    sDisBiDirDC1And2CpldOut();
    sDisBiDirDC3And4CpldOut();
    sDisBiDirDC1PWMOut();
    sDisBiDirDC2PWMOut();
    sDisBiDirDC3PWMOut();
    sDisBiDirDC4PWMOut();               
    sDisInvCpldOut();            
    sDisInvPWMOut();
    sGridRelayOpen();
 	sInvRelayOpen();
 	sAuxiRelayOpen();
    sIsoRelayOpen();    
    sPERelayOpen();
    sDCRelayPOpen();
	//sDCRelayNOpen();
	sDCSoftRelayOpen();
    fInvEnOut = 0;
    fBiDirDCEnOut = 0;
    fDCModeSwitchEn = 0;
    uwSysMode = PowerOnMode; 
	uwSysModePrepare = PowerOnMode;
    fBatteryInitiStateOK = 0;
    fRunOrder = 0;
    fCallDefaultValueOrder = 0;
    fQuitShutModeOrder = 0;
    fPhaseSequ = 0;
    fPhaseSequBak = 1;
    fRelayChkRequire = 0;
    fMpptEnable = 0;
    fIsoStatue = 0;
    fInvSoftStaRequire = 0;
	  
    fRunInvAhdChkRequire = 0;
	fInvLittleVolOk = 0;
	fPLLStaOk = 0;
	fPvPowEnough = 0;
	fRunInvAhdChkRestart = 0;
	  
    uwRFre = 0;
    uwSFre = 0;
    uwTFre = 0;
    uwRFreq = 0;
    uwSFreq = 0;
    uwTFreq = 0;	
    uwGridFreq = 0;	
    wIqRefBakToLVRT = 0;
	wpuRmsCurrBakToLVRT = 0;
    udwPowerOnWaitTime = 200;

	wGridVolBalanceDegree = 0;
    wGridVolAvgBalanceDegree = 0;
	
	//uwMpptVolMin = cMpptVolMinLow;

	fFanStatus = 0;
    fDeCapEnergy = 0; 
    uwDECapEnergyStep = 0;
    fClearBusCtr = 0;
    uwLVRTActiveValue = 14745;
	uwHVRTActiveValue = 18022;
    dwI2TValue = 0;
    //fMpptUpdate = 0;
    uwCurrMax = 16384;//6940;
    
    wGridFreMaxArray[0] = cFreq60Hz;
	wGridFreMaxArray[1] = cFreq60Hz;
	wGridFreMaxArray[2] = cFreq60Hz;
	wGridFreMaxArray[3] = cFreq60Hz;
	
	wGridFreMinArray[0] = cFreq60Hz;
	wGridFreMinArray[1] = cFreq60Hz;
	wGridFreMinArray[2] = cFreq60Hz;
	wGridFreMinArray[3] = cFreq60Hz;
	dwGridFreMaxSum = 61440;
	dwGridFreMinSum = 61440;
    dwFreFiltMaxPrev = 3932160;
    dwFreFiltMinPrev = 3932160;
    wFreMaxBak = cFreq60Hz;
    wFreMinBak = cFreq60Hz;
    fAuxiRelayLost = 0;
    fPowerDanger = 0;
    wReactivePowerPer = 0;
    wPFSet = 16384;
    fReactiveMode = 0;
	fActiveMode = 0;
    wActivePowerPer = 16384;
    fDerating = 0;
    dwReactivePowerAim = 0;
    wPowerDeratingIdmax = 13062;
    fSysFreChange = 0;
    uwpuApparentPowerMax = 16384;
    uwpuReactivePowerMax = 9830;
    fGridPretect = 0;
    fRatedCoeff.all = 0x5000;
    fQCtrEnDelay2s = 0;    
	
	fChargeToDisCharge = 0;
    fDisChargeToCharge = 0;
	fDCChargeToDisCharge = 0;
    fDCDisChargeToCharge = 0;
    
    dwIqRef = 0;
	fFVRTMaxCurrDelay2s = 4;
	wStdBusVolRef = 11520;
	fDC1And2En = 0;
	fDC3And4En = 0;
	fStartByDCOrAC = 0;
	fGridRelayCloseDeal = 0;
	wIdRefBak = 0;
    wIqRefBak = 0;
    fCharge = 0;
    uwDCSoftStep = 0;
    fDCCtrBusSoftSta = 0;
    fBattDarkStart = 0;
    dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
    dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
    uwPFSetMax = 13107;
    wDCCurrSet = 15360;
    wDCCurrAim = 15360;
    wDCCurrAimBak = 15360;
	dwDCCurrSetIng = 3932160;
    wInvMaxRmsCurr = 0;
    dwActivePowerAim = 0;
    fCSVSSwitch = 0;
    fAdjustQEn = 0;
    fHotBackup = 0;
    fLocalPCSComdOnOff = 0;
    fLocalPCSConnect = 0;
    fFloatCharge = 0;
    fGridRelayCtr = 0;
    fDCRelayPCtr = 0;
    fQFMRelayCtr = 0;
    fDCSoftRelayCtr = 0;
    fAuxiRelayCtr = 0;
    fGridRelayCtrBak = 0;
    fDCRelayCtrBak = 0;
    fAuxiRelayCtrBak = 0;
	fInvEnOutBak = 0;
    uwAuxiRelayDelay = 0;
    uwDCSoftRelayDelay = 0;
    uwFVRTCurrSoftDelay = 300;
    wBatChargeVoltLimt = 0;
    wBatDisChargeVoltLimt = 0;
    wBatChargeCurrLimt = 0;
    wBatDisChargeCurrLimt = 0;
    wBatFloatChargeVolt = 0;
    wBatSToFChargeCurr = 0;
    fACPowerCtr = 1;
    fVSRunByGrid = 0;
    fDCOCPEnable = 1; 
    fFreOvStartOrEnd = 0;
    fFreUnStartOrEnd = 0;
    fVoltOvStartOrEnd = 0;
    fVoltUnStartOrEnd = 0;
    fVSFreqCheckEn = 0;
    fVSVoltCheckEn = 0;
    wVSVoltSet = 6271;
    wVSFreqSet = 6032;
    wVSFreqSetRev = 11126;
    dwVSPSet = 0;
    dwVSQSet = 0;
    fATSStateBak = 1;
    fQFMRelayAct = 0;
    fATSForceCS2VS = 320;
	fATSForceStop = 0;
    fPStepSwitch = 0;
    fQStepSwitch = 0;
}
/************************************************************************
Function name:	swModuleEnvTempFit(INT16S wAvgTemp)
Description:  
Calls:		  
Called By:		 
Parameters:  
Return:  INT16S 	 
************************************************************************/
INT16S swModuleEnvTempFit(INT16S wAvgTemp)
{	 
INT16S wEnvTemp=0;

    if(wAvgTemp > cEnvironmentTemp20AD)
    {
        if(wAvgTemp < cEnvironmentTemp40AD)
        {
            wEnvTemp = (INT16S)((((INT32S)(wAvgTemp)*cEnvironmentKLess40)>>8)+cEnvironmenty0Less40);           
        }
        else if(wAvgTemp < cEnvironmentTemp60AD)
        {
            wEnvTemp = (INT16S)((((INT32S)(wAvgTemp)*cEnvironmentKLess60)>>8)+cEnvironmenty0Less60);                       
        }
        else
        {
            wEnvTemp = (INT16S)((((INT32S)(wAvgTemp)*cEnvironmentKLess85)>>8)+cEnvironmenty0Less85);                        
        }
    }
    else 
    {
        if(wAvgTemp > cEnvironmentTemp0AD)
        {
            wEnvTemp = (INT16S)((((INT32S)(wAvgTemp)*cEnvironmentKLess20)>>8)+cEnvironmenty0Less20);                      
        }
        else if(wAvgTemp > cEnvironmentTempN20AD)
        {
            wEnvTemp = (INT16S)((((INT32S)(wAvgTemp)*cEnvironmentKLess0)>>8)+cEnvironmenty0Less0);                      
        }
        else
        {
            wEnvTemp = (INT16S)((((INT32S)(wAvgTemp)*cEnvironmentKLessN20)>>8)+cEnvironmenty0LessN20);   
                                
        }                    
    }
    return(wEnvTemp); 	
}
/************************************************************************
Function name:	swModuleHeatTempFit(INT16S wAvgTemp)
Description:  
Calls:		  
Called By:		 
Parameters:  
Return:  INT16S 	 
************************************************************************/
INT16S swModuleHeatTempFit(INT16S wAvgTemp)
{	 
INT16S wHeatTemp=0;
	
    if(wAvgTemp > cHeatTemp125AD)   //Limit High
    {
    	wAvgTemp = cHeatTemp125AD;		
    }
    else if(wAvgTemp < cHeatTemp_40AD)
    {
        wAvgTemp = cHeatTemp_40AD;
    }
    
    if (wAvgTemp > cHeatTemp70AD)       //70~125
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*3054)>>12)+322);   //wHeatTemp is Q4
    }		
    else if (wAvgTemp > cHeatTemp40AD)  //40~69
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*3627)>>12)+222);   
    }
    else if(wAvgTemp >= cHeatTemp25AD)  //25~39
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*5168)>>12)+14);   	 
    }
    else if(wAvgTemp >= cHeatTemp10AD)   //10~24
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*7564)>>12)-163);   		 
    }
    else if(wAvgTemp >= cHeatTemp0AD)   //0~9
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*11164)>>12)-321);   		 
    }
    else if(wAvgTemp >= cHeatTemp_10AD)//-10~-1
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*15765)>>12)-452);   		 
    }
    else if(wAvgTemp >= cHeatTemp_20AD)//-20~-11
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*23009)>>12)-584);   		 
    }
    else if(wAvgTemp >= cHeatTemp_30AD)//-30~-21
    {
    	wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*35100)>>12)-721);   		 
    }
    else
    {
        wHeatTemp = (INT16S)((((INT32S)(wAvgTemp)*28033)>>11)-863);
    }
    
    return(wHeatTemp);	
}
/*********************************************************************
Function name:INT16U sFanOpenLoopCtrl(INT16S wEnvTemp, INT16S wHeatTemp,INT16U fTmpflag,INT32S dwPPower,INT32S dwQPower,INT32U udwAppPower)
Description:   Fan control
Calls:		
Called By: main(), 500ms = 1 period
Parameters: void
Return: 	INT16U		
 *********************************************************************/
INT16U sFanOpenLoopCtrl(INT16S wEnvTemp, INT16S wHeatTemp,INT16U fTmpflag,INT32S dwPPower,INT32S dwQPower,INT32U udwAppPower)
{
INT16S wFanReguSpeed = 0;
INT16S wFanReguSpeedAim = 0;
static INT16U fFanCtrState = 0;
static INT16S wFanReguSpeedCtr = 9830;
INT32S dwTemp;
INT16U uwpuPower,uwpuPowerTmp;

    uwpuPower = (udwAppPower<<8)/dwRatedPower;//Q12
    
    if((fBiDirDCEnOut == 1)||(fBiDirDCEnOut == 2))
    {
    	dwTemp = labs(dwPPower);
    	uwpuPowerTmp = (dwTemp<<9)/dwRatedPower;//Q12 ,Only One DC Runing So mul 2
    }
    else
    {
    	dwTemp = labs(dwPPower);
    	uwpuPowerTmp = ((INT32U)dwTemp<<8)/dwRatedPower;//Q12
    }
    
    if(uwpuPower < uwpuPowerTmp)
    {
    	uwpuPower = uwpuPowerTmp;
    }
    dwTemp = ((INT64S)dwQPower * 6144)>>12;
    dwTemp = labs(dwTemp);
    uwpuPowerTmp = ((INT32U)dwTemp<<8)/dwRatedPower;//Q12
    
    if(uwpuPower < uwpuPowerTmp)
    {
    	uwpuPower = uwpuPowerTmp;
    }
    
    if (checkbit(fTmpflag,1) == 0)//EnvTmp is OK
    {
    	if(wEnvTemp > cTemp45C)
        {
            sInFanOn();
        }
        else if(wEnvTemp < cTemp40C)
        {
            sInFanOff();
        }    	
    }
    else
    {
    	if(uwpuPower > 1843)
        {
            sInFanOn();
        }
        else if(uwpuPower < 1638)
        {
            sInFanOff();
        }
    }
    
    if(checkbit(fTmpflag,0) == 0)//HeatTmp is OK
    {
    	if(uwSysMode == InvRunMode)
        {
            if(wHeatTemp < cTemp30C)
            {
                wHeatTemp = cTemp30C;
            }
            else if(wHeatTemp > cTemp93C)
            {
            	wHeatTemp = cTemp93C;
            }
            
            wFanReguSpeedAim = (((INT32S)uwpuPower * cFanDutyKt)>>10) + (((INT32S)wHeatTemp*cFanDutyTt)>>10) - cFanDutyBt ;
        }
        else
        {
            wFanReguSpeedAim = 0;
            fFanCtrState = 0;
        }                
    }
    else if(checkbit(fTmpflag,1) == 0)//EnvTmp is OK
    {
    	if(uwSysMode == InvRunMode)
        {
            if(wEnvTemp < cTemp20C) wEnvTemp = cTemp20C;
            wFanReguSpeedAim = (((INT32S)uwpuPower * cFanDutyKp)>>10) + (((INT32S)wEnvFitTemp*cFanDutyTp)>>10) - cFanDutyBp;
        }
        else
        {
            wFanReguSpeedAim = 0;
            fFanCtrState = 0;
        }
    }
    else
    {
    	if(uwSysMode == InvRunMode)
    	{
    	    wFanReguSpeedAim = (((INT32S)uwpuPower * cFanDutyKt)>>10) + (((INT32S)cTemp85C*cFanDutyTt)>>10) - cFanDutyBt;
    	}
    	else
        {
            wFanReguSpeedAim = 0;
            fFanCtrState = 0;
        }
    }
    
    if(wFanReguSpeedAim > 3277)//20%
    {
    	fFanCtrState = 1;
    	if(wFanReguSpeedAim <= 4096)
    	{
    		wFanReguSpeedAim = 4096;
    	}
    	else if(wFanReguSpeedAim >= 9830)
    	{
    		wFanReguSpeedAim = 9830;
    	}   	
    }
    else if(wFanReguSpeedAim < 2458)//15%
    {
    	wFanReguSpeedAim = 1000;
    }
    
    if(fFanCtrState == 1)
    {
    	if(wFanReguSpeedCtr < (wFanReguSpeedAim - 573))
        {
        	wFanReguSpeedCtr = wFanReguSpeedCtr + 573;
        }
        else if(wFanReguSpeedCtr > (wFanReguSpeedAim + 573))
        {
        	wFanReguSpeedCtr = wFanReguSpeedCtr - 573;
        }
        else
        {
        	wFanReguSpeedCtr = wFanReguSpeedAim;
        	if(wFanReguSpeedAim < 2458)
        	{
        		fFanCtrState = 0;
        	}
        }
        wFanReguSpeed = wFanReguSpeedCtr;
    }
        
    if(fFanCtrState == 0)
    {
    	wFanReguSpeedCtr = 9830;
    	wFanReguSpeed = 0;
    }
        
    uwFanDuty = ((INT32U)wFanReguSpeed*EPwm6Regs.TBPRD)>>14;

    if(wFanReguSpeed !=0)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}
/*********************************************************************
Function name: void sSysRatedCoeffChangeInit(void)
Description: AC Fan Ctrl
Calls:		
Called By: main(), 500ms = 1 period
Parameters: void
Return: 	INT16U	
*********************************************************************/
void sSysRatedCoeffChangeInit(void)
{
static INT16S wSysFrebak = 0;

    if(wEepromRatedFreqSel != wSysFrebak)
    {
    	wSysFrebak = wEepromRatedFreqSel;
    	if(wEepromRatedFreqSel == 1)
		{
            uwSysFreq = cFreq60Hz;
            sPLLParamInit();
            fSysFreChange = 1;
            wGridFreMaxArray[0] = cFreq60Hz;
	        wGridFreMaxArray[1] = cFreq60Hz;
	        wGridFreMaxArray[2] = cFreq60Hz;
	        wGridFreMaxArray[3] = cFreq60Hz;
	        
	        wGridFreMinArray[0] = cFreq60Hz;
	        wGridFreMinArray[1] = cFreq60Hz;
	        wGridFreMinArray[2] = cFreq60Hz;
	        wGridFreMinArray[3] = cFreq60Hz;
	        dwGridFreMaxSum = 61440;
	        dwGridFreMinSum = 61440;
            dwFreFiltMaxPrev = 3932160;
            dwFreFiltMinPrev = 3932160;
            wFreMaxBak = cFreq60Hz;
            wFreMinBak = cFreq60Hz;
		}
		else
		{
			uwSysFreq = cFreq50Hz;
            sPLLParamInit();
            fSysFreChange = 1;
            wGridFreMaxArray[0] = cFreq50Hz;
	        wGridFreMaxArray[1] = cFreq50Hz;
	        wGridFreMaxArray[2] = cFreq50Hz;
	        wGridFreMaxArray[3] = cFreq50Hz;
	        
	        wGridFreMinArray[0] = cFreq50Hz;
	        wGridFreMinArray[1] = cFreq50Hz;
	        wGridFreMinArray[2] = cFreq50Hz;
	        wGridFreMinArray[3] = cFreq50Hz;
	        dwGridFreMaxSum = 51200;
	        dwGridFreMinSum = 51200;
            dwFreFiltMaxPrev = 3276800;
            dwFreFiltMinPrev = 3276800;
            wFreMaxBak = cFreq50Hz;
            wFreMinBak = cFreq50Hz;
		}
    }
    else
    {
    	fSysFreChange = 0;
    }
    
    if(uwSysFreq == cFreq60Hz)
    {
    	if(wEepromRatedVolSel == 0)
        {
    	    uwHalfCycSampNum = cHalfCycSampNum60Hz400V;
            uwHalfCysRatedVolSqure = cHalfCysRatedVolSqure60Hz400V;
            udwHalfCysRatedVolSqure0R81 = cHalfCysRatedVolSqure0R8160Hz400V;
            udwHalfCysRatedVolSqure1R21 = cHalfCysRatedVolSqure1R2160Hz400V;
            udwHalfCysRatedVolSqure0R2 = cHalfCysRatedVolSqure0R260Hz400V;
            udwHalfCysRatedVolSqure0R01 = cHalfCysRatedVolSqure0R0160Hz400V;    	    
        }
        else
        {
        	uwHalfCycSampNum = cHalfCycSampNum60Hz480V;
            uwHalfCysRatedVolSqure = cHalfCysRatedVolSqure60Hz480V;
            udwHalfCysRatedVolSqure0R81 = cHalfCysRatedVolSqure0R8160Hz480V;
            udwHalfCysRatedVolSqure1R21 = cHalfCysRatedVolSqure1R2160Hz480V;
            udwHalfCysRatedVolSqure0R2 = cHalfCysRatedVolSqure0R260Hz480V;
            udwHalfCysRatedVolSqure0R01 = cHalfCysRatedVolSqure0R0160Hz480V;
        }
    }
    else
    {
    	if(wEepromRatedVolSel == 0)
        {
    	    uwHalfCycSampNum = cHalfCycSampNum50Hz400V;
            uwHalfCysRatedVolSqure = cHalfCysRatedVolSqure50Hz400V;
            udwHalfCysRatedVolSqure0R81 = cHalfCysRatedVolSqure0R8150Hz400V;
            udwHalfCysRatedVolSqure1R21 = cHalfCysRatedVolSqure1R2150Hz400V;
            udwHalfCysRatedVolSqure0R2 = cHalfCysRatedVolSqure0R250Hz400V;
            udwHalfCysRatedVolSqure0R01 = cHalfCysRatedVolSqure0R0150Hz400V;   	    
        }
        else
        {
        	uwHalfCycSampNum = cHalfCycSampNum50Hz480V;
            uwHalfCysRatedVolSqure = cHalfCysRatedVolSqure50Hz480V;
            udwHalfCysRatedVolSqure0R81 = cHalfCysRatedVolSqure0R8150Hz480V;
            udwHalfCysRatedVolSqure1R21 = cHalfCysRatedVolSqure1R2150Hz480V;
            udwHalfCysRatedVolSqure0R2 = cHalfCysRatedVolSqure0R250Hz480V;
            udwHalfCysRatedVolSqure0R01 = cHalfCysRatedVolSqure0R0150Hz480V;
        }
    }
    
    if(wEepromRatedPowerSel == 0)
    {
    	dwRatedPower = 25000;
    }
    else
    {
        if(wEepromRatedVolSel == 0)
        {
    	    dwRatedPower = 25000;
    	}
    	else
    	{
    	    dwRatedPower = 30000;
    	}
    }
    
    if(wEepromRatedVolSel == 0)
    {
    	wRatedGridLineVol = 6400;//400V
    	wpuGridLineVol = ((INT32S)((INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol) * 13981)>>14;
    }
    else
    {    	
    	wRatedGridLineVol = 7680;//480V   	
        wpuGridLineVol = ((INT32S)((INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol) * 11651)>>14;
    }
    
    wRatedRmsCurr = (((INT64S)dwRatedPower * 9459)/wRatedGridLineVol)>>2;//1/SQRT(3)  Q8
    wRatedInvCurrPeak = ((INT32S)wRatedRmsCurr * 23170)>>14;
    wRatedInvCurrInv = (INT32S)134217728/wRatedInvCurrPeak;
    wInvCurrPeakMax = ((INT32S)wRatedInvCurrPeak * 18022)>>14;
    wInvCurrResolving = ((INT32S)wRatedInvCurrPeak  * cPowerResolving)>>14;    
} 
/*********************************************************************
Function name:  INT16U suwCalBalanceDegree(T_THREE_PHASE_ABC *ptGridPhaseRmsVolt)
Description: cal Balance of Gird voltage
Calls:          
Called By:  main(void)
Parameters:  T_THREE_PHASE_AFBTGM *ptPosValue,T_THREE_PHASE_AFBTGM *ptNegValue
Return:  void
 *********************************************************************/
INT16S swCalBalanceDegree(T_THREE_PHASE_ABC *ptGridPhaseRmsVolt)
{
INT16S wMaxVolt,wAvgVolt;	
INT16S wBalanceDegree;
INT16S wAbsEVoltR,wAbsEVoltS,wAbsEVoltT;

    wAvgVolt = ((INT32S)(ptGridPhaseRmsVolt->a) + (INT32S)(ptGridPhaseRmsVolt->b) + (INT32S)(ptGridPhaseRmsVolt->c))/3;
    
    wAbsEVoltR = abs(ptGridPhaseRmsVolt->a - wAvgVolt);
    wAbsEVoltS = abs(ptGridPhaseRmsVolt->b - wAvgVolt);
    wAbsEVoltT = abs(ptGridPhaseRmsVolt->c - wAvgVolt);
     
    if(wAvgVolt <=0 ) 
    	wAvgVolt = 1;
    	
    if(wAbsEVoltR > wAbsEVoltS) 
    {	
        wMaxVolt = wAbsEVoltR;
    }
    else
    {
    	wMaxVolt = wAbsEVoltS; 
    }
    if(wAbsEVoltT > wMaxVolt)
    {
    	wMaxVolt = wAbsEVoltT;
    } 
    
    wBalanceDegree = ((INT32S)wMaxVolt*1000/wAvgVolt);
    return(wBalanceDegree);         
}
/*********************************************************************
Function name:  sCalLongAvg(INT16S *pData,INT16U uwAvgTimeCnt,INT16S *pAvgData)
Description:  º∆À„¿€º”∆Ωæ˘÷µ
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalSignedLongAvg(INT16S *pData,INT16U uwAvgTimeCnt,INT16S *pAvgData)
{
static INT16U i=0;
static INT32S dwDataSum=0;
    i++;
    dwDataSum += (*pData);  
    if(uwAvgTimeCnt == 0) 
		uwAvgTimeCnt = 1;
    if(i == uwAvgTimeCnt)
    {
        *pAvgData = dwDataSum / uwAvgTimeCnt;
        dwDataSum = 0;
        i = 0;
    }     	
}
/*********************************************************************
Function name:void sCalBusVolRef(INT16S *pRSRmsVol,INT16S *pSTRmsVol,INT16S *pTRRmsVol)
Description:  º∆À„¿€º”∆Ωæ˘÷µ
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalBusVolRef(INT16S *pRSRmsVol,INT16S *pSTRmsVol,INT16S *pTRRmsVol)
{
INT16S wGridVolMax;
INT32U udwStdBusVolRefTemp;
INT16S wBusVolRefLow;
static INT32U udwBusVolRefPrev = 93440;//730V * 128//Q7
//static INT32U udwBusVolRefPrevbak = 93440;
//static INT16U uwLVRTDelay = 50;
    
    if(fCSVSSwitch == 0)
    {
	    wGridVolMax = *pRSRmsVol;
	    
	    if (wGridVolMax < *pSTRmsVol)
	    {
	    	wGridVolMax = *pSTRmsVol;
	    }
	    
	    if (wGridVolMax < *pTRRmsVol)
	    {
	    	wGridVolMax = *pTRRmsVol;
	    }
	    
	    wBusVolRefLow = (((INT32U)wRatedGridLineVol * cSqrt2)>>14) + cVolt70V;
	    udwStdBusVolRefTemp = (((INT32U)wGridVolMax * cSqrt2)>>14) + cVolt70V;
	    	
	    if (udwStdBusVolRefTemp < wBusVolRefLow)  
	    {
	    	udwStdBusVolRefTemp = wBusVolRefLow;			
	    }
	    else if (udwStdBusVolRefTemp > cBusVolRefHigh)  
	    {
	    	udwStdBusVolRefTemp = cBusVolRefHigh;	
	    }
	    
	    if((INT32U)wStdBusVolRef < (udwStdBusVolRefTemp - 160))
	    {
	    	udwBusVolRefPrev = (udwStdBusVolRefTemp + 80)<<3;
	    	udwStdBusVolRefTemp = udwBusVolRefPrev;
	    }
	    else
	    {
	        udwStdBusVolRefTemp = udwStdBusVolRefTemp<<3;
	    }	
		
	    udwBusVolRefPrev = (udwBusVolRefPrev * 3847 + udwStdBusVolRefTemp * 249)>>12;
	    wStdBusVolRef = udwBusVolRefPrev>>3;
	    
	    //if(fLVRTValue < uwLVRTActiveValue)
	    //{
	    //	uwLVRTDelay = 0;
	    //}
	    //
	    //if(++uwLVRTDelay >= 50)
	    //{
	    //	uwLVRTDelay = 50;
	    //	wStdBusVolRef = udwBusVolRefPrev>>3;
	    //}
	    //else
	    //{
	    //	wStdBusVolRef = udwBusVolRefPrevbak>>3;
	    //	udwBusVolRefPrev = udwBusVolRefPrevbak;
	    //}
	    //
	    //udwBusVolRefPrevbak = udwBusVolRefPrev;
	}
	else
	{
	    udwStdBusVolRefTemp = (((INT32U)wVSVoltSet * 28378)>>14) + cVolt100V;
	    if (udwStdBusVolRefTemp < cBusVolRefLow)  
	    {
	    	udwStdBusVolRefTemp = cBusVolRefLow;			
	    }
	    else if (udwStdBusVolRefTemp > cBusVolRefHigh)  
	    {
	    	udwStdBusVolRefTemp = cBusVolRefHigh;	
	    }
	    
	    udwBusVolRefPrev = (INT32U)udwStdBusVolRefTemp<<3;
	    //udwBusVolRefPrevbak = udwBusVolRefPrev;
	    wStdBusVolRef = udwStdBusVolRefTemp;
	}	
}
/*********************************************************************
Function name:void sBiDirDCModeSwitch(INT16S wBatVolt,INT16S wBatCurr,INT32S dwPower)
Description:  º∆À„¿€º”∆Ωæ˘÷µ
Calls:      	  
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sBiDirDCModeSwitch(INT16S wBatVolt,INT16S wBatCurr,INT32S dwPower)
{
static INT16U uwDelay0 = 0;
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;
static INT16U uwDelay3 = 0;
static INT16U uwDelay4 = 0;

    //fBiDirDCEnOut = 0,disable; 
    //                1,Enable 1And2;  
    //                2,Enable 3And4;   
    //                3,Enable 1,2,3,4;
    
    if(fDCModeSwitchEn == 1)
    {
    	if(fDCModeSwitch == 0)//All Run but buck duty = 100%
    	{
            uwDelay1 = 0;
            uwDelay2 = 0;
            uwDelay3 = 0;
            uwDelay4 = 0;
            fBiDirDCEnOut = 3;
	    	    
    		if(wBatVolt <= (wStdBusVolRef + cVolt20V - cVolt5V))
    		{
    			if(++uwDelay0 >= 3)
    			{
    				uwDelay0 = 3;
    			    fDCModeSwitch = 2;
	    		    fBiDirDCEnOut = 3;
	    	    }
    		}
    		else
    		{
    			uwDelay0 = 0;
    		}
    	}
    	else if(fDCModeSwitch == 1)//1,2 or 3,4 Runing
    	{
    		uwDelay0 = 0;
    		uwDelay3 = 0;
    		uwDelay4 = 0;
    		if(wEepromBiDirDCSwitch == 0)
	    	{
	    		fBiDirDCEnOut = 1;
	    	}
	    	else
	    	{
	    		fBiDirDCEnOut = 2;
	    	}
	    	
	    	if(((fActiveMode == 0)&&(abs(wActivePowerPer)> 8192))||((fActiveMode == 1)&&(abs(wDCCurrAim)> cCurr25A))||(fCSVSSwitch == 1))
	    	{
	    		fDCModeSwitch = 2;
	    	    fBiDirDCEnOut = 3;
	    	}
	    	else
	    	{
    		
    		    if(wBatVolt >= (wStdBusVolRef + cVolt20V + cVolt5V))
    		    {
    		    	uwDelay1 = 0;
    		    	if(++uwDelay2 >= 3)
    		    	{
    		    		uwDelay2 = 3;
    		    		fDCModeSwitch = 0;
	    	    	    fBiDirDCEnOut = 3;
    		    	}
    		    }
    		    else
    		    {
    		    	uwDelay2 = 0;
    		    	if((abs(wBatCurr) >= cCurr25A)||(labs(dwPower) > cPower15kW))
    		        {
    		        	if(++uwDelay1 >= 1)
    		        	{
    		        		uwDelay1 = 1;
    		        	    fDCModeSwitch = 2;
	    	        	    fBiDirDCEnOut = 3;
	    	            }
    		        }
    		        else
    		        {
    		        	uwDelay1 = 0;
    		        }
    		    }
    		}   		
    	}
    	else if(fDCModeSwitch == 2)
    	{
    		uwDelay0 = 0;
    		uwDelay1 = 0;
    		uwDelay2 = 0;
    		fBiDirDCEnOut = 3;
    		    	  		
    		if(wBatVolt >= (wStdBusVolRef + cVolt20V + cVolt5V))
    		{
    			uwDelay4 = 0;
    			if(++uwDelay3 >= 3)
    			{
    				uwDelay3 = 3;
    				fDCModeSwitch = 0;
	    		    fBiDirDCEnOut = 3;
    			}
    		}
    		else
    		{
    			uwDelay3 = 0;
    			
    			if(((fActiveMode == 0)&&(abs(wActivePowerPer)> 8192))||((fActiveMode == 1)&&(abs(wDCCurrAim)> cCurr25A))||(fCSVSSwitch == 1))
    			{
    				uwDelay4 = 0;
    			}
    			else
    			{
    				if((abs(wBatCurr) <= cCurr18A)&&(labs(dwPower) < cPower10kW))
    		        {
    		        	if(++uwDelay4 >= 50)
    		        	{
    		        		uwDelay4 = 50;
    		        	    fDCModeSwitch = 1;
	    	        	    if(wEepromBiDirDCSwitch == 0)
	    	                {
	    	                	fBiDirDCEnOut = 1;
	    	                }
	    	                else
	    	                {
	    	                	fBiDirDCEnOut = 2;
	    	                }
	    	            }
    		        }
    		        else
    		        {
    		        	uwDelay4 = 0;
    		        }
    			}    			
    		}    		
    	}
    	else
    	{
    		uwDelay0 = 0;
    	    uwDelay1 = 0;
    	    uwDelay2 = 0;
    	    uwDelay3 = 0;
    	    uwDelay4 = 0;
    	    fBiDirDCEnOut = 0;
    	}	
	}
    else
    {
    	uwDelay0 = 0;
    	uwDelay1 = 0;
    	uwDelay2 = 0;
    	uwDelay3 = 0;
    	uwDelay4 = 0;
    	fBiDirDCEnOut = 0;
    }	
} 
/*********************************************************************
Function name:INT16U sDecCapEnergy(INT16S *pBusVol)
Description:  
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
INT16U sDecCapEnergy(INT16S *pBusVol)
{
static INT16U uwDelay = 0;
INT16U fDecCapOK = 0;

    switch(uwDECapEnergyStep)
    {
        case 0:   
        	fPowerDanger = 1;      	
            if(fDCRelayPCtr == 1)
            {
                sDCRelayPOpen();
                uwDelay = 0;
            }
            else
            {
                if(++uwDelay > 300)
                {
                	if(fDecCapEnergyErrEvent == 1)
                	{
                        sRestartVarInit();
                        uwDelay = 0;  
                        sEnDCSps();
                        sEnInvSps();                                   
                        uwDECapEnergyStep++;
                    }
                    else
                    {
	                    fDecCapOK = 2;
	                    uwDelay = 0;
	                    sRestartVarInit();
                    }
                }
            }
	        break;
	        
	    case 1:
	        sGridRelayOpen();
			sInvRelayOpen(); 
			sAuxiRelayOpen();
			sPERelayOpen();
			sIsoRelayOpen();
			sDCRelayPOpen();
			sDCSoftRelayOpen();
			
			if(fPhaseSequBak == 0)
			{
	            if(++uwDelay > 300) 
			    {
			        uwDelay = 0;
			        fPhaseSequ = 1;
			        uwDECapEnergyStep++;
			        uwModuSoftIncCnt = 0;
			        fRunInvAhdChkRequire = 0;
			    }
			}
			else
			{
				if(++uwDelay > 300) 
			    {
			        uwDelay = 0;
			        fPhaseSequ = fPhaseSequBak;
			        uwDECapEnergyStep++;
			        uwModuSoftIncCnt = 0;
			        fRunInvAhdChkRequire = 0;
			    }
			}
			break;
			
	    case 2:
	        if((fDCRelayPCtr == 0)&&(fGridRelayCtr == 0))
	        {
	            if(uwModuSoftIncCnt < 2048)
	            {
	                uwModuSoftIncCnt += 8;
	            }
	            else
	            {
	                uwModuSoftIncCnt = 2048;
	            }
	            
                sfInvVolNormalChk(twInvPhaseAvgVol,twInvPhaseRmsVol,&fRunInvAhdErr,&fRunInvAhdErrEvent);
                
	            if((fOCPErrEvent == 0)&&(fIGBTHaltErrEvent == 0)&&(fOVPErrEvent == 0)&&(fRunInvAhdErrEvent == 0)&&(fDCOCPErrEvent == 0)&&(fMcuActDspNoActErrEvent == 0))
	            {
	                fRunInvAhdChkRequire = 0;
                    fInvEnOut = 1;
                    if(*pBusVol < cVolt30V)
                    {
                        if(++uwDelay > 5)
                        {
                            uwDelay = 0;
                            uwDECapEnergyStep++;
                        }
                    }
	            }
	            else
	            {
	                sGridRelayOpen();
				    sInvRelayOpen();
				    sAuxiRelayOpen();
				    sDCRelayPOpen();
					sDCSoftRelayOpen();
	                uwModuSoftIncCnt = 0;
	                fRunInvAhdChkRequire = 0;
	                fInvEnOut = 0;
	                fDecCapOK = 2;
	                uwDelay = 0;
	                sRestartVarInit();
	            }    
	        }
	        else
	        {
	            sGridRelayOpen();
				sInvRelayOpen();
				sAuxiRelayOpen();
				sDCRelayPOpen();
				sDCSoftRelayOpen();
	            uwModuSoftIncCnt = 0;
	            fRunInvAhdChkRequire = 0;
	            fInvEnOut = 0;
	            fDecCapOK = 2;
	            uwDelay = 0;
	            sRestartVarInit();
	        }
            break;
        case 3:
            fInvEnOut = 0;
            uwDelay = 0;
            uwModuSoftIncCnt = 0;
            sRestartVarInit();
            sGridRelayOpen();
            sInvRelayOpen();
            sAuxiRelayOpen();
            sDCRelayPOpen();
			sDCSoftRelayOpen();
            fDecCapOK = 1;
            break;
        default: 
            break;         
    } 
    return(fDecCapOK);          
}
/************************************************************************
Function name:INT16U swEfficiencyFit(INT32S dwPPower,INT16S wBatVolt)
Description:  
Calls:		  
Called By:		 
Parameters:  
Return:  INT16S 	 
************************************************************************/
INT16U swEfficiencyFit(INT32S dwPPower,INT16S wBatVolt)
{	 
INT16U uwEfficiency;
INT16S wPPercent;
    
	if(wEepromRatedPowerSel == 1)
    {
        wPPercent = ((INT64S)dwPPower * 21845)>>20;//100% = 10000; 10000/30000/16
    }
    else
    {
    	wPPercent = ((INT64S)dwPPower * 26214)>>20;//100% = 10000; 10000/25000/16
    }
    
    wPPercent = abs(wPPercent);
    
    if((uwSysMode == InvRunMode)&&(fInvEnOut == 1))
    {
        if(wBatVolt < cVolt300V)
        {
            wBatVolt = cVolt300V;
        }
        else if(wBatVolt > cVolt800V)
        {
            wBatVolt = cVolt800V - (wBatVolt - cVolt800V);
        }

        if(wPPercent <= 200)
        {
            uwEfficiency = 8900;
            
            if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 2701)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1667)>>15);
            }
        }
    	else if(wPPercent <= 500)
    	{
    		uwEfficiency = 8625 + (((INT32S)wPPercent * 22528)>>14);
    		
    		if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 2701)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1667)>>15);
            }    		
    	}
    	else if(wPPercent <= 1000)
    	{
    		uwEfficiency = 9075 + (((INT32S)wPPercent * 15569)>>15);
    		
    		if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 802)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1502)>>15);
            }     		
    	}
    	else if(wPPercent <= 1500)
    	{
    		uwEfficiency = 9353 + (((INT32S)wPPercent * 12905)>>16);
    		    		
    		if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 965)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1267)>>15);
            }
    	}
    	else if(wPPercent <= 2000)
    	{
    		uwEfficiency = 9502 + (((INT32S)wPPercent * 6389)>>16);
    		
    		if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 828)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1093)>>15);
            }
    	}
    	else if(wPPercent <= 2500)
    	{
    		uwEfficiency = 9624 + (((INT32S)wPPercent * 2396)>>16);
    		
    		if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 685)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1176)>>15);
            }
    	}
    	else if(wPPercent <= 3000)
    	{
    	    uwEfficiency = 9631 + (((INT32S)wPPercent * 4453)>>17);
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 700)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1109)>>15);
            }
    	}
    	else if(wPPercent <= 4000)
    	{
    	    uwEfficiency = 9700 + (((INT32S)wPPercent * 1439)>>17);
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 893)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1096)>>15);
            }
    	}
    	else if(wPPercent <= 5000)
    	{
    	    uwEfficiency = 9751 - (((INT32S)wPPercent * 1937)>>20);
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 781)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1118)>>15);
            }
    	}
    	else if(wPPercent <= 6000)
    	{
    	    uwEfficiency = 9743 - (((INT32S)wPPercent * 4381)>>24);
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 674)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1008)>>15);
            }
    	}
    	else if(wPPercent <= 7000)
    	{
    	    uwEfficiency = 9754 - (((INT32S)wPPercent * 2220)>>20);
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 681)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 909)>>15);
            }
    	}
    	else if(wPPercent <= 7500)
    	{
    	    uwEfficiency = 9781 - (((INT32S)wPPercent * 6189)>>20);
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 649)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1015)>>15);
            }
    	}
    	else if(wPPercent <= 8000)
    	{
    	    uwEfficiency = 9736;
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 649)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 1015)>>15);
            }
    	}
    	else
    	{
    	    uwEfficiency = 9733;
    	    
    	    if(wBatVolt <= cVolt660V)
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 564)>>15);
            }
            else
            {
                uwEfficiency = uwEfficiency + (((INT32S)(wBatVolt - cVolt660V) * 910)>>15);
            }
    	}
    	
    	if(fCharge == 1)
    	{
    	    uwEfficiency = uwEfficiency - 87;
    	}	
    }
    else
    {
    	uwEfficiency = 0;
    }

    return(uwEfficiency);		
}
/*********************************************************************
Function name:INT16U sfRunInvAhdChk(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
INT16U sfRunInvAhdChk(void)
{
static INT16U uwRunInvAhdDelay5s = 0;
static INT16U uwRunInvAhdStep = 0;
static INT16U uwPLLOKDelay = 0;
static INT16U uwPLLBadDelay = 0;

INT16U fRunInvAndOk = 0;

    if(fRunInvAhdChkRestart == 0)
    {        
        uwRunInvAhdDelay5s = 0;
        uwRunInvAhdStep = 0;   
        uwModuSoftIncCnt = 0;
        fInvEnOut = 0;
        fRunInvAhdChkRestart = 1;
        uwPLLOKDelay = 0;
        uwPLLBadDelay = 0;
        wIdRefBak = 0;
        wIqRefBak = 0;
        wDebugRunInvChkStep = uwRunInvAhdStep;
    }
    else
    {
    	wDebugRunInvChkStep = uwRunInvAhdStep;
        switch(uwRunInvAhdStep)
        {
            case 0:
                if(uwModuSoftIncCnt < 512)
                {	
                    if(fError == 0)
                    {
                        fInvEnOut = 1;
                    }
	                uwModuSoftIncCnt += 2;
	            }
	            else
	            {
	                uwRunInvAhdStep++;
	            }
	            break;
	        
	        case 1:
	            fInvLittleVolOk = sfInvVolNormalChk(twInvPhaseAvgVol,twInvPhaseRmsVol,&fRunInvAhdErr,&fRunInvAhdErrEvent);
	            
	            if(fInvLittleVolOk == 1)
	            {
	                if(uwModuSoftIncCnt < 2048)
	                {
	                    uwModuSoftIncCnt += 2;
	                }
	                else
	                {
	                	uwModuSoftIncCnt = 2048;
	                	
	                	wIdRefBak = ((INT32S)wIdRefBak * 31755+ (INT32S)tInvCurrDqo.d * 1013)>>15;
	                	wIqRefBak = ((INT32S)wIqRefBak * 31755+ (INT32S)tInvCurrDqo.q * 1013)>>15;
	                	
	                    if(++uwRunInvAhdDelay5s > 200)
	                    {
	                        uwRunInvAhdDelay5s = 0;
	                        //uwPLLChkDelay = 0;
	                        uwRunInvAhdStep++;
	                        uwGridAndInvVolChkOk = 0;
	                    }
	                }
	            }
	            else
	            {
	                uwRunInvAhdDelay5s = 0;
	            }
	            break;
	    
	        case 2:	        	           
	            if(((tInvVolDqo.d) < cVolt18V)&&((tInvVolDqo.q) < cVolt15V))
	            {
	            	uwPLLBadDelay = 0;
	            	if(++uwPLLOKDelay >= 50)
	            	{
	            		fGridRelayCloseDeal = 1;
	            		sDCOCPDisable();
	            		sGridRelayClose();
	            		uwModuSoftIncCnt = 0; 
						uwPLLOKDelay = 0;
						uwRunInvAhdDelay5s = 0;
	                    uwRunInvAhdStep++;
	            	}
	            }
	            else
	            {
	            	uwPLLOKDelay = uwPLLOKDelay>>1;
	            	
	            	if(++uwPLLBadDelay > 200)
	            	{
	            		uwPLLBadDelay = 200;
	            		uwRunInvAhdStep = 0;
	                    uwRunInvAhdDelay5s = 0;
				      	uwModuSoftIncCnt = 0;
				      	sRestartVarInit();
				      	sGridRelayOpen();
 	                    sInvRelayOpen();
 	                    sAuxiRelayOpen();
                        sIsoRelayOpen();
                        sPERelayOpen();
						sDCSoftRelayOpen();
						fACPWMLostPreChk=1;
                        uwSysModePrepare = PowerOnMode;
                        uwSysMode = PowerOnMode;
	            	}
	            }
	            break;
	        case 3:
	        	uwPLLOKDelay = 0;
	        	uwPLLBadDelay = 0;	        	
	        	uwModuSoftIncCnt = 0;
	        	uwRunInvAhdDelay5s++;
	        	if(uwRunInvAhdDelay5s >= 3)
	        	{
	        		uwRunInvAhdDelay5s = 0;
	        		uwRunInvAhdStep++;
	        	}	        	
	        	break;
	        
	        case 4:
	        	//fInvEnOut = 0;	        	
	        	wInvCtrBusVolRef = wBusAvgVol - cVolt20V;
	        	fGridRelayCloseDeal = 0;	        	
	            fRunInvAndOk = 1; //Check suceed  
                uwRunInvAhdStep++; 
                uwRunInvAhdDelay5s = 0; 
	            break;
	        
            default:
	            uwRunInvAhdDelay5s = 0;
                uwRunInvAhdStep = 0;   
                uwModuSoftIncCnt = 0;
                fInvEnOut = 0;
                fRunInvAhdChkRestart = 0;
               break;
        }
    }
    return(fRunInvAndOk);   
}
/*********************************************************************
Function name:INT16U sfRunInvAhdChkByAC(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
INT16U sfRunInvAhdChkByAC(void)
{
static INT16U uwRunInvAhdDelay5s = 0;
static INT16U uwRunInvAhdStep = 0;
static INT32S dwIdSum = 0;
static INT32S dwIqSum = 0;

INT16U fRunInvAndOk = 0;

    if(fRunInvAhdChkRestart == 0)
    {        
        uwRunInvAhdDelay5s = 0;
        uwRunInvAhdStep = 0;   
        uwModuSoftIncCnt = 0;
        fInvEnOut = 0;
        fRunInvAhdChkRestart = 1;
        wIdRefBak = 0;
        wIqRefBak = 0;
        dwIdSum = 0;
        dwIqSum = 0;
        wDebugRunInvChkStep = uwRunInvAhdStep;
    }
    else
    {
    	wDebugRunInvChkStep = uwRunInvAhdStep;
        switch(uwRunInvAhdStep)
        {
        	case 0:
        		sGridRelayOpen();
        		if(wBusAvgVol < (wStdBusVolRef - cVolt70V - cVolt70V))
        		{
        			sGridRelayOpen();
        			sAuxiRelayClose();
        			
        			if(++uwRunInvAhdDelay5s >= 1000)
        			{
        				uwRunInvAhdDelay5s = 1000;
        				fAuxiRelayLost = 1;
        			}        			
        		}
        		else
        		{
        			sAuxiRelayOpen();
        			sGridRelayOpen();
        			uwRunInvAhdDelay5s = 0;
        			uwRunInvAhdStep++;        			
        		}
        		break;
        		
            case 1:
                if(uwModuSoftIncCnt < 512)
                {	
                    if(fError == 0)
                    {
                        fInvEnOut = 1;
                    }
	                uwModuSoftIncCnt += 4;
	            }
	            else
	            {
	                uwRunInvAhdStep++;
	            }
	            break;
	        
	        case 2:
	            fInvLittleVolOk = sfInvVolNormalChk(twInvPhaseAvgVol,twInvPhaseRmsVol,&fRunInvAhdErr,&fRunInvAhdErrEvent);
	            
	            if(fInvLittleVolOk == 1)
	            {
	                if(uwModuSoftIncCnt < 1843)//90% * 2048
	                {
	                    uwModuSoftIncCnt += 4;
	                }
	                else
	                {
	                	uwModuSoftIncCnt = 1843;	                	
	                    if(++uwRunInvAhdDelay5s > 200)
	                    {
	                        uwRunInvAhdDelay5s = 0;
	                        uwRunInvAhdStep++;
	                        uwGridAndInvVolChkOk = 0;
	                    }
	                }
	            }
	            else
	            {
	                uwRunInvAhdDelay5s = 0;
	            }
	            break;
	            
	        case 3:	        	
	        	if(wBusAvgVol < (wStdBusVolRef + cVolt40V))//900V Or wStdBusVolRef + 1920  PWM is Runing And BUS can be Up,This Second Charge.
	        	{
	        		sGridRelayOpen();
	        		sAuxiRelayClose();
	        		
	        		if(++uwRunInvAhdDelay5s >= 1000)
        			{
        				uwRunInvAhdDelay5s = 1000;
        				fAuxiRelayLost = 1;
        			} 
	        	}
	        	else
	        	{
	        		sAuxiRelayOpen();
	        		sGridRelayOpen();
        			uwRunInvAhdDelay5s = 0;
        			uwRunInvAhdStep++; 
	        	}
	        	break;
	        	
	        case 4:
	        	sAuxiRelayOpen();
	        	uwModuSoftIncCnt = 2048;
	        	
	        	dwIdSum = dwIdSum + tInvCurrDqo.d;
                dwIqSum = dwIqSum + tInvCurrDqo.q;
                
                ++uwRunInvAhdDelay5s;
                
                if(uwRunInvAhdDelay5s >= 32)//16
	            {
	            	wIdRefBak = dwIdSum>>5;
                    wIqRefBak = dwIqSum>>5;
                    dwIdSum = 0;
                    dwIqSum = 0;
	                uwRunInvAhdDelay5s = 0;
	                uwRunInvAhdStep++;
	                uwGridAndInvVolChkOk = 0;
	            }
	            break;
	            
	        case 5:	 
	        	if(wBusAvgVol >= (wStdBusVolRef - cVolt70V))
	        	{
	        		if((tInvVolDqo.d < cVolt18V)&&(tInvVolDqo.q < cVolt15V))
	                {	                		            	
	                	fGridRelayCloseDeal = 1;
	                	sDCOCPDisable();
	                	sGridRelayClose();
	                	uwModuSoftIncCnt = 0; 
	                    uwRunInvAhdStep++;
	                }
	        	}
	        	else
	        	{
	            	uwRunInvAhdStep = 0;
	                uwRunInvAhdDelay5s = 0;
				    uwModuSoftIncCnt = 0;
				    sRestartVarInit();
				    sGridRelayOpen();
 	                sInvRelayOpen();
 	                sAuxiRelayOpen();
                    sIsoRelayOpen();
                    sPERelayOpen();
					sDCSoftRelayOpen();
                    uwSysModePrepare = PowerOnMode;
                    uwSysMode = PowerOnMode;
	        	}       	           
	            break;
	        case 6:
	        	uwModuSoftIncCnt = 0;
	        	uwRunInvAhdDelay5s++;	        	
	        	if(uwRunInvAhdDelay5s >= 3)
	        	{
	        	    uwRunInvAhdDelay5s = 0;	        	    
	        	    uwRunInvAhdStep++;
	        	}
	        	break;
	        
	        case 7:
	        	//fInvEnOut = 0;	        	
	        	wInvCtrBusVolRef = wStdBusVolRef;
	        	fGridRelayCloseDeal = 0;	        	
	            fRunInvAndOk = 1; //Check suceed  
                uwRunInvAhdStep++; 
                uwRunInvAhdDelay5s = 0;
	            break;
	        
            default:
	            uwRunInvAhdDelay5s = 0;
                uwRunInvAhdStep = 0;   
                uwModuSoftIncCnt = 0;
                fInvEnOut = 0;
                fRunInvAhdChkRestart = 0;
               break;
        }
    }
    return(fRunInvAndOk);   
}
/*********************************************************************
Function name:void sVSGridPretectCheckEn(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
void sVSGridPretectCheckEn(void)
{
static INT16U uwVSFreqChkDelay = 0;
static INT16U uwVSVoltChkDelay = 0;

    if(fCSVSSwitch == 0)
    {
        uwVSFreqChkDelay = 0;
        uwVSVoltChkDelay = 0;
        fVSFreqCheckEn = 0;
        fVSVoltCheckEn = 0;
    }
    else
    {
        if((wRSGridRmsVol > 3200)&&(wSTGridRmsVol > 3200)&&(wTRGridRmsVol > 3200))
        {
            if(++uwVSFreqChkDelay >= 5)
            {
                fVSFreqCheckEn = 1;
                uwVSFreqChkDelay = 5;
            }
        }
        else
        {
            if(uwVSFreqChkDelay > 0)
            {
                uwVSFreqChkDelay--;
            }
            else
            {
                fVSFreqCheckEn = 0;
                uwVSFreqChkDelay = 0;
            }
        }
        
        if(++uwVSVoltChkDelay >= 5)
        {
            fVSVoltCheckEn = 1;
            uwVSVoltChkDelay = 5;
        }
    }  
}
/*********************************************************************
Function name:INT16U sfRunInvAhdChkByVS(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
**********************************************************************/
INT16U sfRunInvAhdChkByVS(void)
{
static INT16U uwRunInvAhdDelay5s = 0;
static INT16U uwRunInvAhdStep = 0;
INT16U fRunInvAndOk = 0;

    if(fRunInvAhdChkRestart == 0)
    {        
        uwRunInvAhdDelay5s = 0;
        uwRunInvAhdStep = 0;   
        uwModuSoftIncCnt = 0;
        fInvEnOut = 0;
        fRunInvAhdChkRestart = 1;
        wDebugRunInvChkStep = uwRunInvAhdStep;
    }
    else
    {
    	wDebugRunInvChkStep = uwRunInvAhdStep;
        switch(uwRunInvAhdStep)
        {
            case 0:
                if(uwModuSoftIncCnt < 512)
                {	
                    if(fError == 0)
                    {
                        fInvEnOut = 1;
                    }
	                uwModuSoftIncCnt += 2;
	            }
	            else
	            {
	                uwRunInvAhdStep++;
	            }
	            break;
	        
	        case 1:
	            fInvLittleVolOk = sfInvVolNormalChk(twInvPhaseAvgVol,twInvPhaseRmsVol,&fRunInvAhdErr,&fRunInvAhdErrEvent);
	            
	            if(fInvLittleVolOk == 1)
	            {
	                if(uwModuSoftIncCnt < 2048)
	                {
	                    uwModuSoftIncCnt += 2;
	                }
	                else
	                {
	                	uwModuSoftIncCnt = 2048;
	                	
	                    if(++uwRunInvAhdDelay5s > 200)
	                    {
	                        uwRunInvAhdDelay5s = 0;
	                        uwRunInvAhdStep++;
	                    }
	                }
	            }
	            else
	            {
	                uwRunInvAhdDelay5s = 0;
	            }
	            break;
	    
	        case 2:	
	            fInvEnOut = 0;
	            if(++uwRunInvAhdDelay5s > 50)
	            {	                
	                sDCOCPDisable();
	            	sGridRelayClose();
	            	uwModuSoftIncCnt = 0; 
					uwRunInvAhdDelay5s = 0;
	                uwRunInvAhdStep++;
	            }
	            break;  
	            
	        case 3:	
	            if(++uwRunInvAhdDelay5s > 50)
	            {
	                fRunInvAndOk = 1;
                    uwRunInvAhdStep++; 
                    uwRunInvAhdDelay5s = 0;
	            }	             
	            break;
	        
            default:
	            uwRunInvAhdDelay5s = 0;
                uwRunInvAhdStep = 0;   
                uwModuSoftIncCnt = 0;
                fInvEnOut = 0;
                fRunInvAhdChkRestart = 0;
               break;
        }
    }
    return(fRunInvAndOk);   
}
/*********************************************************************
Function name:  sPowerFactor(INT32U *pInvLongActivePower,INT32U *pLongApparentPower,INT16S *pPowerFactor)
Description:  º∆À„PF
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sPowerFactor(INT32S *pInvLongActivePower,INT32U *pLongApparentPower,INT16S *pPowerFactor)
{
INT32S dwACPowerTmp = 0;
INT32U udwDCPowerTmp = 0;
INT16U i;
    for(i=0;i<4;i++)
    {
        dwACPowerTmp = (*(pInvLongActivePower+i))<<8;
        if(dwACPowerTmp < 0)
        {
            dwACPowerTmp = -dwACPowerTmp;
        }
		
		udwDCPowerTmp = (*(pLongApparentPower+i))>>2;//Q2
	    if(udwDCPowerTmp >= 1500)
        {
            *(pPowerFactor+i) = (INT16S)(dwACPowerTmp/udwDCPowerTmp);//Q10
            *(pPowerFactor+i) = abs(*(pPowerFactor+i));
        }
        else
        {
            *(pPowerFactor+i) = 0;
        }
		if((*(pPowerFactor+i)) > 1023)
        {
            *(pPowerFactor+i) = 1023;
        }
		else if((*(pPowerFactor+i)) < 0)
		{
		    *(pPowerFactor+i) = 0;
		}
    }
}
/*********************************************************************
Function name:  void sAdjustPF(void)
Description:  µ˜’˚PF
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sAdjustPF(void)
{
INT16S wAdjustPFSet,wAdjustPFFdb,wTemp;
INT32S dwReactiveRef,dwReactiveFdb;



    if((fInvEnOut == 1)&&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))
    {
    	fQCtrEnDelay2s++;
    	if(fQCtrEnDelay2s >= 4)
    	{
    		fQCtrEnDelay2s = 4;
    	}    	
    }
    else
    {
    	fQCtrEnDelay2s = 0;
    }
    
    //fQCtrEnDelay2s = 0;
  
    //---------PFµ˜’˚----------------------------------//
    if(fReactiveMode == 0)//—°‘ÒPFƒ£ Ω
    {
	    wPFSetHex = wPFSet>>4;
        if((wPFSetHex > 0)&&(wPFSetHex <= 1024))
        {    
            wAdjustPFSet = 1024 - wPFSetHex;
        }
        else if((wPFSetHex < 0)&&(wPFSetHex >= -1024))
        {
            wAdjustPFSet= -1024 - wPFSetHex;
        } 
        else
        {
        }     
        if((wPowerFactorAvg[3] > 0)&&(wPowerFactorAvg[3] <= 1024))
        {
            wAdjustPFFdb = 1024 - wPowerFactorAvg[3];
        }
        else if((wPowerFactorAvg[3] < 0)&&(wPowerFactorAvg[3] >= -1024))
        {
            wAdjustPFFdb = -1024 - wPowerFactorAvg[3];
        }
        else
        {
        }
        wReactiveCurrErr = wAdjustPFSet - wAdjustPFFdb;

		
        
        if(wReactiveCurrErr > 200)
        {
        	wReactiveCurrErr = 200;
        }
        else if(wReactiveCurrErr < -200)
        {
        	wReactiveCurrErr = -200;
        }

		if(abs(wPowerFactorAvg[3])>1000)
		{
		  if(wReactiveCurrErr > 50)
	        {
	        	wReactiveCurrErr = 50;
	        }
	        else if(wReactiveCurrErr < -50)
	        {
	        	wReactiveCurrErr = -50;
	        }
		}
		
        wAdjustStep = ((INT32S)tInvCurrDqoRef.d * 5947)>>21;//0.002835882
        
        if(wAdjustStep > 256)//42
        {
        	wAdjustStep = 256;
        }
        else if(wAdjustStep < 2)
        {
        	wAdjustStep = 2;
        }
                    
        if(abs(tInvCurrDqoRef.d) > 512)
        {
        	if((fAdjustQEn == 1)&&(fAIStart == 0)&&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue)&&(fQCtrEnDelay2s >= 4))
        	{
        		wReactiveCurrRefPreSat = wReactiveCurrRefPreSat - wReactiveCurrErr * wAdjustStep;
        	}      
            
            if (wReactiveCurrRefPreSat > cAdjustPFLimitHigh)
            {
                wReactiveCurrRefPreSat = cAdjustPFLimitHigh;
            }
            else if (wReactiveCurrRefPreSat < cAdjustPFLimitLow)
            {
                wReactiveCurrRefPreSat = cAdjustPFLimitLow;
            }
            else
            {                    
            }
			
            wAdjustPF = wReactiveCurrRefPreSat;
        }
        else if(abs(tInvCurrDqoRef.d) < 256)
        {
            wAdjustPF = 0;
            wReactiveCurrRefPreSat = 0;
        }   
    }
    else
    {
    	dwReactiveRef = dwReactivePowerAim;//Q8
    	dwReactiveFdb = dwInvReactivePower[3]<<4;//Q8        
        
    	if(fInvEnOut == 1)
    	{
    		if((fAdjustQEn == 1)&&(fAIStart == 0)&&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue)&&(fQCtrEnDelay2s >= 4))
    		{
    			dwReactiveRef = dwReactiveRef - dwReactiveFdb;
    			if(dwReactiveRef > 12800)
                {
                	wTemp = ((INT32S)wRSGridRmsAvgVol + wSTGridRmsAvgVol + wTRGridRmsAvgVol)>>4;//Q0
					wTemp = ((INT32S)wTemp * 9459)>>14;
                    wTemp = dwReactiveRef/wTemp;
                    wAdjustStep = ((INT32S)wTemp * cSqrt2)>>14; 
                    
                    if(wAdjustStep > 256)
                    {
                    	wAdjustStep = 256;
                    }
                    else if(wAdjustStep < 8)
                    {
                    	wAdjustStep = 8;
                    }
                     
                	wReactiveCurrRefPreSat -= wAdjustStep;
                }
                else if(dwReactiveRef < -12800)
                {
                	wTemp = ((INT32S)wRSGridRmsAvgVol + wSTGridRmsAvgVol + wTRGridRmsAvgVol)>>4;//Q0
					wTemp = ((INT32S)wTemp * 9459)>>14;
                    wTemp = dwReactiveRef/wTemp;
                    wAdjustStep = ((INT32S)wTemp * cSqrt2)>>14; 
                    
                    if(wAdjustStep > 256)
                    {
                    	wAdjustStep = 256;
                    }
                    else if(wAdjustStep < 8)
                    {
                    	wAdjustStep = 8;
                    }
                     
                	wReactiveCurrRefPreSat += wAdjustStep;
                }
                else
                {
                    wReactiveCurrRefPreSat = wReactiveCurrRefPreSat;
                }
    		}           
        }
        else
        {
        	wAdjustPF = 0;
            wReactiveCurrRefPreSat = 0;
        }
        
        if (wReactiveCurrRefPreSat > cAdjustPFLimitHigh)
        {
            wReactiveCurrRefPreSat = cAdjustPFLimitHigh;
        }
        else if (wReactiveCurrRefPreSat < cAdjustPFLimitLow)
        {
            wReactiveCurrRefPreSat = cAdjustPFLimitLow;
        }
        else
        {                
        }
        wAdjustPF = wReactiveCurrRefPreSat;
    }
}
/*********************************************************************
Function name:  void suwMidGridFreqExtract(void)
Description: 
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
INT16U suwMidGridFreqExtract(INT16U uwRGirdFreq,INT16U uwSGirdFreq,INT16U uwTGirdFreq,INT32S *pAngIngSub)
{
//static INT16U uwGirdFreqBak[3] = {0,0,0};
//static INT32S dwAngIngSubBak[2] = {0,0};
//
//    uwGirdFreqBak[0] = uwGirdFreqBak[1];
//    uwGirdFreqBak[1] = uwGirdFreqBak[2];
//    dwAngleIngSubReal = dwAngIngSubBak[0];
//    dwAngIngSubBak[0] = dwAngIngSubBak[1];
//    dwAngIngSubBak[1] = *pAngIngSub;
//    
//    if (((uwRGirdFreq >= uwSGirdFreq) && (uwRGirdFreq <= uwTGirdFreq)) || ((uwRGirdFreq >= uwTGirdFreq) && (uwRGirdFreq <= uwSGirdFreq)))
//	{
//		uwGirdFreqBak[2] = uwRGirdFreq;
//	}
//	else if (((uwSGirdFreq >= uwRGirdFreq) && (uwSGirdFreq <= uwTGirdFreq)) || ((uwSGirdFreq >= uwTGirdFreq) && (uwSGirdFreq <= uwRGirdFreq)))
//	{
//		uwGirdFreqBak[2] = uwSGirdFreq;
//	}
//	else
//	{
//        uwGirdFreqBak[2] = uwTGirdFreq;
//	}
//	if(fLVRTValue < uwLVRTActiveValue)    //
//    {
//    	uwGirdFreqBak[1] = uwGirdFreqBak[0];
//        uwGirdFreqBak[2] = uwGirdFreqBak[0];
//        dwAngIngSubBak[0] = dwAngleIngSubReal;
//        dwAngIngSubBak[1] = dwAngleIngSubReal;
//    }	
//	return(uwGirdFreqBak[0]);
static INT16U uwGirdFreqBak[4] = {0,0,0,0};
static INT32S dwAngIngSubBak[3] = {0,0,0};

    uwGirdFreqBak[0] = uwGirdFreqBak[1];
    uwGirdFreqBak[1] = uwGirdFreqBak[2];
    uwGirdFreqBak[2] = uwGirdFreqBak[3];
    uwGirdFreqBak[3] = ((INT32U)uwRGirdFreq + uwSGirdFreq + uwTGirdFreq)/3;
    
    dwAngleIngSubReal = dwAngIngSubBak[0];
    dwAngIngSubBak[0] = dwAngIngSubBak[1];   
    dwAngIngSubBak[1] = dwAngIngSubBak[2];
    dwAngIngSubBak[2] = *pAngIngSub;
        
	if(fLVRTValue < uwLVRTActiveValue)    //µ¯¢…˙ ±£¨”…”⁄µÕÕ®¬À≤®µƒ◊˜”√£¨ª· π∆µ¬ ≤˙…˙∆´“∆£¨“Ú¥À‘⁄µ¯¬‰∑¢…˙ ±Œ¨≥÷«∞20msµƒ∆µ¬ 
    {
    	uwGirdFreqBak[1] = uwGirdFreqBak[0];
        uwGirdFreqBak[2] = uwGirdFreqBak[0];
        uwGirdFreqBak[3] = uwGirdFreqBak[0];
        dwAngIngSubBak[0] = dwAngleIngSubReal;
        dwAngIngSubBak[1] = dwAngleIngSubReal;
        dwAngIngSubBak[2] = dwAngleIngSubReal;
    }	
	return(uwGirdFreqBak[0]);
}
/*********************************************************************
Function name:  INT16S swIqRefExtract(void)
Description: 
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
INT16S swIqRefExtract(T_THREE_PHASE_DQO *ptInvCurrRefDqo)
{
static INT16S wIqRefBak[3] = {0,0,0};

    wIqRefBak[0] = wIqRefBak[1];
    wIqRefBak[1] = wIqRefBak[2];
    wIqRefBak[2] = ptInvCurrRefDqo->q;
    
    if((fLVRTValue <= uwSoftLVRTValue)||(fHVRTValue >= uwSoftHVRTValue)||(fLVRTStartOrEnd == 1))
    {
    	wIqRefBak[1] = wIqRefBak[0];
        wIqRefBak[2] = wIqRefBak[0];
    }
        	
	return(wIqRefBak[0]);    	
}
/*********************************************************************
Function name:INT16S swRmsCurrExtract(T_THREE_PHASE_ABC *ptRmsCurr)
Description: 
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
INT16S swRmsCurrExtract(T_THREE_PHASE_ABC *ptRmsCurr)
{
static INT16S wRmsCurrBak[3] = {0,0,0};
INT16S wRmsCurrTmp;

    wRmsCurrTmp = ((INT64S)((INT32S)ptRmsCurr->a + ptRmsCurr->b + ptRmsCurr->c) * 15447)>>15;//sqrt(2)/3 Q15
    wRmsCurrTmp = ((INT32S)wRmsCurrTmp * wRatedInvCurrInv)>>13;
    
    wRmsCurrBak[0] = wRmsCurrBak[1];
    wRmsCurrBak[1] = wRmsCurrBak[2];
    wRmsCurrBak[2] = wRmsCurrTmp;
    
    if((fLVRTValue <= uwSoftLVRTValue)||(fHVRTValue >= uwSoftHVRTValue)||(fLVRTStartOrEnd == 1))
    {
    	wRmsCurrBak[1] = wRmsCurrBak[0];
        wRmsCurrBak[2] = wRmsCurrBak[0];
    }
        	
	return(wRmsCurrBak[0]);    	
}
/*********************************************************************
Function name:INT16U suMaxOutputCurrCal(INT16S wEnvTemp, INT16S wHeatTemp, INT16S wBusVoltage,INT16S wMaxRmsCurr)
Description: 
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
INT16U suMaxOutputCurrCal(INT16S wEnvTemp, INT16S wHeatTemp, INT16S wBusVoltage,INT16S wMaxRmsCurr)
{
INT16U uwpuCurrMax = 16384;
INT16U uwpuCurrOut = 16384;
static INT16U uwpuEnvCurrOut = 18022;
static INT16U uwpuHeatCurrOut = 18022;
static INT16U uwpuI2TCurOut = 18022;
static INT16U uwpuBusVolCutCurOut = 18022;
INT16S wCurrdiff;
INT32S dwCurrSqure;
static INT16U uwpuCurrPre = 16384;
//static INT16U uwDelay1s = 0;

    //==========================OVCurr config =============================        
    uwpuCurrMax = 16384;
    if(wEepromCurrOVEn == 1)
    {
    	uwpuCurrMax = 18022;
    }
   
    //==========================Œ¬∂»π˝∏ﬂ“˝∆µƒ◊Ó¥Û ”‘⁄π¶¬ ±‰ªØ============================
    if(wEnvTemp > cTemp73C)
	{
	    wEnvTemp = cTemp73C;
	}
	
	if(wHeatTemp > cTemp93C)
    {
    	wHeatTemp = cTemp93C;
    }
    
    if(wBusVoltage > cVolt910V)
    {
    	wBusVoltage = cVolt910V;
    }
    	
	if(wHeatTemp >= cTemp78C)
	{
	    setbit(fDerating,0);
	    uwpuHeatCurrOut = 16384 - (((INT32U)cHeatDecPowercoef * (wHeatTemp - cTemp78C))>>4);
	}
	else if(wHeatTemp <= cTemp75C)
	{
        uwpuHeatCurrOut = 18022;
        clrbit(fDerating,0);
	}
	else
	{
		uwpuHeatCurrOut = 18022 - (((INT32U)546 * (wHeatTemp - cTemp75C))>>4);
	}
	
	if(wEnvTemp > cTemp58C)
	{
		setbit(fDerating,1);
	    uwpuEnvCurrOut = 16384 - (((INT32U)cEnvDecPowercoef * (wEnvTemp - cTemp58C))>>4);    
	}	
	else if(wEnvTemp <= cTemp55C)
	{
		uwpuEnvCurrOut = 18022;
	    clrbit(fDerating,1);
	}
	else
	{
		uwpuEnvCurrOut = 18022 - (((INT32U)546 * (wEnvTemp - cTemp55C))>>4);
	}
	
	if(wBusVoltage > cVolt810V)
	{
		uwpuBusVolCutCurOut = 134349 - ((INT32S)wBusVoltage * 18641>>11);
		if(uwpuBusVolCutCurOut < 3277)
		{
			uwpuBusVolCutCurOut = 3277;
		}
		setbit(fDerating,2);
	}
	else if(wBusVoltage <= cVolt800V)
	{
		uwpuBusVolCutCurOut = 18022;
		clrbit(fDerating,2);
	}
	else
	{
		uwpuBusVolCutCurOut = 149062 - ((INT32S)wBusVoltage * 20966>>11);
	}
			
	if(uwpuEnvCurrOut < uwpuCurrMax)
	{
	    uwpuCurrOut = uwpuEnvCurrOut;
	}
	else
	{
		uwpuCurrOut = uwpuCurrMax;
	}
	
	if(uwpuHeatCurrOut < uwpuCurrOut)
	{
	    uwpuCurrOut = uwpuHeatCurrOut;
	} 
	 
	if(uwpuBusVolCutCurOut < uwpuCurrOut)
	{
	    uwpuCurrOut = uwpuBusVolCutCurOut;
	}   			
	//==================================I2t≥¨œﬁ“˝∆µƒΩµ∂Ó================================
	wCurrdiff = ((INT32S)wMaxRmsCurr<<14)/wRatedRmsCurr;		
    wCurrdiff = wCurrdiff - 17203;
    wCurrdiff = 0;
    
    if(wCurrdiff > 0)
    {
        dwCurrSqure = ((INT32S)wCurrdiff * wCurrdiff)>>4;        
        if(dwI2TValue >= cI2TValueMax)//I2t ≥¨œﬁ£¨œﬁ÷∆ ‰≥ˆµÁ¡˜µΩ∂Ó∂®
        {
            uwpuI2TCurOut = 16384;//œﬁ÷∆∆‰µΩ∂Ó∂®
            setbit(fDerating,3);
        }
        else
        {
            dwI2TValue += dwCurrSqure;
        }        
    }
    else if(wCurrdiff < 0)
    {
        wCurrdiff = -wCurrdiff;        
        dwCurrSqure = ((INT32S)wCurrdiff * wCurrdiff)>>4;
        if((dwI2TValue <= 0)||(fInvEnOut == 0))  //60∑÷÷”∫Û,‘Ÿ¥Œ‘ –Ìπ˝‘ÿ
        {
            uwpuI2TCurOut = 18022;
            dwI2TValue = 0;
            clrbit(fDerating,3);
        }
        else
        {
            dwI2TValue -= dwCurrSqure;
        }
    }
    
    if(uwpuI2TCurOut < uwpuCurrOut)
    {
        uwpuCurrOut = uwpuI2TCurOut;
    }
    
    uwpuCurrOut = ((INT32U)uwpuCurrPre * cLPFilterDen + (INT32U)uwpuCurrOut * cLPFilterNum)>>15;
	uwpuCurrPre = uwpuCurrOut;
    
    //uwpuCurrOut = ((INT32U)uwpuCurrOut * wRatedInvCurrPeak)>>14;// µº ‘ –Ìµƒ◊Ó¥Û ‰≥ˆµÁ¡˜
		
	return(uwpuCurrOut);
}
/*********************************************************************
Function name:void sCalIdLimit(void)
Description: 
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalIdLimit(void)
{
INT16U uwTemp,uwpuGridI;
INT16S wTemp,wPRef,wIdLimitMax;
INT32S dwDCPowerTmp,dwIdLimtAim,dwIdLimStep,dwDCCurrStep,dwTemp;
INT16U uwpuActivePowerMax = 16384;

    //========================Cal Max ApparentPower======================== 
    if(wpuGridLineVol > 0)
    {
        uwpuApparentPowerMax = ((INT32U)wpuGridLineVol * uwCurrMax)>>14;
    }
    else
    {
    	uwpuApparentPowerMax = uwCurrMax;
    }
          
    if(wEepromPowerOVEn == 1)
    {
    	if(uwpuApparentPowerMax > 18022)
    	{
    		uwpuApparentPowerMax = 18022;
    	}
    }
    else
    {
    	if(uwpuApparentPowerMax > 16384)
    	{
    		uwpuApparentPowerMax = 16384;
    	}
    }
    
    if(fPQPriority == 0)
    {
        if(uwpuActivePowerMax > uwpuApparentPowerMax)
        {
        	uwpuActivePowerMax = uwpuApparentPowerMax;
        }        
        //---------------------------------------Cal Max ActivePower----------------------------------- 
        if(fActiveMode == 0)
        {
        	if(wActivePowerPer > (INT16S)uwpuActivePowerMax)
        	{
        		wPRef = uwpuActivePowerMax;
        	}
        	else if(wActivePowerPer < -((INT16S)uwpuActivePowerMax))
        	{
        		wPRef = -((INT16S)uwpuActivePowerMax);
        	}
        	else
        	{
        		wPRef = wActivePowerPer;    		
        	}
        	
        	dwDCPowerTmp = ((INT32S)wDCCurrAim * wBatteryInAvgVol)>>4;//Q8
        	dwDCPowerTmp = (dwDCPowerTmp<<2)/dwRatedPower;//Q10
        	
        	if(dwDCPowerTmp > 25000)
        	{
        	    dwDCPowerTmp = 25000;
        	}
        	else if(dwDCPowerTmp < -25000)
        	{
        	    dwDCPowerTmp = -25000;
        	}
        	
        	if(wDCCurrAim > 0)
        	{
        	    dwDCPowerTmp = (dwDCPowerTmp * cPowerEff)>>8;//Q14  
        	    if(wPRef > dwDCPowerTmp)
        	    {
        	        wPRef = dwDCPowerTmp;
        	    }	        	    
        	}
        	else if(wDCCurrAim < 0)
        	{
        		dwDCPowerTmp = (dwDCPowerTmp * cPowerEffI)>>8;//Q14
        		if(wPRef < dwDCPowerTmp)
        	    {
        	        wPRef = dwDCPowerTmp;
        	    } 		    		
        	}
        	else
        	{
        	    wPRef = 0;
        	}
        	
        	uwpuReactivePowerMax = sqrt((INT32U)uwpuApparentPowerMax * uwpuApparentPowerMax - (INT32S)wPRef * wPRef);
        	dwActivePowerAim = ((INT64S)dwRatedPower * wPRef)>>10;//Q4
        }
        else if(fActiveMode == 1)
        {
        	dwDCPowerTmp = ((INT32S)wDCCurrAim * wBatteryInAvgVol)>>4;//Q8
        	dwDCPowerTmp = (dwDCPowerTmp<<2)/dwRatedPower;//Q10
        	
        	if(wDCCurrAim >= 0)
        	{
        	    dwDCPowerTmp = (dwDCPowerTmp * cPowerEff)>>8;//Q14
        	}
        	else
        	{
        		dwDCPowerTmp = (dwDCPowerTmp * cPowerEffI)>>8;//Q14
        	}
        	
        	if(dwDCPowerTmp > (INT16S)uwpuActivePowerMax)
        	{
        		wPRef = uwpuActivePowerMax;
        	}
        	else if(dwDCPowerTmp < -((INT16S)uwpuActivePowerMax))
        	{
        	    wPRef = -((INT16S)uwpuActivePowerMax);
        	}
        	else
        	{
        		wPRef = (INT16S)dwDCPowerTmp;    		
        	}
        	
        	if(wPRef < -10923)
        	{
        	    wPRef = -10923;
        	}
        	uwpuReactivePowerMax = sqrt((INT32U)uwpuApparentPowerMax * uwpuApparentPowerMax - (INT32S)wPRef * wPRef);
        	dwActivePowerAim = 0;
        }
    }
    else
    {
        if(fReactiveMode == 0)
        {
            uwpuReactivePowerMax = 9830;
        	uwTemp = abs(wPFSet);  
            uwpuActivePowerMax = ((INT32U)uwpuApparentPowerMax * uwTemp)>>14;        
        }
        else
        {  	
            uwpuReactivePowerMax = 9830;
            if(uwpuReactivePowerMax > uwpuApparentPowerMax)
            {
                uwpuReactivePowerMax = uwpuApparentPowerMax;
            }
                
        	if(abs(wReactivePowerPer) < uwpuApparentPowerMax)
        	{
        		uwpuActivePowerMax = sqrt((INT32U)uwpuApparentPowerMax * uwpuApparentPowerMax - (INT32S)wReactivePowerPer * wReactivePowerPer);
        	    
        	    if(uwpuActivePowerMax < 327)
        	    {
        	    	uwpuActivePowerMax = 327;
        	    }
        	}
        	else
        	{
        		if(wReactivePowerPer >= 0)
        	    {
        	        wReactivePowerPer = uwpuApparentPowerMax;
        	    }
        	    else
        	    {
        	    	wReactivePowerPer = - ((INT16S)uwpuApparentPowerMax);
        	    }
        	    uwpuActivePowerMax = 327;
        	}
        }
        
        if(fActiveMode == 0)
        {
        	if(wActivePowerPer > (INT16S)uwpuActivePowerMax)
        	{
        		wPRef = uwpuActivePowerMax;
        	}
        	else if(wActivePowerPer < -((INT16S)uwpuActivePowerMax))
        	{
        		wPRef = -((INT16S)uwpuActivePowerMax);
        	}
        	else
        	{
        		wPRef = wActivePowerPer;    		
        	}
        	
        	dwDCPowerTmp = ((INT32S)wDCCurrAim * wBatteryInAvgVol)>>4;//Q8
        	dwDCPowerTmp = (dwDCPowerTmp<<2)/dwRatedPower;//Q10
        	
        	if(dwDCPowerTmp > 25000)
        	{
        	    dwDCPowerTmp = 25000;
        	}
        	else if(dwDCPowerTmp < -25000)
        	{
        	    dwDCPowerTmp = -25000;
        	}
        	
        	if(wDCCurrAim > 0)
        	{
        	    dwDCPowerTmp = (dwDCPowerTmp * cPowerEff)>>8;//Q14  
        	    if(wPRef > dwDCPowerTmp)
        	    {
        	        wPRef = dwDCPowerTmp;
        	    }	        	    
        	}
        	else if(wDCCurrAim < 0)
        	{
        		dwDCPowerTmp = (dwDCPowerTmp * cPowerEffI)>>8;//Q14
        		if(wPRef < dwDCPowerTmp)
        	    {
        	        wPRef = dwDCPowerTmp;
        	    } 		    		
        	}
        	else
        	{
        	    wPRef = 0;
        	}
        	
        	dwActivePowerAim = ((INT64S)dwRatedPower * wPRef)>>10;//Q4
        }
        else if(fActiveMode == 1)
        {
        	dwDCPowerTmp = ((INT32S)wDCCurrAim * wBatteryInAvgVol)>>4;//Q8
        	dwDCPowerTmp = (dwDCPowerTmp<<2)/dwRatedPower;//Q10
        	
        	if(wDCCurrAim >= 0)
        	{
        	    dwDCPowerTmp = (dwDCPowerTmp * cPowerEff)>>8;//Q14
        	}
        	else
        	{
        		dwDCPowerTmp = (dwDCPowerTmp * cPowerEffI)>>8;//Q14
        	}
        	
        	if(dwDCPowerTmp > (INT16S)uwpuActivePowerMax)
        	{
        		wPRef = uwpuActivePowerMax;
        	}
        	else if(dwDCPowerTmp < -((INT16S)uwpuActivePowerMax))
        	{
        	    wPRef = -((INT16S)uwpuActivePowerMax);
        	}
        	else
        	{
        		wPRef = (INT16S)dwDCPowerTmp;    		
        	}
        	
        	if(wPRef < -10923)
        	{
        	    wPRef = -10923;
        	}
        	dwActivePowerAim = 0;
        }
    }

    if((uwSysMode == InvRunMode)&&(fInvEnOut == 1)&&(fCSVSSwitch == 0))
    {
        dwTemp = (INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol;
        
        if(dwTemp < (((INT32S)wRatedGridLineVol * 10445)>>12))
        {
            dwTemp = ((INT32S)wRatedGridLineVol * 10445)>>12;
        }
       
        uwpuGridI = ((INT32U)wRatedGridLineVol * 12288)/dwTemp;//µÁÕ¯±ÍÁ€÷µµƒµπ ˝
        wPRef = ((INT32S)wPRef * uwpuGridI)>>12;

        if(wPRef >= (INT16S)uwCurrMax)
        {
        	wPRef = (INT16S)uwCurrMax;
        }
        else if(wPRef <= -((INT16S)uwCurrMax))
        {
        	wPRef = -((INT16S)uwCurrMax);
        }
        
        wIdLimitMax = ((INT32S)wPRef * wRatedInvCurrPeak)>>14;
        
        if(wIdLimitMax > 0)
        {
            wIdLimitMax = wIdLimitMax + (((INT32S)tInvCurrDqoRef.q * 1144)>>15);
            
            if(wIdLimitMax <= 0)
            {
                wIdLimitMax = 1;
            }
        }
        else if(wIdLimitMax < 0)
        {
            wIdLimitMax = wIdLimitMax + (((INT32S)tInvCurrDqoRef.q * 1144)>>15);
            
            if(wIdLimitMax >= 0)
            {
                wIdLimitMax = -1;
            }
        }
        else
        {
            wIdLimitMax = 0;
        }
        //wIdLimitMax = wIdLimitMax + (((INT32S)tInvCurrDqoRef.q * 1144)>>15);//2°„ PLL Compensate

        if(abs(wIdLimitMax) < wInvCurrResolving)
        {
        	if(wIdLimitMax > 0)
        	{
        		wIdLimitMax = wInvCurrResolving;
        	}
        	else if(wIdLimitMax < 0)
        	{
        		wIdLimitMax = -wInvCurrResolving;
        	}
        	else
        	{
        	    if(fCharge == 1)
        	    {
        	        wIdLimitMax = - wInvCurrResolving;
        	    }
        	    else
        	    {
        	        wIdLimitMax = wInvCurrResolving;
        	    }
        	}
        }
        
        wIdLimitMax = -wIdLimitMax;
        wPowerDeratingIdmax = abs(wIdLimitMax);
        
        if(fPStepSwitch == 1)
        {
            uwTemp = 10000;
        }
        else if(fPStepSwitch == 2)
        {
            uwTemp = 10000;
        }
        else
        {
            uwTemp = wEepromPActStep;
        }
        
        ++uwFVRTCurrSoftDelay;
        if(uwFVRTCurrSoftDelay < 300)
        {
        	uwTemp = wEepromFVRTPowerStep;
        }
        else
        {
        	uwFVRTCurrSoftDelay = 300;
        }
                
        if(fCharge == 1)//Charge Model ≥‰µÁƒ£ Ω
        {                                               
            wInvDsCurrLimitLow = -wRatedInvCurrPeak;
			dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
			
	        if(uwTemp <= 6000)
	        {
	        	wTemp = (((INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol)>>4); 
	            if(wTemp > 300)
	            {
	                dwIdLimStep = ((INT32U)128423 * uwTemp)/wTemp;// 3*10*sqrt(2)*256*1024/sqrt(3)/50
	            }
	            else
	            {
	                dwIdLimStep = ((INT32U)128423 * uwTemp)/300;
	            }
	            
	            if(wBatteryInAvgVol > 3200)
	            {
	                dwDCCurrStep = (INT32U)52429 * uwTemp/wBatteryInAvgVol;	//Q14
	            }
	            else
	            {
	                dwDCCurrStep = (INT32U)52429 * uwTemp/3200;	
	            }
	                    	
	        	if((wInvDsCurrLimitHigh != (-tInvCurrDqoRef.d))&&(wIdLimitMax <= (-tInvCurrDqoRef.d))&&(tInvCurrDqoRef.d < 0))
	            {	            	
	            	wInvDsCurrLimitHigh = abs(tInvCurrDqoRef.d);
					dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;		                     	
	            }
	            	                
                dwIdLimtAim = (INT32S)wIdLimitMax<<10;
                                                
                if(dwIdLimitHigh > (dwIdLimtAim + dwIdLimStep))
                {
                    dwIdLimitHigh = dwIdLimitHigh - dwIdLimStep;
                }
                else if(dwIdLimitHigh < (dwIdLimtAim - dwIdLimStep))
                {
                    dwIdLimitHigh = dwIdLimitHigh + dwIdLimStep;
                }
                else
                {
                    dwIdLimitHigh = dwIdLimtAim;               
                }
                
                wTemp = dwIdLimitHigh>>10;
                
                if(wTemp <= wInvCurrResolving)
                {
                	wInvDsCurrLimitHigh = wInvCurrResolving;
                	dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
                	
                	if(wIdLimitMax < 0)
                	{
                		wInvDsCurrLimitLow = -wInvCurrResolving;
			            dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
                		if(fDCModeSwitch == 0)
                		{
                			if(wBatVolLimtLow > wStdBusVolRef)
            			    {
            			    	wInvCtrBusVolRef = wBatVolLimtLow;
            			    }
            			    else
            			    {
            			    	wInvCtrBusVolRef = wStdBusVolRef;
            			    }
                		}
                		else
                		{
                			wInvCtrBusVolRef = wDCCtrBusVolRef - cVolt20V;//cVolt10V
                		}
                		   
                		if(wDCCurrAim > 0)
                		{             		
                		    fChargeToDisCharge = 1;                     		
                		    wDCCurrSet = wDCCurrAim;
                		    wDCCurrAimBak = wDCCurrAim;
						    dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                		    fCharge = 0;
                		}
                	}
                }
                else
                {
                	wInvDsCurrLimitHigh = wTemp;                    	
                	if(wDCCurrAimBak != wDCCurrAim)
                    {
                        if((wDCCurrAimBak >= 0)&&(wDCCurrAim >= 0))
                        {
                            if(wDCCurrAimBak >= wDCCurrAim)
                            {                                
                                dwDCCurrSetIng = dwDCCurrSetIng - dwDCCurrStep;
                                
                                if(dwDCCurrSetIng <= ((INT32S)wDCCurrAim<<8))
                                {
                                    wDCCurrSet = wDCCurrAim;
                                    wDCCurrAimBak = wDCCurrAim;
                                    dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                                }
                                else
                                {
                                    wDCCurrSet = dwDCCurrSetIng>>8;
                                }                                
                            }
                            else
                            {
                                wDCCurrSet = wDCCurrAim;
                                wDCCurrAimBak = wDCCurrAim;
								dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                            }
                        }
                        else if((wDCCurrAimBak <= 0)&&(wDCCurrAim < 0))
                        {
                            if(wDCCurrAimBak <= wDCCurrAim)
                            {
                                dwDCCurrSetIng = dwDCCurrSetIng + dwDCCurrStep;
                                if(dwDCCurrSetIng >= ((INT32S)wDCCurrAim<<8))
                                {
                                    wDCCurrSet = wDCCurrAim;
                                    wDCCurrAimBak = wDCCurrAim;
                                    dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                                }
                                else
                                {
                                    wDCCurrSet = dwDCCurrSetIng>>8;
                                }
                            }
                            else
                            {
                                wDCCurrSet = wDCCurrAim;
                                wDCCurrAimBak = wDCCurrAim;
                                dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                            }
                        }
                    }  
                }                                   	        	
	        }
	        else
	        {	        	
	        	if(wIdLimitMax < 0)
	        	{
	        		wInvDsCurrLimitHigh = wInvCurrResolving;
	        		dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
	        		
	        		wInvDsCurrLimitLow = -wInvCurrResolving;
			        dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
			        
	        		if(fDCModeSwitch == 0)
                	{
                		if(wBatVolLimtLow > wStdBusVolRef)
            			{
            				wInvCtrBusVolRef = wBatVolLimtLow;
            			}
            			else
            			{
            				wInvCtrBusVolRef = wStdBusVolRef;
            			}
                	}
                	else
                	{
                		wInvCtrBusVolRef = wDCCtrBusVolRef - cVolt20V;//cVolt10V
                	}	        		                    	
                	fChargeToDisCharge = 1;
                	fCharge = 0;
	        	} 
	        	else
	        	{
	        	    wInvDsCurrLimitHigh = wIdLimitMax;
	        	    dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10; 	        	    
	        	    //fChargeToDisCharge = 0;
	        	}
	        	wDCCurrSet = wDCCurrAim;
	        	wDCCurrAimBak = wDCCurrAim; 
	        	dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;                   
	        }
	        
	        if((uwTemp >= 4000)&&(uwFVRTCurrSoftDelay >= 300))
	        {
	        	dwDeltaDCCurr = wInvDsCurrLimitHighBak - wInvDsCurrLimitHigh;
	        	wInvDsCurrLimitHighBak = wInvDsCurrLimitHigh;
	        	dwDeltaDsCurr = -dwDeltaDCCurr<<12;
	        	if(labs(dwDeltaDCCurr) > 300)
	        	{
	        		dwDeltaDCCurr = dwDeltaDCCurr * wUdDivBatCoeff;//Q12
	        		fDCVoltCtrDeal = 1;	        		
	        		fInvCtrDeal = 1;
	        	} 
	        }
	        else
	        {
	        	wInvDsCurrLimitHighBak = wInvDsCurrLimitHigh;
	        }
	        wInvDsCurrLimitLowBak = wInvDsCurrLimitLow;	            
        }
        else
        {            
            wInvDsCurrLimitHigh = ((INT32S)wRatedInvCurrPeak * 10922)>>14;//2/3 = 66%
			dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
			         
	        if(uwTemp <= 6000)
	        {
	        	wTemp = (((INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol)>>4); 
	            if(wTemp > 300)
	            {
	                dwIdLimStep = ((INT32U)128423 * uwTemp)/wTemp;// 10*sqrt(2)*256*1024*sqrt(3)/100
	            }
	            else
	            {
	                dwIdLimStep = ((INT32U)128423 * uwTemp)/300;
	            }
	            
	            if(wBatteryInAvgVol > 3200)
	            {
	                dwDCCurrStep = (INT32U)52429 * uwTemp/wBatteryInAvgVol;	//Q14
	            }
	            else
	            {
	                dwDCCurrStep = (INT32U)52429 * uwTemp/3200;	
	            }
	            
	        	if((wInvDsCurrLimitLow != (-tInvCurrDqoRef.d))&&(wIdLimitMax >= (-tInvCurrDqoRef.d))&&(tInvCurrDqoRef.d > 0))
	            {	            	
	            	wInvDsCurrLimitLow = - abs(tInvCurrDqoRef.d);
					dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;		                     	
	            }
	            
                dwIdLimtAim = (INT32S)wIdLimitMax<<10;
                
                if(dwIdLimitLow > (dwIdLimtAim + dwIdLimStep))
                {
                    dwIdLimitLow = dwIdLimitLow - dwIdLimStep;
                }
                else if(dwIdLimitLow < (dwIdLimtAim - dwIdLimStep))
                {
                    dwIdLimitLow = dwIdLimitLow + dwIdLimStep;
                }
                else
                {
                    dwIdLimitLow = dwIdLimtAim;                 
                }
                
                wTemp = dwIdLimitLow>>10;
                
                if(wTemp >= -wInvCurrResolving)
                {
                	wInvDsCurrLimitLow = -wInvCurrResolving;
                	dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
                	
                	if(wIdLimitMax > 0)
                	{
                		wInvDsCurrLimitHigh = wInvCurrResolving;
			            dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
			            
			            if(fDCModeSwitch == 0)
                	    {
                	    	wInvCtrBusVolRef = wBatVolLimtHigh;
                	    }
                	    else
                	    {
                	    	wInvCtrBusVolRef = wDCCtrBusVolRef + cVolt20V;//cVolt10V
                	    }
                 		
                 		if(wDCCurrAim < 0)
                 		{
                		    fDisChargeToCharge = 1;
                		    wDCCurrSet = wDCCurrAim;
                		    wDCCurrAimBak = wDCCurrAim;
                		    dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                		    fCharge = 1;
                		}
                	}
                }
                else
                {
                	wInvDsCurrLimitLow = wTemp; 
                	if(wDCCurrAimBak != wDCCurrAim)
                    {
                        if((wDCCurrAimBak >= 0)&&(wDCCurrAim >= 0))
                        {
                            if(wDCCurrAimBak >= wDCCurrAim)
                            {                                
                                dwDCCurrSetIng = dwDCCurrSetIng - dwDCCurrStep;
                                
                                if(dwDCCurrSetIng <= ((INT32S)wDCCurrAim<<8))
                                {
                                    wDCCurrSet = wDCCurrAim;
                                    wDCCurrAimBak = wDCCurrAim;
                                    dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                                }
                                else
                                {
                                    wDCCurrSet = dwDCCurrSetIng>>8;
                                }                                
                            }
                            else
                            {
                                wDCCurrSet = wDCCurrAim;
                                wDCCurrAimBak = wDCCurrAim;
								dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                            }
                        }
                        else if((wDCCurrAimBak <= 0)&&(wDCCurrAim < 0))
                        {
                            if(wDCCurrAimBak <= wDCCurrAim)
                            {
                                dwDCCurrSetIng = dwDCCurrSetIng + dwDCCurrStep;
                                if(dwDCCurrSetIng >= ((INT32S)wDCCurrAim<<8))
                                {
                                    wDCCurrSet = wDCCurrAim;
                                    wDCCurrAimBak = wDCCurrAim;
                                    dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                                }
                                else
                                {
                                    wDCCurrSet = dwDCCurrSetIng>>8;
                                }
                            }
                            else
                            {
                                wDCCurrSet = wDCCurrAim;
                                wDCCurrAimBak = wDCCurrAim;
                                dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
                            }
                        }
                    } 
                }
            }
            else
            {                 
            	if(wIdLimitMax > 0)
	        	{
	        		wInvDsCurrLimitLow = -wInvCurrResolving;
	        		dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
	        		wInvDsCurrLimitHigh = wInvCurrResolving;
			        dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;//2014-02-11
			            
	        		if(fDCModeSwitch == 0)
                	{
                		wInvCtrBusVolRef = wBatVolLimtHigh;
                	}
                	else
                	{
                		wInvCtrBusVolRef = wDCCtrBusVolRef + cVolt20V;//cVolt10V
                	}	            			        		
                	fDisChargeToCharge = 1;
                	fCharge = 1;
	        	} 
	        	else
	        	{
	        	    wInvDsCurrLimitLow = wIdLimitMax;
	        	    dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10; 
	        	    //fDisChargeToCharge = 0;
	        	} 
	        	wDCCurrSet = wDCCurrAim;
	        	wDCCurrAimBak = wDCCurrAim;
	        	dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;                
            }
            
            if((uwTemp >= 4000)&&(uwFVRTCurrSoftDelay >= 300))
	        {
	        	dwDeltaDCCurr = wInvDsCurrLimitLowBak - wInvDsCurrLimitLow;
	        	wInvDsCurrLimitLowBak = wInvDsCurrLimitLow;
	        	dwDeltaDsCurr = -dwDeltaDCCurr<<12;
	        	if(labs(dwDeltaDCCurr) > 300)
	        	{	        		
	        		dwDeltaDCCurr = dwDeltaDCCurr * wUdDivBatCoeff;//Q12
	        		fDCVoltCtrDeal = 1;	        		
	        		fInvCtrDeal = 1;        		
	        	}
	        }
	        else
	        {
	        	wInvDsCurrLimitLowBak = wInvDsCurrLimitLow;
	        }
	        wInvDsCurrLimitHighBak = wInvDsCurrLimitHigh;
        }
    }
    else if(fCSVSSwitch == 0)
    {
        wDCCurrAimBak = wDCCurrAim;
        wDCCurrSet = wDCCurrAim;
        dwDCCurrSetIng = (INT32S)wDCCurrSet<<8;
        
        if(fStartByDCOrAC == 1)
        {
        	fCharge = 1;
        }
        else
        {
        	if(fActiveMode == 0)
    	    {
    	    	if(wActivePowerPer >= 0)
    	    	{
                    fCharge = 0;
    	    	}
    	    	else
    	    	{
    	    		fCharge = 1;
    	    	}   		
    	    }
    	    else
    	    {
    	    	if(wDCCurrSet >= 0)
    	    	{
                    fCharge = 0;
    	    	}
    	    	else
    	    	{
    	    		fCharge = 1;
    	    	}   		
    	    }
        }
        
        if(fCharge == 1)
        {
        	wInvDsCurrLimitHigh = wInvCurrResolving;
    	    wInvDsCurrLimitLow = -13064;
        }
        else
        {
        	wInvDsCurrLimitHigh = 8709;
    	    wInvDsCurrLimitLow = -wInvCurrResolving;
        }
            	
		wInvDsCurrLimitHighBak = wInvDsCurrLimitHigh;
        wInvDsCurrLimitLowBak = wInvDsCurrLimitLow;

    	dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
    	dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10; 
    	dwDeltaDCCurr = 0; 
    	fDCVoltCtrDeal = 0; 
    	fInvCtrDeal = 0;
    	dwDeltaDsCurr = 0;	
    }   
}
/*********************************************************************
Function name:void sCalIqRef(void)
Description: 
Calls:      	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalIqRef(void)
{ 
INT32S dwTemp,dwIqStep,dwIqAim;
INT16S wTemp,wInvCurrQRefAim;
INT16U uwTemp;
static INT32S dwIqRef = 0;
    
    if(fReactiveMode == 0)//—°‘ÒPFƒ£ Ω
    {   
        if(fPQPriority == 0)
        {   
             wTemp = abs(tInvCurrDqoRef.d);       
             dwTemp = (INT32S)wTemp<<14;
             wTemp = ((INT32U)wRatedInvCurrPeak * uwCurrMax)>>14;
             dwTemp = dwTemp/wTemp;
             if(labs(dwTemp) > 16384)
             {
             	uwPFSetMax = 16384;
             }
             else
             {
             	uwPFSetMax = labs(dwTemp);
             }
             
             if(abs(wPFSet) < uwPFSetMax)
             {        
                 if(wPFSet >= 0)
                 {
                 	 wPFSet = uwPFSetMax;
                 }
                 else
                 {
                 	 wPFSet = -((INT16S)uwPFSetMax);
                 }    
             }
        }
     
        dwTemp= (INT32S)wPFSet*wPFSet;
        wTemp = sqrt(cPFThousandSquare - dwTemp);
        wTemp = ((INT32S)wTemp<<10)/wPFSet; //Q10
        
        if(tInvCurrDqoRef.d >= 0)
        {
            wInvCurrQRefAim = - (((INT32S)wTemp * tInvCurrDqoRef.d)>>10);
        }
        else
        {
            wInvCurrQRefAim = (((INT32S)wTemp * tInvCurrDqoRef.d)>>10);
        }
        dwReactivePowerAim = 0;    
    }
    else
    {
        if(wReactivePowerPer > (INT16S)uwpuReactivePowerMax)
        {
        	wReactivePowerPer = (INT16S)uwpuReactivePowerMax;
        }
        else if(wReactivePowerPer < -((INT16S)uwpuReactivePowerMax))
        {
        	wReactivePowerPer = -((INT16S)uwpuReactivePowerMax);
        }
             
        dwReactivePowerAim = ((INT64S)dwRatedPower * wReactivePowerPer)>>6;//Q8     
        wTemp = ((INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol)>>4;   
        wTemp = ((INT32S)wTemp * 9459)>>14;   
        wTemp = dwReactivePowerAim/wTemp;
        
        wInvCurrQRefAim = -(((INT32S)wTemp * cSqrt2)>>14);
    }    
    
    if((fInvEnOut == 1)&&(fLVRTValue >= uwLVRTActiveValue)&&(fHVRTValue <= uwHVRTActiveValue)&&(fCSVSSwitch == 0))
    {
        if(fQStepSwitch == 1)
        {
            uwTemp = 10000;
        }
    	else
        {
            uwTemp = wEepromQActStep;
        }
        	
        if(uwTemp <= 6000)
	    {
	    	wTemp = (((INT32S)wRSGridRmsAvgVol + wSTGridRmsAvgVol + wTRGridRmsAvgVol)>>4); 
	        if(wTemp > 300)
	        {
	            dwIqStep = ((INT32U)128423 * uwTemp)/wTemp;// 10*sqrt(2)*256*1024*sqrt(3)/100
	        }
	        else
	        {
	            dwIqStep = ((INT32U)128423 * uwTemp)/300;
	        }
	        
            dwIqAim = (INT32S)wInvCurrQRefAim<<10;
            
            if(dwIqRef > (dwIqAim + dwIqStep))
            {
                dwIqRef = dwIqRef - dwIqStep;
                fAdjustQEn = 0;
            }
            else if(dwIqRef < (dwIqAim - dwIqStep))
            {
                dwIqRef = dwIqRef + dwIqStep;
                fAdjustQEn = 0;
            }
            else
            {
                dwIqRef = dwIqAim;
                fAdjustQEn = 1;
            }
            
            tInvCurrPowerReguRefDqo.q = dwIqRef>>10;               	        	
	    }
	    else
	    {
	    	tInvCurrPowerReguRefDqo.q = wInvCurrQRefAim;
	    	dwIqRef = (INT32S)tInvCurrPowerReguRefDqo.q<<10;
	    	fAdjustQEn = 1;
	    }
    }
    else
    {
    	tInvCurrPowerReguRefDqo.q = 0;
    	dwIqRef = (INT32S)tInvCurrPowerReguRefDqo.q<<10;
    	fAdjustQEn = 0;
    }   
    wDebugIqStep = dwIqStep>>4;
}
/********************************************************************* 
Function name:void sSysSleepChk(void)								  
Description:  Check power status
Calls:																   
Called By:	sSysMode(void)											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sSysSleepChk(void)
{
static INT16U uwCSSleepDelay = 0;

	if((((wActivePowerPer == 0)&&(fActiveMode == 0))||((wDCCurrAim == 0)&&(fActiveMode == 1)))
	  &&(((wReactivePowerPer == 0)&&(fReactiveMode == 1))||(fReactiveMode == 0))
	  &&(fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))
	{
		if(abs(tInvCurrDqoRef.d) <= wInvCurrResolving)
		{
		    if(++uwCSSleepDelay >= 400)
		    {
		    	uwCSSleepDelay = 400;
		    	//fDCModeSwitchEn = 0;//Because DC NO run And Grid have harmonious,the bus voltage will be charge to protect.
		    	//fDCModeSwitch = 1;
		    	//fBiDirDCEnOut = 0;
		    	fInvEnOut = 0;		        	
		    	//sDisBiDirDC1And2CpldOut();
                //sDisBiDirDC3And4CpldOut();
		    	sDisInvCpldOut();	                
                //sDisBiDirDC1PWMOut();
                //sDisBiDirDC3PWMOut();
                //sDisBiDirDC2PWMOut();
                //sDisBiDirDC4PWMOut();
                sDisInvPWMOut(); 
                 
	            fHotBackup = 1;
		    }
		}		
	}
	else
	{
		uwCSSleepDelay = 0;		
		if((fError == 0)&&(fHotBackup == 1))
        {
            fHotBackup = 0;
            fInvEnOut = 1;            
            //fDCModeSwitchEn = 1;
            //fDCModeSwitch = 2;
            //fBiDirDCEnOut = 3;
        }
	}	
}
/********************************************************************* 
Function name:void sQUCurveModule(INT16U fClear)							  
Description:  Check power status
Calls:																   
Called By:	sCSPowerComdCal(void)											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sQUCurveModule(INT16U fClear)
{
INT16S wTemp;
INT32S dwTemp;
INT16S wActivePPerTemp;
INT16S wQAimTemp = 0;
INT16S wErr1,wErr2;
INT16S wFiltGainNum,wFiltGainDen;
INT16S wQ_UCurveVolt1s,wQ_UCurveReactPw1s,wQ_UCurveVolt2s,wQ_UCurveReactPw2s;
INT16S wQ_UCurveVolt1i,wQ_UCurveReactPw1i,wQ_UCurveVolt2i,wQ_UCurveReactPw2i;
static INT16S wActivePPerTempPrev = 0;
static INT32S dwpuQUGridVoltPrev = 0;
static INT32S dwpuQUGirdVoltBak = 0;
    
    wQ_UCurveVolt1s = wEepromQ_UCurveVolt1s * 10;
    wQ_UCurveReactPw1s = wEepromQ_UCurveReactPw1s * 10;
    wQ_UCurveVolt2s = wEepromQ_UCurveVolt2s * 10; 
    wQ_UCurveReactPw2s = wEepromQ_UCurveReactPw2s * 10;
    
    wQ_UCurveVolt1i = wEepromQ_UCurveVolt1i * 10;
    wQ_UCurveReactPw1i = wEepromQ_UCurveReactPw1i * 10;
    wQ_UCurveVolt2i = wEepromQ_UCurveVolt2i * 10; 
    wQ_UCurveReactPw2i = wEepromQ_UCurveReactPw2i * 10;
    
    dwTemp = (INT32S)wEepromQUActTime * 23170;
    wFiltGainNum = ((INT64S)32768<<15)/(dwTemp + 32768);
    wFiltGainDen = (INT32S)32768 - (wFiltGainNum<<1);
     	
    if(fClear == 1)
    {
        wActivePPerTempPrev = 0;
        dwpuQUGridVoltPrev = 2560000;
        dwpuQUGirdVoltBak = 2560000;
    }
    wActivePPerTemp = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 8000)/dwRatedPower);            
    wActivePPerTemp = abs(wActivePPerTemp);
    
    wActivePPerTempPrev = ((INT32S)wActivePPerTempPrev * 2185 + (INT32S)wActivePPerTemp * 1911)>>12;
    wActivePPerTemp = wActivePPerTempPrev>>3;
    
    dwTemp = (INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol;            	
    dwTemp = ((INT64S)dwTemp * 2560000)/((INT32S)wRatedGridLineVol * 3);
    	      
    dwpuQUGridVoltPrev = ((INT64S)dwpuQUGridVoltPrev * wFiltGainDen + (INT64S)(dwpuQUGirdVoltBak + dwTemp) * wFiltGainNum)>>15;
    dwpuQUGirdVoltBak = dwTemp;
    wTemp = dwpuQUGridVoltPrev>>8;
   // wDebugTest5 = wTemp;
               
    if(wActivePPerTemp > wEepromQ_UCurveLockInP)
    {        
        fPCSState.bits.Qu=true;
		if(wTemp >= wQ_UCurveVolt2s)
        {
        	wQAimTemp = wQ_UCurveReactPw2s;
        }
        else if(wTemp <= wQ_UCurveVolt2i)
        {
        	wQAimTemp = wQ_UCurveReactPw2i;
        }
        else
        {
        	if(wTemp >= wQ_UCurveVolt1s)
        	{
        		wErr1 = wQ_UCurveVolt1s - wTemp;
                wErr2 = wTemp - wQ_UCurveVolt2s;
                
                dwTemp = (INT32S)wErr2 * wQ_UCurveReactPw1s + (INT32S)wErr1 * wQ_UCurveReactPw2s;
                wErr2 = wErr1 + wErr2;
                wQAimTemp = dwTemp/wErr2;
        	}
        	else if(wTemp <= wQ_UCurveVolt1i)
        	{
        		wErr1 = wQ_UCurveVolt1i - wTemp;
                wErr2 = wTemp - wQ_UCurveVolt2i;
                
                dwTemp = (INT32S)wErr2 * wQ_UCurveReactPw1i + (INT32S)wErr1 * wQ_UCurveReactPw2i;
                wErr2 = wErr1 + wErr2;
                wQAimTemp = dwTemp/wErr2;
        	}
        	else
        	{
        		wQAimTemp = 0;
        	}                	
        }
        
        if(wQAimTemp >= 0)
	    {
            wReactivePowerPer = ((INT32S)wQAimTemp * 26843 + 8192)>>14;
	    }
	    else
	    {
            wReactivePowerPer = ((INT32S)wQAimTemp * 26843 - 8192)>>14;
	    }          	               
    }
    else if(wActivePPerTemp < wEepromQ_UCurveLockOutP)
    {
        wQAimTemp = 0;
        wReactivePowerPer = 0;
		fPCSState.bits.Qu=false;
    }   
}
/********************************************************************* 
Function name:void sPFPCurveModule(void)							  
Description:  Check power status
Calls:																   
Called By:	sCSPowerComdCal(void)											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sPFPCurveModule(void)
{
INT16S wActivePPerTemp,wLockInVolt,wLockOutVolt;
INT16S wEepromPF_PCurvePF1Temp,wEepromPF_PCurvePF2Temp,wPFAimTemp_Mid,wPFAimTemp;
INT16S wErr1,wErr2;
INT32S dwTemp;

    wLockInVolt = ((INT32S)wEepromPF_PCurveLockInV * wRatedGridLineVol)/1000;
    wLockOutVolt = ((INT32S)wEepromPF_PCurveLockOutV * wRatedGridLineVol)/1000;
    
    if((wRSGridRmsVol > wLockInVolt) || (wSTGridRmsVol > wLockInVolt) || (wTRGridRmsVol > wLockInVolt))
    {
        fPCSState.bits.PFP = true;
		if(wEepromPF_PCurvePF1 >= 0) wEepromPF_PCurvePF1Temp = 1000 - wEepromPF_PCurvePF1;
        else wEepromPF_PCurvePF1Temp = (-1000) - wEepromPF_PCurvePF1;
        if(wEepromPF_PCurvePF2 >= 0) wEepromPF_PCurvePF2Temp = 1000 - wEepromPF_PCurvePF2;
        else wEepromPF_PCurvePF2Temp = (-1000) - wEepromPF_PCurvePF2;
    
        wActivePPerTemp = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 1000)/dwRatedPower);
        	
        if(wActivePPerTemp <= wEepromPF_PCurveActPw1)
    	{
    		wPFAimTemp = wEepromPF_PCurvePF1;
    	}
    	else if(wActivePPerTemp >= wEepromPF_PCurveActPw2)
    	{
    		wPFAimTemp = wEepromPF_PCurvePF2;
    	}
    	else
    	{
    	    wErr1 = wEepromPF_PCurveActPw1 - wActivePPerTemp;
            wErr2 = wActivePPerTemp - wEepromPF_PCurveActPw2;
            
            dwTemp = (INT32S)wErr2 * wEepromPF_PCurvePF1Temp + (INT32S)wErr1 * wEepromPF_PCurvePF2Temp;
            wErr2 = wErr1 + wErr2;
            wPFAimTemp_Mid = dwTemp/wErr2;
            
            if (wPFAimTemp_Mid >= 0)
            {
            	wPFAimTemp = 1000 - wPFAimTemp_Mid;
            }                   
            else
            { 
            	wPFAimTemp = (-1000) - wPFAimTemp_Mid;
            }                     
        }
        
        if(abs(wPFAimTemp) < cPF0R80)
        {
            if(wPFAimTemp > 0)  wPFAimTemp = cPF0R80;
            if(wPFAimTemp < 0)  wPFAimTemp = -cPF0R80;    
        }
        else if(abs(wPFAimTemp) > cPF1R00)
        {
            if(wPFAimTemp > 0)  wPFAimTemp = cPF1R00;
            if(wPFAimTemp < 0)  wPFAimTemp = -cPF1R00;    
        }
        
        if(wPFAimTemp >= 0)
	    {
            wPFSet = ((INT32S)wPFAimTemp * cDectoHex + 512)>>10;
	    }
	    else
	    {
            wPFSet = ((INT32S)wPFAimTemp * cDectoHex - 512)>>10;
	    }              
    }
    else if ((wRSGridRmsVol < wLockOutVolt) && (wSTGridRmsVol < wLockOutVolt) && (wTRGridRmsVol < wLockOutVolt)) 
    {
        wPFSet = 16384;
		fPCSState.bits.PFP = false;
    }
}
/********************************************************************* 
Function name:void sVoltWattModule(void)							  
Description:  Check power status
Calls:																   
Called By:	sCSPowerComdCal(void)											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sVoltWattModule(void)
{
INT16S wTemp,wpuVWGirdVolt;
INT16S wDCChargeCurrLimit,wDCDisChargeCurrLimit;
INT16S wFiltGainNum,wFiltGainDen;
INT32S dwTemp;
INT16S wOvVWStartVolt,wOvVWStopVolt,wOvVWRatio;
INT16S wUnVWStartVolt,wUnVWStopVolt,wUnVWRatio;
static INT32S dwpuVWGridVoltPrev = 2560000;
static INT32S dwpuVWGirdVoltBak = 2560000;
static INT16S wPOvVoltfrozen = 0,wPUnVoltfrozen = 0;
    if(((wEepromOvFWEn == 0)||(fFreOvStartOrEnd == 0))&&((wEepromUnFWEn == 0)||(fFreUnStartOrEnd == 0)))fPCSState.bits.VW=((fVoltOvStartOrEnd == 1)||(fVoltUnStartOrEnd==1))?true:false;
	else fPCSState.bits.VW=false;
    wOvVWStartVolt = wEepromOvVWStartVolt * 10;
    wOvVWStopVolt = wEepromOvVWStopVolt * 10;
    wOvVWRatio = wEepromOvVWRatio * 10;
    
    wUnVWStartVolt = wEepromUnVWStartVolt * 10;
    wUnVWStopVolt = wEepromUnVWStopVolt * 10;
    wUnVWRatio = wEepromUnVWRatio * 10;
    
    wDCChargeCurrLimit = ((INT32S)wBatChargeCurrLimt * 10486)>>12;
    wDCDisChargeCurrLimit = ((INT32S)wBatDisChargeCurrLimt * 10486)>>12;
    
    dwTemp = (INT32S)wEepromVWActTime * 23170;//14247;//2/sqrt(2)/Ts/100*32768;
    wFiltGainNum = ((INT64S)32768<<15)/(dwTemp + 32768);
    wFiltGainDen = (INT32S)32768 - (wFiltGainNum<<1);
    
    dwTemp = (INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol;            	
    dwTemp = ((INT64S)dwTemp * 2560000)/((INT32S)wRatedGridLineVol * 3);
    	      
    dwpuVWGridVoltPrev = ((INT64S)dwpuVWGridVoltPrev * wFiltGainDen + (INT64S)(dwpuVWGirdVoltBak + dwTemp) * wFiltGainNum)>>15;
    dwpuVWGirdVoltBak = dwTemp;
    wpuVWGirdVolt = dwpuVWGridVoltPrev>>8;
    
    if(wpuVWGirdVolt > wOvVWStopVolt)
    {
        wpuVWGirdVolt = wOvVWStopVolt;
    }
    else if(wpuVWGirdVolt < wUnVWStopVolt)
    {
        wpuVWGirdVolt = wUnVWStopVolt;
    }

	wDebugTest5=wpuVWGirdVolt;
    if(((wEepromOvFWEn == 0)||(fFreOvStartOrEnd == 0))&&((wEepromUnFWEn == 0)||(fFreUnStartOrEnd == 0)))
    {
        if((fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))
        {        
            if((wpuVWGirdVolt > wOvVWStartVolt)&&(wEerpomOvVWEnable == 1))
            {
                fVoltOvStartOrEnd = 1; 
                fPStepSwitch = 2;           	    
                wTemp = wpuVWGirdVolt - wOvVWStartVolt;
                dwTemp = ((INT32S)wTemp * wOvVWRatio)/100;                    
                dwTemp = (INT32S)wPOvVoltfrozen - dwTemp;
                
                if(dwTemp > 10000)
                {
                	wTemp = 10000;
                }
                else if(dwTemp < -6667)
                {
                	wTemp = -6667;
                }
                else
                {
                    wTemp = dwTemp;
                }
                
                if(wTemp >=0)
            	{
            	    wActivePowerPer = ((INT32S)wTemp * 26843 + 8192)>>14;
					if(wActivePowerPer<=0)wActivePowerPer=1;
            	    wDCCurrAim = wDCDisChargeCurrLimit;
            	}
            	/*else if(wTemp <0)
            	{
            		wActivePowerPer = ((INT32S)wTemp * 26843 - 8192)>>14;
            		wDCCurrAim = wDCChargeCurrLimit;	
            	}
            	else
            	{
            	    wActivePowerPer = 0;//((INT32S)wTemp * cDectoHex - 512)>>10;
            	}*/
            	fActiveMode = 0;              
            }
            else
            {
                wPOvVoltfrozen = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 10000)/dwRatedPower);
                fVoltOvStartOrEnd = 0;
            }
            
            if((wpuVWGirdVolt < wUnVWStartVolt)&&(wEerpomUnVWEnable == 1))
            {
                fVoltUnStartOrEnd = 1; 
                fPStepSwitch = 2;           	    
                wTemp = wUnVWStartVolt - wpuVWGirdVolt;            
                dwTemp = ((INT32S)wTemp * wUnVWRatio)/100;                    
                dwTemp = (INT32S)wPUnVoltfrozen + dwTemp;
                
                if(dwTemp > 10000)
                {
                	wTemp = 10000;
                }
                else if(dwTemp < -6667)
                {
                	wTemp = -6667;
                }
                else
                {
                    wTemp = dwTemp;
                }
                
                if(wTemp > 0)
            	{
            	    wActivePowerPer = ((INT32S)wTemp * 26843 + 8192)>>14;
            	    wDCCurrAim = wDCDisChargeCurrLimit;
            	}
            	else if(wTemp < 0)
            	{
            		wActivePowerPer = ((INT32S)wTemp * 26843 - 8192)>>14;
            		wDCCurrAim = wDCChargeCurrLimit;
            	}
            	else
            	{
            	    wActivePowerPer = 0;//((INT32S)wTemp * cDectoHex - 512)>>10;
            	}
            	fActiveMode = 0;
            }
            else
            {
                wPUnVoltfrozen = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 10000)/dwRatedPower);
                fVoltUnStartOrEnd = 0;
            }
        }
    }  
}
/********************************************************************* 
Function name:void sFreqWattModule(void)							  
Description:  Check power status
Calls:																   
Called By:	sCSPowerComdCal(void)											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sFreqWattModule(void)
{
INT16S wTemp;
INT32S dwTemp;
static INT16U i = 0;   
static INT16U uwOVFreOKCnt = 0,uwOVFreCnt = 0; 
static INT16U uwUnFreOKCnt = 0,uwUnFreCnt = 0; 
static INT16S wPOVFrefrozen = 0,wPUnFrefrozen = 0;
static INT16S wFreMax = 0,wFreMin = 0;
static INT16U uwLVRTDelay = 0;
INT16S wGridCrossZeroFreqMax,wGridCrossZeroFreqMin;
INT16S wUnFreTripFre,wOVFreTripFre,wUnFreExitFre,wOVFreExitFre;
INT16S wDCChargeCurrLimit,wDCDisChargeCurrLimit;
INT16S wFiltGainNum,wFiltGainDen;

    fPCSState.bits.FW=((fFreUnStartOrEnd==1)||(fFreOvStartOrEnd==1))?true:false;
    wDCChargeCurrLimit = ((INT32S)wBatChargeCurrLimt * 10486)>>12;
    wDCDisChargeCurrLimit = ((INT32S)wBatDisChargeCurrLimt * 10486)>>12;
    
    //--------------------------Active Power Ctr---------------------------
    //------------------------Fre Derating---------------------------------
    wGridCrossZeroFreqMax = suwGetMaxGridFreq();
    dwGridFreMaxSum = dwGridFreMaxSum - wGridFreMaxArray[i] + wGridCrossZeroFreqMax;
    wGridFreMaxArray[i] = wGridCrossZeroFreqMax;
    
    wGridCrossZeroFreqMin = suwGetMinGridFreq();
    dwGridFreMinSum = dwGridFreMinSum - wGridFreMinArray[i] + wGridCrossZeroFreqMin;
    wGridFreMinArray[i] = wGridCrossZeroFreqMin;
    
    if(wEepromRatedFreqSel == 1)
    {
        wUnFreTripFre = ((INT32S)(wEepromUnFWTripFreq + 6000)* 10486)>>12;
        wOVFreTripFre = ((INT32S)(wEepromOvFWTripFreq + 6000)* 10486)>>12;
        wUnFreExitFre = ((INT32S)(wEepromUnFWExitFreq + 6000)* 10486)>>12;
        wOVFreExitFre = ((INT32S)(wEepromOvFWExitFreq + 6000)* 10486)>>12;
    }
    else
    {
        wUnFreTripFre = ((INT32S)(wEepromUnFWTripFreq + 5000)* 10486)>>12;
        wOVFreTripFre = ((INT32S)(wEepromOvFWTripFreq + 5000)* 10486)>>12;
        wUnFreExitFre = ((INT32S)(wEepromUnFWExitFreq + 5000)* 10486)>>12;
        wOVFreExitFre = ((INT32S)(wEepromOvFWExitFreq + 5000)* 10486)>>12;
    }
    
    i++;
    if(i == 4)
    {
        i = 0;
    }
    
    if((wEepromStandardValue == cIEEE1547Standard)||(wEepromStandardValue == cRule21Standard))
    {
        dwTemp = (INT32S)wEepromFWActTime * 23170;//14247;//2/sqrt(2)/Ts/100*32768;
        //wFiltGainDen = ((INT64S)(dwTemp - 32768)<<15)/(dwTemp + 32768);
        wFiltGainNum = ((INT64S)32768<<15)/(dwTemp + 32768);
        wFiltGainDen = (INT32S)32768 - (wFiltGainNum<<1);
        //wDebugTest3 = wFiltGainDen;
        //wDebugTest5 = wFiltGainNum;
        
        if(fLVRTValue > 12288)
        {
            if(++uwLVRTDelay >= 4)
            {
                uwLVRTDelay = 4;
                dwFreFiltMaxPrev = ((INT64S)dwFreFiltMaxPrev * wFiltGainDen + (INT64S)((INT32S)((INT32S)wGridCrossZeroFreqMax + wFreMaxBak)<<8) * wFiltGainNum)>>15;
                dwFreFiltMinPrev = ((INT64S)dwFreFiltMinPrev * wFiltGainDen + (INT64S)((INT32S)((INT32S)wGridCrossZeroFreqMin + wFreMinBak)<<8) * wFiltGainNum)>>15;
                wFreMaxBak = wGridCrossZeroFreqMax;
                wFreMinBak = wGridCrossZeroFreqMin;
                wGridCrossZeroFreqMax = dwFreFiltMaxPrev>>8;
                wGridCrossZeroFreqMin = dwFreFiltMaxPrev>>8;
            }
        }
        else
        {
            uwLVRTDelay = 0;
            if(wEepromRatedFreqSel == 1)
            {
                dwFreFiltMaxPrev = 3932160;
                dwFreFiltMinPrev = 3932160;
                wFreMaxBak = cFreq60Hz;
                wFreMinBak = cFreq60Hz;
            }
            else
            {
                dwFreFiltMaxPrev = 3276800;
                dwFreFiltMinPrev = 3276800;
                wFreMaxBak = cFreq50Hz;
                wFreMinBak = cFreq50Hz;
            }
        }
        
        wDebugTest1 = wGridCrossZeroFreqMax;
        wDebugTest2 = wGridCrossZeroFreqMin;
        
        if(((wEerpomOvVWEnable == 0)||(fVoltOvStartOrEnd == 0))&&((wEerpomUnVWEnable == 0)||(fVoltUnStartOrEnd == 0)))
        {
            if((fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))
            {
                if((wGridCrossZeroFreqMax > wOVFreTripFre)&&(wEepromOvFWEn == 1))
                {
                    fFreOvStartOrEnd = 1; 
                    fPStepSwitch = 1;
                    if(wGridCrossZeroFreqMax > wOVFreExitFre)
                    {
                        wTemp = wOVFreExitFre - wOVFreTripFre; 
                    }
                    else
                    {
                        wTemp = wGridCrossZeroFreqMax - wOVFreTripFre;
                        
                    }
                    
                    wTemp = ((INT32S)wTemp * wEepromOvFWRatio)>>8;                    
                    wTemp = wPOVFrefrozen - wTemp;
                    
                    if(wTemp > 1000)
                    {
                    	wTemp = 1000;
                    }
                    else if(wTemp < -667)
                    {
                    	wTemp = -667;
                    }
                    
                    if(wTemp >=0)
                	{
                	    wActivePowerPer = ((INT32S)wTemp * cDectoHex + 512)>>10;
                	    wDCCurrAim = wDCDisChargeCurrLimit;	
                	}
                    if(wGridCrossZeroFreqMax > wOVFreExitFre)wActivePowerPer=0;
					
                	/*else if(wTemp < 0)
                	{
                		//wActivePowerPer = ((INT32S)wTemp * cDectoHex - 512)>>10;
                		wActivePowerPer=0;
                		wDCCurrAim = wDCChargeCurrLimit;
                	}
                	else
                	{
                	    wActivePowerPer = 0;//((INT32S)wTemp * cDectoHex - 512)>>10;
                	}*/
                	fActiveMode = 0;              
                }
                else
                {
                    wPOVFrefrozen = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 1000)/dwRatedPower);
                    fFreOvStartOrEnd = 0;
                }
                
                if((wGridCrossZeroFreqMin < wUnFreTripFre)&&(wEepromUnFWEn == 1))
                {
                    fFreUnStartOrEnd = 1;
                    fPStepSwitch = 1; 
                    if(wGridCrossZeroFreqMin < wUnFreExitFre)
                    {
                        wTemp = wUnFreTripFre - wUnFreExitFre;
                    }
                    else
                    {
                        wTemp = wUnFreTripFre - wGridCrossZeroFreqMin;
                    }                                    
                    wTemp = ((INT32S)wTemp * wEepromUnFWRatio)>>8;//wEepromUnFreRatio Œ™’˝                
                    wTemp = wPUnFrefrozen + wTemp;
                    
                    if(wTemp > 1000)
                    {
                    	wTemp = 1000;
                    }
                    else if(wTemp < -667)
                    {
                    	wTemp = -667;
                    }
                    
                    if(wTemp > 0)
                    {
                        wActivePowerPer = ((INT32S)wTemp * cDectoHex + 512)>>10;
                        wDCCurrAim = wDCDisChargeCurrLimit;
                    }
                    else if(wTemp < 0)
                    {
                    	wActivePowerPer = ((INT32S)wTemp * cDectoHex - 512)>>10;
                    	wDCCurrAim = wDCChargeCurrLimit;
                    }
                    else
                    {
                        wActivePowerPer = 0;//((INT32S)wTemp * cDectoHex - 512)>>10;
                    }
                    fActiveMode = 0;
                }
                else
                {
                    wPUnFrefrozen = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 1000)/dwRatedPower);
                    fFreUnStartOrEnd = 0;
                }
            }
        }      
    }
    else
    {    
        wGridCrossZeroFreqMax = dwGridFreMaxSum>>2;
        wGridCrossZeroFreqMin = dwGridFreMinSum>>2;
        wDebugTest1 = wGridCrossZeroFreqMax;
        wDebugTest2 = wGridCrossZeroFreqMin;
        fPStepSwitch = 0;
        
        if((fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))
        {
            if(wEepromUnFWEn == 1)
            {
            	if(wGridCrossZeroFreqMin < wUnFreTripFre)
            	{
            		++uwUnFreCnt;
            		uwUnFreOKCnt = 0;
            		
            		if(uwUnFreCnt < 5)
                    {
                        wPUnFrefrozen = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 1000)/dwRatedPower);
                    }
                    else if(uwUnFreCnt >= 5)
                    {
                    	wTemp = wUnFreTripFre - wGridCrossZeroFreqMin;
                    	
                    	if(wTemp > wFreMin)
                    	{
                    		wFreMin = wTemp;
                    	}
                    	
                        uwUnFreCnt = 5;
                        fFreUnStartOrEnd = 1;
                        
                        if(wFreMin <= 0)
                        {
                            wFreMin = 0;
                        }
                        else if(wFreMin >= 1280) 
                        {
                            wFreMin = 1280;
                        }  
                        
                        wTemp = ((INT32S)wFreMin * wEepromUnFWRatio)>>8;//wEepromUnFreRatio Œ™’˝
                        
                        wTemp = wPUnFrefrozen + wTemp;
                        
                        if(wTemp > 1000)
                        {
                        	wTemp = 1000;
                        }
                        else if(wTemp < -667)
                        {
                        	wTemp = -667;
                        }
                        
                        if(wTemp > 0)
                		{
                		    wActivePowerPer = ((INT32S)wTemp * cDectoHex + 512)>>10;
                		    wDCCurrAim = wDCDisChargeCurrLimit;
                		}
                		else if(wTemp < 0)
                		{
                			wActivePowerPer = ((INT32S)wTemp * cDectoHex - 512)>>10;
                			wDCCurrAim = wDCChargeCurrLimit;
                		}
                		else
                		{
                		    wActivePowerPer = 0;//((INT32S)wTemp * cDectoHex - 512)>>10;
                		}
                		fActiveMode = 0;
                    }
            	}
            	else
            	{
            		if(fFreUnStartOrEnd == 1)
                	{
                		if(wGridCrossZeroFreqMin >= wUnFreExitFre)
                		{
                			if(++uwUnFreOKCnt >= 5)
                            {
                                uwUnFreOKCnt = 5;
                                uwUnFreCnt = 0;
                                fFreUnStartOrEnd = 0;  
                                wFreMin = 0;
                            }
                		}
                	}
                    else
                    {
                        uwUnFreOKCnt = 5;
                        uwUnFreCnt = 0;
                        fFreUnStartOrEnd = 0; 
                        wFreMin = 0;
                    }
            	}
            }
            else
            {
            	uwUnFreOKCnt = 5;
                uwUnFreCnt = 0;
                fFreUnStartOrEnd = 0; 
                wFreMin = 0;
            }
            
            if(wEepromOvFWEn == 1)
            {
            	if(wGridCrossZeroFreqMax > wOVFreTripFre)
                {                
                    ++uwOVFreCnt;
                    uwOVFreOKCnt = 0;
                    
                    if(uwOVFreCnt < 5)
                    {
                        wPOVFrefrozen = (INT16S)(((INT64S)(dwInvActivePower[3]>>4) * 1000)/dwRatedPower);
                    }
                    else if(uwOVFreCnt >= 5)//¡¨–¯5¥Œº¥20ms∑¢…˙π˝∆µ ±
                    {
                    	wTemp = wGridCrossZeroFreqMax - wOVFreTripFre;
                    	
                    	if(wTemp > wFreMax)
                    	{
                    		wFreMax = wTemp;
                    	}
                    	
                        uwOVFreCnt = 5;
                        fFreOvStartOrEnd = 1;
                        
                        if(wFreMax <= 0)
                        {
                            wFreMax = 0;
                        }
                        else if(wFreMax >= 1280) 
                        {
                            wFreMax = 1280;
                        }  
                        
                        wTemp = ((INT32S)wFreMax * wEepromOvFWRatio)>>8;
                        
                        wTemp = wPOVFrefrozen - wTemp;
                        
                        if(wTemp > 1000)
                        {
                        	wTemp = 1000;
                        }
                        else if(wTemp < -667)
                        {
                        	wTemp = -667;
                        }
                        
                        if(wTemp > 0)
                		{
                		    wActivePowerPer = ((INT32S)wTemp * cDectoHex + 512)>>10;
                		    wDCCurrAim = wDCDisChargeCurrLimit;
                		}
                		else if(wTemp < 0)
                		{
                			wActivePowerPer = ((INT32S)wTemp * cDectoHex - 512)>>10;
                			wDCCurrAim = wDCChargeCurrLimit;
                		}
                		else
                		{
                		    wActivePowerPer = 0;//((INT32S)wTemp * cDectoHex - 512)>>10;
                		}
                		fActiveMode = 0;
                    }
                }
                else
                {
                	if(fFreOvStartOrEnd == 1)
                	{
                		if(wGridCrossZeroFreqMax <= wOVFreExitFre)
                		{
                			if(++uwOVFreOKCnt >= 5)
                            {
                                uwOVFreOKCnt = 5;
                                uwOVFreCnt = 0;
                                fFreOvStartOrEnd = 0;  
                                wFreMax = 0;
                            }
                		}
                	}
                    else
                    {
                        uwOVFreOKCnt = 5;
                        uwOVFreCnt = 0;
                        fFreOvStartOrEnd = 0; 
                        wFreMax = 0;
                    }          
                }
            }
            else
            {
            	uwOVFreOKCnt = 5;
                uwOVFreCnt = 0;
                fFreOvStartOrEnd = 0;
                wFreMax = 0;
                
                uwUnFreOKCnt = 5;
                uwUnFreCnt = 0;
                fFreUnStartOrEnd = 0; 
                wFreMin = 0;
            }
        }
    }
}
/********************************************************************* 
Function name:void sCSVSSwitch(void)							  
Description:  Check power status
Calls:																   
Called By:										   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sCSVSSwitch(void)
{
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;
static INT16U uwDelay3 = 0;
    
    if(fStartOrNormal == 1)
    {
        if(wEepromAutoCSVSSwitchEn == 1)
        {
            if(fATSState == 1)
            {
                if(fQFMRelayCtr == 0)
                {
                    uwDelay1 = 0;
                    if(++uwDelay2 >= 2000)
                    {
                        uwDelay2 = 2000;
                        
                        if(fInvEnOut == 0)
                        {
                            sQFMRelayClose();
                            uwDelay3 = 0;
                            fSciRunOrder = 1;                   
                        }
                        else
                        {
                            fSciRunOrder = 0;
                            fATSForceStop = 1;
                        }
                    }
                }
                else
                {
                    uwDelay2 = 0;
                    
                    if(++uwDelay3 >= 100)
                    {
                        uwDelay3 = 100;
                        
                        if((fGridChkValue > 18841)||(fGridChkValue < 13107))
                        {
                            if(++uwDelay1 >= 2)
                            {
                                uwDelay1 = 2;
                                sQFMRelayOpen();
                                fATSForceCS2VS = 0;
                                fCSVSSwitch = 1;
                                if((uwSysMode == InvRunMode)&&(fError == 0))
	                            {
	                                fInvEnOut = 1;
	                            } 
                            }
                        }
                        else
                        {
                            if(wEepromRemoteSel == 1)
                            {
                                if(wRemoteCSVSSwitch == 0)
                                {
                                    fCSVSSwitch = 0;
                                }
                                else if(wRemoteCSVSSwitch == 1)
                                {
                                    fCSVSSwitch = 1;
                                }
                            }
                            else
                            {        
                                if(wEepromCSVSSwitch == 0)
                                {
                                    fCSVSSwitch = 0;
                                }
                                else if(wEepromCSVSSwitch == 1)
                                {
                                    fCSVSSwitch = 1;
                                }
                            }               
                        }                    
                    }                        
                }       
            }
            else
            {
                uwDelay1 = 0;
                uwDelay2 = 0;
                sQFMRelayOpen();
                fCSVSSwitch = 1;
            }
        }
        else
        {
            if(wEepromRemoteSel == 1)
            {
                if(wRemoteCSVSSwitch == 0)
                {
                    fCSVSSwitch = 0;
                }
                else if(wRemoteCSVSSwitch == 1)
                {
                    fCSVSSwitch = 1;
                }
            }
            else
            {        
                if(wEepromCSVSSwitch == 0)
                {
                    fCSVSSwitch = 0;
                }
                else if(wEepromCSVSSwitch == 1)
                {
                    fCSVSSwitch = 1;
                }
            } 
        }
    }
    else
    {
        uwDelay1 = 0;
        uwDelay2 = 0;
        uwDelay3 = 0;
        sQFMRelayClose();
    }
}

/********************************************************************* 
Function name:void sCSPowerComdCal(void)							  
Description:  Check power status
Calls:																   
Called By:	sSysMode(void)											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sCSPowerComdCal(void)
{
INT16S wDCChargeCurrLimit,wDCDisChargeCurrLimit;
static INT16U fQUCurveClc = 1;

    wDCChargeCurrLimit = ((INT32S)wBatChargeCurrLimt * 10486)>>12;
    wDCDisChargeCurrLimit = ((INT32S)wBatDisChargeCurrLimt * 10486)>>12;
        
    if(fCSVSSwitch == 0)
    {
        sFreqWattModule();
        sVoltWattModule();
                           
        if(((wEepromOvFWEn == 0)||(fFreOvStartOrEnd == 0))&&((wEepromUnFWEn == 0)||(fFreUnStartOrEnd == 0))
         &&((wEerpomOvVWEnable == 0)||(fVoltOvStartOrEnd == 0))&&((wEerpomUnVWEnable == 0)||(fVoltUnStartOrEnd == 0)))
        {
            fPStepSwitch = 0;
			fPCSState.bits.VW=false;
			fPCSState.bits.FW=false;
            if(wEepromRemoteSel == 1)
            {
            	if(wRemoteCSPCtrModeSel == 0) 
            	{
            		if(wRemoteCSPPer > 0)
            		{
            		    wActivePowerPer = ((INT32S)wRemoteCSPPer * cDectoHex + 512)>>10;
            		    wDCCurrAim = wDCDisChargeCurrLimit;
            		}
            		else if(wRemoteCSPPer < 0)
            		{
            			wActivePowerPer = ((INT32S)wRemoteCSPPer * cDectoHex - 512)>>10;
            			wDCCurrAim = wDCChargeCurrLimit;
            		}
            		else
            		{
            		    wActivePowerPer = 0;//((INT32S)wRemoteCSPPer * cDectoHex - 512)>>10;
            		}
            		
            		fActiveMode = 0;
            	}
            	else
            	{
            		if(wRemoteCSCurrSet >= 0)
            		{
            		    wDCCurrAim = ((INT32S)wRemoteCSCurrSet * 20971 + 4096)>>13;//10 => 10000
            		    
            		    if(wDCCurrAim > wDCDisChargeCurrLimit)
            		    {
            		    	wDCCurrAim = wDCDisChargeCurrLimit;
            		    }
            		}
            		else
            		{
            			wDCCurrAim = ((INT32S)wRemoteCSCurrSet * 20971 - 4096)>>13;
            			
            			if(wDCCurrAim < wDCChargeCurrLimit)
            		    {
            		    	wDCCurrAim = wDCChargeCurrLimit;
            		    }
            		}
            		fActiveMode = 1;
            	}
            }
            else
            {
            	if(wEepromCSPCtrModeSel == 0)
            	{
            		if(wEepromCSPPer > 0)
            		{
            		    wActivePowerPer = ((INT32S)wEepromCSPPer * cDectoHex + 512)>>10;
            		    wDCCurrAim = wDCDisChargeCurrLimit;
            		}
            		else if(wEepromCSPPer < 0)
            		{
            			wActivePowerPer = ((INT32S)wEepromCSPPer * cDectoHex - 512)>>10;
            			wDCCurrAim = wDCChargeCurrLimit;
            		}
            		else
            		{
            		    wActivePowerPer = 0;//((INT32S)wEepromCSPPer * cDectoHex - 512)>>10;
            		}
            		fActiveMode = 0;
            	}
            	else
            	{
            		if(wEepromCSCurrSet >= 0)
            		{
            		    wDCCurrAim = ((INT32S)wEepromCSCurrSet * 20971 + 4096)>>13;
            		    
            		    if(wDCCurrAim > wDCDisChargeCurrLimit)
            		    {
            		    	wDCCurrAim = wDCDisChargeCurrLimit;
            		    }
            		}
            		else
            		{
            			wDCCurrAim = ((INT32S)wEepromCSCurrSet * 20971 - 4096)>>13;
            			
            			if(wDCCurrAim < wDCChargeCurrLimit)
            		    {
            		    	wDCCurrAim = wDCChargeCurrLimit;
            		    }
            		}
            		fActiveMode = 1;
            	}
            }
        }
        
        //-----------------Reactive Power Ctr--------------------------
        if(wEepromRemoteSel == 1)
        {
			
			fQStepSwitch = 0;
            fQUCurveClc = 1;
        	if(wRemoteCSQCtrModeSel == 0)
        	{
        		if(wRemoteCSPFSet >= 0)
	    	    {
                    wPFSet = ((INT32S)wRemoteCSPFSet * cDectoHex + 512)>>10;
	    	    }
	    	    else
	    	    {
                    wPFSet = ((INT32S)wRemoteCSPFSet * cDectoHex - 512)>>10;
	    	    }
	    	    
	    	    fReactiveMode = 0;
	    	    wReactivePowerPer = 0;
        	}
        	else if(wRemoteCSQCtrModeSel == 1)
        	{
        	    if(wRemoteCSQPer >= 0)
	    	    {
                    wReactivePowerPer = ((INT32S)wRemoteCSQPer * cDectoHex + 512)>>10;
	    	    }
	    	    else
	    	    {
                    wReactivePowerPer = ((INT32S)wRemoteCSQPer * cDectoHex - 512)>>10;
	    	    }
	    	    
	    	    fReactiveMode = 1;
	    	    wPFSet = 16384;
	    	}
	    	else
	    	{
	    		wReactivePowerPer = 0;
	    		fReactiveMode = 1;
	    		wPFSet = 16384;
	    	}
        }
        else
        {
        	if(wEepromCSQCtrModeSel == cPFSetCtr)
        	{
        	    fQStepSwitch = 0;
        	    fQUCurveClc = 1;
        		if(wEepromCSPFSet >= 0)
	    	    {
                    wPFSet = ((INT32S)wEepromCSPFSet * cDectoHex + 512)>>10;
	    	    }
	    	    else
	    	    {
                    wPFSet = ((INT32S)wEepromCSPFSet * cDectoHex - 512)>>10;
	    	    }
	    	    
	    	    fReactiveMode = 0;
	    	    wReactivePowerPer = 0;
        	}
        	else if(wEepromCSQCtrModeSel == cQSetCtr)
        	{
        	    fQStepSwitch = 0;
        	    fQUCurveClc = 1;
                fPCSState.bits.PFP=false;
			    fPCSState.bits.Qu=false;
				if(wEepromCSQPer >= 0)
	    	    {
                    wReactivePowerPer = ((INT32S)wEepromCSQPer * cDectoHex + 512)>>10;
	    	    }
	    	    else
	    	    {
                    wReactivePowerPer = ((INT32S)wEepromCSQPer * cDectoHex - 512)>>10;
	    	    }
	    	    fReactiveMode = 1;
	    	    wPFSet = 16384;
	    	}
	    	else if(wEepromCSQCtrModeSel == cPFPCurveCtr)
	    	{
	    	    fQStepSwitch = 0;
	    	    fQUCurveClc = 1;
	    		sPFPCurveModule();
                wReactivePowerPer = 0;
                fReactiveMode = 0; 
	    	}
	    	else if (wEepromCSQCtrModeSel == cQUCurveCtr)
            {  
                fQStepSwitch = 1;     	
                sQUCurveModule(fQUCurveClc);
                wPFSet = 16384;
                fReactiveMode = 1;
                fQUCurveClc = 0;
            }   
            else
            {
                fQStepSwitch = 0;
                wReactivePowerPer = 0;
                wPFSet = 16384;
                fReactiveMode = 0;
            }
        }
    }
}
/********************************************************************* 
Function name:void sVSPowerComdCal(void)							  
Description:  Check power status
Calls:																   
Called By:	sSysMode(void)											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sVSPowerComdCal(void)
{
INT16S wPPerTemp,wQPerTemp;

    if(wEepromRemoteSel == 1)
    {
        wVSVoltSet = ((INT32S)wRemoteVSVoltSet * 21404 + 8192)>>14;
        wVSFreqSet = ((INT32S)wRemoteVSFreqSet * 16471 + 8192)>>14;
        
        wPPerTemp = ((INT32S)wRemoteVSPSet * cDectoHex + 512)>>10;
        wQPerTemp = ((INT32S)wRemoteVSQSet * cDectoHex + 512)>>10;
        
        dwVSPSet = ((INT32S)wPPerTemp * dwRatedPower)>>10;
        dwVSQSet = ((INT32S)wQPerTemp * dwRatedPower)>>10;
    }
    else
    {
        wVSVoltSet = ((INT32S)wEepromVSVoltSet * 21404 + 8192)>>14;
        wVSFreqSet = ((INT32S)wEepromVSFreqSet * 16471 + 8192)>>14;
        
        wPPerTemp = ((INT32S)wEepromVSPSet * cDectoHex + 512)>>10;
        wQPerTemp = ((INT32S)wEepromVSQSet * cDectoHex + 512)>>10;
        
        dwVSPSet = ((INT32S)wPPerTemp * dwRatedPower)>>10;
        dwVSQSet = ((INT32S)wQPerTemp * dwRatedPower)>>10; 
    }
}
/********************************************************************* 
Function name:void sVSCoeffCal(void)							  
Description:  Check power status
Calls:																   
Called By:	main()											   
Parameters:  void													   
Return:  void														   
 *********************************************************************/	
void sVSCoeffCal(void)
{
INT16S wRatedPhaseVoltPeak;
INT16S wRatedFreq;
INT32S dwRatedPowerTemp;
    
    dwRatedPowerTemp = dwRatedPower<<4;
    wRatedPhaseVoltPeak = ((INT32S)wRatedGridLineVol * 13377)>>14;//SQRT(2/3)
    wRatedFreq = ((INT32S)uwSysFreq * 6434)>>14;
    dwDampD = ((INT64S)dwRatedPowerTemp * 16297)/((INT32S)wRatedFreq * wEepromVSFreqDroopKByP);//100/2/pi
    dwDroopKQ = ((INT64S)wRatedPhaseVoltPeak * wEepromVSVoltDroopKByQ * 1049)/dwRatedPowerTemp;//1049 = 1024*1024/1000;
    
    wVSFreqSetRev = (INT32S)67108864/wRatedFreq;//Q22
}
/********************************************************************* 
Function name:INT16U suwInvAndDCPwmKCal(INT16U wDCVol)							  
Description:used for master compete
Calls:main() 20ms
Called By:
Parameters:  void
Return:  void
 *********************************************************************/
INT16U suwInvAndDCPwmKCal(INT16U wDCVol)
{
INT16U uwGridInvPwmK;
INT16S wDCPwmCarrTmp;
static INT16S wBusVoltFiltPrev = 7360;

    if(fBatteryInitiStateOK >= 1)
    {
        uwInvPwmBusComp = (INT16U)(((INT32U)cSqrt3PwmK<<10)/wDCVol);              
    }
    else
    {
        if(fDeCapEnergy == 1)
        {
            uwInvPwmBusComp = 1847;
        }
        else
        {
            uwInvPwmBusComp = 0;
        } 
    }
        	
	wBusVoltFiltPrev = (INT16S)(((INT32S)wBusVoltFiltPrev*cBusVoltFiltGainDen + (INT32S)wDCVol*cBusVoltFiltGainNum)>>12);
	
	if(wBusVoltFiltPrev <= 800)
	{
		wBusVoltFiltPrev = 800;
	}
	
    wDCPwmCarrTmp = ((INT32S)8192<<11)/wBusVoltFiltPrev;
	
	if(wDCPwmCarrTmp > 10486)
	{
		wDCPwmCarrTmp = 10486;
	}
	else if(wDCPwmCarrTmp < 1103)
	{
		wDCPwmCarrTmp = 1103;
	}
	wDCPwmCarrFrwd = wDCPwmCarrTmp;
	
	uwGridInvPwmK = (INT16U)(((INT32U)cSqrt3PwmK<<10)/(wBusVoltFiltPrev + 800));            
    if(uwGridInvPwmK > cInvPwmKLimtHigh)  
    {
        uwGridInvPwmK = cInvPwmKLimtHigh;
    }
    else if(uwGridInvPwmK < cInvPwmKLimtLow)
    {
        uwGridInvPwmK = cInvPwmKLimtLow;
    }
    return(uwGridInvPwmK);
}
/********************************************************************* 
Function name:INT16S swGirdDivBatteryVolCoeff(INT16U wDCVol)							  
Description:used for master compete
Calls:main() 20ms
Called By:
Parameters:  void
Return:  void
 *********************************************************************/
INT16S swGirdDivBatteryVolCoeff(INT16U wDCVol)
{
INT32S dwUd;
INT16S wCoeff;
    dwUd = (((INT32S)wRSGridRmsVol + wSTGridRmsVol + wTRGridRmsVol) * 6689)>>2;//Q16   
    if(wDCVol < 4000)
    {
    	wCoeff = dwUd/4000;//Q12
    }
    else
    {
        wCoeff = dwUd/wDCVol;//Q12
    }
    return(wCoeff);
}
/*********************************************************************
Function name: INT16U sBatteryInitiStateJudge(INT16S *pVol,INT16U *pErrFlag,INT16U *pEventFlag)
Description:  ≈–∂œPVµÁ—π «∑Ò¥ÔµΩ∆Ù∂ØµÁ—π 
Calls:        	 
Called By:  sSysMode(void)
Parameters:  void
Return:  INT16U PVµÁ—π «∑Ò¥ÔµΩ∆Ù∂ØµÁ—π      
 *********************************************************************/
void sBatteryInitiStateJudge(INT16S wBattVolt)
{
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;
INT16S wBattStartUpVol;
INT16U uwStartTime;

    wBattStartUpVol = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
    wBattStartUpVol = wBattStartUpVol - cVolt10V;
    
    if(fFactoryFastStart == 1)
    {
        uwStartTime = c1Secs;
    }
    else
    {
        uwStartTime = c1Secs;//c30Secs;
    }
		
	if(fBatteryInitiStateOK == 0)
	{
		uwDelay1 = 0;
		uwDelay2 = 0;
		fBatteryInitiStateOK = 1;
	}
	else
	{
		if((wBattVolt >= wBattStartUpVol)&&(fBatteryReady == 1))
		{
			if(++uwDelay1 >= uwStartTime)
			{
				fBatteryInitiStateOK = 2;
				uwDelay1 = 0;
				sEnDCSps();
				sEnInvSps();
			}			
		}
		else
		{
			uwDelay1 = 0;
			
			fBattDarkStart = fBatteryPreChargeEn;//uwDataSci[5];
			
			if((fBattDarkStart == 1)&&(fCSVSSwitch == 0))
			{
				if(++uwDelay2 >= c10Secs)
			    {
			    	fBatteryInitiStateOK = 3;
			    	uwDelay2 = 0;
			    	sEnDCSps();
				    sEnInvSps();
			    }
			}
			else
			{
				fBatteryInitiStateOK = 1;
				uwDelay2 = 0;
			}
		}
	}
}
/*********************************************************************
Function name:void sBatteryChargeDeal(INT16S wBattVolt,INT16S wBattCurr)
Description:Battery Charge Mode need deal how to float charge
Calls:        	 
Called By:  sSysMode(void)
Parameters:  void
Return:void   
 *********************************************************************/
void sBatteryChargeDeal(INT16S wBattVolt,INT16S wBattCurr)
{
INT16S wSToFChargeCurr;
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;
    
    wSToFChargeCurr = ((INT32S)wBatSToFChargeCurr * 10486)>>12;//256/100 Q12
    
	if(fCharge == 1)
	{
		if(fFloatCharge == 1)
		{
			uwDelay2 = 0;
			wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
			
			if((wBattCurr >= wSToFChargeCurr)&&(wInvDsCurrLimitHigh != (-tInvCurrDqoRef.d)))
			{
				if(++uwDelay1 >= 20)
				{
					uwDelay1 = 20;
					fFloatCharge = 2;
				}
			}
			else
			{
				uwDelay1 = 0;
			}
		}
		else if(fFloatCharge == 2)
		{
			uwDelay1 = 0;
			wBatVolLimtHigh = ((INT32S)wBatFloatChargeVolt * 26214)>>14;
			
			if(wBattCurr < (wSToFChargeCurr - 128))	
			{
				if(++uwDelay2 >= 100)
				{
					uwDelay2 = 100;
					fFloatCharge = 1;
				}
			}
			else
			{
				uwDelay2 = 0;
			}		
		}
		else
		{
			wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;			
			fFloatCharge = 1;//∫„≥‰
			uwDelay1 = 0;
			uwDelay2 = 0;
		}
	}
	else
	{
		wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
		fFloatCharge = 0;
		uwDelay1 = 0;
		uwDelay2 = 0;
	}
}
/*********************************************************************
Function name:INT16U sfBiDirDCCtrSoftStart(INT16S wBatVolt,wDCVolt)
Description: ≈–∂œ «∑Ò–Ë“™Ω¯––DC»Ì∆BUS
Calls:        	 
Called By:  sSysMode(void)
Parameters:  void
Return:  INT16U ≈–∂œ «∑Ò–Ë“™Ω¯––DC»Ì∆BUS     
 *********************************************************************/
INT16U sfBiDirDCCtrSoftStart(INT16S wBatInVolt,INT16S wBatOutVolt)//0,Œ¥ø™ º£¨1£¨ÕÍ≥…
{
INT16S wBusRefAim;
INT16U fDCSoftOK = 0;
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;

    wBusRefAim = wStdBusVolRef + cVolt20V;
    
    switch(uwDCSoftStep)
    {
    	case 0:
    		uwDelay1 = 0;
    		uwDelay2 = 0;
    		uwDCSoftStep++;
    		break;
    		
    	case 1:    		
    		if(fDCRelayPCtr == 0)
    		{
    			sDCSoftRelayClose();
    			
    			if(abs(wBatInVolt - wBatOutVolt) > cVolt60V)
    			{
    				uwDelay2 = 0;   				
    				if(++uwDelay1 >= 1000)
    				{
    					uwDelay1 = 0;
    					fDCSoftRelayLost = 1;
    				}
    			}
    			else
    			{   				
    				if(++uwDelay2 >= 150)
    				{
    					uwDelay2 = 0;
    					uwDelay1 = 0;
    					sDCRelayPClose();    					
    				}
    			}
    		}
    		else
    		{
    			if(abs(wBatInVolt - wBatOutVolt) < cVolt50V)
    			{
    				uwDelay2 = 0;
    				if(++uwDelay1 >= 50)
    				{
    					uwDelay1 = 0;
    					sDCSoftRelayOpen();
    					uwDCSoftStep++;
    				}
    			}
    			else
    			{
    				uwDelay1 = 0;
    				if(++uwDelay2 >= 10)
    				{
    					uwDelay2 = 0;
    					fDCRelayBurn = 1;
    				}
    			}
    		}
    		break;
    		
    	case 2:
    		sDCSoftRelayOpen();
    		uwDelay2 = 0;
    		if(++uwDelay1 >= 10)
    		{
    			uwDelay1 = 0;
    			uwDCSoftStep++;
    		}
    		break;
    		
    	case 3:
    		//uwDelay2 = 0;
    		sDCSoftRelayOpen();
    		wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
			wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
    		if(fDCModeSwitchEn == 0)
            {
            	if(wBatteryLimitCurrHigh > 2048)
            	{
            	    wBatteryLimitCurrHigh = 2048;
            	}
            	
            	if(wBatteryLimitCurrLow < -2048)
            	{
                    wBatteryLimitCurrLow = -2048;
                }
                
                if(wBusAvgVol < (cBusVolRefHigh + cVolt20V))
                {            
            	    wDCCtrBusVolRef = wBusAvgVol;// + cVolt5V;
                }
                else
                {
                	wDCCtrBusVolRef = cBusVolRefHigh + cVolt20V;// + cVolt5V;
                }
	        	fDCModeSwitchEn = 1;
            }
            else
            {
            	if((fBiDirDCEnOut >= 1)&&(fError == 0))
		        {
		            if(wDCCtrBusVolRef < (wBusRefAim - 8))
		            {
		            	wDCCtrBusVolRef = wDCCtrBusVolRef + 8;
		            }
		            else if(wDCCtrBusVolRef > (wBusRefAim + 8))
		            {
		            	wDCCtrBusVolRef = wDCCtrBusVolRef - 8;
		            }
		            else
		            {
		            	wDCCtrBusVolRef = wBusRefAim;
		            }
		            
		            if(wBusAvgVol >= wStdBusVolRef)
		            {
		            	if(++uwDelay1 >= 100)
		            	{
		            		uwDelay1 = 0;		    		
		            		uwDCSoftStep++; 
							
		            	}		
						uwDelay2=0;
		            }
		            else
		            {
						if(++uwDelay2>=500)fDCPWMLostPreChk=1;
						uwDelay1 = 0;
		            }
                }
    		}  		
            break;
            
        case 4:
        	sDCSoftRelayOpen();
        	uwDelay1 = 0;
        	uwDelay2 = 0;
        	fDCSoftOK = 1;
        	break;
        	
        default:
        	sDCSoftRelayOpen();
        	sDCRelayPOpen();
        	uwDelay1 = 0;
        	uwDelay2 = 0;
        	fDCSoftOK = 0;
        	break;    		
    }
    return(fDCSoftOK);
}
/*********************************************************************
Function name:INT16U sfBiDirDCCtrSoftStartByAC(INT16S wBatInVolt,INT16S wBatOutVolt)
Description: ≈–∂œ «∑Ò–Ë“™Ω¯––DC»Ì∆BUS
Calls:        	 
Called By:  sSysMode(void)
Parameters:  void
Return:  INT16U ≈–∂œ «∑Ò–Ë“™Ω¯––DC»Ì∆BUS     
 *********************************************************************/
INT16U sfBiDirDCCtrSoftStartByAC(INT16S wBatInVolt,INT16S wBatOutVolt)//0,Œ¥ø™ º£¨1£¨ÕÍ≥…
{
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;
INT16U fSoftByACOK = 0;
INT16S wDCRelayVoltDiff;

    wDCRelayVoltDiff = ((INT32S)wEepromDCRelayVoltDiff * 26214)>>14;

	switch(uwDCSoftStep)
	{
		case 0:
			uwDelay1 = 0;
			uwDelay2 = 0;			
			if(wBatteryLimitCurrHigh > 2048)
    	    {
    	        wBatteryLimitCurrHigh = 2048;
    	    }
    	    
    	    if(wBatteryLimitCurrLow < -2048)
    	    {
                wBatteryLimitCurrLow = -2048;
            }
			uwDCSoftStep = 1;
			break;
		
		case 1:
			if(wBusAvgVol > wStdBusVolRef)
			{
			    wInvCtrBusVolRef = wBusAvgVol;
			}
			else
			{
				wInvCtrBusVolRef = wStdBusVolRef;
			}
			if(fError == 0)
			{
				fInvEnOut = 1;
				uwDCSoftStep = 2;
			}		    
			break;
			
		case 2:
			if(wInvCtrBusVolRef < (wStdBusVolRef + cVolt20V + cVolt20V - 8))
			{
				wInvCtrBusVolRef = wInvCtrBusVolRef + 8;
			}
			else if(wInvCtrBusVolRef > (wStdBusVolRef + cVolt20V + cVolt20V + 8))
			{
				wInvCtrBusVolRef = wInvCtrBusVolRef - 8;
			}
			else
			{
				wInvCtrBusVolRef = wStdBusVolRef + cVolt20V + cVolt20V;
				if(fError == 0)
				{
				    uwDCSoftStep = 3;
				}				
			}
			break;
			
		case 3:
			wInvCtrBusVolRef = wStdBusVolRef + cVolt20V + cVolt20V;
			if(fDCRelayPCtr == 0)
			{
				uwDelay2 = 0;
				if(++uwDelay1 >= 5)
				{
					uwDelay1 = 0;
					uwDCSoftStep = 4;
				}				
			}
			else
			{
				uwDelay1 = 0;
				sDCSoftRelayOpen();
				if(++uwDelay2 >= 5)
				{
					uwDelay2 = 0;
					uwDCSoftStep = 5;
				}
			}
			break;
			
		case 4:
			wInvCtrBusVolRef = wStdBusVolRef + cVolt20V + cVolt20V;
			wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
			if(fDCModeSwitchEn == 0)
		    {
		    	if(wBatteryLimitCurrHigh > 2048)
    	        {
    	            wBatteryLimitCurrHigh = 2048;
    	        }
    	        
    	        if(wBatteryLimitCurrLow < -2048)
    	        {
                    wBatteryLimitCurrLow = -2048;
                }
                
                if(wBatInVolt < cVolt100V)
		    	{
		    		wBatVolLimtHigh = cVolt100V;
		    	}
		    	else
		    	{
		    	    wBatVolLimtHigh = wBatInVolt;
		    	}
                                
		    	wDCCtrBusVolRef = wStdBusVolRef + cVolt20V;
		    	fDCModeSwitchEn = 1;
		    }
		    else
		    {		    	
		    	if(abs(wBatVolLimtHigh - wBatInVolt) >= cVolt30V)
		        {
		        	if(++uwDelay2 >= 100)
		            {
		            	uwDelay2 = 100;
		            	fDCPWMLost = 1;
		            }
		        }
		        else
		        {
		        	uwDelay2 = 0;
		        }
		        
		        if(wBatOutVolt < cVolt100V)
		    	{
		    		wBatOutVolt = cVolt100V;
		    	}		    	
		    	
		    	if(wBatVolLimtHigh < (wBatOutVolt - 8))
		        {
		        	wBatVolLimtHigh = wBatVolLimtHigh + 8;
		        }
		        else if(wBatVolLimtHigh > (wBatOutVolt + 8))
		        {
		        	wBatVolLimtHigh = wBatVolLimtHigh - 8;
		        }
		        else
		        {
		        	wBatVolLimtHigh = wBatOutVolt;
		        	
		        	if(abs(wBatOutVolt - wBatInVolt) < wDCRelayVoltDiff)
		        	{		        		
		                if(++uwDelay1 >= 10)
		                {
		                	uwDelay1 = 0;
		                	sDCRelayPClose();		                	
		                	uwDCSoftStep = 6;		        			        		
		                }
		        	}
		        	else
		        	{
		        		uwDelay1 = 0;		        		
		        	}
		        }
		    }
		    break;

		case 5:
			wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
			wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
			wInvCtrBusVolRef = wStdBusVolRef + cVolt20V + cVolt20V;
			
			if(fDCModeSwitchEn == 0)
		    {
		    	if(wBatteryLimitCurrHigh > 2048)
    	        {
    	            wBatteryLimitCurrHigh = 2048;
    	        }
    	        
    	        if(wBatteryLimitCurrLow < -2048)
    	        {
                    wBatteryLimitCurrLow = -2048;
                }
                                
		    	wDCCtrBusVolRef = wStdBusVolRef + cVolt20V;
		    	fDCModeSwitchEn = 1;
		    }
		    else
		    {
		    	if(++uwDelay1 >= 10)
		    	{
		    		uwDelay1 = 0;
		    		uwDCSoftStep = 6;
		    	}
		    }
		    break;
		    
		case 6:
			wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
			wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
			wInvCtrBusVolRef = wStdBusVolRef + cVolt20V + cVolt20V;
			wDCCtrBusVolRef = wStdBusVolRef + cVolt20V;
			if(++uwDelay1 >= 3)
			{
				uwDelay1 = 0;
				uwDCSoftStep = 7;
			}
			break;
	    
	    case 7:
	    	fSoftByACOK = 1;
			uwDelay1 = 0;
			uwDelay2 = 0;
			uwDCSoftStep = 8;
	    	break;
			
		default:
			fSoftByACOK = 0;
			uwDelay1 = 0;
			uwDelay2 = 0;
			break;		
	}
	
	return(fSoftByACOK);
}
/*********************************************************************
Function name:void sDC12And34OptSwitch(void)
Description:
Calls:        	 
Called By:main 500ms
Parameters:void
Return:  INT16U ≈–∂œ «∑Ò–Ë“™Ω¯––DC»Ì∆BUS     
**********************************************************************/
void sDC12And34OptSwitch(void)
{
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;
static INT16U fBiDirDCSwitch = 0;
	if(uwSysMode == InvRunMode)
	{
		if(wEepromBiDirDCSwitch == 0)
		{
			uwDelay2 = 0;
			if(++uwDelay1 > 3600)
			{
				uwDelay1 = 3600;
				fBiDirDCSwitch = 1;
			}
		}
		else
		{
			uwDelay1 = 0;
			if(++uwDelay2 > 3600)
			{
				uwDelay2 = 3600;
				fBiDirDCSwitch = 2;
			}
		}
	}
	else
	{
		uwDelay1 = 0;
		uwDelay2 = 0;
		
		if(fBiDirDCSwitch == 1)
		{
			wEepromBiDirDCSwitch = 1;
			fEepromOperateRequire = 1;
            fHoldRegisterStatus = 2;
			fBiDirDCSwitch = 0;			
		}
		else if(fBiDirDCSwitch == 2)
		{
			wEepromBiDirDCSwitch = 0;
			fEepromOperateRequire = 1;
            fHoldRegisterStatus = 2;
			fBiDirDCSwitch = 0;
		}		
	}
}
/*********************************************************************
Function name:void sRelayDelayDeal(void)
Description:
Calls:        	 
Called By:main 10ms
Parameters:void
Return:   
**********************************************************************/
void sRelayDelayDeal(void)
{
	if(fAuxiRelayCtr == 1)
	{
		uwAuxiRelayDelay = 12000;
	}
	else
	{
		if(uwAuxiRelayDelay > 0)
		{
			uwAuxiRelayDelay--;
		}
		else
		{
			uwAuxiRelayDelay = 0;
		}
	}
	
	if(fDCSoftRelayCtr == 1)
	{
		uwDCSoftRelayDelay = 10000;
	}
	else
	{
		if(uwDCSoftRelayDelay > 0)
		{
			uwDCSoftRelayDelay--;
		}
		else
		{
			uwDCSoftRelayDelay = 0;
		}
	}
}
/*********************************************************************
Function name:void sACPowerDeal(INT16S wBatVolt)
Description:
Calls:        	 
Called By:main 20ms
Parameters:void
Return:   
**********************************************************************/
void sACPowerDeal(INT16S wBatVolt)
{
static INT16U uwDelay1 = 0;
static INT16S uwDelay2 = 0;
 
    if(uwSysMode >= PreCheckMode)
    {
        uwDelay2 = 0;
        if(wBatVolt >= 5120)//320V
        {
            if(++uwDelay1 >= 50)
            {
                uwDelay1 = 50;
                sACPowerDisable();
            }
        }
        else if(wBatVolt <= 4800)//300V
        {
            uwDelay1 = 0;
            sACPowerEnable();
        } 
    }  
    else
    {
        uwDelay1 = 0;
        if(wBatVolt >= 3968)//248
        {
            if(++uwDelay2 >= 50)
            {   
                uwDelay2 = 50;
                sACPowerDisable();
            }
        }
        else if(wBatVolt <= 3728)//233V
        {
            uwDelay2 = 0;
            sACPowerEnable();
        }
    }      
}
/*********************************************************************
Function name:void sBatteryDataUpdateChk(INT16S wBatPortVolt)
Description:
Calls:        	 
Called By:main 100ms
Parameters:void
Return:   
**********************************************************************/
void sBatteryDataUpdateChk(INT16S wBatPortVolt)
{
static INT16S wBatPortVoltBak = 0;
static INT16U uwDelay10S = 0;
static INT16U uwDelay1800S = 0;

    if(fBMSActive == 0)
    {
        uwDelay10S = 0;
        uwDelay1800S = 0;
        
        if((wBatPortVoltBak != wBatPortVolt)&&(wBatPortVolt >= 2000)&&(wBatPortVolt <= 9500))
        {
            fBMSActive = 1;
        }
    }
    else
    {
        if((wBatPortVolt < 2000)||(wBatPortVolt > 9500))
        {
            if(++uwDelay10S >= 200)
            {
                uwDelay10S =200;
                fBMSActive = 0;
            }
        }
        else
        {
            uwDelay10S = 0;
        }
                
        if((wBatPortVoltBak == wBatPortVolt)&&(abs(wBatteryAvgCurr) > 2560))
        {
            if(++uwDelay1800S >= 18000)
            {
                uwDelay1800S = 18000;
                fBMSActive = 0;
            }
        }
        else
        {
            uwDelay1800S = 0;
        }
    }  
    
    wBatPortVoltBak = wBatPortVolt; 
}
/*********************************************************************
Function name:void StatusPowerCalc(void)
Description:
Calls:        	 
Called By:main 20ms
Parameters:void
Return:   
**********************************************************************/

void StatusPowerCalc(void)
{
	wDebugTest5=fPCSState.all;

	fPCSState.bits.LVRT=(fFVRTType==1)?true:false;
	fPCSState.bits.HVRT=(fFVRTType==2)?true:false;
	
    fPCSState.bits.AI=false;
}
/********************************************** end of file *****************************************************/


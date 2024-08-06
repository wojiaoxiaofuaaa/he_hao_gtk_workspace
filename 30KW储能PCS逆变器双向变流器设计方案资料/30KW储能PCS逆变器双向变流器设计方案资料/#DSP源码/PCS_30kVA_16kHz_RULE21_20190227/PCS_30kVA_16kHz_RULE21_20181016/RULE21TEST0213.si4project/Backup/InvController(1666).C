/**************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  Invcontroller.c 
Description:  This file gives definition and implementation of Inverter control function
Version :  V1.0
Author:  Zhou xu  
Date:2011-11-17
**************************************/
/**** head-files including****/
#include "..\FunSrc\Funheader.h"

/**** macro definition****/
#define cFreq60Hz               15360 
#define cFreq50Hz               12800

//macro constant for Clarke and Park converter 
#define cDivide3                5461    //Q14
#define cSqrt3                  28378   //Q14
#define cDivideSqrt3            9459    //Q14
#define c3Divide2               24576   //Q14
                                
//macro constant for svm use    
#define cSqrt3Divide2           14189   //Q14 sqrt(3)/2 *Q14 = 14189
#define cSqrt3Divide3           9459    //Q14 sqrt(3)/3 *Q14 = 9459
        
//macro constant for PLL use          
#define cSinTabNumb             720     
#define cPLLMaxFreqAdjust       904     //Q4   [9Hz*2*pi*Q4 = 1005]
#define cPLLFreqAngleGain       42723   //Q20  1/16/8000/pi*180*32768/360     Q20
#define cPLLAngleGainOpen       16777   //cPLLFreqAngleGain * 2 * pi * 16 / 256  8kHz

#define cDCI40mA                320   //Q3
#define cDCI64mA                512   //Q3
#define cDCI150mA               1200  //Q3

#define cVSBusVoltFiltGainNum   158
#define cVSBusVoltFiltGainDen   3938

#define cBusSumDivideGridD      702    //8kHz
#define cBusSumVoltFiltGainNum	410//208//105
#define cBusSumVoltFiltGainDen	3686//3888//3991
#define cBusSumVoltVKp          5147//5147//2574
#define cBusSumVoltVKi          12//24//6
#define cBusSumVoltKaw          10//19//10

#define cDCCurrFiltGainNum      410    
#define cDCCurrFiltGainDen      3686
#define cDCCurrKp               1086//5147
#define cDCCurrKi               2//12
#define cDCCurrKaw              9//10

#define cBusDiffVoltFiltGainNum 0        //8kHz
#define cBusDiffVoltFiltGainDen 0 
#define cBusDiffVoltKp          774
#define cBusDiffVoltKi          1
#define cBusDiffVoltKaw         1
#define cInvOsCurrLimitHigh     16383
#define cInvOsCurrLimitLow     -16383

#define cGridVoltFiltGainNum       26
#define cGridVoltFiltGainDen       998
#define cGridVoltKp                2224//1112
#define cGridVoltKi                5//3
#define cGridVoltKaw               10//10

//==================================
//inverter control
#define cInvCurrFiltGainNum       3826//3498//3526//3498  //8kHz
#define cInvCurrFiltGainDen       270//598//570//598
#define cInvCurrKp                9780//4427//4632//4427
#define cInvCurrKi                355//57//130//57
#define cInvCurrKaw               149//53//115//53 

#define cVSCurrFiltGainNum       3045
#define cVSCurrFiltGainDen       1051
#define cVSCurrKp                4890
#define cVSCurrKi                89
#define cVSCurrKaw               74

#define cInertiaInvJ              3255//6510//15523//3255//31047//1/J/8000 Q20 J = D * T  0.02s
#define cVSVoltIngQKi             279//556//279//K = T * w * Kv; 0.01s

#define cInvPwmK                1135
#define cInvPwmKRec             7390
#define cInvModuLimitHigh       8191 
#define cInvModuLimitLow       -8191 

#define cInvChkUdComp           16321//16182    //16224//16135////15897
#define cInvChkUqComp           1427//2595     //2302//2889////4085

//macro constant for svm use
#define cMaxPwm                 8160//8191    //
#define cMinPwm                 32//15       // update on 20100225 for DPWMMIN //50
//==========================================

//macro definitions constant for DCI Control
#define cDCIReguHigh	        160
#define cDCIReguLow 	       -160

//macro definitions constant for AI
//#define cAIK1  12
#define cAIK1                   60
#define cAIK2                   23
#define cAIDeltaIqLimitUp       24000
#define cAIDeltaIqLimitLow     -24000

#define cCurrRefFiltDen         32640    //8kHz   5Hz
#define cCurrRefFiltNum         128

#define cPuGridVolK480V         2675     //vol*1024/480*1024

#define cPLLChkVolFiltDen       32512    //8kHz 20Hz
#define cPLLChkVolFiltNum       256
#define cCurrOR125A             32
#define cPwmDeadTime            275//175
#define cUqMaxTrip              720//512
#define cUqMaxExit              640
/**** variable-type parameters definition ****/
//variable-type parameters for PLL, for swith between 50Hz and 60Hz Grid  
INT32S dwPLLPdFiltGainNum    = 34312;   //8kHz
INT32S dwPLLPdFiltGainDen    = 31224;
INT16S wPLLFiltGainNum       = 1380;    //8kHz 50Hz
INT16S wPLLFiltGainDen       = 31388;
INT16S wPLLKp                = 10507;
INT16S wPLLKi                = 60;
INT16S wPLLKaw               = 188;
INT16S wPLLFreqSet           = 6032; 
INT16S wAIFreqSet            = 15360;   //Q8 Frequency = 60Hz*Q8
INT16U uwSysFreq             = 0;
INT16S wWL                   = 293;//120uH
INT16S wWC                   = 9;
INT16S wRGrid                = 410;
/**** variable definition ****/
T_THREE_PHASE_ABC tGridPhaseVoltAbc;
T_THREE_PHASE_ABC tInvCurrAbc;
T_THREE_PHASE_ABC tInvCurrAbcBak;
T_THREE_PHASE_ABC tInvCurrAbcRef;
T_THREE_DUTY_ABC tInvModuAbc;
T_THREE_PHASE_ABC tInvPWMModuAbc;
T_THREE_PHASE_ABC tPLLPosSeqGridVoltABC;
T_THREE_PHASE_ABC tPLLNosSeqGridVoltABC;

T_THREE_PHASE_AFBTGM tInvCurrAfBtGm;
T_THREE_PHASE_AFBTGM tGridVoltAfBtGm;
T_THREE_PHASE_AFBTGM tInvModuAfBtGm;
T_THREE_PHASE_AFBTGM tInvPosModuAfBtGm;
T_THREE_PHASE_AFBTGM tInvNosModuAfBtGm;
T_THREE_PHASE_AFBTGM tInvCurrAfBtGmRef;
T_THREE_PHASE_AFBTGM tInvNosCurrAfBtGmRef;
T_THREE_PHASE_AFBTGM tDeadCompModuAfBtGm;
T_THREE_PHASE_AFBTGM tPLLPosGridVoltSampAfbtgm;
T_THREE_PHASE_AFBTGM tPLLNosGridVoltSampAfbtgm;

T_THREE_PHASE_DQO tInvCurrDqoRef = {0,0,0}; 
T_THREE_PHASE_DQO tVSInvCurrDqoRef = {0,0,0}; 
T_THREE_PHASE_DQO tInvNosCurrDqoRef = {0,0,0};
T_THREE_PHASE_DQO tInvCurrDqo = {0,0,0};
T_THREE_PHASE_DQO tInvCurrTDqo = {0,0,0};
T_THREE_PHASE_DQO tInvVolDqo = {0,0,0};
T_THREE_PHASE_DQO tInvNosCurrDqo = {0,0,0};
T_THREE_PHASE_DQO tInvPosCurrSampDqo = {0,0,0};
T_THREE_PHASE_DQO tInvNosCurrSampDqo = {0,0,0};
T_THREE_PHASE_DQO tGridVoltDqo = {0,0,0};
T_THREE_PHASE_DQO tGridVoltTDqo = {0,0,0};
T_THREE_PHASE_DQO tGridNosVoltDqo = {0,0,0};
T_THREE_PHASE_DQO tGridPosVoltSampDqo = {0,0,0};
T_THREE_PHASE_DQO tGridNosVoltSampDqo = {0,0,0};
T_THREE_PHASE_DQO tInvModuDqo = {0,0,0};
T_THREE_PHASE_DQO tInvNosModuDqo = {0,0,0};
T_THREE_PHASE_DQO tInvCurrPowerReguRefDqo = {0,0,0};
T_THREE_PHASE_DQO tInvPosCurrDqoRef = {0,0,0};

//PLL uses global variables
T_THREE_PHASE_ABC tPLLPDGridVoltABCPrev; //Phase delay 90 filter uses the previous variables for delayed abc three-phase 
T_THREE_PHASE_ABC tPLLGridVoltABCPrev;   //Phase delay 90 filter uses the previous variables for normal abc three-phase
T_THREE_PHASE_ABC_dw tPLLPDGridTmp1ABC,tPLLPDGridTmp2ABC;
T_THREE_PHASE_ABC tPLLPDGridVoltABC,tPLLGridVoltBPFABC;
INT16S wPLLCoeffW                = 1544; //1029; //2*pi*50/16000*Q15
INT16S wPLLCoeffK                = 1480; //Q10*1.414=1448
INT16S wPLLSinTheta              = 0;         //wSinTabArray[0]=0;      
INT16S wPLLCosTheta              = 16384;     //wSinTabArray[0+720/4]=16384;     
INT16S wPLLNosSinTheta           = 0;     
INT16S wPLLNosCosTheta           = 16384;
INT16S wPLLOpenSinTheta          = 0;     
INT16S wPLLOpenCosTheta          = 16384;
INT16S wTorqueSinTheta           = 0;
INT16S wTorqueCosTheta           = 16384;
INT16S wPLLFiltUqPrev            = 0;
INT16S wPLLNosFiltUqPrev         = 0;
INT16S wPLLFreqSatErr            = 0;
INT16S wPLLNosFreqSatErr         = 0;
INT32S dwPLLFreqIngCtrl          = 0;
INT32S dwPLLNosFreqIngCtrl       = 0;
INT32S dwTorqueFreIngCtrl        = 0;
INT32U udwPLLAngleIngCtrl        = 0;
INT32U udwPLLAngleIngCtrlOpen    = 0;
INT32U udwNosPLLAngleIngCtrl     = 0;
INT32U udwPNPhDifPLLAngleIngCtrl = 0;
INT32U udwTorqueAngleIngCtrl     = 0;
INT32S dwPLLAngleIngOpen         = 0;
INT16S wPLLSinIndex              = 0;
INT16S wPLLCosIndex              = 0;
INT16S wPLLNosSinIndex           = 0;
INT16S wPLLNosCosIndex           = 0;
INT16S wPLLPNPhDifSinIndex       = 0;
INT16S wPLLPNPhDifCosIndex       = 0;
INT16S wTorqueSinIndex           = 0;
INT16S wTorqueCosIndex           = 0;
INT16S wPLLAngle                 = 0;
INT16S wPLLAngleOpen             = 0;
INT16S wPLLNosAngle              = 0;
INT16S wTorqueAngle              = 0;
INT16S wPLLFrequency             = 0;
INT16S wPLLNosFrequency          = 0;
INT16S wTorqueFrequency          = 0;
//**********inverter control global variables********************      
INT16S wBusSumVoltErrFiltPrev    = 0;
INT32S dwInvDsCurrSatErr         = 0;
INT32S dwBusSumVoltIngCtrl       = 0;

INT16S wBusDisChargeErrFiltPrev  = 0;
INT32S dwInvDisChargeCurrSatErr  = 0;
INT32S dwBusDisChargeIngCtrl     = 0;

INT16S wGridVoltDAxisErrFiltPrev = 0;
INT16S wGridDsCurrSatErr         = 0;
INT32S dwGridVoltDAxisIngCtrl    = 0;

INT16S wGridVoltQAxisErrFiltPrev = 0;
INT16S wGridVoltQAxisSatErr      = 0;
INT32S dwGridVoltQAxisIngCtrl    = 0;

INT16S wBusChargeErrFiltPrev     = 0;
INT32S dwInvChargeCurrSatErr     = 0;
INT32S dwBusChargeIngCtrl        = 0;

INT16S wInvVSCurrErrFiltPrev     = 0;
INT32S dwInvVSCurrSatErr         = 0;
INT32S dwInvVSCurrIngCtrl        = 0;

INT16S wDCCurrErrFiltPrev        = 0;
INT32S dwDCCurrSatErr            = 0;
INT32S dwDCCurrIngCtrl           = 0;

INT16S wBusDiffVoltErrFiltPrev   = 0;
INT32S dwInvOsCurrSatErr         = 0;
INT32S dwBusDiffVoltIngCtrl      = 0;
INT16S wInvOsCurrRef;
//current control D channel
INT16S wInvDsCurrErrFiltPrev     = 0;
INT32S dwInvDsModuSatErr         = 0;
INT32S dwInvDsCurrIngCtrl        = 0;

INT16S wVSInvDsCurrErrFiltPrev   = 0;
INT32S dwVSInvDsModuSatErr       = 0;
INT32S dwVSInvDsCurrIngCtrl      = 0;

INT16S wInvNosDsCurrErrFiltPrev  = 0;
INT32S dwInvNosDsModuSatErr      = 0;
INT32S dwInvNosDsCurrIngCtrl     = 0;
//current control Q channel
INT16S wInvQsCurrErrFiltPrev     = 0;
INT32S dwInvQsModuSatErr         = 0;
INT32S dwInvQsCurrIngCtrl        = 0;

INT16S wVSInvQsCurrErrFiltPrev   = 0;
INT32S dwVSInvQsModuSatErr       = 0;
INT32S dwVSInvQsCurrIngCtrl      = 0;

INT16S wInvNosQsCurrErrFiltPrev  = 0;
INT32S dwInvNosQsModuSatErr      = 0;
INT32S dwInvNosQsCurrIngCtrl     = 0;

//===============================================

//Define the global variables for DCI control   
INT16U uwRPhaseZeroCrossCnt = 0;

INT16S wDCIReguR = 0;
INT16S wDCIReguS = 0;
INT16S wDCIReguT = 0;

INT16U fDCIReguRefurbish = 0;   //=1 AD need to refurbish the DCI regulation value

INT16S wRefurbishDCIReguR = 0;
INT16S wRefurbishDCIReguS = 0;
INT16S wRefurbishDCIReguT = 0;

//Define the global variables for AI 
INT16S wAIGetInvDsCurrRef = 0;
INT16S wAISystemNoise = 0;
INT32S dwAIPLLFreqNoise = 0;
INT16S wAIDeltaIq = 0,wAIDeltaIqRef = 0;
INT16U fAIStart = 0;

//===reactive power compensation===
INT16S swReactiveCurrQs = 945;//768;
INT16S wGridPosDFF = 0,wGridPosQFF = 0;
INT16S wInvDsCurrLimitHigh = 8709,wInvDsCurrLimitLow = -13064;
INT16S wInvDsCurrLimitHighBak = 8709,wInvDsCurrLimitLowBak = -13064;
INT16U uwModuSoftIncCnt = 0,uwInvPwmBusComp = 0;

INT16U uwInvPwmKFeedBack = 1135;
INT32S dwAngleIngSub = 0;
INT32S dwAngleIngSubReal = 0;
INT32S dwAngleErr;
INT32S dwNosAngleErr;
INT16U fLVRTStartUpdate = 1;
INT16U fLVRTEndUpdate = 1;
INT16U fSoftLVRTEn = 0;
INT16U fUqUnconv = 0;
INT16U fUqUnconvBak = 0;
INT16S wDebugPLLDAxis = 0;
INT16S wDebugPLLQAxis = 0;

T_NotchOut_Prev tInvPosCurrDSampOut;
T_NotchIn_Prev  tInvPosCurrDSampIn;
T_NotchOut_Prev tInvPosCurrQSampOut;
T_NotchIn_Prev  tInvPosCurrQSampIn;
T_NotchOut_Prev tInvNosCurrDSampOut;
T_NotchIn_Prev  tInvNosCurrDSampIn;
T_NotchOut_Prev tInvNosCurrQSampOut;
T_NotchIn_Prev  tInvNosCurrQSampIn;

T_NotchOut_Prev tGridPosVolDSampOut;
T_NotchIn_Prev  tGridPosVolDSampIn;
T_NotchOut_Prev tGridPosVolQSampOut;
T_NotchIn_Prev  tGridPosVolQSampIn;
T_NotchOut_Prev tGridNosVolDSampOut;
T_NotchIn_Prev  tGridNosVolDSampIn;
T_NotchOut_Prev tGridNosVolQSampOut;
T_NotchIn_Prev  tGridNosVolQSampIn;

INT16U cNotch8KNum1 = 15648;    //8kHz
INT16U cNotch8KNum2 = 31158;
INT16U cNotch8KNum3 = 14912;
INT16S wDebugPosVol,wDebugNosVol;
INT16S wpuPosVolData[80] = {820,820,820,820,820,820,820,820,820,820,820,820,820,820,820,
                            820,820,820,820,820,820,820,820,820,820,820,820,820,820,820,
                            820,820,820,820,820,820,820,820,820,820,820,820,820,820,820,
                            820,820,820,820,820,820,820,820,820,820,820,820,820,820,820,
                            820,820,820,820,820,820,820,820,820,820,820,820,820,820,820,
                            820,820,820,820,820};
INT16S wpuNosVolData[80] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

INT16U uwFreqMidTemp[16] = {15360,15360,15360,15360,15360,15360,15360,15360,
	                        15360,15360,15360,15360,15360,15360,15360,15360};
INT32U udwFreqMidSum = 245760;
INT16U uwTwiceFreSampNum = 80;
INT16U uwSoftLVRTValue = 14745;
INT16U uwSoftHVRTValue = 18022; 
INT16U fFVRTType = 0; 
INT16U uwLVRTActDelay = 0;
INT16U uwLVRTExtDelay = 0;
INT32S dwBusDiffCtrlPrev = 0;
INT16U uwMasterCtrDelay150ms = 1200;
INT16S wInvCtrBusVolRef = 12480;
INT16S wBusDisChargeVoltRef = 12480;
INT16S wBusChargeVoltRef = 12480;
INT16U fInvCtrDeal = 0;
INT16S wTorqueM = 0;
INT16S wTorqueE = 0;
INT16S wTorqueD = 0;
INT16S wDeltaW = 0;
INT32S dwDampD = 12969;//2Hz->(30000/(2*pi*60)) Q10
INT32S dwDroopKQ = 8165;
INT16S wInvDisChargeCurrMax = 0;
INT16S wInvChargeCurrMin = 0;
INT32S dwDeltaDsCurr = 0;
INT16U fCPLDShutInvDeal = 0;
INT16U uwCPLDShutInvDelay = 0;
INT16S wDebugInvDsCurrRef = 0;
INT16S wDebugInvDCCurrRef = 0;
INT16S wDebugCPLDShutInv = 0;
INT16S wDebugAIStart = 0;
INT16S wDebugAIEnd = 0;
INT16S wDebugAIDeltaIqRef = 0;
INT16S wInvVoltLimit = 0;
INT16U uwVSStartDelay = 0;
INT16S wImaxTmp = 0;
INT16S wVoltDAim = 0;
INT16S wVoltQAim = 0;
INT16S wVSIdPrev = 0;
INT16S wVSIqPrev = 0;
/********************** Sin table ***********************/
const INT16S wSinTabArray[cSinTabNumb] =
{
     0,       143,     286,     429,     572,     715,     857,   1000,     1143,    1285,    1428,
  1570,      1713,    1855,    1997,    2139,    2280,    2422,   2563,     2704,    2845,    2986,
  3126,      3266,    3406,    3546,    3686,    3825,    3964,   4102,     4240,    4378,    4516,
  4653,      4790,    4927,    5063,    5199,    5334,    5469,   5604,     5738,    5872,    6005,
  6138,      6270,    6402,    6533,    6664,    6794,    6924,   7053,     7182,    7311,    7438,
  7565,      7692,    7818,    7943,    8068,    8192,    8316,   8438,     8561,    8682,    8803,
  8923,      9043,    9162,    9280,    9397,    9514,    9630,   9746,     9860,    9974,   10087,
  10199,    10311,   10422,   10531,   10641,   10749,   10856,  10963,    11069,   11174,   11278,
  11381,    11484,   11585,   11686,   11786,   11885,   11982,  12080,    12176,   12271,   12365,
  12458,    12551,   12642,   12733,   12822,   12911,   12998,  13085,    13170,   13255,   13338,
  13421,    13502,   13583,   13662,   13741,   13818,   13894,  13970,    14044,   14117,   14189,
  14260,    14330,   14399,   14466,   14533,   14598,   14663,  14726,    14788,   14849,   14909,
  14968,    15025,   15082,   15137,   15191,   15244,   15296,  15346,    15396,   15444,   15491,
  15537,    15582,   15626,   15668,   15709,   15749,   15788,  15826,    15862,   15897,   15931,
  15964,    15996,   16026,   16055,   16083,   16110,   16135,  16159,    16182,   16204,   16225,
  16244,    16262,   16279,   16294,   16309,   16322,   16333,  16344,    16353,   16362,   16368,
  16374,    16378,   16382,   16383,   16384,   16383,   16382,  16378,    16374,   16368,   16362,
  16353,    16344,   16333,   16322,   16309,   16294,   16279,  16262,    16244,   16225,   16204,
  16182,    16159,   16135,   16110,   16083,   16055,   16026,  15996,    15964,   15931,   15897,
  15862,    15826,   15788,   15749,   15709,   15668,   15626,  15582,    15537,   15491,   15444,
  15396,    15346,   15296,   15244,   15191,   15137,   15082,  15025,    14968,   14909,   14849,
  14788,    14726,   14663,   14598,   14533,   14466,   14399,  14330,    14260,   14189,   14117,
  14044,    13970,   13894,   13818,   13741,   13662,   13583,  13502,    13421,   13338,   13255,
  13170,    13085,   12998,   12911,   12822,   12733,   12642,  12551,    12458,   12365,   12271,
  12176,    12080,   11982,   11885,   11786,   11686,   11585,  11484,    11381,   11278,   11174,
  11069,    10963,   10856,   10749,   10641,   10531,   10422,  10311,    10199,   10087,    9974,
  9860,      9746,    9630,    9514,    9397,    9280,    9162,   9043,     8923,    8803,    8682,
  8561,      8438,    8316,    8192,    8068,    7943,    7818,   7692,     7565,    7438,    7311,
  7182,      7053,    6924,    6794,    6664,    6533,    6402,   6270,     6138,    6005,    5872,
  5738,      5604,    5469,    5334,    5199,    5063,    4927,   4790,     4653,    4516,    4378,
  4240,      4102,    3964,    3825,    3686,    3546,    3406,   3266,     3126,    2986,    2845,
  2704,      2563,    2422,    2280,    2139,    1997,    1855,   1713,     1570,    1428,    1285,
  1143,      1000,     857,     715,     572,     429,     286,    143,        0,    -143,    -286,
  -429,      -572,    -715,    -857,   -1000,   -1143,   -1285,  -1428,    -1570,   -1713,   -1855,
  -1997,    -2139,   -2280,   -2422,   -2563,   -2704,   -2845,  -2986,    -3126,   -3266,   -3406,
  -3546,    -3686,   -3825,   -3964,   -4102,   -4240,   -4378,  -4516,    -4653,   -4790,   -4927,
  -5063,    -5199,   -5334,   -5469,   -5604,   -5738,   -5872,  -6005,    -6138,   -6270,   -6402,
  -6533,    -6664,   -6794,   -6924,   -7053,   -7182,   -7311,  -7438,    -7565,   -7692,   -7818,
  -7943,    -8068,   -8192,   -8316,   -8438,   -8561,   -8682,  -8803,    -8923,   -9043,   -9162,
  -9280,    -9397,   -9514,   -9630,   -9746,   -9860,   -9974,  -10087,  -10199,  -10311,  -10422,
  -10531,  -10641,  -10749,  -10856,  -10963,  -11069,  -11174,  -11278,  -11381,  -11484,  -11585,
  -11686,  -11786,  -11885,  -11982,  -12080,  -12176,  -12271,  -12365,  -12458,  -12551,  -12642,
  -12733,  -12822,  -12911,  -12998,  -13085,  -13170,  -13255,  -13338,  -13421,  -13502,  -13583,
  -13662,  -13741,  -13818,  -13894,  -13970,  -14044,  -14117,  -14189,  -14260,  -14330,  -14399,
  -14466,  -14533,  -14598,  -14663,  -14726,  -14788,  -14849,  -14909,  -14968,  -15025,  -15082,
  -15137,  -15191,  -15244,  -15296,  -15346,  -15396,  -15444,  -15491,  -15537,  -15582,  -15626,
  -15668,  -15709,  -15749,  -15788,  -15826,  -15862,  -15897,  -15931,  -15964,  -15996,  -16026,
  -16055,  -16083,  -16110,  -16135,  -16159,  -16182,  -16204,  -16225,  -16244,  -16262,  -16279,
  -16294,  -16309,  -16322,  -16333,  -16344,  -16353,  -16362,  -16368,  -16374,  -16378,  -16382,
  -16383,  -16384,  -16383,  -16382,  -16378,  -16374,  -16368,  -16362,  -16353,  -16344,  -16333,
  -16322,  -16309,  -16294,  -16279,  -16262,  -16244,  -16225,  -16204,  -16182,  -16159,  -16135,
  -16110,  -16083,  -16055,  -16026,  -15996,  -15964,  -15931,  -15897,  -15862,  -15826,  -15788,
  -15749,  -15709,  -15668,  -15626,  -15582,  -15537,  -15491,  -15444,  -15396,  -15346,  -15296,
  -15244,  -15191,  -15137,  -15082,  -15025,  -14968,  -14909,  -14849,  -14788,  -14726,  -14663,
  -14598,  -14533,  -14466,  -14399,  -14330,  -14260,  -14189,  -14117,  -14044,  -13970,  -13894,
  -13818,  -13741,  -13662,  -13583,  -13502,  -13421,  -13338,  -13255,  -13170,  -13085,  -12998,
  -12911,  -12822,  -12733,  -12642,  -12551,  -12458,  -12365,  -12271,  -12176,  -12080,  -11982,
  -11885,  -11786,  -11686,  -11585,  -11484,  -11381,  -11278,  -11174,  -11069,  -10963,  -10856,
  -10749,  -10641,  -10531,  -10422,  -10311,  -10199,  -10087,   -9974,   -9860,   -9746,   -9630,
  -9514,    -9397,   -9280,   -9162,   -9043,   -8923,   -8803,   -8682,   -8561,   -8438,   -8316,
  -8192,    -8068,   -7943,   -7818,   -7692,   -7565,   -7438,   -7311,   -7182,   -7053,   -6924,
  -6794,    -6664,   -6533,   -6402,   -6270,   -6138,   -6005,   -5872,   -5738,   -5604,   -5469,
  -5334,    -5199,   -5063,   -4927,   -4790,   -4653,   -4516,   -4378,   -4240,   -4102,   -3964,
  -3825,    -3686,   -3546,   -3406,   -3266,   -3126,   -2986,   -2845,   -2704,   -2563,   -2422,
  -2280,    -2139,   -1997,   -1855,   -1713,   -1570,   -1428,   -1285,   -1143,   -1000,    -857,
  -715,      -572,    -429,    -286,    -143
    
}; //Q14

const INT32S dwATANArray[200] =
{
	         0,   10679838,   21354465,   32018685,   42667331,   53295284,   63897482, 
      74468939,   85004756,   95500135,  105950391,  116350962,  126697423,  136985493, 
     147211045,  157370116,  167458907,  177473799,  187411349,  197268300,  207041579, 
     216728303,  226325781,  235831508,  245243172,  254558647,  263775993,  272893455, 
     281909457,  290822599,  299631651,  308335554,  316933406,  325424463,  333808132, 
     342083962,  350251643,  358310992,  366261957,  374104599,  381839095,  389465727, 
     396984877,  404397019,  411702716,  418902610,  425997422,  432987938,  439875013, 
     446659557,  453342536,  459924966,  466407904,  472792449,  479079736,  485270931, 
     491367227,  497369841,  503280012,  509098996,  514828063,  520468494,  526021581, 
     531488619,  536870912,  547386471,  557578674,  567457882,  577034352,  586318199, 
     595319362,  604047574,  612512346,  620722945,  628688385,  636417416,  643918517, 
     651199896,  658269485,  665134944,  671803662,  678282760,  684579096,  690699270, 
     696649634,  702436292,  708065112,  713541732,  718871567,  724059819,  729111480, 
     734031343,  738824009,  743493896,  748045241,  752482114,  756808418,  765144167, 
     773080708,  780644054,  787858195,  794745269,  801325719,  807618432,  813640877, 
     819409224,  824938448,  830242437,  835334077,  840225337,  844927346,  849450458, 
     853804318,  862039645,  869698320,  876837123,  883505841,  889748273,  895603078, 
     901104491,  906282917,  915776268,  924267326,  931904609,  938809074,  950800217, 
     960851923,  976745925,  988737068,  998100390, 1005611794, 1011769873, 1016909365, 
    1021263225, 1024998517, 1028238139, 1031074493, 1033578384, 1035804973, 1037797869, 
    1039591999, 1041215667, 1042692047, 1044040301, 1045276403, 1046413782, 1047463804, 
    1048436158, 1049339150, 1050179943, 1050964748, 1051698975, 1052387359, 1053034062, 
    1053642757, 1054216699, 1054758781, 1055271583, 1055757415, 1056218349, 1056656251, 
    1057072805, 1057469535, 1057847822, 1058208924, 1058553984, 1058884050, 1059200076, 
    1059502941, 1059793449, 1060072341, 1060340301, 1060597958, 1060845896, 1061084655, 
    1061314734, 1061536598, 1061750681, 1061957383, 1062157081, 1062350124, 1062536840, 
    1062717534, 1062892493, 1063061986, 1063226265, 1063385567, 1063540115, 1063690118, 
    1063835774, 1063977270, 1064114781, 1064248472, 1064378502, 1064505018, 1064628160, 
    1064748063, 1064864852, 1064978647, 1065089561
};

#pragma CODE_SECTION(sPLLControl, "sInvControlRamFuncs")
#pragma CODE_SECTION(sInvControl, "sInvControlRamFuncs")
#pragma CODE_SECTION(sInvController, "sInvControlRamFuncs")
#pragma CODE_SECTION(sDciControl, "sInvControlRamFuncs")
#pragma CODE_SECTION(sClarke, "sInvControlRamFuncs")
#pragma CODE_SECTION(sPark, "sInvControlRamFuncs")
#pragma CODE_SECTION(sParkT, "sInvControlRamFuncs")
#pragma CODE_SECTION(sNosPark, "sInvControlRamFuncs")
#pragma CODE_SECTION(sInvClarke, "sInvControlRamFuncs")
#pragma CODE_SECTION(sInvPark, "sInvControlRamFuncs")
#pragma CODE_SECTION(sInvParkT, "sInvControlRamFuncs")
#pragma CODE_SECTION(sNosInvPark, "sInvControlRamFuncs")
#pragma CODE_SECTION(sdwArcTan, "sInvControlRamFuncs")
#pragma CODE_SECTION(sNotchFilter8K, "sInvControlRamFuncs")
#pragma CODE_SECTION(sCalLVRTReactivePowerRef, "sInvControlRamFuncs")
#pragma CODE_SECTION(sInvAndGridVolDeal, "sInvControlRamFuncs")
#pragma CODE_SECTION(sVSGControl, "sInvControlRamFuncs")
#pragma CODE_SECTION(sVSGBatCtrl, "sInvControlRamFuncs")
//------------------------------Data Ram-----------------------------------
#pragma DATA_SECTION(fCPLDShutInvDeal,"RamSafetyProt");
#pragma DATA_SECTION(uwCPLDShutInvDelay,"RamSafetyProt");
#pragma DATA_SECTION(fUqUnconv,"RamSafetyProt");
#pragma DATA_SECTION(fUqUnconvBak,"RamSafetyProt");
/************************** Functions Declare ******************************/
void sInvController(INT16S wPOBusVolSamp,INT16S wONBusVolSamp,T_THREE_PHASE_ABC wInvCurrSamp,T_THREE_PHASE_ABC wGridPhaseVoltSamp,INT16S wBatCurr,INT16U uwfInvCtrlFlg);
void sInvParamInit(void);
void sPLLParamInit(void);     
void sPLLControl(T_THREE_PHASE_ABC *ptPLLGridVoltAbc);
void sClarke(T_THREE_PHASE_ABC *ptAbc, T_THREE_PHASE_AFBTGM *ptAfBtGm);
void sPark(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO);
void sParkT(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO);
void sNosPark(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO);
void sInvClarke(T_THREE_PHASE_AFBTGM *ptAfBtGm,T_THREE_PHASE_ABC *ptAbc);
void sInvPark(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm);
void sInvParkT(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm);
void sNosInvPark(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm);
void sInvControl(INT16S wBusSumVoltSamp, T_THREE_PHASE_DQO *ptGridVolt, T_THREE_PHASE_DQO *ptInvCurr, T_THREE_PHASE_DQO *ptInvModu,T_THREE_PHASE_DQO *ptGridNosVolt,
                 T_THREE_PHASE_DQO *ptInvNosCurr, T_THREE_PHASE_DQO *ptInvNosModu,INT16S wBusDiffVoltSamp,INT16S wBattCurr);
void sSVModu(T_THREE_PHASE_AFBTGM *ptModuAfBtGm,T_THREE_PHASE_ABC *ptInvCurrAbc,INT16S wBusDiffCtrl,T_THREE_DUTY_ABC *ptModuAbc);
INT32S sdwArcTan(INT16S wUd,INT16S wUq);
void sNotchFilter8K(T_NotchOut_Prev *ptOutPrev,T_NotchIn_Prev *ptInPrev,INT16S InNow);
void sCalLVRTReactivePowerRef(T_THREE_PHASE_DQO *ptPosCurrRef,T_THREE_PHASE_DQO *ptNosCurrRef);
void sInvAndGridVolDeal(T_THREE_PHASE_ABC *ptInvPhaseAbc,T_THREE_PHASE_ABC *ptGridPhaseAbc);
void sVSGControl(INT16S wBusDiffVoltSamp, T_THREE_PHASE_DQO *ptGridVolt,T_THREE_PHASE_DQO *ptInvModu, T_THREE_PHASE_DQO *ptInvNosModu);
void sVSGBatCtrl(INT16S wBusSumVoltSamp);
/****************************Funtion Implement**********************************/
/***********************************************************************************
Function name:  void sPLLParamInit(void)
Description:  initialization global variables for PLL 
Calls:          
Called By:  Maybe: sGridFreqSelect(void), sGridVoltChk(void), sGridFreqChk(void)  
Parameters:  void
Return:  void
 ***********************************************************************************/
void sPLLParamInit(void)
{
    wPLLSinTheta = 0;
    wPLLCosTheta = 16384;  
    wPLLNosSinTheta = 0;     
    wPLLNosCosTheta = 16384;
    wPLLOpenSinTheta = 0;     
    wPLLOpenCosTheta = 16384;
    wTorqueSinTheta = 0;
    wTorqueCosTheta = 16384;
    fLVRTStartUpdate = 1;
    fLVRTEndUpdate = 1;
    tPLLPDGridVoltABCPrev.a = 0;
    tPLLPDGridVoltABCPrev.b = 0;
    tPLLPDGridVoltABCPrev.c = 0;
    tPLLGridVoltABCPrev.a = 0;
    tPLLGridVoltABCPrev.b = 0;
    tPLLGridVoltABCPrev.c = 0;
    
    tPLLPDGridTmp1ABC.a = 0;
    tPLLPDGridTmp1ABC.b = 0;
    tPLLPDGridTmp1ABC.c = 0;
    tPLLPDGridTmp2ABC.a = 0;
    tPLLPDGridTmp2ABC.b = 0;
    tPLLPDGridTmp2ABC.c = 0;
    tPLLPDGridVoltABC.a = 0;
    tPLLPDGridVoltABC.b = 0;
    tPLLPDGridVoltABC.c = 0;
    tPLLGridVoltBPFABC.a = 0;
    tPLLGridVoltBPFABC.b = 0;
    tPLLGridVoltBPFABC.c = 0;
    
    wPLLFiltUqPrev = 0;
    wPLLNosFiltUqPrev = 0;
    wPLLFreqSatErr = 0;
    wPLLNosFreqSatErr = 0;
    dwPLLFreqIngCtrl = 0;
    dwPLLNosFreqIngCtrl = 0;
    dwTorqueFreIngCtrl = 0;
    udwPLLAngleIngCtrl = 0;
    udwPLLAngleIngCtrlOpen = 0;
    udwNosPLLAngleIngCtrl = 0;
    udwPNPhDifPLLAngleIngCtrl = 0;
    udwTorqueAngleIngCtrl = 0;
	dwPLLAngleIngOpen = 0;
    wPLLFrequency = 0;
    wPLLNosFrequency = 0;
    wTorqueFrequency = 0;
    dwAngleIngSub = 0;
    dwAngleIngSubReal = 0;
    dwAngleErr = 0;
    dwNosAngleErr = 0;
    fUqUnconv = 0;
    if (uwSysFreq == cFreq60Hz)
    {
        dwPLLPdFiltGainNum = 34312;
        dwPLLPdFiltGainDen = 31224;
        wPLLFiltGainNum = 1380;
        wPLLFiltGainDen = 31388;
        wPLLKp = 10507;
        wPLLKi = 60;
        wPLLKaw = 188;
        wPLLFreqSet = 6032; 
		wAIFreqSet = 15360;   
        wWL = 293;
        wWC = 11;
        wPLLFrequency = 0;
        wPLLNosFrequency = 0;
        cNotch8KNum1 = 15648;    //8kHz
        cNotch8KNum2 = 31158;
        cNotch8KNum3 = 14912;
        uwTwiceFreSampNum = 67;
    }  
    else
    {     
        uwSysFreq = cFreq50Hz;
        dwPLLPdFiltGainNum = 34055;
        dwPLLPdFiltGainDen = 31481;   
        wPLLFiltGainNum = 1380;
        wPLLFiltGainDen = 31388;
        wPLLKp = 10507;
        wPLLKi = 60;
        wPLLKaw = 188;                    
        wPLLFreqSet = 5027; 
		wAIFreqSet = 12800; 
        wWL = 245;
        wWC = 9;
        wPLLFrequency = 0;
        wPLLNosFrequency = 0;        
        cNotch8KNum1 = 15766;   //8kHz
        cNotch8KNum2 = 31435;
        cNotch8KNum3 = 15148;
        uwTwiceFreSampNum = 80;        
    }   
}   

/*********************************************************************
Function name:  void sInvParamInit(void)
Description:  initialization global variable for inverter controll
Calls:          
Called By:  sPreChkMode(void)
Parameters:  void
Return:  void
 *********************************************************************/
void sInvParamInit(void)
{
    wBusSumVoltErrFiltPrev = 0;
    dwInvDsCurrSatErr = 0;
    dwBusSumVoltIngCtrl = 0;
    
    wDCCurrErrFiltPrev = 0;
    dwDCCurrSatErr = 0;
    dwDCCurrIngCtrl = 0;
    
    wBusDiffVoltErrFiltPrev = 0;
    dwInvOsCurrSatErr = 0;
    dwBusDiffVoltIngCtrl = 0;
    wInvOsCurrRef = 0;
        
    wInvDsCurrErrFiltPrev = 0;
    dwInvDsModuSatErr = 0;
    dwInvDsCurrIngCtrl = 0;
    
    wVSInvDsCurrErrFiltPrev = 0;
    dwVSInvDsModuSatErr = 0;
    dwVSInvDsCurrIngCtrl = 0;
    
    wInvNosDsCurrErrFiltPrev = 0;
    dwInvNosDsModuSatErr = 0;
    dwInvNosDsCurrIngCtrl = 0;
    
    wInvQsCurrErrFiltPrev = 0;
    dwInvQsModuSatErr = 0;
    dwInvQsCurrIngCtrl = 0;
    
    wVSInvQsCurrErrFiltPrev = 0;
    dwVSInvQsModuSatErr = 0;
    dwVSInvQsCurrIngCtrl = 0;
    
    wInvNosQsCurrErrFiltPrev = 0;
    dwInvNosQsModuSatErr = 0;
    dwInvNosQsCurrIngCtrl = 0;
    
    wGridVoltDAxisErrFiltPrev = 0;
    wGridDsCurrSatErr = 0;
    dwGridVoltDAxisIngCtrl = 0;

    wGridVoltQAxisErrFiltPrev = 0;
    wGridVoltQAxisSatErr = 0;
    dwGridVoltQAxisIngCtrl = 0;
 
    tInvModuAbc.A.word0 = 0;
    tInvModuAbc.B.word0 = 0;
	tInvModuAbc.C.word0 = 0;
	
	tInvCurrAbcRef.a = 0;
	tInvCurrAbcRef.b = 0;
	tInvCurrAbcRef.c = 0;

    tInvModuDqo.d = 0;
    tInvModuDqo.q = 0;
    tInvNosModuDqo.d = 0;
    tInvNosModuDqo.q = 0;
    tInvCurrDqoRef.d = 0;
    tInvCurrDqoRef.q = 0;
    tVSInvCurrDqoRef.d = 0;
    tVSInvCurrDqoRef.q = 0;
    tVSInvCurrDqoRef.o = 0;
    tInvCurrPowerReguRefDqo.d = 0;
    tInvCurrPowerReguRefDqo.q = 0;
    dwIqRef = 0;

    tInvPosCurrDqoRef.d = 0;
    tInvPosCurrDqoRef.q = 0;
    tInvPosCurrDqoRef.o = 0;
    tInvNosCurrDqoRef.d = 0;
    tInvNosCurrDqoRef.q = 0;
    tInvNosCurrDqoRef.o = 0;
    tInvCurrTDqo.d = 0;
    tInvCurrTDqo.q = 0;
    tInvCurrTDqo.o = 0;
    tInvCurrAfBtGmRef.Af = 0;
    tInvCurrAfBtGmRef.Bt = 0;
    tDeadCompModuAfBtGm.Af = 0;
    tDeadCompModuAfBtGm.Bt = 0;
    wDCIReguR = 0;
    wDCIReguS = 0;
    wDCIReguT = 0;
    wRefurbishDCIReguR = 0;
    wRefurbishDCIReguS = 0;
    wRefurbishDCIReguT = 0;
    fDCIReguRefurbish = 0;
    
    tInvPWMModuAbc.a = 0;
    tInvPWMModuAbc.b = 0;
    tInvPWMModuAbc.c = 0;

  	tInvPosCurrDSampOut.N0 = 0;
  	tInvPosCurrDSampOut.N1 = 0;
  	tInvPosCurrDSampOut.N2 = 0;
  	tInvPosCurrDSampIn.N0 = 0;
  	tInvPosCurrDSampIn.N1 = 0;
  	tInvPosCurrQSampOut.N0 = 0;
  	tInvPosCurrQSampOut.N1 = 0;
  	tInvPosCurrQSampOut.N2 = 0;
  	tInvPosCurrQSampIn.N0 = 0;
  	tInvPosCurrQSampIn.N1 = 0;
  	tInvNosCurrDSampOut.N0 = 0;
  	tInvNosCurrDSampOut.N1 = 0;
  	tInvNosCurrDSampOut.N2 = 0;
  	tInvNosCurrDSampIn.N0 = 0;
  	tInvNosCurrDSampIn.N1 = 0;
  	tInvNosCurrQSampOut.N0 = 0;
  	tInvNosCurrQSampOut.N1 = 0;
  	tInvNosCurrQSampOut.N2 = 0;
  	tInvNosCurrQSampIn.N0 = 0;
  	tInvNosCurrQSampIn.N1 = 0;
  	
  	tGridPosVolDSampOut.N0 = 0;
  	tGridPosVolDSampOut.N1 = 0;
  	tGridPosVolDSampOut.N2 = 0;
  	tGridPosVolDSampIn.N0 = 0;
  	tGridPosVolDSampIn.N1 = 0;
  	tGridPosVolQSampOut.N0 = 0;
  	tGridPosVolQSampOut.N1 = 0;
  	tGridPosVolQSampOut.N2 = 0;
  	tGridPosVolQSampIn.N0 = 0;
  	tGridPosVolQSampIn.N1 = 0;
  	tGridNosVolDSampOut.N0 = 0;
  	tGridNosVolDSampOut.N1 = 0;
  	tGridNosVolDSampOut.N2 = 0;
  	tGridNosVolDSampIn.N0 = 0;
  	tGridNosVolDSampIn.N1 = 0;
  	tGridNosVolQSampOut.N0 = 0;
  	tGridNosVolQSampOut.N1 = 0;
  	tGridNosVolQSampOut.N2 = 0;
  	tGridNosVolQSampIn.N0 = 0;
  	tGridNosVolQSampIn.N1 = 0;
    
    wAIGetInvDsCurrRef = 0;
    wAIDeltaIq = 0;
    wAIDeltaIqRef = 0;
    fAIStart = 0;
    wAISystemNoise = 0;
    dwAIPLLFreqNoise = 0;
    fFVRTType = 0;
    uwLVRTActDelay = 0;
    uwLVRTExtDelay = 0;
    dwBusDiffCtrlPrev = 0;
    uwMasterCtrDelay150ms = 1200;
    fInvCtrDeal = 0;
    dwDeltaDsCurr = 0;
    wTorqueM = 0;
    wTorqueE = 0;
    wTorqueD = 0;
    wDeltaW = 0;
    fCPLDShutInvDeal = 0;
    uwCPLDShutInvDelay = 0;  
    wDebugCPLDShutInv = 0;  
    dwTorqueFreIngCtrl = 0;

    uwVSStartDelay = 0;
    fATSForceCS2VS = 320;
    wVoltDAim = 0;
    wVoltQAim = 0;
    wVSIdPrev = 0;
    wVSIqPrev = 0;
}

/**************************************************************************************************************************************
Function name:void sInvController(INT16S wPOBusVolSamp,INT16S wONBusVolSamp,T_THREE_PHASE_ABC wInvCurrSamp,
                                  T_THREE_PHASE_ABC wGridPhaseVoltSamp,INT16U uwfInvCtrlFlg)
Description:  PLL and inverter closed-loop control algorithm 
Calls: sLinetoPhase(),sClarke,sPark,sInvPark,sPLLControl,sInvControl,sSVModu         
Called By:  Maybe, ADC interrupt
Parameters:  INT16S
Return:  void
 **************************************************************************************************************************************/
void sInvController(INT16S wPOBusVolSamp,INT16S wONBusVolSamp,T_THREE_PHASE_ABC wInvCurrSamp,T_THREE_PHASE_ABC wGridPhaseVoltSamp,INT16S wBatCurr,INT16U uwfInvCtrlFlg)
{
INT16S wBusSumVolt,wBusVoltDiff;
    //==============PLL==============
    tGridPhaseVoltAbc.a = wGridPhaseVoltSamp.a;
    tGridPhaseVoltAbc.b = wGridPhaseVoltSamp.b;
    tGridPhaseVoltAbc.c = wGridPhaseVoltSamp.c;
        
    tInvCurrAbc.a = wInvCurrSamp.a;
    tInvCurrAbc.b = wInvCurrSamp.b;
    tInvCurrAbc.c = wInvCurrSamp.c;
    wBusSumVolt = wPOBusVolSamp + wONBusVolSamp;
    wBusVoltDiff = wPOBusVolSamp - wONBusVolSamp;

    //=========PLL 8kHz=================
    sPLLControl(&tGridPhaseVoltAbc); 
    
    sClarke(&tInvCurrAbc, &tInvCurrAfBtGm);  
    sPark(&tInvCurrAfBtGm, &tInvCurrDqo);
    sNosPark(&tInvCurrAfBtGm, &tInvNosCurrDqo);
    
    sClarke(&tGridPhaseVoltAbc, &tGridVoltAfBtGm);              
    sPark(&tGridVoltAfBtGm, &tGridVoltDqo); 
    
	if(uwfInvCtrlFlg == 1)
	{	
	    if(fCSVSSwitch == 0)
	    {	
		    sNotchFilter8K(&tInvPosCurrDSampOut,&tInvPosCurrDSampIn,tInvCurrDqo.d);   
            sNotchFilter8K(&tInvPosCurrQSampOut,&tInvPosCurrQSampIn,tInvCurrDqo.q); 
            sNotchFilter8K(&tInvNosCurrDSampOut,&tInvNosCurrDSampIn,tInvNosCurrDqo.d);   
            sNotchFilter8K(&tInvNosCurrQSampOut,&tInvNosCurrQSampIn,tInvNosCurrDqo.q);
                
		    if((fImbalanceLVRT == 1)&&(fLVRTStartUpdate == 0))
            { 
                sClarke(&tPLLPosSeqGridVoltABC,&tPLLPosGridVoltSampAfbtgm);
                sClarke(&tPLLNosSeqGridVoltABC,&tPLLNosGridVoltSampAfbtgm);
                sPark(&tPLLPosGridVoltSampAfbtgm,&tGridPosVoltSampDqo);
                sNosPark(&tPLLNosGridVoltSampAfbtgm,&tGridNosVoltSampDqo); 
                
                tInvPosCurrSampDqo.d = tInvPosCurrDSampOut.N2>>14;
                tInvPosCurrSampDqo.q = tInvPosCurrQSampOut.N2>>14;
                tInvPosCurrSampDqo.o = tInvCurrDqo.o;
                tInvNosCurrSampDqo.d = tInvNosCurrDSampOut.N2>>14;
                tInvNosCurrSampDqo.q = tInvNosCurrQSampOut.N2>>14;
            }
            else
            {
                tGridPosVoltSampDqo.d = tGridVoltDqo.d;
                tGridPosVoltSampDqo.q = tGridVoltDqo.q;  
                tGridNosVoltSampDqo.d = 0;
                tGridNosVoltSampDqo.q = 0;
                
                tInvPosCurrSampDqo.d = tInvCurrDqo.d;
                tInvPosCurrSampDqo.q = tInvCurrDqo.q;
                tInvPosCurrSampDqo.o = tInvCurrDqo.o;
                tInvNosCurrSampDqo.d = 0;
                tInvNosCurrSampDqo.q = 0;
            }
            sCalLVRTReactivePowerRef(&tInvPosCurrDqoRef,&tInvNosCurrDqoRef);
            
            sInvControl(wBusSumVolt,&tGridPosVoltSampDqo,&tInvPosCurrSampDqo,&tInvModuDqo,&tGridNosVoltSampDqo,
                        &tInvNosCurrSampDqo,&tInvNosModuDqo,wBusVoltDiff,wBatCurr);
                        
            sInvPark(&tInvModuDqo, &tInvPosModuAfBtGm);
	        sNosInvPark(&tInvNosModuDqo, &tInvNosModuAfBtGm);
	        tInvModuAfBtGm.Af = tInvPosModuAfBtGm.Af + tInvNosModuAfBtGm.Af;
            tInvModuAfBtGm.Bt = tInvPosModuAfBtGm.Bt + tInvNosModuAfBtGm.Bt;
	        
	        sInvPark(&tInvCurrDqoRef, &tInvCurrAfBtGmRef);  
	        sNosInvPark(&tInvNosCurrDqoRef, &tInvNosCurrAfBtGmRef);
	        tInvCurrAfBtGmRef.Af = tInvCurrAfBtGmRef.Af + tInvNosCurrAfBtGmRef.Af;
	        tInvCurrAfBtGmRef.Bt = tInvCurrAfBtGmRef.Bt + tInvNosCurrAfBtGmRef.Bt;    
	        
	        tVSInvCurrDqoRef.d = tInvCurrDqoRef.d;
	        tVSInvCurrDqoRef.q = tInvCurrDqoRef.q;
	        
	        wVSInvDsCurrErrFiltPrev = wInvDsCurrErrFiltPrev;
            dwVSInvDsModuSatErr = dwInvDsModuSatErr;
            dwVSInvDsCurrIngCtrl = dwInvDsCurrIngCtrl;
            
            wVSInvQsCurrErrFiltPrev = wInvQsCurrErrFiltPrev;
            dwVSInvQsModuSatErr = dwInvQsModuSatErr;
            dwVSInvQsCurrIngCtrl = dwInvQsCurrIngCtrl;                
        }
        else
        {
            sVSGControl(wBusVoltDiff,&tGridVoltDqo,&tInvModuDqo,&tInvNosModuDqo);
            sInvParkT(&tInvModuDqo, &tInvModuAfBtGm);
            sInvParkT(&tVSInvCurrDqoRef,&tInvCurrAfBtGmRef); 
            
            tInvCurrDqoRef.d = tVSInvCurrDqoRef.d;
	        tInvCurrDqoRef.q = tVSInvCurrDqoRef.q;
	        
	        dwBusSumVoltIngCtrl = -((INT32S)tInvCurrDqoRef.d<<12);
	        dwDCCurrIngCtrl = -((INT32S)tInvCurrDqoRef.d<<12);
	        
	        wInvDsCurrErrFiltPrev = wVSInvDsCurrErrFiltPrev;
            dwInvDsModuSatErr = dwVSInvDsModuSatErr;
            dwInvDsCurrIngCtrl = dwVSInvDsCurrIngCtrl;
            
            wInvQsCurrErrFiltPrev = wVSInvQsCurrErrFiltPrev;
            dwInvQsModuSatErr = dwVSInvQsModuSatErr;
            dwInvQsCurrIngCtrl = dwVSInvQsCurrIngCtrl;
        }
                                 
        
        sInvClarke(&tInvModuAfBtGm,&tInvPWMModuAbc);	
        sInvClarke(&tInvCurrAfBtGmRef,&tInvCurrAbcRef);
                 
        sSVModu(&tInvModuAfBtGm,&tInvCurrAbc,wInvOsCurrRef,&tInvModuAbc);//tInvCurrAbc  tInvCurrAbcBak
	}
    else
    {
    	sInvParamInit();
    }  
}
/*********************************************************************
Function name:  void sPLLControl(T_THREE_PHASE_ABC *ptPLLGridVoltAbc)
Description:  Phase-locked-loop function for inverter control and AI
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_ABC, means grid phase voltage, not grid line voltage
Return:  void      
*********************************************************************/
void sPLLControl(T_THREE_PHASE_ABC *ptPLLGridVoltAbc)
{ 
    //Used local variables in this function
    //T_THREE_PHASE_ABC tPLLPDGridVoltABC;
    T_THREE_PHASE_AFBTGM tPLLPosGridVoltAfbtgm;
    T_THREE_PHASE_AFBTGM tPLLNosGridVoltAfbtgm;   
    //T_THREE_PHASE_ABC tPLLPDGridErr1ABC,tPLLPDGridErr2ABC; 
    INT16S wPLLPosUq = 0;
    INT16S wPLLPosUd = 0;
    INT16S wPLLNosUq = 0;
    INT16S wPLLNosUd = 0;    
    
    INT16S wPLLFiltUq = 0;
    INT16S wPLLNosFiltUq = 0;
    INT32U udwTemp;
    INT16S wPLLFreqReguPreSat = 0;
    INT16S wPLLFreqRegu = 0;
    static INT16U uwLVRTStartDelay10ms = 0;  
    static INT16U uwLVRTEndDelay10ms = 0;    
    static INT16U uwDelay500ms = 0;  
    static INT16U uwSoftLVRTDelay = 0;
    static INT16U uwGFCIDelay = 0;
     
    //All Pass Filter with Phase Delay 90 degree, thransfer function: G(s) = [-(1/(2pi*f))s+1]/[(1/(2pi*f))s+1],f=50Hz or 60Hz
    tPLLPDGridVoltABC.a = ((INT16S)(((INT32S)tPLLPDGridVoltABCPrev.a * dwPLLPdFiltGainDen) >> 15) - ptPLLGridVoltAbc->a) + (INT16S)(((INT32S)tPLLGridVoltABCPrev.a * dwPLLPdFiltGainNum) >> 15);
    tPLLPDGridVoltABC.b = ((INT16S)(((INT32S)tPLLPDGridVoltABCPrev.b * dwPLLPdFiltGainDen) >> 15) - ptPLLGridVoltAbc->b) + (INT16S)(((INT32S)tPLLGridVoltABCPrev.b * dwPLLPdFiltGainNum) >> 15);
    tPLLPDGridVoltABC.c = ((INT16S)(((INT32S)tPLLPDGridVoltABCPrev.c * dwPLLPdFiltGainDen) >> 15) - ptPLLGridVoltAbc->c) + (INT16S)(((INT32S)tPLLGridVoltABCPrev.c * dwPLLPdFiltGainNum) >> 15);
    
    tPLLPDGridVoltABCPrev.a = tPLLPDGridVoltABC.a;
    tPLLPDGridVoltABCPrev.b = tPLLPDGridVoltABC.b;
    tPLLPDGridVoltABCPrev.c = tPLLPDGridVoltABC.c;
    
    tPLLGridVoltABCPrev.a = ptPLLGridVoltAbc->a;
    tPLLGridVoltABCPrev.b = ptPLLGridVoltAbc->b;
    tPLLGridVoltABCPrev.c = ptPLLGridVoltAbc->c;
    
    //************************************************************************************************************    
    //Detect positive sequence components of grid voltage 
    tPLLPosSeqGridVoltABC.a = (INT16S)((((INT32S)ptPLLGridVoltAbc->a * 2 - (ptPLLGridVoltAbc->b + ptPLLGridVoltAbc->c) - (INT16S)(((INT32S)(tPLLPDGridVoltABC.b - tPLLPDGridVoltABC.c) * cSqrt3) >> 14)) * cDivide3)>> 15);
    tPLLPosSeqGridVoltABC.c = (INT16S)((((INT32S)ptPLLGridVoltAbc->c * 2 - (ptPLLGridVoltAbc->a + ptPLLGridVoltAbc->b) - (INT16S)(((INT32S)(tPLLPDGridVoltABC.a - tPLLPDGridVoltABC.b) * cSqrt3) >> 14)) * cDivide3)>> 15);
    tPLLPosSeqGridVoltABC.b = -(tPLLPosSeqGridVoltABC.a + tPLLPosSeqGridVoltABC.c);  
    
    tPLLNosSeqGridVoltABC.a = (INT16S)((((INT32S)ptPLLGridVoltAbc->a * 2 - (ptPLLGridVoltAbc->b + ptPLLGridVoltAbc->c) + (INT16S)(((INT32S)(tPLLPDGridVoltABC.b - tPLLPDGridVoltABC.c) * cSqrt3) >> 14)) * cDivide3)>> 15);
    tPLLNosSeqGridVoltABC.c = (INT16S)((((INT32S)ptPLLGridVoltAbc->c * 2 - (ptPLLGridVoltAbc->a + ptPLLGridVoltAbc->b) + (INT16S)(((INT32S)(tPLLPDGridVoltABC.a - tPLLPDGridVoltABC.b) * cSqrt3) >> 14)) * cDivide3)>> 15);   
    tPLLNosSeqGridVoltABC.b = -(tPLLNosSeqGridVoltABC.a + tPLLNosSeqGridVoltABC.c);
    
    //ABC reference frame to alfa-beta-gama reference frame, gama = 0
    sClarke(&tPLLPosSeqGridVoltABC, &tPLLPosGridVoltAfbtgm);
    sClarke(&tPLLNosSeqGridVoltABC, &tPLLNosGridVoltAfbtgm);
    //alfa-beta-gama reference frame to d-q-o reference frame, O = 0
    wPLLPosUq = (INT16S)(((INT32S)tPLLPosGridVoltAfbtgm.Af * (-wPLLSinTheta) + (INT32S)tPLLPosGridVoltAfbtgm.Bt * wPLLCosTheta) >> 14);
    wPLLPosUd = (INT16S)(((INT32S)tPLLPosGridVoltAfbtgm.Af * wPLLCosTheta + (INT32S)tPLLPosGridVoltAfbtgm.Bt * wPLLSinTheta) >> 14);    
    wGridPosDFF = wPLLPosUd;
    wGridPosQFF = wPLLPosUq;
    
    if((fInvEnOut == 1)||(uwSysMode == InvRunMode))  //for GFCI 后续考虑移到主程序中执行
    {
        if(abs(wPLLPosUq) > cUqMaxTrip)
        {
        	fUqUnconv = 1;
        	uwGFCIDelay = 0;
        }
        else if(abs(wPLLPosUq) < cUqMaxExit)
        {
            if(++uwGFCIDelay >= 2400)
            {
                uwGFCIDelay = 2400;
                fUqUnconv = 0;
            }
        }
    }
    else
    {
        fUqUnconv = 0;
        uwGFCIDelay = 0;
    }  
              
    wPLLNosUq = (INT16S)(((INT32S)tPLLNosGridVoltAfbtgm.Af * (-wPLLNosSinTheta) + (INT32S)tPLLNosGridVoltAfbtgm.Bt * wPLLNosCosTheta) >> 14);
    wPLLNosUd = (INT16S)(((INT32S)tPLLNosGridVoltAfbtgm.Af * wPLLNosCosTheta + (INT32S)tPLLNosGridVoltAfbtgm.Bt * wPLLNosSinTheta) >> 14);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //LPF 
    wPLLFiltUq = (INT16S)(((INT32S)wPLLFiltUqPrev * wPLLFiltGainDen + (INT32S)wPLLPosUq * wPLLFiltGainNum) >> 15);
    wPLLFiltUqPrev = wPLLFiltUq;
    //Integral
    dwPLLFreqIngCtrl = dwPLLFreqIngCtrl + (INT32S)wPLLFiltUq * wPLLKi + (INT32S)wPLLFreqSatErr * wPLLKaw;
    
    //PI
    wPLLFreqReguPreSat = (INT16S)(((INT32S)wPLLFiltUq * wPLLKp + dwPLLFreqIngCtrl) >> 15);
    
    //Limit of Regulated frequecny value
    if (wPLLFreqReguPreSat > cPLLMaxFreqAdjust)        
    {
        wPLLFreqRegu = cPLLMaxFreqAdjust;
    }
    else if (wPLLFreqReguPreSat < -cPLLMaxFreqAdjust)
    {
        wPLLFreqRegu = -cPLLMaxFreqAdjust;
    }
    else
    {
        wPLLFreqRegu = wPLLFreqReguPreSat;
    }
    //Saturation error for anti-windup
    wPLLFreqSatErr = wPLLFreqRegu - wPLLFreqReguPreSat;   
    
    wPLLFrequency = wPLLFreqRegu + wPLLFreqSet;
    //PLL angle obtained through integral of frequency
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //------------------------------------------------------------------------
    wPLLNosUq = 0 - wPLLNosUq;
    wPLLNosFiltUq = (INT16S)(((INT32S)wPLLNosFiltUqPrev * wPLLFiltGainDen + (INT32S)wPLLNosUq * wPLLFiltGainNum) >> 15);
    wPLLNosFiltUqPrev = wPLLNosFiltUq;
    //Integral
    dwPLLNosFreqIngCtrl = dwPLLNosFreqIngCtrl + (INT32S)wPLLNosFiltUq * wPLLKi + (INT32S)wPLLNosFreqSatErr * wPLLKaw;    
    //PI
    wPLLFreqReguPreSat = (INT16S)(((INT32S)wPLLNosFiltUq * wPLLKp + dwPLLNosFreqIngCtrl) >> 15);
    //Limit of Regulated frequecny value
    if (wPLLFreqReguPreSat > cPLLMaxFreqAdjust)        
    {
        wPLLFreqRegu = cPLLMaxFreqAdjust;
    }
    else if (wPLLFreqReguPreSat < -cPLLMaxFreqAdjust)
    {
        wPLLFreqRegu = -cPLLMaxFreqAdjust;
    }
    else
    {
        wPLLFreqRegu = wPLLFreqReguPreSat;
    }
    //Saturation error for anti-windup
    wPLLNosFreqSatErr = wPLLFreqRegu - wPLLFreqReguPreSat;   
    wPLLNosFrequency = wPLLFreqRegu + wPLLFreqSet;
    //---------------------------------------------------------------------------   
    if(fCSVSSwitch == 0)
    {   
        uwSoftLVRTValue = uwLVRTActiveValue + 819;
        if(uwSoftLVRTValue > 14745) 
        {
        	uwSoftLVRTValue = 14745;
        }
        
        uwSoftHVRTValue = uwHVRTActiveValue - 819;
        if(uwSoftHVRTValue < 18022)
        {
        	uwSoftHVRTValue = 18022;
        }
          
        if((fLVRTValue <= uwLVRTActiveValue)||(fHVRTValue >= uwHVRTActiveValue))//||(fImbalanceLVRT == 1))   //故障穿越发生时开始计算，延时10ms后，不再计算。
        {
            fLVRTEndUpdate = 1;
            uwLVRTEndDelay10ms = 0;  
            fQCtrEnDelay2s = 0;//Disable sAdjustPF 
            fFVRTMaxCurrDelay2s = 0;
            uwFVRTCurrSoftDelay = 0;            
            
            uwSoftLVRTDelay = 0;
            fSoftLVRTEn = 0;
        
            if((fLVRTValue > 1474)||(fImbalanceLVRT == 1))   //跌落足够大，或者当前为不平衡跌落，则进行计算
            {
                if(fLVRTStartUpdate == 1)    //角度刷新未完成
                {
                    dwAngleErr = sdwArcTan(wPLLPosUd,wPLLPosUq);
                    wPLLNosUq = - wPLLNosUq;
                    dwNosAngleErr = sdwArcTan(wPLLNosUd,wPLLNosUq);  
          
                    if(++uwLVRTStartDelay10ms > 80)
                    {
                    	uwLVRTStartDelay10ms = 0;
                    	udwPLLAngleIngCtrl = udwPLLAngleIngCtrl + dwAngleErr;
                    	udwNosPLLAngleIngCtrl = udwNosPLLAngleIngCtrl + dwNosAngleErr;
                    	wPLLFreqSatErr = 0;
                    	wPLLFiltUqPrev = 0;
                    	dwPLLFreqIngCtrl = 0;
                    	wPLLFrequency = wPLLFreqSet;//wPLLFreqSet;
                    	wPLLNosFreqSatErr = 0;
                    	wPLLNosFiltUqPrev = 0;
                    	dwPLLNosFreqIngCtrl = 0;
                    	wPLLNosFrequency = wPLLFreqSet;//wPLLFreqSet;
                    	fLVRTStartUpdate = 0;          //禁止刷新
                    }
                }
                else
                {
                    uwLVRTStartDelay10ms = 0;
                }            
            }
            else          //跌落足够小，且为平衡跌落
            {
                if(++uwLVRTStartDelay10ms > 8)
                {
                    uwLVRTStartDelay10ms = 8;
                    //udwPLLAngleIngCtrl = udwPLLAngleIngCtrlOpen - dwAngleIngSubReal;
                    //wPLLFreqSatErr = 0;
                    //wPLLFiltUqPrev = 0;
                    //wPLLFrequency = uwGridFreq;//wPLLFreqSet;
                    fLVRTStartUpdate = 0;          //禁止刷新
                }      
            }        
        }
        else if((fLVRTValue <= uwSoftLVRTValue)||(fHVRTValue >= uwSoftHVRTValue))
        {
            fLVRTEndUpdate = 1;
            uwLVRTEndDelay10ms = 0;
            fQCtrEnDelay2s = 0;
            fFVRTMaxCurrDelay2s = 0;
            //uwLVRTStartDelay10ms = 0; //need?
            //fLVRTStartUpdate = 0;     //need?
            if(fSoftLVRTEn == 0)
            {
                dwAngleErr = sdwArcTan(wPLLPosUd,wPLLPosUq);
                wPLLNosUq = - wPLLNosUq;
                dwNosAngleErr = sdwArcTan(wPLLNosUd,wPLLNosUq);  
                if(++uwSoftLVRTDelay > 80)   //120 for Nanjin LVRT Test
                {
                	uwSoftLVRTDelay = 0;
                	udwPLLAngleIngCtrl = udwPLLAngleIngCtrl + dwAngleErr;
                	udwNosPLLAngleIngCtrl = udwNosPLLAngleIngCtrl + dwNosAngleErr;
                	wPLLFreqSatErr = 0;
                	wPLLFiltUqPrev = 0;
                	dwPLLFreqIngCtrl = 0;
                	wPLLFrequency = wPLLFreqSet;//wPLLFreqSet;
                	wPLLNosFreqSatErr = 0;
                	wPLLNosFiltUqPrev = 0;
                	dwPLLNosFreqIngCtrl = 0;
                	wPLLNosFrequency = wPLLFreqSet;//wPLLFreqSet;
                	fSoftLVRTEn = 1;
                }
            }
            else
            {
                uwSoftLVRTDelay = 0;
            }
        }
        else
        {
            fLVRTStartUpdate = 1;
            fImbalanceLVRT = 0;
            uwLVRTStartDelay10ms = 0;
            uwSoftLVRTDelay = 0;
            fSoftLVRTEn = 0;
            if(fLVRTEndUpdate == 1)
            {
                dwAngleErr = sdwArcTan(wPLLPosUd,wPLLPosUq);
                if(++uwLVRTEndDelay10ms > 50)//60 for Nanjing LVRT Test
                {
                    udwPLLAngleIngCtrl = udwPLLAngleIngCtrl + dwAngleErr;
                	wPLLFreqSatErr = 0;
                	wPLLFiltUqPrev = 0;   
                	dwPLLFreqIngCtrl = 0;
                	wPLLFrequency = wPLLFreqSet;//wPLLFreqSet;
                	uwLVRTEndDelay10ms = 0;
                	fLVRTEndUpdate = 0;
                }
            }
            else
            {
                uwLVRTEndDelay10ms = 0;
            }   
        }
        if(((fLVRTValue <= uwLVRTActiveValue)||(fHVRTValue >= uwHVRTActiveValue))&&(fLVRTStartUpdate == 0))//跌落发生且更新完毕
        {
            fLVRTStartOrEnd = 1;
	    	fLVRTStartOrEndBak = 1;
            uwDelay500ms = 0;
        }
        else if((fLVRTValue > uwSoftLVRTValue)&&(fHVRTValue < uwSoftHVRTValue))
        {
        	if(wEepromStandardValue == cBDEWStandard)
            {
                if(fLVRTEndUpdate == 0)
                {
                    if(++uwDelay500ms > 4000)
                    {
                        fLVRTStartOrEnd = 0;
                        uwDelay500ms = 4000;
                    }
	    	        fLVRTStartOrEndBak = 0;
                }
            }
            else
            {
            	if(fLVRTEndUpdate == 0)
                {                
                    fLVRTStartOrEnd = 0;
                    uwDelay500ms = 0;
	    	    	fLVRTStartOrEndBak = 0;
                }
            }
        }
        else if((fLVRTStartUpdate == 1)&&(wEepromStandardValue == cBDEWStandard))//在500ms延时时间内，再次发生跌落
        {
            fLVRTStartOrEnd = 0;
	    	fLVRTStartOrEndBak = 0;
            uwDelay500ms = 0;
        }
    }
    else
    {
        fLVRTStartOrEnd = 0;
        fLVRTStartOrEndBak = 0;
        uwDelay500ms = 0;
        fLVRTStartUpdate = 1;
        fLVRTEndUpdate = 1;
        uwLVRTStartDelay10ms = 0;  
        uwLVRTEndDelay10ms = 0; 
        uwSoftLVRTDelay = 0;
    }
    //正序角更新
    udwTemp = ((INT32U)wPLLFrequency * cPLLFreqAngleGain)>>3;
    udwPLLAngleIngCtrl = udwPLLAngleIngCtrl + udwTemp;
    udwTemp = udwPLLAngleIngCtrl>>20;
    wPLLAngle = (udwTemp * 720)>>12;
    //负序角更新  
    udwTemp = ((INT32U)wPLLNosFrequency * cPLLFreqAngleGain)>>3;
    udwNosPLLAngleIngCtrl = udwNosPLLAngleIngCtrl - udwTemp;
    udwTemp = udwNosPLLAngleIngCtrl>>20;
    wPLLNosAngle = (udwTemp * 720)>>12;
    //开环角更新
    udwTemp = ((INT32U)uwGridFreq * cPLLAngleGainOpen)>>3;
    udwPLLAngleIngCtrlOpen = udwPLLAngleIngCtrlOpen + udwTemp;  
    dwAngleIngSub = udwPLLAngleIngCtrlOpen - udwPLLAngleIngCtrl;
    if(fLVRTEndUpdate == 1)//跌落发生时，此标志置为1，跌落恢复后延时10ms清0
    {
    	udwTemp = (udwPLLAngleIngCtrlOpen - dwAngleIngSubReal)>>20;
    	wPLLAngleOpen = (udwTemp * 720)>>12;
    	if((fLVRTValue <= 1474)&&(fImbalanceLVRT == 0))//当前为平衡跌落，且跌落幅值非常小，进行开环锁相
    	{
    	    wPLLAngle = wPLLAngleOpen;
    	    udwPLLAngleIngCtrl = udwPLLAngleIngCtrlOpen - dwAngleIngSubReal;
            wPLLFreqSatErr = 0;
            wPLLFiltUqPrev = 0;
            dwPLLFreqIngCtrl = 0;
            wPLLFrequency = wPLLFreqSet;//wPLLFreqSet;
    	}
    }
    wPLLSinIndex = wPLLAngle;  //wPLLAngle is pointer or index of sin table
    wPLLCosIndex = wPLLSinIndex + (cSinTabNumb >> 2);
    
    if (wPLLCosIndex >= cSinTabNumb)
    {
        wPLLCosIndex = wPLLCosIndex - cSinTabNumb;
    }
    
    wPLLSinTheta = wSinTabArray[wPLLSinIndex];
    wPLLCosTheta = wSinTabArray[wPLLCosIndex];  
    wPLLNosSinIndex = wPLLNosAngle;
    wPLLNosCosIndex = wPLLNosSinIndex + (cSinTabNumb >> 2);    
    if (wPLLNosCosIndex >= cSinTabNumb)
    {
        wPLLNosCosIndex = wPLLNosCosIndex - cSinTabNumb;
    }    
    //find out SinTheta and CosTheta value in angle table
    wPLLNosSinTheta = wSinTabArray[wPLLNosSinIndex];
    wPLLNosCosTheta = wSinTabArray[wPLLNosCosIndex];
    
    if(((fCSVSSwitch == 1)&&(fVSRunByGrid == 1))||(fCSVSSwitch == 0))
    {
        udwTorqueAngleIngCtrl = udwPLLAngleIngCtrl;
        dwTorqueFreIngCtrl = 0;
        wTorqueFrequency = wPLLFrequency;
        wDeltaW = wTorqueFrequency - wVSFreqSet;
    }
}  

/************************************************************************
Function name:  void sClarke(T_THREE_PHASE_ABC *ptAbc, T_THREE_PHASE_AFBTGM *ptAfBtGm)
Description:  from ABC reference frame to alfa-beta reference frame
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_ABC, T_THREE_PHASE_AFBTGM
Return:  void      
************************************************************************/
void sClarke(T_THREE_PHASE_ABC *ptAbc, T_THREE_PHASE_AFBTGM *ptAfBtGm)
{
    ptAfBtGm->Af = ((INT32S)((INT32S)((INT32S)ptAbc->a * 2) - ptAbc->b - ptAbc->c)*cDivide3)>>14;
    ptAfBtGm->Bt = ((INT32S)((INT32S)ptAbc->b - ptAbc->c)*cDivideSqrt3)>>14;
	ptAfBtGm->Gm = ((INT32S)((INT32S)ptAbc->a + ptAbc->b + ptAbc->c)*cDivide3)>>14;
}

/************************************************************************
Function name:  void sInvClarke(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_ABC *ptAbc)
Description:  from  alfa-beta reference frame to ABC reference frame
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_AFBTGM, T_THREE_PHASE_ABC
Return:  void      
************************************************************************/
void sInvClarke(T_THREE_PHASE_AFBTGM *ptAfBtGm,T_THREE_PHASE_ABC *ptAbc)
{
    ptAbc->a = ptAfBtGm->Af;
	ptAbc->b = ((-ptAfBtGm->Af)>>1) + (((INT32S)ptAfBtGm->Bt*cSqrt3)>>15);
	ptAbc->c = ((-ptAfBtGm->Af)>>1) - (((INT32S)ptAfBtGm->Bt*cSqrt3)>>15);
}

/************************************************************************
Function name:  void sPark(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO)
Description:  from alfa-beta-gama reference frame to d-q-o reference frame
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_AFBTGM, T_THREE_PHASE_DQO
Return:  void      
************************************************************************/
void sPark(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO)
{
   ptDQO->d = (INT16S)(((INT32S)ptAfBtGm->Af *  wPLLCosTheta   + (INT32S)ptAfBtGm->Bt * wPLLSinTheta) >> 14);
   ptDQO->q = (INT16S)(((INT32S)ptAfBtGm->Af * (-wPLLSinTheta) + (INT32S)ptAfBtGm->Bt * wPLLCosTheta) >> 14); 
   ptDQO->o = ptAfBtGm->Gm;
}
/************************************************************************
Function name:  void sParkT(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO)
Description:  from alfa-beta-gama reference frame to d-q-o reference frame
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_AFBTGM, T_THREE_PHASE_DQO
Return:  void      
************************************************************************/
void sParkT(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO)
{
   ptDQO->d = (INT16S)(((INT32S)ptAfBtGm->Af *  wTorqueCosTheta   + (INT32S)ptAfBtGm->Bt * wTorqueSinTheta) >> 14);
   ptDQO->q = (INT16S)(((INT32S)ptAfBtGm->Af * (-wTorqueSinTheta) + (INT32S)ptAfBtGm->Bt * wTorqueCosTheta) >> 14); 
   ptDQO->o = ptAfBtGm->Gm;
}
/************************************************************************
Function name:  void sNosPark(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO)
Description:  from alfa-beta-gama reference frame to d-q-o reference frame
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_AFBTGM, T_THREE_PHASE_DQO
Return:  void      
************************************************************************/
void sNosPark(T_THREE_PHASE_AFBTGM *ptAfBtGm, T_THREE_PHASE_DQO *ptDQO)
{
    ptDQO->d = (INT16S)(((INT32S)ptAfBtGm->Af *  wPLLNosCosTheta   + (INT32S)ptAfBtGm->Bt * wPLLNosSinTheta) >> 14);
    ptDQO->q = (INT16S)(((INT32S)ptAfBtGm->Af * (-wPLLNosSinTheta) + (INT32S)ptAfBtGm->Bt * wPLLNosCosTheta) >> 14); 
    ptDQO->o = ptAfBtGm->Gm;
}
/************************************************************************
Function name:  void sInvPark(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm)
Description:  from d-q-o reference frame to alfa-beta-gama referenc frame
Calls:        
Called By:   sInvController(......)    
Parameters:  T_THREE_PHASE_DQO, T_THREE_PHASE_AFBTGM
Return:  void      
************************************************************************/
void sInvPark(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm)
{
   ptAfBtGm->Af = (INT16S)(((INT32S)ptDQO->d * wPLLCosTheta - (INT32S)ptDQO->q * wPLLSinTheta) >> 14);
   ptAfBtGm->Bt = (INT16S)(((INT32S)ptDQO->d * wPLLSinTheta + (INT32S)ptDQO->q * wPLLCosTheta) >> 14);  
}
/************************************************************************
Function name:  void sInvParkT(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm)
Description:  from d-q-o reference frame to alfa-beta-gama referenc frame
Calls:        
Called By:   sInvController(......)    
Parameters:  T_THREE_PHASE_DQO, T_THREE_PHASE_AFBTGM
Return:  void      
************************************************************************/
void sInvParkT(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm)
{
   ptAfBtGm->Af = (INT16S)(((INT32S)ptDQO->d * wTorqueCosTheta - (INT32S)ptDQO->q * wTorqueSinTheta) >> 14);
   ptAfBtGm->Bt = (INT16S)(((INT32S)ptDQO->d * wTorqueSinTheta + (INT32S)ptDQO->q * wTorqueCosTheta) >> 14);  
}
/************************************************************************
Function name:  void sNosInvPark(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm)
Description:  from d-q-o reference frame to alfa-beta-gama referenc frame
Calls:        
Called By:   sInvController(......)    
Parameters:  T_THREE_PHASE_DQO, T_THREE_PHASE_AFBTGM
Return:  void      
************************************************************************/
void sNosInvPark(T_THREE_PHASE_DQO *ptDQO, T_THREE_PHASE_AFBTGM *ptAfBtGm)
{
    ptAfBtGm->Af = (INT16S)(((INT32S)ptDQO->d * wPLLNosCosTheta - (INT32S)ptDQO->q * wPLLNosSinTheta) >> 14);
    ptAfBtGm->Bt = (INT16S)(((INT32S)ptDQO->d * wPLLNosSinTheta + (INT32S)ptDQO->q * wPLLNosCosTheta) >> 14);  
}
/*********************************************************************************************************************************
Function name:  void sInvControl(INT16S wBusSumVoltSamp,INT16S wBusDiffVoltSamp, T_THREE_PHASE_DQO *ptGridVolt, T_THREE_PHASE_DQO *ptInvCurr, T_THREE_PHASE_DQO *ptInvModu)
Description:   Inverter closed-loop control algorithm 
Calls:        
Called By:  sInvController(......)    
Parameters:  INT16S, T_THREE_PHASE_DQO
Return:  void      
*********************************************************************************************************************************/
void sInvControl(INT16S wBusSumVoltSamp, T_THREE_PHASE_DQO *ptGridVolt, T_THREE_PHASE_DQO *ptInvCurr, T_THREE_PHASE_DQO *ptInvModu,T_THREE_PHASE_DQO *ptGridNosVolt,
                 T_THREE_PHASE_DQO *ptInvNosCurr, T_THREE_PHASE_DQO *ptInvNosModu,INT16S wBusDiffVoltSamp,INT16S wBattCurr)
{              
	//Define local variables used by BusSum control 
    INT32S dwBusSumVoltErr;
    INT16S wBusSumVoltErrFilt;
    INT32S dwBusSumVoltPICtrl;
    INT32S dwInvDsCurrRefPreSat;
    INT16S wInvDsCurrRef;
    
    INT32S dwDCCurrErr;
    INT16S wDCCurrErrFilt;
    INT32S dwDCCurrPICtrl;
    INT32S dwDCCurrRefPreSat;
    INT16S wInvDCCurrRef;
    
    INT16S wBusDiffVoltErr;
    INT16S wBusDiffVoltErrFilt;
    INT32S dwBusDiffVoltPICtrl;
    INT32S dwInvOsCurrRefPreSat;
    //Define local varialbes used by D channel control
    INT16S wInvDsCurrErr;
    INT16S wInvDsCurrErrFilt;
    INT32S dwInvDsCurrPICtrl;
    INT32S dwInvDsModuPreSat;
    INT16S wInvDsModu;
    
    INT16S wInvNosDsCurrErr;
    INT16S wInvNosDsCurrErrFilt;
    INT32S dwInvNosDsCurrPICtrl;
    INT32S dwInvNosDsModuPreSat;
    INT16S wInvNosDsModu;
    //Define local variables used by Q channel control
    INT16S wInvQsCurrErr;
    INT16S wInvQsCurrErrFilt;
    INT32S dwInvQsCurrPICtrl;
    INT32S dwInvQsModuPreSat;
    INT16S wInvQsModu;
    
    INT16S wInvNosQsCurrErr;
    INT16S wInvNosQsCurrErrFilt;
    INT32S dwInvNosQsCurrPICtrl;
    INT32S dwInvNosQsModuPreSat;
    INT16S wInvNosQsModu;
    INT16S wDCCurrLimitHigh,wDCCurrLimitLow;
    INT16S wGridDFFSoftIncTmp,wGridQFFSoftIncTmp;
    INT32S dwTemp; 
    static INT16S wDCCurrSetBak = 0;
    //---------------------------------------DC Vol Ctrl---------------------------------------
	dwBusSumVoltErr = wInvCtrBusVolRef - wBusSumVoltSamp;	
	dwBusSumVoltErr = ((INT32S)dwBusSumVoltErr * cBusSumDivideGridD)>>6;//Q8
	if(dwBusSumVoltErr > 32000)
	{
		dwBusSumVoltErr = 32000;
	}
	else if(dwBusSumVoltErr < -32000)
	{
		dwBusSumVoltErr = - 32000;
	}
	//Low-pass filter
	wBusSumVoltErrFilt = (INT16S)(((INT32S)wBusSumVoltErrFiltPrev*cBusSumVoltFiltGainDen + dwBusSumVoltErr*cBusSumVoltFiltGainNum)>>12);
	wBusSumVoltErrFiltPrev = wBusSumVoltErrFilt;	      
	//integral
	dwBusSumVoltIngCtrl = dwBusSumVoltIngCtrl + (INT32S)wBusSumVoltErrFilt*cBusSumVoltVKi + dwInvDsCurrSatErr*cBusSumVoltKaw;
	
	//PI 
	dwBusSumVoltPICtrl = ((INT32S)wBusSumVoltErrFilt*cBusSumVoltVKp + dwBusSumVoltIngCtrl)>>12;
	dwInvDsCurrRefPreSat = dwBusSumVoltPICtrl;
	    
	if(fLVRTValue <= uwLVRTActiveValue)
	{
        fFVRTType = 1;
        wInvDsCurrLimitHigh = 512;//2A
        wInvDsCurrLimitLow = -512;
    
		dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
		dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
           
        fDCIDelayClear = 1;
        fClearBusCtr = 1;     
	}
	else if(fHVRTValue >= uwHVRTActiveValue)
	{
        fFVRTType = 2;
        fDCIDelayClear = 1;
        if(wEepromStandardValue == cRule21Standard)
        {
            wInvDsCurrLimitHigh = 512;//2A
            wInvDsCurrLimitLow = -512;
        }
        else
        {
            if(wInvDsCurrLimitHigh > 8709)
            {
            	wInvDsCurrLimitHigh = 8709;//66.7%
            }
            
            if(wInvDsCurrLimitLow < -10450)
            {
            	wInvDsCurrLimitLow = -10450;//80%
            }
        }
    
        dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
		dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;            
	}
	else if(fLVRTStartOrEndBak == 0)
	{
		fFVRTType = 0;
	}
	
	if(fLVRTStartOrEndBak == 1)
	{					
		if(fFVRTType == 1)
		{
			wInvDsCurrLimitHigh = 512;
            wInvDsCurrLimitLow = -512;            
		    dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
		    dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;               
            fDCIDelayClear = 1;
            fClearBusCtr = 1;            
		}
		else if(fFVRTType == 2)
		{
			fDCIDelayClear = 1;
			if(wEepromStandardValue == cRule21Standard)
            {
                wInvDsCurrLimitHigh = 512;//2A
                wInvDsCurrLimitLow = -512;
            }
            else
            {
                if(wInvDsCurrLimitHigh > 8709)
                {
                	wInvDsCurrLimitHigh = 8709;//66.7%
                }
                
                if(wInvDsCurrLimitLow < -10450)
                {
                	wInvDsCurrLimitLow = -10450;//80%
                }
            }
           
           dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
		   dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;
		}		
	}
	else
	{		
		if((fSoftLVRTEn == 1)||(fFVRTType == 2))
        {
        	fDCIDelayClear = 1;
            if(wInvDsCurrLimitHigh > 8709)
            {
            	wInvDsCurrLimitHigh = 8709;//99%
            }
            
            if(wInvDsCurrLimitLow < -12931)
            {
            	wInvDsCurrLimitLow = -12931;//99%
            }
            dwIdLimitHigh = (INT32S)wInvDsCurrLimitHigh<<10;
		    dwIdLimitLow = (INT32S)wInvDsCurrLimitLow<<10;           
        }
        else
        {
        	if(fChargeToDisCharge == 1)
            {
                wBusSumVoltErrFiltPrev = 0;                   
                dwBusSumVoltIngCtrl = -53510144;
                dwInvDsCurrSatErr = 0;
                
                wDCCurrErrFiltPrev = 0;
	            dwDCCurrIngCtrl = -57704448;
	            dwDCCurrSatErr = 0;
	            fDCChargeToDisCharge = 1;           
                fChargeToDisCharge = 0;
            }
            else if(fDisChargeToCharge == 1)
            {
                wBusSumVoltErrFiltPrev = 0;
                dwBusSumVoltIngCtrl = 35672064;
                dwInvDsCurrSatErr = 0;
                
                wDCCurrErrFiltPrev = 0;
	            dwDCCurrIngCtrl = 39866368;
	            dwDCCurrSatErr = 0;
	            fDCDisChargeToCharge = 1;
                fDisChargeToCharge = 0;
            }
            
            if(fInvCtrDeal == 1)
            {           	
            	wBusSumVoltErrFiltPrev = 0;
                dwBusSumVoltIngCtrl = dwBusSumVoltIngCtrl + dwDeltaDsCurr;
                if(dwBusSumVoltIngCtrl > 35672064)
                {
                	dwBusSumVoltIngCtrl = 35672064;
                }
                else if(dwBusSumVoltIngCtrl < -53510144)
                {
                	dwBusSumVoltIngCtrl = -53510144;
                }
                dwInvDsCurrSatErr = 0;
                
                wDCCurrErrFiltPrev = 0;
	            dwDCCurrIngCtrl = dwDCCurrIngCtrl + dwDeltaDsCurr;
	            if(dwDCCurrIngCtrl > 39866368)
                {
                	dwDCCurrIngCtrl = 39866368;
                }
                else if(dwDCCurrIngCtrl < -57704448)
                {
                	dwDCCurrIngCtrl = -57704448;
                }
	            dwDCCurrSatErr = 0;
	            
            	fInvCtrDeal = 0;
            }
            
        }
	}
		
	if (dwInvDsCurrRefPreSat > wInvDsCurrLimitHigh)
	{		
		wInvDsCurrRef = wInvDsCurrLimitHigh;
	}
	else if (dwInvDsCurrRefPreSat < wInvDsCurrLimitLow)
	{		
		wInvDsCurrRef = wInvDsCurrLimitLow;
	}
	else
	{
		wInvDsCurrRef = (INT16S)dwInvDsCurrRefPreSat;
	}    
	//saturate error for anti-windup use 
	dwInvDsCurrSatErr = (INT32S)wInvDsCurrRef - dwInvDsCurrRefPreSat;	
    tInvCurrDqoRef.d = -wInvDsCurrRef;
    //----------------------DC Curr Ctr----------------------------------
    dwDCCurrErr = (INT32S)wBattCurr - wDCCurrSet;//取负与BUS环保持一致
	if(dwDCCurrErr > 32000)
	{
		dwDCCurrErr = 32000;
	}
	else if(dwDCCurrErr < -32000)
	{
		dwDCCurrErr = -32000;
	}
    wDCCurrErrFilt = (INT16S)(((INT32S)wDCCurrErrFiltPrev * cDCCurrFiltGainDen + (INT32S)dwDCCurrErr * cDCCurrFiltGainNum)>>12);
	wDCCurrErrFiltPrev = wDCCurrErrFilt;
    
    dwDCCurrIngCtrl = dwDCCurrIngCtrl + (INT32S)wDCCurrErrFilt*cDCCurrKi + (INT32S)dwDCCurrSatErr*cDCCurrKaw;
	dwDCCurrPICtrl = ((INT32S)wDCCurrErrFilt*cDCCurrKp + dwDCCurrIngCtrl)>>12;
	dwDCCurrRefPreSat = dwDCCurrPICtrl;
	
	if(wInvDsCurrRef >= 0)
	{
        wDCCurrLimitHigh = wInvDsCurrLimitHigh + wInvCurrResolving;
        wDCCurrLimitLow = -wInvCurrResolving;
    }
    else
    {
        wDCCurrLimitHigh = wInvCurrResolving;
        wDCCurrLimitLow = wInvDsCurrLimitLow - wInvCurrResolving;
    }
    
    //wDCCurrLimitHigh = wInvDsCurrLimitHigh + wInvCurrResolving;
    //wDCCurrLimitLow = wInvDsCurrLimitLow - wInvCurrResolving;
    
	if (dwDCCurrRefPreSat > wDCCurrLimitHigh)
	{		
		wInvDCCurrRef = wDCCurrLimitHigh;
	}
	else if (dwDCCurrRefPreSat < wDCCurrLimitLow)
	{		
		wInvDCCurrRef = wDCCurrLimitLow;
	}
	else
	{
		wInvDCCurrRef = (INT16S)dwDCCurrRefPreSat;
	}
    
	//saturate error for anti-windup use 
	dwDCCurrSatErr = (INT32S)wInvDCCurrRef - dwDCCurrRefPreSat; 
	
	wDebugInvDsCurrRef = wInvDsCurrRef;
	wDebugInvDCCurrRef = wInvDCCurrRef;
	
	if(fBattDarkStart == 0)
	{
	    if((wBatteryChargeCurr < 0)&&(wBatteryDisChargeCurr > 0))
	    {
	        if((fCharge == 0)&&(wInvDsCurrRef < wInvCurrResolving))
	        {
	            if(wInvDsCurrRef < wInvDCCurrRef)
	            {
	                tInvCurrDqoRef.d = -wInvDCCurrRef;
	            }
	        }
	        else if((fCharge == 1)&&(wInvDsCurrRef > -wInvCurrResolving))
	        {
	            if(wInvDsCurrRef > wInvDCCurrRef)
	            {
	                tInvCurrDqoRef.d = -wInvDCCurrRef;
	            }
	        }
	    }
	}
	
	if(wDCCurrSetBak != wDCCurrSet)
	{
	    if(wDCCurrSetBak >= 0)
	    {
	        if(wDCCurrSet < 0)
	        {
	            wDCCurrErrFiltPrev = 0;
	            dwDCCurrIngCtrl = -((INT32S)tInvCurrDqoRef.d<<12);
	            dwDCCurrSatErr = 0;
	        }	            
	    }
	    else
	    {
	        if(wDCCurrSet >= 0)
	        {
	            wDCCurrErrFiltPrev = 0;
	            dwDCCurrIngCtrl = -((INT32S)tInvCurrDqoRef.d<<12);
	            dwDCCurrSatErr = 0;
	        }
	    }
	    wDCCurrSetBak = wDCCurrSet;
	}
    
    if(fGridRelayCloseDeal == 1)
    {
    	tInvCurrDqoRef.d = wIdRefBak;
    	fClearBusCtr = 1;
    }
    
    if(fCPLDShutInvPWM == 0)//Shut
    {
    	fCPLDShutInvDeal = 1;
    	uwCPLDShutInvDelay = 0;
    }
    
    if(fCPLDShutInvDeal == 1)
    {
    	tInvCurrDqoRef.d = 0;
    	fClearBusCtr = 1;
    	wDebugCPLDShutInv++;
    	if(++uwCPLDShutInvDelay >= 4)
    	{
    		uwCPLDShutInvDelay = 4;
    		fCPLDShutInvDeal = 0;
    	}
    }
    else
    {
    	uwCPLDShutInvDelay = 0;
    }
    
    if(fClearBusCtr == 1)
	{
	    fClearBusCtr = 0;
	    wBusSumVoltErrFiltPrev = 0;
	    dwInvDsCurrSatErr = 0;
	    dwBusSumVoltIngCtrl = -((INT32S)tInvCurrDqoRef.d<<12);
	}
	
	//***********Bus voltage Diff closed-loop control**********************// 
	if(fRunInvAhdChkRequire == 1)
	{         
	    wBusDiffVoltErr = 0 - wBusDiffVoltSamp;	    	
	    //Low-pass filter
	    //wBusDiffVoltErrFilt = (INT16S)(((INT32S)wBusDiffVoltErrFiltPrev*cBusDiffVoltFiltGainDen + (INT32S)wBusDiffVoltErr*cBusDiffVoltFiltGainNum)>>15);
	    wBusDiffVoltErrFilt = wBusDiffVoltErr;
	    //wBusDiffVoltErrFiltPrev = wBusDiffVoltErrFilt;	
	    
	    //integral	    
	    dwBusDiffVoltIngCtrl = dwBusDiffVoltIngCtrl + (INT32S)wBusDiffVoltErrFilt*cBusDiffVoltKi + (INT32S)dwInvOsCurrSatErr*cBusDiffVoltKaw;
	    
	    //PI 
	    dwBusDiffVoltPICtrl = ((INT32S)wBusDiffVoltErrFilt*cBusDiffVoltKp + dwBusDiffVoltIngCtrl)>>8;
	    dwInvOsCurrRefPreSat = dwBusDiffVoltPICtrl;
	    
	    //Limit of the d channel current reference 
	    if (dwInvOsCurrRefPreSat > cInvOsCurrLimitHigh)
	    {		
	    	wInvOsCurrRef = cInvOsCurrLimitHigh;
	    }
	    else if (dwInvOsCurrRefPreSat < cInvOsCurrLimitLow)
	    {		
	    	wInvOsCurrRef = cInvOsCurrLimitLow;
	    }
	    else
	    {
	    	wInvOsCurrRef = dwInvOsCurrRefPreSat;
	    }
	    //saturate error for anti-windup use 
	    dwInvOsCurrSatErr = (INT32S)wInvOsCurrRef- dwInvOsCurrRefPreSat;
	    wInvOsCurrRef = wInvOsCurrRef + 16383;
	}
	else
	{
		wInvOsCurrRef = 16383;
	}
    
    //=============inverter current closed-loop D Channel control=============//
	//current error in d channel c  
	wInvDsCurrErr = tInvCurrDqoRef.d - ptInvCurr->d;    //Q8
    
	//Low-pass filter
	wInvDsCurrErrFilt = (INT16S)(((INT32S)wInvDsCurrErrFiltPrev*cInvCurrFiltGainDen + (INT32S)wInvDsCurrErr*cInvCurrFiltGainNum)>>12);
	wInvDsCurrErrFiltPrev = wInvDsCurrErrFilt; //Q8
	
	//integral
	dwInvDsCurrIngCtrl = dwInvDsCurrIngCtrl + (INT32S)wInvDsCurrErrFilt*cInvCurrKi + dwInvDsModuSatErr*cInvCurrKaw;//Q4+15
    
    if(fCPLDShutInvDeal == 1)
    {
        dwInvDsCurrIngCtrl = 0;
    }
	//PI 
	dwInvDsCurrPICtrl = ((INT32S)wInvDsCurrErrFilt*cInvCurrKp + dwInvDsCurrIngCtrl)>>12;//Q8
	
	dwInvDsModuPreSat = dwInvDsCurrPICtrl - (((INT32S)tInvCurrDqoRef.q*wWL)>>10);//Q8
	
	if((fRunInvAhdChkRequire == 1)||(fGridRelayCloseDeal == 1))           //Normal procedure  
	{	
	    dwInvDsModuPreSat = (((INT64S)dwInvDsModuPreSat * cInvPwmK)>>14) + (((INT32S)ptGridVolt->d * uwInvPwmKFeedBack)>>10); //ptGridVolt->d wGridPosDFF
    }
    else                                    //Check procedure
    {
        if((wGridPosDFF < 2000) && (fDeCapEnergy == 1))
        {
        	wGridPosDFF = 2000;
    	}
        wGridDFFSoftIncTmp = (INT16S)(((INT32S)wGridPosDFF * uwModuSoftIncCnt)>>11);
        wGridDFFSoftIncTmp = (INT16S)(((INT32S)wGridDFFSoftIncTmp * cInvChkUdComp)>>14);
        dwInvDsModuPreSat = ((INT32S)wGridDFFSoftIncTmp * uwInvPwmBusComp)>>10;
        wInvDsCurrErrFiltPrev = 0;
        dwInvDsModuSatErr = 0;
        dwInvDsCurrIngCtrl = 0;   
    }
	if(dwInvDsModuPreSat > cInvModuLimitHigh)
	{
		wInvDsModu = cInvModuLimitHigh;
	}
	else if (dwInvDsModuPreSat < cInvModuLimitLow)
	{
		wInvDsModu = cInvModuLimitLow;
	}
	else
	{
		wInvDsModu = (INT16S)dwInvDsModuPreSat;
	}
	//Csaturate error for anti-windup use 
	dwInvDsModuSatErr = (((INT32S)wInvDsModu - dwInvDsModuPreSat)*cInvPwmKRec)>>9;//对应16384为最大占空比
		
	//==========================Pos inverter current closed-loop Q Channel control========================//
    if((wAIDeltaIq + cCurrOR125A) < wAIDeltaIqRef)
    {
        wAIDeltaIq = wAIDeltaIq + cCurrOR125A;
    }
    else if(wAIDeltaIq > (wAIDeltaIqRef + cCurrOR125A))
    {
        wAIDeltaIq = wAIDeltaIq - cCurrOR125A;
    }
    else
    {
        wAIDeltaIq = wAIDeltaIqRef;
    }
    
    if(fLVRTStartOrEnd == 1)
    {
        dwTemp = (INT32S)tInvPosCurrDqoRef.q + wAIDeltaIq;//wAIDeltaIq;
        
        if(dwTemp > 16000)
        {
            tInvCurrDqoRef.q = 16000;
        }
        else if(dwTemp < -16000)
        {
            tInvCurrDqoRef.q = -16000;
        }
        else
        {
            tInvCurrDqoRef.q = (INT16S)dwTemp;
        }
    }
    else
    {    	
        tInvCurrDqoRef.q = (INT32S)tInvCurrPowerReguRefDqo.q + wAdjustPF+ swReactiveCurrQs;// + wAIDeltaIq;
               
        if(fSoftLVRTEn == 1)
        {
            if(fLVRTValue <= uwSoftLVRTValue)
            {
                tInvCurrDqoRef.q = -1306 + wIqRefBakToLVRT;                
            }
            else if(fHVRTValue >= uwSoftHVRTValue)
            {
                tInvCurrDqoRef.q = 1306 + wIqRefBakToLVRT;  
            }
            else
            {
            	tInvCurrDqoRef.q = 0;
            }          
        }
        
        if(tInvCurrDqoRef.q > 9117)//8310
        {
        	tInvCurrDqoRef.q = 9117;//8310 80%RatedPower must be output PF 0.8 7837+(945/2)  tInvCurrDqoRef.q
        }
        else if(tInvCurrDqoRef.q < -8310)//7837
        {
        	tInvCurrDqoRef.q = -8310;
        }
        
        dwTemp = (INT32S)tInvCurrDqoRef.q + wAIDeltaIq;
        
        if(dwTemp > 11104)//0.85*Rated
        {
        	tInvCurrDqoRef.q = 11104;
        }
        else if(dwTemp < -11104)
        {
        	tInvCurrDqoRef.q = -11104;
        }
        else
        {
            tInvCurrDqoRef.q = (INT16S)dwTemp;
        }
    }
    
    if(fGridRelayCloseDeal == 1)
    {
    	tInvCurrDqoRef.q = wIqRefBak;
    }
    
    if(fCPLDShutInvDeal == 1)
    {
    	tInvCurrDqoRef.q = 0;
    	tInvNosCurrDqoRef.q = 0;   	
    }
       
	wInvQsCurrErr = tInvCurrDqoRef.q - ptInvCurr->q;	 
	//=============inverter current closed-loop Q Channel control =============//
	//Low-pass filter 
	wInvQsCurrErrFilt = (INT16S)(((INT32S)wInvQsCurrErrFiltPrev*cInvCurrFiltGainDen + (INT32S)wInvQsCurrErr*cInvCurrFiltGainNum)>>12);
	wInvQsCurrErrFiltPrev = wInvQsCurrErrFilt;
	
	//integral
	dwInvQsCurrIngCtrl = dwInvQsCurrIngCtrl + (INT32S)wInvQsCurrErrFilt*cInvCurrKi + dwInvQsModuSatErr*cInvCurrKaw;
    
    if(fCPLDShutInvDeal == 1)
    {
        dwInvQsCurrIngCtrl = 0;
    }
    //PI
	dwInvQsCurrPICtrl = ((INT32S)wInvQsCurrErrFilt*cInvCurrKp + dwInvQsCurrIngCtrl)>>12;//Q8
	
	dwInvQsModuPreSat = dwInvQsCurrPICtrl + (((INT32S)tInvCurrDqoRef.d*wWL)>>10);//Q8
    
	if((fRunInvAhdChkRequire == 1)||(fGridRelayCloseDeal == 1))           //Normal procedure
	{	
	    dwInvQsModuPreSat = (((INT64S)dwInvQsModuPreSat * cInvPwmK)>>14)  + (((INT32S)ptGridVolt->q * uwInvPwmKFeedBack )>>10);
	}
	else
	{
		wGridQFFSoftIncTmp = (INT16S)(((INT32S)wGridDFFSoftIncTmp * cInvChkUqComp)>>14);
        dwInvQsModuPreSat = ((INT32S)wGridQFFSoftIncTmp * uwInvPwmBusComp)>>10;
        wInvQsCurrErrFiltPrev = 0;
        dwInvQsModuSatErr = 0;
        dwInvQsCurrIngCtrl = 0;  
    }
	if (dwInvQsModuPreSat > cInvModuLimitHigh)
	{
		wInvQsModu = cInvModuLimitHigh;
	}
	else if (dwInvQsModuPreSat < cInvModuLimitLow)
	{
		wInvQsModu = cInvModuLimitLow;
	}
	else
	{
		wInvQsModu = (INT16S)dwInvQsModuPreSat;
	}
    dwInvQsModuSatErr = (((INT32S)wInvQsModu - dwInvQsModuPreSat)*cInvPwmKRec)>>9;
    
	ptInvModu->d = wInvDsModu; 
	ptInvModu->q = wInvQsModu; // for open-loop debugging ,set D= 3000, Q = 0;//    
    //---------------------------------------------------------------------------------------//
    if((fImbalanceLVRT == 1)&&(fLVRTStartOrEnd == 1))
    {
        //=============inverter current closed-loop D Channel control=============//
	    //current error in d channel c    
	    tInvNosCurrDqoRef.d = 0;
	    wInvNosDsCurrErr = tInvNosCurrDqoRef.d - ptInvNosCurr->d;
	    //Low-pass filter
	    wInvNosDsCurrErrFilt = (INT16S)(((INT32S)wInvNosDsCurrErrFiltPrev*cInvCurrFiltGainDen + (INT32S)wInvNosDsCurrErr*cInvCurrFiltGainNum)>>12);
	    wInvNosDsCurrErrFiltPrev = wInvNosDsCurrErrFilt;//Q3
	    //integral
	    dwInvNosDsCurrIngCtrl = dwInvNosDsCurrIngCtrl + (INT32S)wInvNosDsCurrErrFilt*cInvCurrKi + dwInvNosDsModuSatErr*cInvCurrKaw;
	    
        if(fCPLDShutInvDeal == 1)
        {
            dwInvNosDsCurrIngCtrl = 0;
        }
	    //PI 
	    dwInvNosDsCurrPICtrl = ((INT32S)wInvNosDsCurrErrFilt*cInvCurrKp + dwInvNosDsCurrIngCtrl)>>12;//Q8
	    dwInvNosDsModuPreSat = dwInvNosDsCurrPICtrl + (((INT32S)tInvNosCurrDqoRef.q*wWL)>>10);//Q8
	    
	    dwInvNosDsModuPreSat = (((INT64S)dwInvNosDsModuPreSat * cInvPwmK)>>14) + (((INT32S)ptGridNosVolt->d * uwInvPwmKFeedBack)>>10);
	    
	    if (dwInvNosDsModuPreSat > cInvModuLimitHigh)
	    {
	    	wInvNosDsModu = cInvModuLimitHigh;
	    }
	    else if (dwInvNosDsModuPreSat < cInvModuLimitLow)
	    {
	    	wInvNosDsModu = cInvModuLimitLow;
	    }
	    else
	    {
	    	wInvNosDsModu = (INT16S)dwInvNosDsModuPreSat;
	    }
	    //Csaturate error for anti-windup use 
	    dwInvNosDsModuSatErr = (((INT32S)wInvNosDsModu - dwInvNosDsModuPreSat)*cInvPwmKRec)>>9;
	    
	    //**************inverter current closed-loop Q Channel control *************************//
        wInvNosQsCurrErr = tInvNosCurrDqoRef.q - ptInvNosCurr->q;  
	    //Low-pass filter 
	    wInvNosQsCurrErrFilt = (INT16S)(((INT32S)wInvNosQsCurrErrFiltPrev*cInvCurrFiltGainDen + (INT32S)wInvNosQsCurrErr*cInvCurrFiltGainNum)>>12);
	    wInvNosQsCurrErrFiltPrev = wInvNosQsCurrErrFilt;	
	    //integral
	    dwInvNosQsCurrIngCtrl = dwInvNosQsCurrIngCtrl + (INT32S)wInvNosQsCurrErrFilt*cInvCurrKi + dwInvNosQsModuSatErr*cInvCurrKaw;
        
        if(fCPLDShutInvDeal == 1)
        {
            dwInvNosQsCurrIngCtrl = 0;
        }
        //PI
	    dwInvNosQsCurrPICtrl = ((INT32S)wInvNosQsCurrErrFilt*cInvCurrKp + dwInvNosQsCurrIngCtrl)>>12;
	    dwInvNosQsModuPreSat = dwInvNosQsCurrPICtrl - (((INT32S)tInvNosCurrDqoRef.d*wWL)>>10);
	    
	    dwInvNosQsModuPreSat = (((INT64S)dwInvNosQsModuPreSat * cInvPwmK)>>14) + (((INT32S)ptGridNosVolt->q * uwInvPwmKFeedBack)>>10);
	    if (dwInvNosQsModuPreSat > cInvModuLimitHigh)
	    {
	    	wInvNosQsModu = cInvModuLimitHigh;
	    }
	    else if (dwInvNosQsModuPreSat < cInvModuLimitLow)
	    {
	    	wInvNosQsModu = cInvModuLimitLow;
	    }
	    else
	    {
	    	wInvNosQsModu = (INT16S)dwInvNosQsModuPreSat;
	    }
        dwInvNosQsModuSatErr = (((INT32S)wInvNosQsModu - dwInvNosQsModuPreSat)*cInvPwmKRec)>>9;
        
	    ptInvNosModu->d = wInvNosDsModu; 
	    ptInvNosModu->q = wInvNosQsModu; // for open-loop debugging ,set D= 3000, Q = 0;//
    }
    else
    {
        wInvNosDsCurrErrFiltPrev = 0;
        dwInvNosDsModuSatErr = 0;
        dwInvNosDsCurrIngCtrl = 0;
        
        wInvNosQsCurrErrFiltPrev = 0;
        dwInvNosQsModuSatErr = 0;
        dwInvNosQsCurrIngCtrl = 0;
        
        ptInvNosModu->d = 0;
        ptInvNosModu->q = 0;
    }
}
/*********************************************************************************************************************************
Function name:  void sVSGBatCtrl(INT16S wBusSumVoltSamp)
Description:   Inverter closed-loop control algorithm 
Calls:        
Called By:  sInvController(......)    
Parameters:  INT16S, T_THREE_PHASE_DQO
Return:  void      
*********************************************************************************************************************************/
void sVSGBatCtrl(INT16S wBusSumVoltSamp)
{
INT32S dwBusDisChargeErr;
INT16S wBusDisChargeErrFilt;
INT32S dwBusDisChargePICtrl;
INT32S dwInvDisChargeCurrRefPreSat;

INT32S dwBusChargeErr;
INT16S wBusChargeErrFilt;
INT32S dwBusChargePICtrl;
INT32S dwInvChargeCurrRefPreSat;

INT16S wInvVSCurrErr;
INT16S wInvVSCurrErrFilt;
INT32S dwInvVSCurrPICtrl;
INT32S dwInvVSVoltPreSat;
INT16S wInvVSVoltRef;

INT16S wIdTmp,wIqTmp;
INT16S wAngleTmp1,wAngleTmp2;
INT16S wSinTmp,wCosTmp;
INT32S dwAngleErrTmp;

    //----------------------------------------Battery DisCharge Protect-----------------------------------------
	dwBusDisChargeErr = wBusDisChargeVoltRef - wBusSumVoltSamp;	
	dwBusDisChargeErr = ((INT32S)dwBusDisChargeErr * cBusSumDivideGridD)>>6;//Q8
	if(dwBusDisChargeErr > 32000)
	{
		dwBusDisChargeErr = 32000;
	}
	else if(dwBusDisChargeErr < -32000)
	{
		dwBusDisChargeErr = - 32000;
	}
	//Low-pass filter
	wBusDisChargeErrFilt = (INT16S)(((INT32S)wBusDisChargeErrFiltPrev*cBusSumVoltFiltGainDen + dwBusDisChargeErr*cBusSumVoltFiltGainNum)>>12);
	wBusDisChargeErrFiltPrev = wBusDisChargeErrFilt;	      
	//integral
	dwBusDisChargeIngCtrl = dwBusDisChargeIngCtrl + (INT32S)wBusDisChargeErrFilt*cBusSumVoltVKi + dwInvDisChargeCurrSatErr*cBusSumVoltKaw;
	
	//PI 
	dwBusDisChargePICtrl = ((INT32S)wBusDisChargeErrFilt*cBusSumVoltVKp + dwBusDisChargeIngCtrl)>>12;
	dwInvDisChargeCurrRefPreSat = dwBusDisChargePICtrl;
	
	if (dwInvDisChargeCurrRefPreSat > wInvDsCurrLimitHigh)
	{		
		wInvDisChargeCurrMax = wInvDsCurrLimitHigh;
	}
	else if (dwInvDisChargeCurrRefPreSat < wInvDsCurrLimitLow)
	{		
		wInvDisChargeCurrMax = wInvDsCurrLimitLow;
	}
	else
	{
		wInvDisChargeCurrMax = (INT16S)dwInvDisChargeCurrRefPreSat;
	}    
	//saturate error for anti-windup use 
	dwInvDisChargeCurrSatErr = (INT32S)wInvDisChargeCurrMax - dwInvDisChargeCurrRefPreSat;
	
	//----------------------------------------Battery Charge Protect-----------------------------------------	
	dwBusChargeErr = wBusChargeVoltRef - wBusSumVoltSamp;	
	dwBusChargeErr = ((INT32S)dwBusChargeErr * cBusSumDivideGridD)>>6;//Q8
	if(dwBusChargeErr > 32000)
	{
		dwBusChargeErr = 32000;
	}
	else if(dwBusChargeErr < -32000)
	{
		dwBusChargeErr = - 32000;
	}
	//Low-pass filter
	wBusChargeErrFilt = (INT16S)(((INT32S)wBusChargeErrFiltPrev*cBusSumVoltFiltGainDen + dwBusChargeErr*cBusSumVoltFiltGainNum)>>12);
	wBusChargeErrFiltPrev = wBusChargeErrFilt;	      
	//integral
	dwBusChargeIngCtrl = dwBusChargeIngCtrl + (INT32S)wBusChargeErrFilt*cBusSumVoltVKi + dwInvChargeCurrSatErr*cBusSumVoltKaw;
	
	//PI 
	dwBusChargePICtrl = ((INT32S)wBusChargeErrFilt*cBusSumVoltVKp + dwBusChargeIngCtrl)>>12;
	dwInvChargeCurrRefPreSat = dwBusChargePICtrl;
	
	if (dwInvChargeCurrRefPreSat > wInvDsCurrLimitHigh)
	{		
		wInvChargeCurrMin = wInvDsCurrLimitHigh;
	}
	else if (dwInvChargeCurrRefPreSat < wInvDsCurrLimitLow)
	{		
		wInvChargeCurrMin = wInvDsCurrLimitLow;
	}
	else
	{
		wInvChargeCurrMin = (INT16S)dwInvChargeCurrRefPreSat;
	}    
	//saturate error for anti-windup use 
	dwInvChargeCurrSatErr = (INT32S)wInvChargeCurrMin - dwInvChargeCurrRefPreSat;
	
    //----------------------------------------Maximum Current Protect-----------------------------------------
    wIdTmp = abs(tInvCurrTDqo.d);
    wIqTmp = abs(tInvCurrTDqo.q);
    
    if((wIdTmp + wIqTmp) < 10000)
    {
        wImaxTmp = wIdTmp + wIqTmp;
    }
    else
    {
        dwAngleErrTmp = sdwArcTan(wIdTmp,wIqTmp);
        dwAngleErrTmp = dwAngleErrTmp>>20;
        wAngleTmp1 = (dwAngleErrTmp * 720)>>12;    
        wAngleTmp2 = wAngleTmp1 + (cSinTabNumb >> 2);
        
        if (wAngleTmp2 >= cSinTabNumb)
        {
            wAngleTmp2 = wAngleTmp2 - cSinTabNumb;
        }
        
        wSinTmp = wSinTabArray[wAngleTmp1];
        wCosTmp = wSinTabArray[wAngleTmp2]; 
                
        if(wIdTmp > wIqTmp)
        {
            wImaxTmp = ((INT32S)wIdTmp<<14)/wCosTmp;
        }
        else
        {
            wImaxTmp = ((INT32S)wIqTmp<<14)/wSinTmp;
        }
    }
    
    wImaxTmp = wImaxTmp>>4;
    
    wInvVSCurrErr = 890 - wImaxTmp;
	//Low-pass filter
	wInvVSCurrErrFilt = (INT16S)(((INT32S)wInvVSCurrErrFiltPrev*cVSCurrFiltGainDen + (INT32S)wInvVSCurrErr*cVSCurrFiltGainNum)>>12);
	wInvVSCurrErrFiltPrev = wInvVSCurrErrFilt; //Q8	
	//integral
	dwInvVSCurrIngCtrl = dwInvVSCurrIngCtrl + (INT32S)wInvVSCurrErrFilt*cVSCurrKi + dwInvVSCurrSatErr*cVSCurrKaw;//Q4+15    
	//PI 
	dwInvVSCurrPICtrl = ((INT32S)wInvVSCurrErrFilt*cVSCurrKp + dwInvVSCurrIngCtrl)>>12;//Q8	
	dwInvVSVoltPreSat = dwInvVSCurrPICtrl;// + wGridVoltTmp;
	
	if(dwInvVSVoltPreSat > 800)
	{
		wInvVSVoltRef = 800;
	}
	else if (dwInvVSVoltPreSat < 0)
	{
		wInvVSVoltRef = 0;
	}
	else
	{
		wInvVSVoltRef = (INT16S)dwInvVSVoltPreSat;
	}
	//Csaturate error for anti-windup use 
	dwInvVSCurrSatErr = (INT32S)wInvVSVoltRef - dwInvVSVoltPreSat;//对应16384为最大占空比	
	wInvVoltLimit = wInvVSVoltRef;//wInvVSVoltRef>>4;
}
/*********************************************************************************************************************************
Function name:void sVSGControl(INT16S wBusDiffVoltSamp, T_THREE_PHASE_DQO *ptGridVolt,T_THREE_PHASE_DQO *ptInvModu, T_THREE_PHASE_DQO *ptInvNosModu)
Description:   Inverter closed-loop control algorithm 
Calls:        
Called By:  sInvController(......)    
Parameters:  INT16S, T_THREE_PHASE_DQO
Return:  void      
*********************************************************************************************************************************/
void sVSGControl(INT16S wBusDiffVoltSamp, T_THREE_PHASE_DQO *ptGridVolt,T_THREE_PHASE_DQO *ptInvModu, T_THREE_PHASE_DQO *ptInvNosModu)
{
INT16S wBusDiffVoltErr;
INT16S wBusDiffVoltErrFilt;
INT32S dwBusDiffVoltPICtrl;
INT32S dwInvOsCurrRefPreSat;

INT32S dwGirdVoltDAxisErr;
INT16S wGirdVoltDAxisErrFilt;
INT16S wGirdVoltDAxisPICtrl;
INT16S wGridDsCurrRefPreSat;
INT16S wGridDsCurrRef;

INT32S dwGridVoltQAxisErr;
INT16S wGridVoltQAxisErrFilt;
INT16S wGridVoltQAxisPICtrl;
INT16S wGridVoltQAxisPreSat;
INT16S wGridQsCurrRef;

//Define local varialbes used by D channel control
INT16S wVSInvDsCurrErr;
INT16S wVSInvDsCurrErrFilt;
INT32S dwVSInvDsCurrPICtrl;
INT32S dwVSInvDsModuPreSat; 
INT16S wVSInvDsModu;
//Define local variables used by Q channel control
INT16S wVSInvQsCurrErr;
INT16S wVSInvQsCurrErrFilt;
INT32S dwVSInvQsCurrPICtrl;
INT32S dwVSInvQsModuPreSat;
INT16S wVSInvQsModu;

INT16S wTorqueErr;
INT32U udwTemp;
INT32S dwQfb;
INT16S wVoltErr;
INT16S wVSIdFilt,wVSIqFilt;
INT16S wVirtualRUd = 0;
INT16S wVirtualRUq = 0;
INT16S wGridDFFSoftIncTmp,wGridQFFSoftIncTmp;

    //-------------------------------------------VSG PLL-----------------------------------------  
    if(fCPLDShutInvPWM == 0)//Shut
    {
    	fCPLDShutInvDeal = 1;
    	uwCPLDShutInvDelay = 0;
    }
    
    if(fCPLDShutInvDeal == 1)
    {
    	wDebugCPLDShutInv++;
    	if(++uwCPLDShutInvDelay >= 4)
    	{
    		uwCPLDShutInvDelay = 4;
    		fCPLDShutInvDeal = 0;
    	}
    }
    else
    {
    	uwCPLDShutInvDelay = 0;
    }
    
    udwTemp = ((INT32U)wTorqueFrequency * cPLLFreqAngleGain)>>3;
    udwTorqueAngleIngCtrl = udwTorqueAngleIngCtrl + udwTemp;
          
    udwTemp = udwTorqueAngleIngCtrl>>20;
    wTorqueAngle = (udwTemp * 720)>>12;
    
    wTorqueSinIndex = wTorqueAngle;  //wPLLAngle is pointer or index of sin table
    wTorqueCosIndex = wTorqueSinIndex + (cSinTabNumb >> 2);    
    //Reset CosTheta index 
    if (wTorqueCosIndex >= cSinTabNumb)
    {
        wTorqueCosIndex = wTorqueCosIndex - cSinTabNumb;
    }
    
    wTorqueSinTheta = wSinTabArray[wTorqueSinIndex];
    wTorqueCosTheta = wSinTabArray[wTorqueCosIndex];

    sParkT(&tGridVoltAfBtGm,&tGridVoltTDqo);
    sParkT(&tInvCurrAfBtGm,&tInvCurrTDqo);
    
    if(fCPLDShutInvDeal == 1)
    {
        dwTorqueFreIngCtrl = 0;
        wDeltaW = 0;
    }
    //--------------------------------------------VSG---------------------------------------------------
    wTorqueM = ((INT64S)dwVSPSet * wVSFreqSetRev)>>22;
    wTorqueE = (((INT32S)wVoltDAim * tVSInvCurrDqoRef.d + (INT32S)wVoltQAim * tVSInvCurrDqoRef.q)>>4)/wTorqueFrequency;
    wTorqueE = ((INT32S)wTorqueE * 3)>>1;
    
    wTorqueD = ((INT64S)dwDampD * wDeltaW)>>10;//Q4  10
	wTorqueErr = wTorqueM - wTorqueD - wTorqueE;
	
	dwTorqueFreIngCtrl = dwTorqueFreIngCtrl + (INT32S)wTorqueErr * cInertiaInvJ;//Q20
	
	if(dwTorqueFreIngCtrl > 527071785)//2*pi*8 Q24
	{
		dwTorqueFreIngCtrl = 527071785;
	}
	else if(dwTorqueFreIngCtrl < -527071785)
	{
		dwTorqueFreIngCtrl = -527071785;
	}
	
	wDeltaW = dwTorqueFreIngCtrl>>20;

	if(fRunInvAhdChkRequire == 1)
	{
	    wTorqueFrequency = wVSFreqSet + wDeltaW;//Q4 wVSFreqSet
	}
	else
	{
	    wTorqueFrequency = wVSFreqSet;//wVSFreqSet
	}
	
	wDebugTest4 = wTorqueFrequency;
	    
    //-------------------------------------------VSG PLL-----------------------------------------
    dwQfb = ((INT32S)wVoltQAim * tVSInvCurrDqoRef.d - (INT32S)wVoltDAim * tVSInvCurrDqoRef.q)>>8;
	dwQfb = (dwQfb * 3)>>1;
	
	wVoltErr = ((INT64S)(dwVSQSet - dwQfb) * dwDroopKQ)>>20;
	    
	wVoltDAim = wVSVoltSet + wVoltErr;
    wVoltQAim = 0;//- wVSVoltSet - wVoltErr;
    
    wVSIdFilt = ((INT32S)wVSIdPrev * 910 + (INT32S)tInvCurrTDqo.d * 114)>>10;// 985 39    910 114
    wVSIqFilt = ((INT32S)wVSIqPrev * 910 + (INT32S)tInvCurrTDqo.q * 114)>>10;// 985 39    910 114
    
    wVSInvDsCurrErrFilt = wVSIdFilt - wVSIdPrev;
    wVSInvQsCurrErrFilt = wVSIqFilt - wVSIqPrev;
    
    wVSIdPrev = wVSIdFilt;
    wVSIqPrev = wVSIqFilt;
    
    wVSIdFilt = ((INT32S)wVSInvDsCurrErrFilt * 3113)>>10;
    wVSIqFilt = ((INT32S)wVSInvQsCurrErrFilt * 3113)>>10;
    
    dwGirdVoltDAxisErr = (INT32S)((INT32S)(wVoltDAim - tGridVoltTDqo.d)<<4) + (((INT32S)wVSIqPrev * wWL)>>10) - (((INT32S)wVSIdPrev * wRGrid)>>10) - wVSIdFilt;
    if(dwGirdVoltDAxisErr > 32000)
	{
	    dwGirdVoltDAxisErr = 32000;
	}
	else if(dwGirdVoltDAxisErr < -32000)
	{
	    dwGirdVoltDAxisErr = -32000;
	}
	if(fCPLDShutInvDeal == 1)
    {
        wGridVoltDAxisErrFiltPrev = 0;
        dwGridVoltDAxisIngCtrl = 0;
        wGridDsCurrSatErr = 0;
    }
    //Low-pass filter
	wGirdVoltDAxisErrFilt = (INT16S)(((INT32S)wGridVoltDAxisErrFiltPrev*cGridVoltFiltGainDen + (INT32S)dwGirdVoltDAxisErr*cGridVoltFiltGainNum)>>10);
	wGridVoltDAxisErrFiltPrev = wGirdVoltDAxisErrFilt;
    //integral
	dwGridVoltDAxisIngCtrl = dwGridVoltDAxisIngCtrl + (INT32S)wGirdVoltDAxisErrFilt*cGridVoltKi + (INT32S)wGridDsCurrSatErr*cGridVoltKaw;
	//PI 
	wGirdVoltDAxisPICtrl = (INT16S)(((INT32S)wGirdVoltDAxisErrFilt*cGridVoltKp + dwGridVoltDAxisIngCtrl)>>12);
		
	wGridDsCurrRefPreSat = wGirdVoltDAxisPICtrl - (INT16S)(((INT32S)tGridVoltTDqo.q * wWC)>>6);// + tLoadCurrTDqo.d;
	//Limit of the d channel current reference 
	    
	if(wGridDsCurrRefPreSat > 13064)//wInvDsCurrLimitHigh 4580
	{		
		wGridDsCurrRef = 13064;
	}
	else if(wGridDsCurrRefPreSat < -13064)//wInvDsCurrLimitLow
	{		
		wGridDsCurrRef = -13064;
	}
	else
	{
		wGridDsCurrRef = wGridDsCurrRefPreSat;
	}
    
	wGridDsCurrSatErr = wGridDsCurrRef - wGridDsCurrRefPreSat;
	tVSInvCurrDqoRef.d = wGridDsCurrRef;
	
	//===============================Grid Volt Q Axis closed-loop control==============================//  	 		
    dwGridVoltQAxisErr = (INT32S)((INT32S)(wVoltQAim - tGridVoltTDqo.q)<<4) - (((INT32S)wVSIdPrev * wWL)>>10) - (((INT32S)wVSIqPrev * wRGrid)>>10) - wVSIqFilt;
   
    if(dwGridVoltQAxisErr > 32000)
	{
	    dwGridVoltQAxisErr = 32000;
	}
	else if(dwGridVoltQAxisErr < -7838)
	{
	    dwGridVoltQAxisErr = -7838;
	}
	
	if(fCPLDShutInvDeal == 1)
    {
        wGridVoltQAxisErrFiltPrev = 0;
        dwGridVoltQAxisIngCtrl = 0;
        dwGridVoltQAxisErr = 0;
    }
    
	wGridVoltQAxisErrFilt = (INT16S)(((INT32S)wGridVoltQAxisErrFiltPrev*cGridVoltFiltGainDen + (INT32S)dwGridVoltQAxisErr*cGridVoltFiltGainNum)>>10);
	wGridVoltQAxisErrFiltPrev = wGridVoltQAxisErrFilt;
    
	//integral
	dwGridVoltQAxisIngCtrl = dwGridVoltQAxisIngCtrl + (INT32S)wGridVoltQAxisErrFilt*cGridVoltKi + (INT32S)wGridVoltQAxisSatErr*cGridVoltKaw;    
    //PI
	wGridVoltQAxisPICtrl = (INT16S)(((INT32S)wGridVoltQAxisErrFilt*cGridVoltKp + dwGridVoltQAxisIngCtrl)>>12);
	
	wGridVoltQAxisPreSat = wGridVoltQAxisPICtrl + (INT16S)(((INT32S)tGridVoltTDqo.d * wWC)>>6);// + tLoadCurrTDqo.q;
    
    if(wGridVoltQAxisPreSat > 7838)
	{
	    wGridQsCurrRef = 7838;
	}
	else if(wGridVoltQAxisPreSat < -7838)
	{
	    wGridQsCurrRef = -7838;
	}
	else
	{
	    wGridQsCurrRef = wGridVoltQAxisPreSat;
	}
	
	wGridVoltQAxisSatErr = wGridQsCurrRef - wGridVoltQAxisPreSat;
	tVSInvCurrDqoRef.q = wGridQsCurrRef;
	
	
    wVirtualRUd = 0;//((INT32S)tInvCurrTDqo.d * wRGrid)>>10; 
	wVirtualRUq = 0;//((INT32S)tInvCurrTDqo.q * wRGrid)>>10; 
 
	//--------------------dddddddddddddddddddddddddddddddddddddddd---------------------
	wVSInvDsCurrErr = tVSInvCurrDqoRef.d - tInvCurrTDqo.d;
    
	//Low-pass filter
	wVSInvDsCurrErrFilt = (INT16S)(((INT32S)wVSInvDsCurrErrFiltPrev*cInvCurrFiltGainDen + (INT32S)wVSInvDsCurrErr*cInvCurrFiltGainNum)>>12);
	wVSInvDsCurrErrFiltPrev = wVSInvDsCurrErrFilt; //Q8
	
	//integral
	dwVSInvDsCurrIngCtrl = dwVSInvDsCurrIngCtrl + (INT32S)wVSInvDsCurrErrFilt*cInvCurrKi + dwVSInvDsModuSatErr*cInvCurrKaw;
    if(fCPLDShutInvDeal == 1)
    {
        dwVSInvDsCurrIngCtrl = 0;
    }
	//PI 
	dwVSInvDsCurrPICtrl = ((INT32S)wVSInvDsCurrErrFilt*cInvCurrKp + dwVSInvDsCurrIngCtrl)>>12;
	
	dwVSInvDsModuPreSat = dwVSInvDsCurrPICtrl - (((INT32S)tVSInvCurrDqoRef.q*wWL)>>10);
      
    if(fRunInvAhdChkRequire == 1)
	{	
	    dwVSInvDsModuPreSat = (((INT64S)dwVSInvDsModuPreSat * cInvPwmK)>>14) - (((INT32S)wVirtualRUd * uwInvPwmKFeedBack)>>10) + (((INT32S)wVSVoltSet * uwInvPwmKFeedBack)>>10);
    }
    else 
    {
        wGridDFFSoftIncTmp = ((INT32S)wVSVoltSet * uwModuSoftIncCnt)>>11;
        dwVSInvDsModuPreSat = ((INT32S)wGridDFFSoftIncTmp * uwInvPwmBusComp)>>10;
    }
    
	if(dwVSInvDsModuPreSat > cInvModuLimitHigh)
	{
		wVSInvDsModu = cInvModuLimitHigh;
	}
	else if (dwVSInvDsModuPreSat < cInvModuLimitLow)
	{
		wVSInvDsModu = cInvModuLimitLow;
	}
	else
	{
		wVSInvDsModu = (INT16S)dwVSInvDsModuPreSat;
	}
	//Csaturate error for anti-windup use 
	dwVSInvDsModuSatErr = (((INT32S)wVSInvDsModu - dwVSInvDsModuPreSat)*cInvPwmKRec)>>9;
	
	//--------------------qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq---------------------
	wVSInvQsCurrErr = tVSInvCurrDqoRef.q - tInvCurrTDqo.q;	 
	//=============inverter current closed-loop Q Channel control =============//
	//Low-pass filter 
	wVSInvQsCurrErrFilt = (INT16S)(((INT32S)wVSInvQsCurrErrFiltPrev*cInvCurrFiltGainDen + (INT32S)wVSInvQsCurrErr*cInvCurrFiltGainNum)>>12);
	wVSInvQsCurrErrFiltPrev = wVSInvQsCurrErrFilt;
	
	//integral
	dwVSInvQsCurrIngCtrl = dwVSInvQsCurrIngCtrl + (INT32S)wVSInvQsCurrErrFilt*cInvCurrKi + dwVSInvQsModuSatErr*cInvCurrKaw;
    if(fCPLDShutInvDeal == 1)
    {
        dwVSInvQsCurrIngCtrl = 0;
    }
    //PI
	dwVSInvQsCurrPICtrl = ((INT32S)wVSInvQsCurrErrFilt*cInvCurrKp + dwVSInvQsCurrIngCtrl)>>12;
	
	dwVSInvQsModuPreSat = dwVSInvQsCurrPICtrl + (((INT32S)tVSInvCurrDqoRef.d*wWL)>>10);

	if(fRunInvAhdChkRequire == 1)//Normal procedure
	{	
	    dwVSInvQsModuPreSat = (((INT64S)dwVSInvQsModuPreSat * cInvPwmK)>>14)  - (((INT32S)wVirtualRUq * uwInvPwmKFeedBack)>>10) + (((INT32S)0 * uwInvPwmKFeedBack )>>10);
	}
	else
	{
        dwVSInvQsModuPreSat = 0; 
    }
	
	if (dwVSInvQsModuPreSat > cInvModuLimitHigh)
	{
		wVSInvQsModu = cInvModuLimitHigh;
	}
	else if (dwVSInvQsModuPreSat < cInvModuLimitLow)
	{
		wVSInvQsModu = cInvModuLimitLow;
	}
	else
	{
		wVSInvQsModu = (INT16S)dwVSInvQsModuPreSat;
	}
    dwVSInvQsModuSatErr = (((INT32S)wVSInvQsModu - dwVSInvQsModuPreSat)*cInvPwmKRec)>>9;
    
	ptInvModu->d = wVSInvDsModu;
	ptInvModu->q = wVSInvQsModu;
	ptInvModu->o = 0;
	//***********Bus voltage Diff closed-loop control**********************//
    //*********************************************************************//
    if(fRunInvAhdChkRequire == 1)
	{ 
	    fVSRunByGrid = 0;
	            
	    wBusDiffVoltErr = 0 - wBusDiffVoltSamp;	    	
	    //Low-pass filter
	    //wBusDiffVoltErrFilt = (INT16S)(((INT32S)wBusDiffVoltErrFiltPrev*cBusDiffVoltFiltGainDen + (INT32S)wBusDiffVoltErr*cBusDiffVoltFiltGainNum)>>15);
	    wBusDiffVoltErrFilt = wBusDiffVoltErr;
	    //wBusDiffVoltErrFiltPrev = wBusDiffVoltErrFilt;	
	    
	    //integral	    
	    dwBusDiffVoltIngCtrl = dwBusDiffVoltIngCtrl + (INT32S)wBusDiffVoltErrFilt*cBusDiffVoltKi + (INT32S)dwInvOsCurrSatErr*cBusDiffVoltKaw;
	    
	    //PI 
	    dwBusDiffVoltPICtrl = ((INT32S)wBusDiffVoltErrFilt*cBusDiffVoltKp + dwBusDiffVoltIngCtrl)>>7;
	    dwInvOsCurrRefPreSat = dwBusDiffVoltPICtrl;
	    
	    //Limit of the d channel current reference 
	    if (dwInvOsCurrRefPreSat > cInvOsCurrLimitHigh)
	    {		
	    	wInvOsCurrRef = cInvOsCurrLimitHigh;
	    }
	    else if (dwInvOsCurrRefPreSat < cInvOsCurrLimitLow)
	    {		
	    	wInvOsCurrRef = cInvOsCurrLimitLow;
	    }
	    else
	    {
	    	wInvOsCurrRef = dwInvOsCurrRefPreSat;
	    }
	    //saturate error for anti-windup use 
	    dwInvOsCurrSatErr = (INT32S)wInvOsCurrRef- dwInvOsCurrRefPreSat;
	    wInvOsCurrRef = wInvOsCurrRef + 16383;
	}
	else
	{
		wInvOsCurrRef = 16383;
	}
    
    wInvNosDsCurrErrFiltPrev = 0;
    dwInvNosDsModuSatErr = 0;
    dwInvNosDsCurrIngCtrl = 0;
    
    wInvNosQsCurrErrFiltPrev = 0;
    dwInvNosQsModuSatErr = 0;
    dwInvNosQsCurrIngCtrl = 0;
    
    ptInvNosModu->d = 0;
    ptInvNosModu->q = 0;
}
/*********************************************************************
Function name:  void sSVModu(T_THREE_PHASE_AFBTGM *ptModuAfBt, T_THREE_PHASE_ABC *ptModuAbc)
Description: SVPWM algorithm 
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_AFBTGM, T_THREE_PHASE_ABC
Return:  void        
*********************************************************************/
void sSVModu(T_THREE_PHASE_AFBTGM *ptModuAfBtGm,T_THREE_PHASE_ABC *ptInvCurrAbc,INT16S wBusDiffCtrl,T_THREE_DUTY_ABC *ptModuAbc)
{ 
    //Define local variables in this function
    INT16S wModuAf, wModuBt;          //common referenc vector for all sector
    INT16S wModuAfTemp,wModuBtTemp,wSqrt3ModuAfTemp,wSqrt3ModuBtTemp;
    INT16S wtemp,wtemp1,wtemp2,wtemp3,wtemp4;//wtemp variables for select sector and region. every wtemp variable denotes a beeline
    INT16S wIo1,wIo2;                 //neutral current is I01 for B,D,F region and I02 for ACE region           
    INT16S wSector = 0,wRegion = 0;   //define sector variables,where there are 6 Regions.
    INT16S wTa,wTb,wTc;               //vector dwelling time in the first sector
    INT16S wTaQ1,wTaQ2,wTbQ1,wTbQ2,wTcQ1,wTcQ2;   //A phase, B phase, C phase duty in the first sector on Ts period
    INT16S wFactor1,wFactor2;         //Q15  Bus voltage balance factor,wFactor1 + wFactor2 = 1
    INT16S wPwmTaQ1,wPwmTaQ2,wPwmTbQ1,wPwmTbQ2,wPwmTcQ1,wPwmTcQ2; //A, B, C phase common duty for all sectors on Ts period 
	/***<Step 1>*********Judge Which sector the reference vector in ********************/
    wModuAfTemp = ptModuAfBtGm->Af;      //Ualfa 
    wModuBtTemp = ptModuAfBtGm->Bt;      //Ubeta 
    wSqrt3ModuAfTemp = (INT16S)(((INT32S)wModuAfTemp*cSqrt3)>>14);//sqrt3*Ualfa
    wSqrt3ModuBtTemp = (INT16S)(((INT32S)wModuBtTemp*cSqrt3)>>14);//sqrt3*Ubeta
    wtemp1 = wModuBtTemp;                //Ubeta 
    wtemp2 = wModuBtTemp - wSqrt3ModuAfTemp; //Ubeta - sqrt3*Ualta
    wtemp3 = wModuBtTemp + wSqrt3ModuAfTemp; //Ubeta + sqrt3*Ualta    

    wFactor1 = wBusDiffCtrl;             //Bus Balance control value   
    wFactor2 = 0x7FFF - wBusDiffCtrl;
    /****Judge sector ******start*********/      
    if(wtemp1>=0)
    {
        if(wtemp2<=0)
        {
            wSector=1;
        }
        else 
        {
            if(wtemp3<=0) 
            { 
                wSector=3;
            }
            else
            {
                wSector=2;
            }
        }
    }
    if(wtemp1<0)
    {
        if(wtemp2>=0)
        {
            wSector=4;
        }
        else 
        {
        	  if(wtemp3>=0)
            {
            	  wSector=6;
            }
            else
            {
            	  wSector=5;
            }
        }
    }
    /*******<Step 1>***Judge sector ****End*************/
    
    /*******<Step 2>*******Referenc vectors in other sector transformed to first sector***********/
    wtemp1= wSqrt3ModuBtTemp >>1;   // Ubeta*sqrt3/2    
    wtemp2= wSqrt3ModuAfTemp >>1;   // Ualfa*sqrt3/2
    //ReadSector= wSector;//Tiger add 0910018
    switch (wSector)  
    {    
        case 1:
            wIo1 =  -ptInvCurrAbc->a;  //for B,D,F Region
            wIo2 = ptInvCurrAbc->c;    //for A,C,E Region 
            wModuAf = wModuAfTemp;                                         
            wModuBt = wModuBtTemp;	 		                                  
        break;    
                                                              
        case 2:   
            wIo1 =  -ptInvCurrAbc->c;  // wModuAfTemp,wModuBtTemp
            wIo2 =  ptInvCurrAbc->b;   //                                                               
            wModuAf = (INT16S)((INT32S)(((INT32S)wModuAfTemp<<2)+((INT32S)wtemp1<<3))>>3);      //   Ucos(theta-60) = Ualfa/2 + Ubeta*sqrt(3)/2
            wModuBt = (INT16S)((INT32S)(((INT32S)wModuBtTemp<<2)-((INT32S)wtemp2<<3))>>3);	    //   Usin(theta-60) = Ubeta/2 - Ualfa*sqrt(3)/2
        break;  
                                                                      
        case 3:   
            wIo1 = -ptInvCurrAbc->b;   //
            wIo2 = ptInvCurrAbc->a;    //                                                        //   
 		    wModuAf = (INT16S)((INT32S)(((INT32S)-wModuAfTemp<<2)+((INT32S)wtemp1<<3))>>3);     //   Ucos(theta-120) = -Ualfa/2 + Ubeta*sqrt(3)/2  
            wModuBt = (INT16S)((INT32S)(((INT32S)-wModuBtTemp<<2)-((INT32S)wtemp2<<3))>>3);     //   Usin(theta-120) = -Ubeta/2 - Ualfa*sqrt(3)/2  
        break; 
                                                                   
        case 4:   
            wIo1 =  -ptInvCurrAbc->a; // 
            wIo2 = ptInvCurrAbc->c;   //                                           
            wModuAf = -wModuAfTemp;                                                             //   Ucos(theta-180) = -Ualfa 
            wModuBt = -wModuBtTemp;                                                             //   Usin(theta-180) = -Ubeta 
        break;
                                                                                          
        case 5:  
            wIo1 = -ptInvCurrAbc->c;  //
            wIo2 = ptInvCurrAbc->b;   //                          
            wModuAf = (INT16S)((INT32S)(((INT32S)-wModuAfTemp<<2)-((INT32S)wtemp1<<3))>>3);     //
            wModuBt = (INT16S)((INT32S)(((INT32S)-wModuBtTemp<<2)+((INT32S)wtemp2<<3))>>3);     //
        break; 
                                        
        case 6:
            wIo1 = -ptInvCurrAbc->b;
            wIo2 = ptInvCurrAbc->a;
            wModuAf = (INT16S)((INT32S)(((INT32S)wModuAfTemp<<2)-((INT32S)wtemp1<<3))>>3);
            wModuBt = (INT16S)((INT32S)(((INT32S)wModuBtTemp<<2)+((INT32S)wtemp2<<3))>>3);  
        break;
        
        default:
            wIo1 = ptInvCurrAbc->a;
            wIo2 = ptInvCurrAbc->c;
            wModuAf = wModuAfTemp;
            wModuBt = wModuBtTemp;
        break;
    }
    /********<Step 2>********transformed *****End**********************/
    
    /********<Step 3>****Judge which region in the first sector *******/
    wtemp = (INT16S)(((INT32S)wModuAf*cSqrt3)>>14);                          //    |     ** 3* *     |
    //wtemp2 = wtemp1;                                                       //    |    **--*--**    |
    wtemp1 = wtemp + wModuBt - 0x1fff;    //sqrt3*Ualfa + Ubeta - 0x3fff ;   //    |   **---2---**   |	              
    wtemp2 = wtemp - wModuBt - 0x1fff;    //sqrt3*Ualfa - Ubeta - 0x3fff ;   //    |  **----*----**  |
    wtemp3 = wModuBt - 0x1000;            //Ubeta - 0x2000 ;                 //    | **1*********4** |
                                                          
    if(wtemp1<=0)                                             
	{
	    wRegion = 1;
	}                                  
	else if(wtemp3>=0)                 
	{ 	                               
	    wRegion = 3;   
	}                                  
	
	else if(wtemp2>=0)                 
	{        
	    wRegion = 4; 
	}                                  
	else                               
	{    
	    wRegion = 2;         	                 
    }		
    /********<Step 3>*****Judge **********End *******/     
    
    /********<Step 4>****compute dwelling time based on the different regions ****Start*******/
    wtemp4 = (INT16S)(((INT32S)wModuAf*cSqrt3Divide3)>>14)-wModuBt; 
    wtemp1 = wtemp - wModuBt; //sqrt3*Ualfa-Ubeta;
	wtemp2 = wtemp + wModuBt; //sqrt3*Ualfa+Ubeta;
    wtemp3 = wModuBt<<1;      //2*Ubeta
    switch  (wRegion)
	{
	    case 1:
	        //wTa = wtemp1;
	        //wTb = 0x1fff-wtemp2;
	        //wTc = wtemp3;
	        //break;
	    case 2:
	    	if (wtemp4 > 0) //<30 degree
	    	{
	            //wTa = 0x3fff-wtemp3;
	            //wTb = wtemp2-0x3fff;                      
                //wTc = 0x3fff-wtemp1;   
                
                wTa = 0x3fff-wtemp2;    //按第4小扇区处理            
	            wTb = wtemp3;                      
                wTc = wtemp1-0x1fff;
                  
            }
            else  //>30 degree 
            {
                wTa = wtemp3-0x1fff;      //按第3小扇区处理            
		        wTb = wtemp1;                      
                wTc = 0x3fff-wtemp2;
            }		    			    	
		    
		    //wTa = 0x1fff-wtemp3;
		    //wTb = wtemp2-0x1fff;                      
            //wTc = 0x1fff-wtemp1;
            break;
        case 3:
            wTa = wtemp3-0x1fff;                
		    wTb = wtemp1;                      
            wTc = 0x3fff-wtemp2;   
        break;
        
        case 4:
            wTa = 0x3fff-wtemp2;                
		    wTb = wtemp3;                      
            wTc = wtemp1-0x1fff;
        break;
        
        default:
            wTa = wtemp1;
		    wTb = 0x1fff-wtemp2;
		    wTc = wtemp3;
	    break;     	                    
    }
    //Limiter of dwelling time           
    //if(wTa>0x1fff)
    //{
    //    wTa=0x1fff;
    //}
    //if(wTb>0x1fff)
    //{
    //    wTb=0x1fff;
    //}
    //if(wTc>0x1fff)
    //{
    //    wTc=0x1fff;
    //}       
    /********<Step 4>****compute dwelling time  **********End *******/ 
    
    /********<Step 5>****compute three phase switch active time (duty) for first sector ***Start*******/           
    wtemp1 = (INT16S)(((INT32S)wModuAf*cSqrt3Divide3)>>14)-wModuBt;;      
    switch (wRegion)
    {              
        case 1:
            //if(wtemp1>0) // 30 degree bisector, >0 means lower space, theta <30 degress; <0 means higher space, theta >30 degress
            //{
            //    if(wIo1<0)  // swap bus voltage balance factor
            //    {
            //        wtemp = wFactor1;
            //        wFactor1 = wFactor2;
            //        wFactor2 = wtemp; 
            //    }
            //    wTaQ1 = (INT16S)(((INT32S)wTa*wFactor1)>>15);
            //    wTaQ2 = 0x1fff;
            //    wTbQ1 = 0;
            //    wTbQ2 = 0x1fff-(INT16S)(((INT32S)wTa*wFactor2)>>15);  
            //    wTcQ1 = 0;
            //    wTcQ2 = 0x1fff-(INT16S)(((INT32S)wTa*wFactor2)>>15)-wTc;
            //}
            //else
            //{
            //    if(wIo2<0) // swap bus voltage balance factor
            //    {
            //        wtemp = wFactor1;
            //        wFactor1 = wFactor2;
            //        wFactor2 = wtemp; 
            //    }                      
            //    wTaQ1 = (INT16S)(((INT32S)wTc*wFactor1)>>15)+wTa;
            //    wTaQ2 = 0x1fff;
            //    wTbQ1 = (INT16S)(((INT32S)wTc*wFactor1)>>15); 
            //    wTbQ2 = 0x1fff;
            //    wTcQ1 = 0;
            //    wTcQ2 = 0x1fff-(INT16S)(((INT32S)wTc*wFactor2)>>15);                                            
            //}
            //break;
        case 2:
            if(wtemp1>0) 
            {
                if(wIo1<0)
                {
               	    wtemp = wFactor1;
               	    wFactor1 = wFactor2;
               	    wFactor2 = wtemp; 
              	}  
                wTaQ1 = (INT16S)(((INT32S)wTa*wFactor1)>>15)+wTb+wTc; 
                wTaQ2 = 0x1fff;
                wTbQ1 = 0;
                wTbQ2 = (INT16S)(((INT32S)wTa*wFactor1)>>15)+wTb;
                wTcQ1 = 0;
                wTcQ2 = (INT16S)(((INT32S)wTa*wFactor1)>>15); 
            }  
            else 
            {
                if(wIo2<0)
                {
                    wtemp = wFactor1;
                    wFactor1 = wFactor2;
                    wFactor2 = wtemp; 
                }   
                wTaQ1 = (INT16S)(((INT32S)wTc*wFactor1)>>15)+wTa+wTb; 
                wTaQ2 = 0x1fff;
                wTbQ1 = (INT16S)(((INT32S)wTc*wFactor1)>>15)+wTa;
                wTbQ2 = 0x1fff;
                wTcQ1 = 0;
                wTcQ2 = (INT16S)(((INT32S)wTc*wFactor1)>>15);
            }
            break;
        case 3:
            if(wIo2<0)
            {
                wtemp = wFactor1;
                wFactor1 = wFactor2;
                wFactor2 = wtemp; 
            }
            wTaQ1 = (INT16S)(((INT32S)wTc*wFactor1)>>15)+wTa+wTb; 
            wTaQ2 = 0x1fff;
            wTbQ1 = (INT16S)(((INT32S)wTc*wFactor1)>>15)+wTa;
            wTbQ2 = 0x1fff;
            wTcQ1 = 0;
            wTcQ2 = (INT16S)(((INT32S)wTc*wFactor1)>>15);
            break;
        case 4:
            if(wIo1<0)
            {
                wtemp = wFactor1;
                wFactor1 = wFactor2;
                wFactor2 = wtemp; 
            }
            wTaQ1 = (INT16S)(((INT32S)wTa*wFactor1)>>15)+wTb+wTc; //
            wTaQ2 = 0x1fff;
            wTbQ1 = 0;
            wTbQ2 = (INT16S)(((INT32S)wTa*wFactor1)>>15)+wTb;//
            wTcQ1 = 0;
            wTcQ2 = (INT16S)(((INT32S)wTa*wFactor1)>>15);            
            break;  
        default:
            wTaQ1 =(wTc>>1)+(wTa>>1)+wTb;
            wTaQ2 = 0x1fff;
            wTbQ1 = (wTc>>1);
            wTbQ2 = 0x1fff-(wTa>>1);
            wTcQ1 = 0;
            wTcQ2 = 0x1fff-(wTc>>1)-(wTa>>1);
            break;
    }
    /********<Step 5>****compute three phase switch active time (duty) for first sector ***END *******/ 
    
    /********<Step 6>****compute three phase switch active time (duty) for six sectors ****START *******/    
    switch(wSector)
    {
        case 1:
            wPwmTaQ1 = wTaQ1;
            wPwmTaQ2 = wTaQ2;
            wPwmTbQ1 = wTbQ1;
            wPwmTbQ2 = wTbQ2;
            wPwmTcQ1 = wTcQ1;
            wPwmTcQ2 = wTcQ2;
            break;
        case 2:
            wPwmTaQ1 = 0x1fff-wTbQ2;
            wPwmTaQ2 = 0x1fff-wTbQ1;
            wPwmTbQ1 = 0x1fff-wTcQ2;
            wPwmTbQ2 = 0x1fff-wTcQ1;
            wPwmTcQ1 = 0x1fff-wTaQ2;
            wPwmTcQ2 = 0x1fff-wTaQ1;
            break;    
        case 3:
            wPwmTaQ1 = wTcQ1;
            wPwmTaQ2 = wTcQ2;
            wPwmTbQ1 = wTaQ1;
            wPwmTbQ2 = wTaQ2;
            wPwmTcQ1 = wTbQ1;
            wPwmTcQ2 = wTbQ2;
            break;
        case 4:
            wPwmTaQ1 = 0x1fff-wTaQ2;
            wPwmTaQ2 = 0x1fff-wTaQ1;
            wPwmTbQ1 = 0x1fff-wTbQ2;
            wPwmTbQ2 = 0x1fff-wTbQ1;
            wPwmTcQ1 = 0x1fff-wTcQ2;
            wPwmTcQ2 = 0x1fff-wTcQ1;
            break;
        case 5:                
            wPwmTaQ1 = wTbQ1;
            wPwmTaQ2 = wTbQ2;
            wPwmTbQ1 = wTcQ1;
            wPwmTbQ2 = wTcQ2;
            wPwmTcQ1 = wTaQ1;
            wPwmTcQ2 = wTaQ2;
            break;
        case 6:
            wPwmTaQ1 = 0x1fff-wTcQ2;
            wPwmTaQ2 = 0x1fff-wTcQ1;
            wPwmTbQ1 = 0x1fff-wTaQ2;
            wPwmTbQ2 = 0x1fff-wTaQ1;
            wPwmTcQ1 = 0x1fff-wTbQ2;
            wPwmTcQ2 = 0x1fff-wTbQ1;
            break;
        default:                  
            wPwmTaQ1 = wTaQ1;
            wPwmTaQ2 = wTaQ2;
            wPwmTbQ1 = wTbQ1;
            wPwmTbQ2 = wTbQ2;
            wPwmTcQ1 = wTcQ1;
            wPwmTcQ2 = wTcQ2;
            break;                                               
    }         
     //converte the duty of Q2 (switch 2) to the duty of Q4(switch 4). there are Q1,Q2,Q3,Q4 switchs in every bridge branch 
    wPwmTaQ2 = 0x1fff - wPwmTaQ2;
    wPwmTbQ2 = 0x1fff - wPwmTbQ2;
    wPwmTcQ2 = 0x1fff - wPwmTcQ2;
    
    if(fCSVSSwitch == 0)
    {
        if(fRunInvAhdChkRequire == 1)
        {
            if(tInvPWMModuAbc.a > 0)
            {
                if(tInvCurrAbcRef.a > 0) wPwmTaQ1 = wPwmTaQ1 + cPwmDeadTime;
                else wPwmTaQ1 = wPwmTaQ1 - cPwmDeadTime;
            }
            else 
            {
                if(tInvCurrAbcRef.a > 0) wPwmTaQ2 = wPwmTaQ2 - cPwmDeadTime;
                else wPwmTaQ2 = wPwmTaQ2 + cPwmDeadTime;	
            }
            
            if(tInvPWMModuAbc.b > 0)
            {
                if(tInvCurrAbcRef.b > 0) wPwmTbQ1 = wPwmTbQ1 + cPwmDeadTime;
                else wPwmTbQ1 = wPwmTbQ1 - cPwmDeadTime;
            }
            else 
            {
                if(tInvCurrAbcRef.b > 0) wPwmTbQ2 = wPwmTbQ2 - cPwmDeadTime;
                else wPwmTbQ2 = wPwmTbQ2 + cPwmDeadTime;	
            }
            
            if(tInvPWMModuAbc.c > 0)
            {
                if(tInvCurrAbcRef.c > 0) wPwmTcQ1 = wPwmTcQ1 + cPwmDeadTime;
                else wPwmTcQ1 = wPwmTcQ1 - cPwmDeadTime;
            }
            else 
            {
                if(tInvCurrAbcRef.c > 0) wPwmTcQ2 = wPwmTcQ2 - cPwmDeadTime;
                else wPwmTcQ2 = wPwmTcQ2 + cPwmDeadTime;	
            }
        }
    }
    
    //Limiter of max PWM and min PWM
    if(wPwmTaQ1>cMaxPwm)
    {
        wPwmTaQ1 = 8192;
    }
    if(wPwmTaQ1<cMinPwm)
    {
    	wPwmTaQ1 = 0; 
    }  
    
    if(wPwmTaQ2>cMaxPwm)
    {
    	wPwmTaQ2 = 8192;
    }
    if(wPwmTaQ2<cMinPwm)
    {
    	wPwmTaQ2 = 0; 
    }
      
    if(wPwmTbQ1>cMaxPwm)
    {
    	wPwmTbQ1 = 8192;
    }
    if(wPwmTbQ1<cMinPwm)
    {
    	wPwmTbQ1 = 0;
    } 
    if(wPwmTbQ2>cMaxPwm)
    {
    	wPwmTbQ2 = 8192;
    }
    if(wPwmTbQ2<cMinPwm)
    {
    	wPwmTbQ2 = 0;  
    }              
                       
    if(wPwmTcQ1>cMaxPwm)
    {
    	wPwmTcQ1 = 8192;
    }
    if(wPwmTcQ1<cMinPwm)
    { 
    	wPwmTcQ1 = 0;
    }                                
      
    if(wPwmTcQ2>cMaxPwm)
    {
    	wPwmTcQ2 = 8192;
    }
    if(wPwmTcQ2<cMinPwm)
    {
    	wPwmTcQ2 = 0; 
    }
   
    /********<Step 6>****compute three phase switch active time (duty) for six sectors ****END *******/              
    
    /********<Step 7>****compute modulate based on duty****Start *******/         
    wPwmTaQ1 = (INT16S)(((INT32S)wPwmTaQ1*uwInvCtrPWMPrd)>>13);
    //if(wPwmTaQ1<10)
    //{
    //    wPwmTaQ1 = 0;
    //} 
    wPwmTaQ2 = (INT16S)(((INT32S)wPwmTaQ2*uwInvCtrPWMPrd)>>13);
    //if(wPwmTaQ2<10)
    //{
    //    wPwmTaQ2 = 0;
    //}
    wPwmTbQ1 = (INT16S)(((INT32S)wPwmTbQ1*uwInvCtrPWMPrd)>>13);
    //if(wPwmTbQ1<10)
    //{            
    //    wPwmTbQ1 = 0;
    //}
    wPwmTbQ2 = (INT16S)(((INT32S)wPwmTbQ2*uwInvCtrPWMPrd)>>13);
    //if(wPwmTbQ2<10)
    //{    
    //    wPwmTbQ2 = 0;
    //}
    wPwmTcQ1 = (INT16S)(((INT32S)wPwmTcQ1*uwInvCtrPWMPrd)>>13);
    //if(wPwmTcQ1<10)
    //{ 
    //    wPwmTcQ1 = 0;
    //}
    wPwmTcQ2 = (INT16S)(((INT32S)wPwmTcQ2*uwInvCtrPWMPrd)>>13);  
    //if(wPwmTcQ2<10)
    //{
    //    wPwmTcQ2 = 0;
    //} 
    /********<Step 7>****compute modulate based on duty****End*******/
    (*ptModuAbc).A.word0 = wPwmTaQ1;
    (*ptModuAbc).A.word1 = wPwmTaQ2;
    (*ptModuAbc).B.word0 = wPwmTbQ1;
    (*ptModuAbc).B.word1 = wPwmTbQ2;
    (*ptModuAbc).C.word0 = wPwmTcQ1;
    (*ptModuAbc).C.word1 = wPwmTcQ2;
    //ptModuAbc->a = wPwmTaQ1 - wPwmTaQ2;
    //ptModuAbc->b = wPwmTbQ1 - wPwmTbQ2;
    //ptModuAbc->c = wPwmTcQ1 - wPwmTcQ2;
}
/*********************************************************************************
Function name: void sDciControl(INT16U uwfInvCtrlFlg)
Description: In R phase CrossZero interrupt or 20ms period task
Calls:  void
Called By: In R phase CrossZero interrupt or 20ms period task                                                           
Parameters: void
Return: void ,to interrupt sample		
**********************************************************************************/
void sDciControl(INT16U uwfInvCtrlFlg)
{			
	//Define local varibles in this function  
	static INT32S dwRDCIAvgSum = 0;
	static INT32S dwSDCIAvgSum = 0; 
	static INT32S dwTDCIAvgSum = 0;
	
	INT16S wRDCIAvg = 0;
	INT16S wSDCIAvg = 0;
	INT16S wTDCIAvg = 0;
	  
    INT16U uwDCIAbsR = 0;
    INT16U uwDCIAbsS = 0;
    INT16U uwDCIAbsT = 0;
    INT16S wDCISum = 0;

    INT16S wDCIMax = 0;
    INT16S wDCIMaxPhase = 0;
    
    INT16S wDCIMid = 0;
    INT16S wDCIMidPhase = 0;
    
    INT16U uwDCIMaxAbs = 0;
    INT16U uwDCIMidAbs = 0;
    
    INT16S wDCICtrlR = 0;
    INT16S wDCICtrlS = 0;
    INT16S wDCICtrlT = 0; 
    INT16S wDCIReguStep = 1;
    INT16S wDCIReguMax;
    INT16S wDCIReguMin;
    INT16S wDCIReguMid;
	//average value
	wRDCIAvgCurr = swGetAvg(&dwRDCICurrSum,&uwRDCISumCnt,&fRDCISumClear);
	wSDCIAvgCurr = swGetAvg(&dwSDCICurrSum,&uwSDCISumCnt,&fSDCISumClear);
	wTDCIAvgCurr = swGetAvg(&dwTDCICurrSum,&uwTDCISumCnt,&fTDCISumClear);		
	
	wDCISum = (INT16S)((((INT32S)wRDCIAvgCurr + wSDCIAvgCurr + wTDCIAvgCurr)*cDivide3)>>14);
    
    wRDCIAvgCurr = wRDCIAvgCurr - wDCISum;
    wSDCIAvgCurr = wSDCIAvgCurr - wDCISum;
    wTDCIAvgCurr = wTDCIAvgCurr - wDCISum;
    
	//when inverter operation in closed-loop
	if((uwfInvCtrlFlg == 1)&&(fGridRelayCtr == 1)&&(fCSVSSwitch == 0))
	{
		dwRDCIAvgSum = dwRDCIAvgSum + wRDCIAvgCurr;
        dwSDCIAvgSum = dwSDCIAvgSum + wSDCIAvgCurr;
        dwTDCIAvgSum = dwTDCIAvgSum + wTDCIAvgCurr;
        
        if((++uwRPhaseZeroCrossCnt) >= 2)
        {
        	uwRPhaseZeroCrossCnt = 0;
        	
        	wRDCIAvg = (INT16S)(dwRDCIAvgSum>>1);
	    	wSDCIAvg = (INT16S)(dwSDCIAvgSum>>1);
	    	wTDCIAvg = (INT16S)(dwTDCIAvgSum>>1);
	    	
	    	dwRDCIAvgSum = 0;
	    	dwSDCIAvgSum = 0;
	    	dwTDCIAvgSum = 0;
	    	
		    uwDCIAbsR = abs(wRDCIAvg);
		    uwDCIAbsS = abs(wSDCIAvg);
		    uwDCIAbsT = abs(wTDCIAvg);
		    
		    //found max dc value and corresponding phase 
		    wDCIMax = wRDCIAvg;
		    uwDCIMaxAbs = uwDCIAbsR;
		    wDCIMaxPhase = 1;
		    						
		    if (uwDCIAbsS > uwDCIMaxAbs)
		    {
		    	wDCIMax = wSDCIAvg;
		    	uwDCIMaxAbs = uwDCIAbsS;
		    	wDCIMaxPhase = 2;
		    }			
		    if (uwDCIAbsT > uwDCIMaxAbs)
		    {
		    	wDCIMax = wTDCIAvg;
		    	uwDCIMaxAbs = uwDCIAbsT;
		    	wDCIMaxPhase = 3;
		    }
		    
		    if(uwDCIMaxAbs > cDCI150mA)
		    {
		    	wDCIReguStep = 2;
		    }
            else
            {
            	wDCIReguStep = 1;
            }
		    //control value is added 1 or -1, based on the polarity of max value  
		    if (wDCIMax > 0)
		    {
		    	if (wDCIMaxPhase == 1)
		    	{
		    		wDCICtrlR = wDCIReguStep;
		    	}
		    	else if (wDCIMaxPhase == 2)
		    	{
		    		wDCICtrlS = wDCIReguStep;		    		
		    	}
		    	else
		    	{
		    		wDCICtrlT = wDCIReguStep;
		    	}
		    }
		    else
		    {
		    	if (wDCIMaxPhase == 1)
		    	{
		    		wDCICtrlR = -wDCIReguStep;
		    	}
		    	else if (wDCIMaxPhase == 2)
		    	{
		    		wDCICtrlS = -wDCIReguStep;
		    		
		    	}
		    	else
		    	{
		    		wDCICtrlT = -wDCIReguStep;
		    	}
		    }
		    //found middle dc value and corresponding phase 
		    if (((uwDCIAbsR > uwDCIAbsS) && (uwDCIAbsR < uwDCIAbsT)) || ((uwDCIAbsR > uwDCIAbsT) && (uwDCIAbsR < uwDCIAbsS)))
		    {
		    	wDCIMid = wRDCIAvg;
		    	uwDCIMidAbs = uwDCIAbsR;
		    	wDCIMidPhase = 1;
		    }
		    else if (((uwDCIAbsS > uwDCIAbsR) && (uwDCIAbsS < uwDCIAbsT)) || ((uwDCIAbsS > uwDCIAbsT) && (uwDCIAbsS < uwDCIAbsR)))
		    {
		    	wDCIMid = wSDCIAvg;
		    	uwDCIMidAbs = uwDCIAbsS;
		    	wDCIMidPhase = 2;
		    }
		    else
		    {
		    	wDCIMid = wTDCIAvg;
		    	uwDCIMidAbs = uwDCIAbsT;
		    	wDCIMidPhase = 3;
		    }
		    
		    if(uwDCIMidAbs > cDCI150mA)
		    {
		    	wDCIReguStep = 2;
		    }
		    else
		    {
		    	wDCIReguStep = 1;
		    }
            //control value is added 1 or -1, based on the polarity of middle value 
		    if (wDCIMid > 0)
		    {
		    	if (wDCIMidPhase == 1)
		    	{
		    		wDCICtrlR = wDCIReguStep;
		    	}
		    	else if (wDCIMidPhase == 2)
		    	{
		    		wDCICtrlS = wDCIReguStep;
		    		
		    	}
		    	else
		    	{
		    		wDCICtrlT = wDCIReguStep;
		    	}
		    }
		    else
		    {
		    	if (wDCIMidPhase == 1)
		    	{
		    		wDCICtrlR = -wDCIReguStep;
		    	}
		    	else if (wDCIMidPhase == 2)
		    	{
		    		wDCICtrlS = -wDCIReguStep;
		    		
		    	}
		    	else
		    	{
		    		wDCICtrlT = -wDCIReguStep;
		    	}
		    }
		    //only when max dc value is bigger than 30mA, control value increase, otherwise do nothing.
		    if (uwDCIMaxAbs > cDCI64mA) //uwDCIMaxAbs:Q3,unit:mA
		    {
		    	if (wDCIMaxPhase == 1)
		    	{
		    		wDCIReguR = wDCIReguR + wDCICtrlR;
		    	}
		    	else if (wDCIMaxPhase == 2)
		    	{
		    		wDCIReguS = wDCIReguS + wDCICtrlS;
		    	}
		    	else
		    	{
		    		wDCIReguT = wDCIReguT + wDCICtrlT;
		    	}
		    
                if (uwDCIMidAbs > cDCI40mA)//uwDCIMidAbs:Q3,unit:mA 
		    	{
		    	    if (wDCIMidPhase == 1)
		    	    {
		    	        wDCIReguR = wDCIReguR + wDCICtrlR;
		    	    }
		    	    else if (wDCIMidPhase == 2)
		    	    {
		    		    wDCIReguS = wDCIReguS + wDCICtrlS;
		    	    }
		    	    else
		    	    {
		    		    wDCIReguT = wDCIReguT + wDCICtrlT;
		    	    }							
		    	}
		    }
		    
		    wDCIReguMax = wDCIReguT;
		    if(wDCIReguR > wDCIReguMax)
		    {
		    	wDCIReguMax = wDCIReguR;
		    }
		    
		    if(wDCIReguS > wDCIReguMax)
		    {
		    	wDCIReguMax = wDCIReguS;
		    }
		    
		    wDCIReguMin = wDCIReguT;
		    if(wDCIReguR < wDCIReguMin)
		    {
		    	wDCIReguMin = wDCIReguR;
		    }
		    
		    if(wDCIReguS < wDCIReguMin)
		    {
		    	wDCIReguMin = wDCIReguS;
		    }
		    
		    wDCIReguMid = wDCIReguR + wDCIReguS + wDCIReguT - wDCIReguMax - wDCIReguMin; 
		    
		    wDCIReguR = wDCIReguR - wDCIReguMid;
		    wDCIReguS = wDCIReguS - wDCIReguMid;
		    wDCIReguT = wDCIReguT - wDCIReguMid;
		    //wDCIReguR = wDCIReguR - wDCIReguT;             
		    //wDCIReguS = wDCIReguS - wDCIReguT;            
		    //wDCIReguT = 0;             
		    
		    //R
		    if (wDCIReguR > cDCIReguHigh)
		    {
		    	wDCIReguR = cDCIReguHigh;
		    }
		    else if (wDCIReguR < cDCIReguLow)
		    {
		    	wDCIReguR = cDCIReguLow;
		    }
		    else
		    {
		    	wDCIReguR = wDCIReguR;
		    }
	        //S
		    if (wDCIReguS > cDCIReguHigh)
		    {
		    	wDCIReguS = cDCIReguHigh;
		    }
		    else if (wDCIReguS < cDCIReguLow)
		    {
		    	wDCIReguS = cDCIReguLow;
		    }			
		    else
		    {
		    	wDCIReguS = wDCIReguS;
		    }
		    //T			
		    if (wDCIReguT > cDCIReguHigh)
		    {
		    	wDCIReguT = cDCIReguHigh;
		    }
		    else if (wDCIReguT < cDCIReguLow)
		    {
		    	wDCIReguT = cDCIReguLow;
		    }			
		    else
		    {
		    	wDCIReguT = wDCIReguT;
		    }
		}		
	}
	else
	{
		wDCIReguR = 0;
		wDCIReguS = 0;
		wDCIReguT = 0;
		dwRDCIAvgSum = 0;
	    dwSDCIAvgSum = 0;
	    dwTDCIAvgSum = 0;
        uwRPhaseZeroCrossCnt = 0;
	}
	
	fDCIReguRefurbish = 1;
}
/************************************************************************
Function name: INT32S sdwArcTan(INT16S wUd,INT16S wUq)
Description:  calculate atan value
Calls:        
Called By:   sPLLControl(void)    
Parameters:  INT32S
Return:  INT32S      
************************************************************************/
INT32S sdwArcTan(INT16S wUd,INT16S wUq)
{
INT16U fSighflag = 0;
INT32U udwTemp;
INT32S dwTempCal;
		
    dwTempCal = (INT32S)wUq<<10;    
    if(wUd != 0)
    {
    	dwTempCal = dwTempCal/wUd;
    }
    if(dwTempCal >= 0)
    {
    	fSighflag = 0;
    }
    else
    {
    	fSighflag = 1;
    }
    udwTemp = labs(dwTempCal);
    if(udwTemp <= 1024)        //1以内分为64份
    {
    	udwTemp = udwTemp>>4;
    }
    else if(udwTemp <= 2048)   //2以内分为32份
    {
    	udwTemp = (udwTemp>>5) + 32;
    }
    else if(udwTemp <= 3072)   //3以内分为16份
    {
    	udwTemp = (udwTemp>>6) + 64;
    }
    else if(udwTemp <= 4096)   //4以内分为8份
    {
    	udwTemp = (udwTemp>>7) + 88;
    }
    else if(udwTemp <= 5120)   //5以内分为4份
    {
    	udwTemp = (udwTemp>>8) + 104;
    }
    else if(udwTemp <= 6144)   //6以内分为2份
    {
    	udwTemp = (udwTemp>>9) + 114;
    }
    else                       //7以上分为1份
    {
    	udwTemp = (udwTemp>>10) + 120;
    	if(udwTemp > 199)
    	{
    		udwTemp = 199;
    	}
    }
    if(fSighflag == 0)
    {
        dwTempCal = dwATANArray[(INT16U)udwTemp];
    }
    else
    {
    	dwTempCal = -dwATANArray[(INT16U)udwTemp];
    }
    if(wUd < 0)
    {
    	dwTempCal = 0x80000000 + dwTempCal;
    }
    return(dwTempCal);
}
/************************************************************************
Function name:  void sNotchFilter(T_NotchOut_Prev *ptOutPrev,T_NotchIn_Prev *ptInPrev,INT16S InNow)
Description:  from  alfa-beta reference frame to ABC reference frame
Calls:        
Called By:  sInvController(......)    
Parameters:  T_THREE_PHASE_AFBTGM, T_THREE_PHASE_ABC
Return:  void      
************************************************************************/
void sNotchFilter8K(T_NotchOut_Prev *ptOutPrev,T_NotchIn_Prev *ptInPrev,INT16S InNow)
{
INT16S wPrev,wPrevPrev;
    wPrev = (ptOutPrev->N1)>>14;
    wPrevPrev = (ptOutPrev->N0)>>14;
    ptOutPrev->N2 = (INT32S)cNotch8KNum1*InNow - (INT32S)cNotch8KNum2*ptInPrev->N1 + (INT32S)cNotch8KNum1*ptInPrev->N0 \
                    + (INT32S)cNotch8KNum2*wPrev - (INT32S)cNotch8KNum3*wPrevPrev;
    ptOutPrev->N0 = ptOutPrev->N1;
    ptOutPrev->N1 = ptOutPrev->N2;//Q18
    ptInPrev->N0 = ptInPrev->N1;
    ptInPrev->N1 = InNow;       
}	
/*********************************************************************
Function name: void sCalLVRTReactivePowerRef(T_THREE_PHASE_DQO *ptPosCurrRef,T_THREE_PHASE_DQO *ptNosCurrRef)
Description: 计算LVRT发生时的正负序无功电流指令
Calls:      	 
Called By:  main(void) 5ms
Parameters:  void
Return:  void      
 *********************************************************************/
void sCalLVRTReactivePowerRef(T_THREE_PHASE_DQO *ptPosCurrRef,T_THREE_PHASE_DQO *ptNosCurrRef)
{
INT16S wpuPosCurrRef,wpuNosCurrRef;
INT16S wpuPosVol,wpuNosVol;
INT32S dwTemp;
INT16S wTemp,wpuIqRefbak;
static INT16S wPosCurrRefFiltPrev = 0;   
static INT16S wNosCurrRefFiltPrev = 0;
static INT16U fFirstUpdateEn = 1;
static INT32S dwPosSum = 65600;
static INT32S dwNosSum = 0;
static INT16U i = 0;
static INT16U fFVRT = 0;
INT16S wPosCurrRefFilt,wNosCurrRefFilt;
INT16S wRatedInvCurrPeakLVRT,wInvCurrPeakMaxLVRT;
    
    wRatedInvCurrPeakLVRT = wRatedInvCurrPeak;
    wInvCurrPeakMaxLVRT = ((INT32S)wRatedInvCurrPeak * 18022)>>14;

    if(fLVRTStartOrEnd == 1)  //跌落发生
    {
        if(fImbalanceLVRT == 1)//不平衡跌落时，需要考察相电压
        {
            wpuPosVol = abs(tGridPosVoltSampDqo.d);
            wpuNosVol = abs(tGridNosVoltSampDqo.d);
            wpuPosVol = ((INT32S)wpuPosVol * cPuGridVolK480V)>>10;
            wpuNosVol = ((INT32S)wpuNosVol * cPuGridVolK480V)>>10;                
            
            wpuPosVol = abs(16384 - wpuPosVol);
            if(wpuPosVol < 820)
            {
                wpuPosVol = 820;
            }
            if(wpuNosVol > 14745)
            {
                wpuNosVol = 14745;
            }         
	        
	        dwPosSum = dwPosSum + wpuPosVol - wpuPosVolData[i];
	        wpuPosVolData[i] = wpuPosVol;
	        
	        dwNosSum = dwNosSum + wpuNosVol - wpuNosVolData[i];
	        wpuNosVolData[i] = wpuNosVol;
	        
	        wpuPosVol = dwPosSum/((INT16S)uwTwiceFreSampNum);
	        wpuNosVol = dwNosSum/((INT16S)uwTwiceFreSampNum);
	        
	        i++;
	        if(i >= uwTwiceFreSampNum)
	        {
	            i = 0;
	        }
        }
        else if(fLVRTValue <= uwSoftLVRTValue)//uwLVRTActiveValue
        {
            wpuPosVol = abs(16384 - fLVRTValue);
            wpuNosVol = 0;
        }
        else if(fHVRTValue >= uwSoftHVRTValue)//uwHVRTActiveValue
        {
        	wpuPosVol = abs(fHVRTValue - 16384);
            wpuNosVol = 0;
        }
        else
        {
            wpuPosVol = 820;
            wpuNosVol = 0;
        }        

        wDebugPosVol = wpuPosVol;
        wDebugNosVol = wpuNosVol;
                 	
        wpuIqRefbak = ((INT32S)wIqRefBakToLVRT * wRatedInvCurrInv)>>13;
                                
        if(wEepromStandardValue == cBDEWStandard)
        {
        	wTemp = ((INT32S)wEepromFVRTPosCurrK * 13107 + 32)>>6;//Q11
            dwTemp = ((INT32S)wTemp * wpuPosVol)>>11;
            
            if(fLVRTValue <= uwLVRTActiveValue)
            {
            	fFVRT = 1;
            	dwTemp = dwTemp - wpuIqRefbak;
            	
            	if(fImbalanceLVRT == 1)
                {
                    if(dwTemp > 6554)
                    {
                        wpuPosCurrRef = 6554;
                    }
                    else if(dwTemp < -16384)
                    {
                    	wpuPosCurrRef = -16384;
                    }
                    else
                    {
                    	wpuPosCurrRef = dwTemp;
                    }
                }
                else
                {
                    if(dwTemp > 16384)//18022
                    {
                        wpuPosCurrRef = 16384;//16384
                    }
                    else if(dwTemp < -16384)
                    {
                    	wpuPosCurrRef = -16384;
                    }
                    else
                    {
                    	wpuPosCurrRef = dwTemp;
                    }                                      
                }

                wpuNosCurrRef = 16384 - abs(wpuPosCurrRef);
                if(wpuNosCurrRef < 0)
                {
                    wpuNosCurrRef = 0;
                }                
            }
            else if(fHVRTValue >= uwHVRTActiveValue)
            {            	
                fFVRT = 2;
                dwTemp = dwTemp + wpuIqRefbak;
                
                if(fImbalanceLVRT == 1)
                {
                    if(dwTemp > 3277)
                    {
                        wpuPosCurrRef = 3277;
                    }
                    else if(dwTemp < -3277)
                    {
                    	wpuPosCurrRef = -3277;
                    }
                    else
                    {
                    	wpuPosCurrRef = dwTemp;
                    }
                }
                else
                {
                    if(dwTemp > 6554)
                    {
                        wpuPosCurrRef = 6554;
                    }
                    else if(dwTemp < -6554)
                    {
                    	wpuPosCurrRef = -6554;
                    }
                    else
                    {
                    	wpuPosCurrRef = dwTemp;
                    }                    
                }
                
                wpuNosCurrRef = 6554 - abs(wpuPosCurrRef);
                if(wpuNosCurrRef < 0)
                {
                    wpuNosCurrRef = 0;
                }
            }
        }
        else if(wEepromStandardValue == cRule21Standard)
        {
            wTemp = ((INT32S)wEepromFVRTPosCurrK * 13107 + 32)>>6;//Q11
            dwTemp = ((INT32S)wTemp * wpuPosVol)>>11;
            wTemp = ((INT32S)wpuRmsCurrBakToLVRT * 14745)>>14;

            if(fLVRTValue <= uwLVRTActiveValue)
            {            	
                fFVRT = 1;
                
                if(dwTemp > 18022)
                {
                    wpuPosCurrRef = 18022;
                }
                else if(dwTemp < wTemp)
                {
                	wpuPosCurrRef = wTemp;
                }
                else
                {
                	wpuPosCurrRef = dwTemp;
                }
                
                if(fLVRTValue <= 8192)
                {
                    if(fImbalanceLVRT == 1)
                    {
                        wpuPosCurrRef = 0;
                    }
                    else
                    {
                        wpuPosCurrRef = 820;
                    }
                }
                               
                wpuNosCurrRef = 18022 - abs(wpuPosCurrRef);
                if(wpuNosCurrRef < 0)
                {
                    wpuNosCurrRef = 0;
                }
            }
            else if(fHVRTValue >= uwHVRTActiveValue)
            {
                fFVRT = 2;
                dwTemp = 820;
                wpuPosCurrRef = dwTemp;

                wpuNosCurrRef = 6554 - abs(wpuPosCurrRef);
                if(wpuNosCurrRef < 0)
                {
                    wpuNosCurrRef = 0;
                }
            }
        }
        else
        {
        	wTemp = ((INT32S)wEepromFVRTPosCurrK * 13107 + 32)>>6;//Q11
            dwTemp = ((INT32S)wTemp * wpuPosVol)>>11;            
            
            if(fLVRTValue <= uwLVRTActiveValue)
            {            	
                fFVRT = 1;
                dwTemp = dwTemp - wpuIqRefbak;                           
                
                if(dwTemp > 18022)
                {
                    wpuPosCurrRef = 18022;
                }
                else if(dwTemp < -18022)
                {
                	wpuPosCurrRef = -18022;
                }
                else
                {
                	wpuPosCurrRef = dwTemp;
                }
               
                wpuNosCurrRef = 18022 - abs(wpuPosCurrRef);
                if(wpuNosCurrRef < 0)
                {
                    wpuNosCurrRef = 0;
                }
            }
            else if(fHVRTValue >= uwHVRTActiveValue)
            {
                fFVRT = 2;
                dwTemp = dwTemp + wpuIqRefbak;
                
                if(fImbalanceLVRT == 1)
                {
                    if(dwTemp > 3277)
                    {
                        wpuPosCurrRef = 3277;
                    }
                    else if(dwTemp < -3277)
                    {
                    	wpuPosCurrRef = -3277;
                    }
                    else
                    {
                    	wpuPosCurrRef = dwTemp;
                    }
                }
                else
                {
                    if(dwTemp > 6554)
                    {
                        wpuPosCurrRef = 6554;
                    }
                    else if(dwTemp < -6554)
                    {
                    	wpuPosCurrRef = -6554;
                    }
                    else
                    {
                    	wpuPosCurrRef = dwTemp;
                    }
                    
                    wpuNosCurrRef = 6554 - abs(wpuPosCurrRef);
                    if(wpuNosCurrRef < 0)
                    {
                        wpuNosCurrRef = 0;
                    }
                }
            }
        }
               
        wTemp = ((INT32S)wEepromFVRTNegCurrK * 13107 + 32)>>6;//Q11  
        dwTemp = ((INT32S)wTemp * wpuNosVol)>>11;//k*wpuNosVol
                       
        if(((INT32S)wpuNosCurrRef) > dwTemp)
        {
            wpuNosCurrRef = dwTemp;
        }
                
        wpuPosCurrRef = ((INT32S)wpuPosCurrRef * wRatedInvCurrPeakLVRT)>>14;
        
        if(wpuPosCurrRef > wInvCurrPeakMaxLVRT)
        {
            wpuPosCurrRef = wInvCurrPeakMaxLVRT;
        }
        else if(wpuPosCurrRef < -wInvCurrPeakMaxLVRT)
        {
        	wpuPosCurrRef = -wInvCurrPeakMaxLVRT;
        }
              
        wpuNosCurrRef = ((INT32S)wpuNosCurrRef * wRatedInvCurrPeakLVRT)>>14;        
        if(wpuNosCurrRef > wInvCurrPeakMaxLVRT)
        {
            wpuNosCurrRef = wInvCurrPeakMaxLVRT;
        }      
            
        wPosCurrRefFilt = (INT16S)(((INT32S)wPosCurrRefFiltPrev*cCurrRefFiltDen + (INT32S)wpuPosCurrRef*cCurrRefFiltNum)>>15);
	    wPosCurrRefFiltPrev = wPosCurrRefFilt;
	    wNosCurrRefFilt = (INT16S)(((INT32S)wNosCurrRefFiltPrev*cCurrRefFiltDen + (INT32S)wpuNosCurrRef*cCurrRefFiltNum)>>15);
	    wNosCurrRefFiltPrev = wNosCurrRefFilt;
        if(fFirstUpdateEn == 1)
        {
            //ptPosCurrRef->q = -wpuPosCurrRef;
            wPosCurrRefFiltPrev = wpuPosCurrRef;
            //ptNosCurrRef->q = -wpuNosCurrRef;
            wNosCurrRefFiltPrev = wpuNosCurrRef;
            
            if(i == 0)
            {
                fFirstUpdateEn = 0;
            }
        }        
         	     
        if(fLVRTValue <= uwLVRTActiveValue)
        {      
            ptPosCurrRef->q = -wPosCurrRefFiltPrev;
            ptNosCurrRef->q = -wNosCurrRefFiltPrev; 
        }
        else if(fHVRTValue >= uwHVRTActiveValue)
        {
        	ptPosCurrRef->q = wPosCurrRefFiltPrev;
            ptNosCurrRef->q = -wNosCurrRefFiltPrev; 
        }
        else
        {
        	//deal 
        } 
        
        if((fLVRTValue > uwSoftLVRTValue)&&(fHVRTValue < uwSoftHVRTValue))//((fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))
        {
        	if(wEepromStandardValue == cBDEWStandard)
            {
            	wTemp = ((INT32S)wEepromFVRTPosCurrK * 13107 + 32)>>6;//Q11
            	
            	if(fFVRT == 1)
            	{
            		dwTemp = ((INT32S)wTemp * (16384 - uwSoftLVRTValue))>>11;//uwLVRTActiveValue
            	}
            	else if(fFVRT == 2)
            	{
            		dwTemp = ((INT32S)wTemp * (uwSoftHVRTValue - 16384))>>11;//uwHVRTActiveValue
            	}
            	else
            	{
            		dwTemp = 0;
            	}
                
                if(dwTemp > 820)
                {
                    dwTemp = 820;
                }
                
                if(fFVRT == 1)
                {            	
                    dwTemp = dwTemp - wpuIqRefbak;
                    
                    if(fImbalanceLVRT == 1)
                    {
                        if(dwTemp > 6554)
                        {
                            dwTemp = 6554;
                        }
                        else if(dwTemp < -16384)
                        {
                        	dwTemp = -16384;
                        }
                    }
                    else
                    {
                        if(dwTemp > 16384)
                        {
                            dwTemp = 16384;
                        }
                        else if(dwTemp < -16384)
                        {
                        	dwTemp = -16384;
                        }                                     
                    }                    
                }
                else if(fFVRT == 2)
                {
                    dwTemp = dwTemp + wpuIqRefbak;
                    
                    if(fImbalanceLVRT == 1)
                    {
                        if(dwTemp > 3277)
                        {
                            dwTemp = 3277;
                        }
                        else if(dwTemp < -3277)
                        {
                        	dwTemp = -3277;
                        }                        
                    }
                    else
                    {
                        if(dwTemp > 6554)
                        {
                            dwTemp = 6554;
                        }
                        else if(dwTemp < -6554)
                        {
                        	dwTemp = -6554;
                        }                                     
                    }                
                }
                                
                wPosCurrRefFiltPrev = (dwTemp * wRatedInvCurrPeakLVRT)>>14;
            }
            else if(wEepromStandardValue == cRule21Standard)
            {
                //dwTemp = 0;     
                //wPosCurrRefFiltPrev = (dwTemp * wRatedInvCurrPeakLVRT)>>14;
            }
            else
            {
            	wTemp = ((INT32S)wEepromFVRTPosCurrK * 13107 + 32)>>6;//Q11
                
            	if(fFVRT == 1)
            	{
            		dwTemp = ((INT32S)wTemp * (16384 - uwSoftLVRTValue))>>11;//uwLVRTActiveValue
            	}
            	else if(fFVRT == 2)
            	{
            		dwTemp = ((INT32S)wTemp * (uwSoftHVRTValue - 16384))>>11;//uwHVRTActiveValue
            	}
            	else
            	{
            		dwTemp = 0;
            	}
                
                if(dwTemp > 18022)
                {
                    dwTemp = 18022;
                }
                
                //dwTemp = 0;
                
                if(fFVRT == 1)
                {            	
                    dwTemp = dwTemp - wpuIqRefbak;
                    if(dwTemp > 18022)
                    {
                        dwTemp = 18022;
                    }
                    else if(dwTemp < -18022)
                    {
                    	dwTemp = -18022;
                    }
                }
                else if(fFVRT == 2)
                {
                    dwTemp = dwTemp + wpuIqRefbak;
                    if(dwTemp > 3277)
                    {
                        dwTemp = 3277;
                    }
                    else if(dwTemp < -3277)
                    {
                    	dwTemp = -3277;
                    }
                }                
                wPosCurrRefFiltPrev = (dwTemp * wRatedInvCurrPeakLVRT)>>14;
            }
        	        	
        	if(wPosCurrRefFiltPrev > wInvCurrPeakMaxLVRT)
        	{
        		wPosCurrRefFiltPrev = wInvCurrPeakMaxLVRT;
        	}
        	else if(wPosCurrRefFiltPrev < -wInvCurrPeakMaxLVRT)
        	{
        		wPosCurrRefFiltPrev = -wInvCurrPeakMaxLVRT;
        	}
        	
        	if(fImbalanceLVRT == 0)//2014-06-27 update
        	{
        		wNosCurrRefFiltPrev = 0;
        	}
                        
            if(fFVRT == 1)
            {
            	ptPosCurrRef->q = -wPosCurrRefFiltPrev;
                ptNosCurrRef->q = -wNosCurrRefFiltPrev;
            }
            else if(fFVRT == 2)
            {
            	ptPosCurrRef->q = wPosCurrRefFiltPrev;
                ptNosCurrRef->q = -wNosCurrRefFiltPrev;
            }
            else
            {
            	ptPosCurrRef->q = 0;
                ptNosCurrRef->q = 0;
            }
        } 
        ptNosCurrRef->q = 0;
    }
    else
    {
        wPosCurrRefFiltPrev = 0;
        wNosCurrRefFiltPrev = 0;
        ptNosCurrRef->q = 0;
        ptPosCurrRef->q = 0;
        fFirstUpdateEn = 1;
        fFVRT = 0;
        
        i = 0; 
        dwPosSum = (INT32S)820 * uwTwiceFreSampNum;
        dwNosSum = 0;       
        wpuPosVolData[0] = 820;
        wpuPosVolData[1] = 820;
        wpuPosVolData[2] = 820;
        wpuPosVolData[3] = 820;
        wpuPosVolData[4] = 820;
        wpuPosVolData[5] = 820;
        wpuPosVolData[6] = 820;
        wpuPosVolData[7] = 820;
        wpuPosVolData[8] = 820;
        wpuPosVolData[9] = 820;
        wpuPosVolData[10] = 820;
        wpuPosVolData[11] = 820;
        wpuPosVolData[12] = 820;
        wpuPosVolData[13] = 820;
        wpuPosVolData[14] = 820;
        wpuPosVolData[15] = 820;
        wpuPosVolData[16] = 820;
        wpuPosVolData[17] = 820;
        wpuPosVolData[18] = 820;
        wpuPosVolData[19] = 820;
        wpuPosVolData[20] = 820;
        wpuPosVolData[21] = 820;
        wpuPosVolData[22] = 820;
        wpuPosVolData[23] = 820;
        wpuPosVolData[24] = 820;
        wpuPosVolData[25] = 820;
        wpuPosVolData[26] = 820;
        wpuPosVolData[27] = 820;
        wpuPosVolData[28] = 820;
        wpuPosVolData[29] = 820;
        wpuPosVolData[30] = 820;
        wpuPosVolData[31] = 820;
        wpuPosVolData[32] = 820;
        wpuPosVolData[33] = 820;
        wpuPosVolData[34] = 820;
        wpuPosVolData[35] = 820;
        wpuPosVolData[36] = 820;
        wpuPosVolData[37] = 820;
        wpuPosVolData[38] = 820;
        wpuPosVolData[39] = 820;
        wpuPosVolData[40] = 820;
        wpuPosVolData[41] = 820;
        wpuPosVolData[42] = 820;
        wpuPosVolData[43] = 820;
        wpuPosVolData[44] = 820;
        wpuPosVolData[45] = 820;
        wpuPosVolData[46] = 820;
        wpuPosVolData[47] = 820;
        wpuPosVolData[48] = 820;
        wpuPosVolData[49] = 820;
        wpuPosVolData[50] = 820;
        wpuPosVolData[51] = 820;
        wpuPosVolData[52] = 820;
        wpuPosVolData[53] = 820;
        wpuPosVolData[54] = 820;
        wpuPosVolData[55] = 820;
        wpuPosVolData[56] = 820;
        wpuPosVolData[57] = 820;
        wpuPosVolData[58] = 820;
        wpuPosVolData[59] = 820;
        wpuPosVolData[60] = 820;
        wpuPosVolData[61] = 820;
        wpuPosVolData[62] = 820;
        wpuPosVolData[63] = 820;
        wpuPosVolData[64] = 820;
        wpuPosVolData[65] = 820;
        wpuPosVolData[66] = 820;
        wpuPosVolData[67] = 820;
        wpuPosVolData[68] = 820;
        wpuPosVolData[69] = 820;
        wpuPosVolData[70] = 820;
        wpuPosVolData[71] = 820;
        wpuPosVolData[72] = 820;
        wpuPosVolData[73] = 820;
        wpuPosVolData[74] = 820;
        wpuPosVolData[75] = 820;
        wpuPosVolData[76] = 820;
        wpuPosVolData[77] = 820;
        wpuPosVolData[78] = 820;
        wpuPosVolData[79] = 820;
        
        wpuNosVolData[0] = 0;
        wpuNosVolData[1] = 0;
        wpuNosVolData[2] = 0;
        wpuNosVolData[3] = 0;
        wpuNosVolData[4] = 0;
        wpuNosVolData[5] = 0;
        wpuNosVolData[6] = 0;
        wpuNosVolData[7] = 0;
        wpuNosVolData[8] = 0;
        wpuNosVolData[9] = 0;
        wpuNosVolData[10] = 0;
        wpuNosVolData[11] = 0;
        wpuNosVolData[12] = 0;
        wpuNosVolData[13] = 0;
        wpuNosVolData[14] = 0;
        wpuNosVolData[15] = 0;
        wpuNosVolData[16] = 0;
        wpuNosVolData[17] = 0;
        wpuNosVolData[18] = 0;
        wpuNosVolData[19] = 0;
        wpuNosVolData[20] = 0;
        wpuNosVolData[21] = 0;
        wpuNosVolData[22] = 0;
        wpuNosVolData[23] = 0;
        wpuNosVolData[24] = 0;
        wpuNosVolData[25] = 0;
        wpuNosVolData[26] = 0;
        wpuNosVolData[27] = 0;
        wpuNosVolData[28] = 0;
        wpuNosVolData[29] = 0;
        wpuNosVolData[30] = 0;
        wpuNosVolData[31] = 0;
        wpuNosVolData[32] = 0;
        wpuNosVolData[33] = 0;
        wpuNosVolData[34] = 0;
        wpuNosVolData[35] = 0;
        wpuNosVolData[36] = 0;
        wpuNosVolData[37] = 0;
        wpuNosVolData[38] = 0;
        wpuNosVolData[39] = 0;
        wpuNosVolData[40] = 0;
        wpuNosVolData[41] = 0;
        wpuNosVolData[42] = 0;
        wpuNosVolData[43] = 0;
        wpuNosVolData[44] = 0;
        wpuNosVolData[45] = 0;
        wpuNosVolData[46] = 0;
        wpuNosVolData[47] = 0;
        wpuNosVolData[48] = 0;
        wpuNosVolData[49] = 0;
        wpuNosVolData[50] = 0;
        wpuNosVolData[51] = 0;
        wpuNosVolData[52] = 0;
        wpuNosVolData[53] = 0;
        wpuNosVolData[54] = 0;
        wpuNosVolData[55] = 0;
        wpuNosVolData[56] = 0;
        wpuNosVolData[57] = 0;
        wpuNosVolData[58] = 0;
        wpuNosVolData[59] = 0;
        wpuNosVolData[60] = 0;
        wpuNosVolData[61] = 0;
        wpuNosVolData[62] = 0;
        wpuNosVolData[63] = 0;
        wpuNosVolData[64] = 0;
        wpuNosVolData[65] = 0;
        wpuNosVolData[66] = 0;
        wpuNosVolData[67] = 0;
        wpuNosVolData[68] = 0;
        wpuNosVolData[69] = 0;
        wpuNosVolData[70] = 0;
        wpuNosVolData[71] = 0;
        wpuNosVolData[72] = 0;
        wpuNosVolData[73] = 0;
        wpuNosVolData[74] = 0;
        wpuNosVolData[75] = 0;
        wpuNosVolData[76] = 0;
        wpuNosVolData[77] = 0;
        wpuNosVolData[78] = 0;
        wpuNosVolData[79] = 0;
    }
}
/************************************************************************
Function name:void sInvAndGridVolDeal(T_THREE_PHASE_ABC *ptInvPhaseAbc,T_THREE_PHASE_ABC *ptGridPhaseAbc)
Description:  converter ABC line to ABC phase
Calls:        
Called By:   sADCIsr(void)    
Parameters:  T_THREE_PHASE_ABC, T_THREE_PHASE_ABC
Return:  void      
************************************************************************/
void sInvAndGridVolDeal(T_THREE_PHASE_ABC *ptInvPhaseAbc,T_THREE_PHASE_ABC *ptGridPhaseAbc)
{
T_THREE_PHASE_ABC tInvLineVol,tInvPhaseVol;
T_THREE_PHASE_AFBTGM twInvPhaseVolAfBtGm,twGridPhaseVolAfBtGm;
T_THREE_PHASE_DQO twGridPhaseVolDqo;
static INT16S wInvVoltDAxisFiltPrev = 0;
static INT16S wGridVoltDAxisFiltPrev = 0;
static INT16S wGridVoltQAxisFiltPrev = 0;
    
    tInvLineVol.a = ptInvPhaseAbc->a - ptInvPhaseAbc->b;
    tInvLineVol.b = ptInvPhaseAbc->b - ptInvPhaseAbc->c;
    tInvLineVol.c = ptInvPhaseAbc->c - ptInvPhaseAbc->a;
    
    tInvVolDqo.o = 0;
    
    if(fRunInvAhdChkRequire == 0)
    {
    	tInvPhaseVol.a = (INT16S)(((INT32S)((INT32S)tInvLineVol.b + (INT32S)2*tInvLineVol.a)*cDivide3)>>14);
        tInvPhaseVol.b = (INT16S)(((INT32S)((INT32S)tInvLineVol.b - tInvLineVol.a)*cDivide3)>>14);
        tInvPhaseVol.c = -(tInvPhaseVol.a + tInvPhaseVol.b);                   
        twInvPhaseVolAfBtGm.Af = (INT16S)(((INT32S)(((INT32S)tInvPhaseVol.a * 2) - tInvPhaseVol.b - tInvPhaseVol.c)*cDivide3)>>14);
        twInvPhaseVolAfBtGm.Bt = (INT16S)(((INT32S)(tInvPhaseVol.b - tInvPhaseVol.c)*cDivideSqrt3)>>14);  
        
        tInvVolDqo.d = (INT16S)(((INT32S)twInvPhaseVolAfBtGm.Af *  wPLLCosTheta + (INT32S)twInvPhaseVolAfBtGm.Bt * wPLLSinTheta) >> 14);
        //tInvVolDqo.q = (INT16S)(((INT32S)twInvPhaseVolAfBtGm.Af * (-wPLLSinTheta) + (INT32S)twInvPhaseVolAfBtGm.Bt * wPLLCosTheta) >> 14);
        twGridPhaseVolAfBtGm.Af = (INT16S)(((INT32S)(((INT32S)ptGridPhaseAbc->a * 2) - ptGridPhaseAbc->b - ptGridPhaseAbc->c)*cDivide3)>>14);
        twGridPhaseVolAfBtGm.Bt = (INT16S)(((INT32S)(ptGridPhaseAbc->b - ptGridPhaseAbc->c)*cDivideSqrt3)>>14); 
        
        twGridPhaseVolDqo.d = (INT16S)(((INT32S)twGridPhaseVolAfBtGm.Af *  wPLLCosTheta   + (INT32S)twGridPhaseVolAfBtGm.Bt * wPLLSinTheta) >> 14);
        twGridPhaseVolDqo.q = (INT16S)(((INT32S)twGridPhaseVolAfBtGm.Af * (-wPLLSinTheta) + (INT32S)twGridPhaseVolAfBtGm.Bt * wPLLCosTheta) >> 14);
        tInvVolDqo.d = ((INT32S)wInvVoltDAxisFiltPrev*cPLLChkVolFiltDen + (INT32S)tInvVolDqo.d*cPLLChkVolFiltNum)>>15;
	    wInvVoltDAxisFiltPrev = tInvVolDqo.d;
	    
	    twGridPhaseVolDqo.d = ((INT32S)wGridVoltDAxisFiltPrev*cPLLChkVolFiltDen + (INT32S)twGridPhaseVolDqo.d*cPLLChkVolFiltNum)>>15;
	    wGridVoltDAxisFiltPrev = twGridPhaseVolDqo.d;
	    
	    tInvVolDqo.d = abs(tInvVolDqo.d - twGridPhaseVolDqo.d);
	    
	    tInvVolDqo.q = ((INT32S)wGridVoltQAxisFiltPrev*cPLLChkVolFiltDen + (INT32S)twGridPhaseVolDqo.q*cPLLChkVolFiltNum)>>15;
	    wGridVoltQAxisFiltPrev = tInvVolDqo.q;	
	    
	    tInvVolDqo.q = abs(tInvVolDqo.q);
	    
	    wDebugPLLDAxis = tInvVolDqo.d;
	    wDebugPLLQAxis = tInvVolDqo.q;   
    }
    else
    {
    	tInvVolDqo.d = 0;
    	tInvVolDqo.q = 0;
    	wInvVoltDAxisFiltPrev = 0;
    	wGridVoltDAxisFiltPrev = 0;
    	wGridVoltQAxisFiltPrev = 0;
    }
}
/*********************************************************************************
Function name: void sAiCurrRefCal(INT16U uwfInvCtrlFlg)
Description: get the steady frequence of the grid for AI disturbing
Calls:  suwGetMinGridFreq();suwAIGetMinGridFreq()
Called By: 20ms period task                                                           
Parameters: void
Return: void	
**********************************************************************************/
void sAiCurrRefCal(void)
{ 
INT16U uwMinFreq;
INT16U uwMaxFreq;
INT16U uwMidFreq;
INT16U uwSysFreqNow;
INT16S wDeltaFre,wDeltaFreMax,wDeltaFreMin;
INT32S dwAICurrRef;
INT16S wTemp;
static INT16U i = 0;
static INT16U uwSysFreqReal = 15360;
static INT16U uwDelay = 0;
static INT16U uwDelay1 = 0;
static INT16U uwDelay2 = 0;

    //-----------------------------SystemNoise Deal---------------------------------
    wAIGetInvDsCurrRef = ((INT32S)tInvCurrDqoRef.d * 121 + (INT32S)wAIGetInvDsCurrRef * 903)>>10;
    if(abs(wAIGetInvDsCurrRef) > 3000)
    {
    	wAISystemNoise = (INT16S)(((INT32S)wAIGetInvDsCurrRef * cAIK2)>> 9);
    }
    else
    {
    	wAISystemNoise = (INT16S)(((INT32S)3000 * cAIK2)>> 9);
    }
    wAISystemNoise = abs(wAISystemNoise);
    //------------------------------FreqNoise Deal----------------------------------
	uwMinFreq = suwGetMinGridFreq();
	uwMaxFreq = suwGetMaxGridFreq();
	uwMidFreq = suwGetMidGridFreq();
	uwSysFreqNow = udwFreqMidSum>>4;
	wDeltaFreMin = uwMinFreq - uwSysFreqNow;
	wDeltaFreMax = uwMaxFreq - uwSysFreqNow;
	udwFreqMidSum = udwFreqMidSum - uwFreqMidTemp[i] + uwMidFreq;		
	uwFreqMidTemp[i] = uwMidFreq;
	
	i++;
	if(i >= 16)
	{
		i = 0;
	}
	
	if(abs(wDeltaFreMin) >= abs(wDeltaFreMax))
	{
		wDeltaFre = wDeltaFreMin;
	}
	else
	{
		wDeltaFre = wDeltaFreMax;
	}
	
	if(wDeltaFre > 1024)
	{
		wDeltaFre = 1024;
	}
	else if(wDeltaFre < -1024)
	{
		wDeltaFre = -1024;
	}
	
	if((uwSysMode == InvRunMode)&&(fLVRTValue > 9830))//&&(fHVRTValue < uwHVRTActiveValue))
	{
		if(++uwDelay >= 25)
		{
		    uwDelay = 25;
		    if((abs(wDeltaFre) >= 8)&&(fAIStart == 0))
            {
            	uwDelay2 = 0;
            	
            	if((++uwDelay1) >= 1)
            	{
            		uwDelay1 = 0;
            		fAIStart = 1;
            	    uwSysFreqReal = uwSysFreqNow;//uwMinFreq;
            	    if(abs(wAIGetInvDsCurrRef) > 3000)
            	    {
            	        dwAIPLLFreqNoise = ((INT32S)wDeltaFre*wAIGetInvDsCurrRef*cAIK1)>>15;
            	    }
            	    else
            	    {
            	    	dwAIPLLFreqNoise = ((INT32S)wDeltaFre*3000*cAIK1)>>15;
            	    }
            	    
            	    dwAIPLLFreqNoise = labs(dwAIPLLFreqNoise);
            	    wDebugAIStart++;
            	}    	
            }
            else if(fAIStart == 1)
            {
            	uwDelay1 = 0;
            	if(abs(wAIGetInvDsCurrRef) > 3000)
            	{
            	    wTemp = ((INT32S)32*wAIGetInvDsCurrRef*cAIK1)>>15;//1s 6.25Hz
            	}
            	else
            	{
            		wTemp = ((INT32S)32*3000*cAIK1)>>15;//1s 6.25Hz
            	}
            	
            	dwAIPLLFreqNoise = dwAIPLLFreqNoise + abs(wTemp);
            	
            	wDeltaFre = uwMinFreq - uwSysFreqReal;
            	
            	if(wDeltaFre > -16)
            	{
            		if(++uwDelay2 >= 4)
            		{
            			fAIStart = 0;
            			uwDelay2 = 0;
            			wDebugAIEnd++;
            		}
            	}
            	else
            	{
            		uwDelay2 = 0;    		
            	}    	 
            }
            else
            {
            	if(uwDelay1 > 0)
            	{
            	    uwDelay1--;
            	}
            	uwDelay2 = 0;
            	dwAIPLLFreqNoise = 0;
            	uwSysFreqReal = uwMinFreq;
            }
            
            if(wEepromAIEnable == 1)
            {
            	dwAICurrRef = - dwAIPLLFreqNoise - wAISystemNoise;
            }
            else
            {
                dwAICurrRef = 0;//wAISystemNoise;
            }
            
            if (dwAICurrRef >= cAIDeltaIqLimitUp)   // Limiter of Delta Iq 
            {
            	wAIDeltaIqRef = cAIDeltaIqLimitUp;
            }
            else if (dwAICurrRef < cAIDeltaIqLimitLow)
            {
            	wAIDeltaIqRef = cAIDeltaIqLimitLow;
            }
            else
            {
            	wAIDeltaIqRef = dwAICurrRef;
            }
            wDebugAIDeltaIqRef = wAIDeltaIqRef;
        }
	}
	else
	{
		uwDelay = 0;
		uwDelay1 = 0;
        uwDelay2 = 0;
        dwAIPLLFreqNoise = 0;
        fAIStart = 0;
        wAIDeltaIqRef = 0;
	}      
}
/*** end of file *****************************************************/
	



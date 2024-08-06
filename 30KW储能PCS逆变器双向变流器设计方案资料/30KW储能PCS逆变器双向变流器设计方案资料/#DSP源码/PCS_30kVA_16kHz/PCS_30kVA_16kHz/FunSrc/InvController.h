/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef INVCTR_H
#define INVCTR_H
#include "..\FunSrc\SysDefine.h"


//---------------------------------------------------------------------------
// Function Prototypes
//
extern void sPLLParamInit(void);
extern void sInvParamInit(void);
extern void sInvController(INT16S wPOBusVolSamp,INT16S wONBusVolSamp,T_THREE_PHASE_ABC wInvCurrSamp,T_THREE_PHASE_ABC wGridPhaseVoltSamp,INT16S wBatCurr,INT16U uwfInvCtrlFlg);
extern void sDciControl(INT16U uwfInvCtrlFlg);
extern void sAiCurrRefCal(void);
extern void sInvAndGridVolDeal(T_THREE_PHASE_ABC *ptInvPhaseAbc,T_THREE_PHASE_ABC *ptGridPhaseAbc);
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern const INT16S wSinTabArray[];
extern const INT32S dwATANArray[];
extern T_THREE_DUTY_ABC tInvModuAbc;   //==for SVPWM
extern T_THREE_PHASE_ABC tInvPWMModuAbc;   //==for sPWM

extern T_THREE_PHASE_DQO tInvModuDqo;


extern T_THREE_PHASE_AFBTGM tDeadCompModuAfBtGm;

extern INT16S wPLLCosTheta;
extern INT16S wPLLNosCosTheta;
extern INT16S wPLLPNPhDifSinTheta;
extern INT16S wPLLPNPhDifCosTheta;
extern INT16S wPLLOpenCosTheta;
extern T_THREE_PHASE_ABC tGridPhaseVoltAbc;
extern T_THREE_PHASE_ABC tPLLPosSeqGridVoltABC;
extern T_THREE_PHASE_ABC tPLLNosSeqGridVoltABC;
extern T_THREE_PHASE_AFBTGM tInvModuAfBtGm,tInvCurrAfBtGmRef;
extern T_THREE_PHASE_DQO tInvCurrDqo,tInvCurrDqoRef,tGridVoltDqo,tInvCurrPowerReguRefDqo,tInvVolDqo;
extern T_THREE_PHASE_DQO tInvPosCurrSampDqo,tInvNosCurrSampDqo,tInvNosCurrDqoRef,tInvPosCurrDqoRef;
extern INT16U uwSysFreq;
extern INT16S wDCIReguR,wDCIReguS,wDCIReguT;
extern INT16S swReactiveCurrQs;
//===for AI Test===
extern INT16U uwModuSoftIncCnt,uwInvPwmBusComp;
extern INT16S wRDCIAvg;
extern INT16S wSDCIAvg;
extern INT16S wTDCIAvg;
extern INT16S wInvDsCurrLimitHigh;
extern INT16S wInvDsCurrLimitLow;
extern INT16S wInvDsCurrLimitHighBak;
extern INT16S wInvDsCurrLimitLowBak;
extern INT16S wBusSumVoltErrFiltPrev;
extern INT32S dwInvDsCurrSatErr;
extern INT32S dwBusSumVoltIngCtrl;
extern INT16S wGridPosDFF,wGridPosQFF;
extern INT16U uwInvPwmKFeedBack;
extern INT32S dwAngleIngSub;
extern INT32S dwAngleIngSubReal;
extern INT32S dwAngleErr;
extern INT32S dwNosAngleErr;
extern INT16U fLVRTStartUpdate;
extern INT16U fSoftLVRTEn;
extern INT16S wDebugPosVol,wDebugNosVol;
extern INT16S wDebugPLLDAxis;
extern INT16S wDebugPLLQAxis;
extern INT32S dwPLLPdFiltGainNum;
extern INT32S dwPLLPdFiltGainDen;
extern INT16U cNotch8KNum1;
extern INT16U cNotch8KNum2;
extern INT16U cNotch8KNum3;
extern INT16U uwTwiceFreSampNum;
extern INT16S wAIDeltaIq,wAIDeltaIqRef;
extern INT16S wInvOsK;
extern INT16S wAIGetInvDsCurrRef;
extern INT16U uwFreqMidTemp[];
extern INT32U udwFreqMidSum;
extern INT16U fAIStart;
extern INT16U uwSoftLVRTValue;
extern INT16U uwSoftHVRTValue;
extern INT16U fFVRTType;
extern INT16S wInvDsRepetRef;
extern INT16S wInvQsRepetRef;
extern INT16U fAPFCtrEnable;
extern INT16S wInvCtrBusVolRef;
extern INT16S wRefurbishDCIReguR;
extern INT16S wRefurbishDCIReguS;
extern INT16S wRefurbishDCIReguT;
extern INT16U fDCIReguRefurbish;
extern INT16U fInvCtrDeal;
extern INT32S dwDeltaDsCurr;
extern INT16U fUqUnconv;
extern INT16U fUqUnconvBak;
extern INT16S wDebugInvDsCurrRef;
extern INT16S wDebugInvDCCurrRef;
extern INT16S wDebugCPLDShutInv;
extern INT16S wDebugAIStart;
extern INT16S wDebugAIEnd;
extern INT16S wDebugAIDeltaIqRef;
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

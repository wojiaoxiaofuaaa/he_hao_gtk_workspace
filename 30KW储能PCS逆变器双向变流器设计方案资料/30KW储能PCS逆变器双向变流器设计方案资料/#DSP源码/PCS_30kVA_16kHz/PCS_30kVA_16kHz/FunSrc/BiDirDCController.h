/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef BOOSTCTR_H
#define BOOSTCTR_H

//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
#define cDCCurrMax                 4480
#define cDCCurrMin                -4480

extern INT16U fDCModeSwitch;
extern INT16U fDCModeSwitchEn;
extern INT16S wDCCtrCoeff;
extern INT16S wBatteryCurrRef;
extern INT16S wDCCtrBusVolRef;
extern INT16S wBatteryLimitCurrHigh;
extern INT16S wBatteryLimitCurrLow;
extern INT16S wBatVolLimtHigh;
extern INT16S wBatVolLimtLow;
extern INT16S wBiDCCtrModu1;
extern INT16S wBiDCCtrModu2;
extern INT16S wBiDCCtrModu3;
extern INT16S wBiDCCtrModu4;
extern INT16S wUdDivBatCoeff;
extern INT16S wBatteryChargeCurr;
extern INT16S wBatteryDisChargeCurr;
extern INT16U fDCVoltCtrDeal;
extern INT32S dwDeltaDCCurr;
extern INT16S wDCPwmCarrFrwd;
extern INT16S wDebugCPLDShutDC;
extern INT16S wDebugDeltaDCCurr;
extern INT16S wDebugDeltaDCCurrCharge;
extern INT16S wDebugDeltaDCCurrDisCharge;
//---------------------------------------------------------------------------
// Function Prototypes
//
extern void sBiDirDCCtrParamInit(void);
extern INT16S swBiDirDCVoltCtrl(INT16S wDCVoltRef,INT16S wPOVolt,INT16S wONVolt);
extern void sBiDirBatteryVoltCtr(INT16S wBatVolt);
extern INT16S swBiDirectionalDCCurrCtr1st(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt);
extern INT16S swBiDirectionalDCCurrCtr2nd(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt);
extern INT16S swBiDirectionalDCCurrCtr3rd(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt);
extern INT16S swBiDirectionalDCCurrCtr4th(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt);
extern void sBiDirDCCon1And3Controller(INT16S wBatVolt,INT16S wBatCurr1,INT16S wBatCurr3,INT16U fDCOutEn);
extern void sBiDirDCCon2And4Controller(INT16S wBatVolt,INT16S wPOVolt,INT16S wONVolt,INT16S wBatCurr2,INT16S wBatCurr4,INT16S wBatCurr,INT16U fDCOutEn);
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

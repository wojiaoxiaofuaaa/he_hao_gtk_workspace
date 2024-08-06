/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef PWM_H
#define PWM_H
#include "..\FunSrc\SysDefine.h"

//---------------------------------------------------------------------------
// Function Prototypes
//
extern void InitPWMVar(void);
extern void InitEPwm(void);

//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
extern INT16U uwInvCtrPWMPrd,uwBiDirDCCtrPWMPrd;
extern INT16U uwFanDuty;
//---------------------------------------------------------------------------
// Global symbols defined in source files

#define InvPWM_Fre         16000
#define DCPWM_Fre          16000

#define PWM_MAX_Duty       4687 

#define sEnInvPWMOut()              {EPwm1Regs.AQCSFRC.all = 0; EPwm2Regs.AQCSFRC.all = 0; EPwm3Regs.AQCSFRC.all = 0;}
#define sDisInvPWMOut()             {EPwm1Regs.AQCSFRC.all = 5; EPwm2Regs.AQCSFRC.all = 5; EPwm3Regs.AQCSFRC.all = 5;}

#define sEnBiDirDC1PWMOut()         {EPwm4Regs.AQCSFRC.bit.CSFA = 0;}
#define sDisBiDirDC1PWMOut()        {EPwm4Regs.AQCSFRC.bit.CSFA = 1;}

#define sEnBiDirDC2PWMOut()         {EPwm5Regs.AQCSFRC.bit.CSFA = 0;}
#define sDisBiDirDC2PWMOut()        {EPwm5Regs.AQCSFRC.bit.CSFA = 1;}

#define sEnBiDirDC3PWMOut()         {EPwm4Regs.AQCSFRC.bit.CSFB = 0;}
#define sDisBiDirDC3PWMOut()        {EPwm4Regs.AQCSFRC.bit.CSFB = 1;}

#define sEnBiDirDC4PWMOut()         {EPwm5Regs.AQCSFRC.bit.CSFB = 0;}
#define sDisBiDirDC4PWMOut()        {EPwm5Regs.AQCSFRC.bit.CSFB = 1;}
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

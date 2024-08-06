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
#include "..\FunSrc\SysDefine.h"

//---------------------------------------------------------------------------
// Function Prototypes
extern void sMPPTParamInit(void);
extern void sMpptStrategy(INT16S wPVVolMppt,INT16S wPVCurrMppt,INT16S wDCPowerMppt);
extern void sSetMPVolt(INT16S wVolt);
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
extern INT16S wMpptPvVoltMP;
extern INT16S fLoadDerating;
extern INT16U uwMpptVolMin;
extern INT32S dwPVVoltSum;   
extern INT32S dwPVPowerSum;   
extern INT16S wMpptPvVoltMPPrev;
extern INT32S dwMpptPvPowerMPPrev;
extern INT16S wMpptCount;
extern INT16S wMpptPVCurrOffset;
//---------------------------------------------------------------------------
// Global symbols defined in source files
#define cMpptVoltHigh     13600  //850V  //Q4   820V 
#define cMpptVoltLow      7520   //500v  //Q4    430V
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

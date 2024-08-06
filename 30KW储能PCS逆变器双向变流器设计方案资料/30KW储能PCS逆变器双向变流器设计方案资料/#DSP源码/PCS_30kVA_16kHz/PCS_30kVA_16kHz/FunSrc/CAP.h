/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef CAP_H
#define CAP_H
//#include "..\FunSrc\SysDefine.h"



typedef struct 
{	 
    INT16U       RCap;
    INT16U       SCap;
    INT16U       TCap;
}T_CAPEVENTFLAG;

//---------------------------------------------------------------------------
// Function Prototypes
//
extern void InitECap(void);
extern void InitCapVar(void);
extern void CAPIsr(void);
extern void sCapDeal(void);
extern INT16U sfPhaseSequJudge(INT16S swLineVoltSAtRScz,INT16S swLineVoltTAtRScz);
extern INT16U suwGetMinGridFreq(void);
extern INT16U suwGetMaxGridFreq();
extern INT16U suwGetMidGridFreq();
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//

extern INT16U fFreDetect;
extern INT16U fCapIntEvent;
extern T_CAPEVENTFLAG tLstPhase;
extern INT16U uwfPhaseSequencChkOk;   
extern INT32U udwRSOffsetBak;
extern INT32U udwSTOffsetBak;
extern INT32U udwTROffsetBak;  
//---------------------------------------------------------------------------
// Global symbols defined in source files
//
      
#define fRLstPhase tLstPhase.RCap   
#define fSLstPhase tLstPhase.SCap      
#define fTLstPhase tLstPhase.TCap
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

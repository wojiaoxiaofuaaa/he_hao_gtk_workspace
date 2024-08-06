/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef EXAMPLE_H
#define EXAMPLE_H


//---------------------------------------------------------------------------
// Include Standard C Language Header Files
//
#include <string.h>


//---------------------------------------------------------------------------
// Include any other Header Files
//
#include "DSP2833x_DefaultIsr.h"            // ISR definitions (for non-BIOS projects only)


//---------------------------------------------------------------------------
// Function Prototypes
//
extern void DelayUs(Uint16);
extern void SetDBGIER(Uint16);
extern void InitSysCtrl(void);
extern void InitFlash(void);
extern void InitPieCtrl(void);
extern void InitGpio(void);
extern void InitXINT3(void);
extern void CPURegister_check();


//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//




extern Uint16 secureRamFuncs_loadstart;
extern Uint16 secureRamFuncs_loadend;
extern Uint16 secureRamFuncs_runstart;

extern Uint16 AdcIsrRamFuncs_loadstart;
extern Uint16 AdcIsrRamFuncs_loadend;
extern Uint16 AdcIsrRamFuncs_runstart;

extern Uint16 sMpptStrategyRamFuncs_loadstart;
extern Uint16 sMpptStrategyRamFuncs_loadend;
extern Uint16 sMpptStrategyRamFuncs_runstart;

extern Uint16 sInvControlRamFuncs_loadstart;
extern Uint16 sInvControlRamFuncs_loadend;
extern Uint16 sInvControlRamFuncs_runstart;

extern Uint16 CAPRamFuncs_loadstart;
extern Uint16 CAPRamFuncs_loadend;
extern Uint16 CAPRamFuncs_runstart;

extern Uint16 ProtectRamFuncs_loadstart;
extern Uint16 ProtectRamFuncs_loadend;
extern Uint16 ProtectRamFuncs_runstart;

extern Uint16 eCanRamFuncs_loadstart;
extern Uint16 eCanRamFuncs_loadend;
extern Uint16 eCanRamFuncs_runstart;

extern Uint16 BiDirDCCtrFuncs_loadstart;
extern Uint16 BiDirDCCtrFuncs_loadend;
extern Uint16 BiDirDCCtrFuncs_runstart;

//---------------------------------------------------------------------------
// Global symbols defined in source files
//
extern const struct PIE_VECT_TABLE PieVectTableInit;

//---------------------------------------------------------------------------
// Macros
//
#define ADC_cal_func_ptr (void (*)(void))0x380080

//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

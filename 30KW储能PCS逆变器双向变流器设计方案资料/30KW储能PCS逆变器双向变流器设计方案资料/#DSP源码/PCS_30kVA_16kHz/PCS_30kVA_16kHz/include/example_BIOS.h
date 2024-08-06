/**********************************************************************
* File: example_BIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example BIOS project.  Include this
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
// Include DSP/BIOS Header Files
//

// EXAMPLE_FLASH or EXAMPLE_RAM are defined in the CCS project build options
#ifdef EXAMPLE_FLASH
    #include "example_BIOS_flashcfg.h"
#endif

#ifdef EXAMPLE_RAM
    #include "example_BIOS_ramcfg.h"
#endif


//---------------------------------------------------------------------------
// Include any other Header Files
//


//---------------------------------------------------------------------------
// Function Prototypes
//
extern void DelayUs(Uint16);
extern void SetDBGIER(Uint16);
extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitGpio(void);
extern void InitFlash(void);
extern void InitAdc(void);
extern void InitEv(void);
void UserInit(void);


//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern Uint16 hwi_vec_loadstart;
extern Uint16 hwi_vec_loadend;
extern Uint16 hwi_vec_runstart;
extern Uint16 secureRamFuncs_loadstart;
extern Uint16 secureRamFuncs_loadend;
extern Uint16 secureRamFuncs_runstart;
extern Uint16 trcdata_loadstart;
extern Uint16 trcdata_loadend;
extern Uint16 trcdata_runstart;


//---------------------------------------------------------------------------
// Global symbols defined in source files
//



//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

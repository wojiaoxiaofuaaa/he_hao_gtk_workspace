/**********************************************************************
* File: f28335_BIOS_ram.cmd -- Linker command file for DSP/BIOS code
* with DSP in Boot to M0 SARAM bootmode.
*
* History: 11/02/07 - original (D. Alter)
**********************************************************************/


/**************************************************************/
/* Link all user defined sections                             */
/**************************************************************/
SECTIONS
{
/*** User Defined Sections ***/
   codestart         : > BEGIN_M0,         PAGE = 0                /* Used by file CodeStartBranch.asm */
   internalMemFuncs  : > L0123SARAM,       PAGE = 0                /* Used by file Xintf.c.  Link to internal memory */
   secureRamFuncs    : > L0123SARAM,       PAGE = 0                /* Must be CSM secured RAM */
}

/******************* end of file ************************/

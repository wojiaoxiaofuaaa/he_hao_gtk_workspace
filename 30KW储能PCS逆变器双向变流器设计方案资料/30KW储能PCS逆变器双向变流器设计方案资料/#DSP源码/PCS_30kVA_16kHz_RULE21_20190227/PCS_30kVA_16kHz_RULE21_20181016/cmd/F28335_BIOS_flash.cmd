/**********************************************************************
* File: f28335_BIOS_flash.cmd -- Linker command file for DSP/BIOS code
* with DSP in Boot to Flash bootmode.
*
* History: 09/29/08 - Corrected linkage of internalMemFuncs (D. Alter)
*          12/13/07 - original (D. Alter)
**********************************************************************/


/**************************************************************/
/* Link all user defined sections                             */
/**************************************************************/
SECTIONS
{
/*** User Defined Sections ***/
   codestart         : > BEGIN_FLASH,           PAGE = 0           /* Used by file CodeStartBranch.asm */
   csm_rsvd          : > CSM_RSVD,              PAGE = 0           /* Used by file passwords.asm */
   internalMemFuncs  : > FLASH_ABCDEFGH,        PAGE = 0           /* Used by file Xintf.c.  Link to internal memory */
   passwords         : > PASSWORDS,             PAGE = 0           /* Used by file passwords.asm */
   secureRamFuncs    :   LOAD = FLASH_ABCDEFGH, PAGE = 0           /* Can be Flash */ 
                         RUN = L0123SARAM,      PAGE = 0           /* Must be CSM secured RAM */
                         LOAD_START(_secureRamFuncs_loadstart),
                         LOAD_END(_secureRamFuncs_loadend),
                         RUN_START(_secureRamFuncs_runstart)
}

/******************* end of file ************************/

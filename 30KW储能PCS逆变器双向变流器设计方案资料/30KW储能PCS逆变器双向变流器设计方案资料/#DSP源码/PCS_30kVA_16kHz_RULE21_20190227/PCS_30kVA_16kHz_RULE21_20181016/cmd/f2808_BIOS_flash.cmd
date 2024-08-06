/**********************************************************************
* File: f2808_BIOS_flash.cmd -- Linker command file for DSP/BIOS code
* with DSP in Boot to Flash bootmode.
*
* History: 02/11/04 - original (D. Alter)
**********************************************************************/


/**************************************************************/
/* Link all user defined sections                             */
/**************************************************************/
SECTIONS
{

/*** Code Security Password Locations ***/
   passwords         : > PASSWORDS,         PAGE = 0        /* Used by file passwords.asm */
   csm_rsvd          : > CSM_RSVD,          PAGE = 0        /* Used by file passwords.asm */

/*** User Defined Sections ***/
   codestart         : > BEGIN_FLASH,       PAGE = 0        /* Used by file CodeStartBranch.asm */

                         /* Section ramfuncs used by InitFlash() in SysCtrl.c */
   secureRamFuncs    :   LOAD = FLASH_BCD,  PAGE = 0        /* Can be Flash */ 
                         RUN = L0SARAM,     PAGE = 1        /* Must be CSM secured RAM */
                         LOAD_START(_secureRamFuncs_loadstart),
                         LOAD_END(_secureRamFuncs_loadend),
                         RUN_START(_secureRamFuncs_runstart)
}

/******************* end of file ************************/

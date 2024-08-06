/**********************************************************************
* File: f2808_BIOS_ram.cmd -- Linker command file for DSP/BIOS code
* with DSP in Boot to H0 SARAM bootmode.
*
* History: 02/11/05 - original (D. Alter)
**********************************************************************/


/**************************************************************/
/* Link all user defined sections                             */
/**************************************************************/
SECTIONS
{

/*** Code Security Password Locations ***/
   csm_rsvd          : > CSM_RSVD,         PAGE = 0, TYPE = DSECT  /* Not used in RAM example */
   passwords         : > PASSWORDS,        PAGE = 0, TYPE = DSECT  /* Not used in RAM example */

/*** User Defined Sections ***/
   codestart         : > BEGIN_M0,         PAGE = 0                /* Used by file CodeStartBranch.asm */

                         /* Section ramfuncs used by InitFlash() in SysCtrl.c */ 
   secureRamFuncs    :   LOAD = H0SARAM,   PAGE = 0                /* Can be ROM */
                         RUN = L0SARAM,    PAGE = 1                /* Must be CSM secured RAM */
                         LOAD_START(_secureRamFuncs_loadstart),
                         LOAD_END(_secureRamFuncs_loadend),
                         RUN_START(_secureRamFuncs_runstart)
}

/******************* end of file ************************/

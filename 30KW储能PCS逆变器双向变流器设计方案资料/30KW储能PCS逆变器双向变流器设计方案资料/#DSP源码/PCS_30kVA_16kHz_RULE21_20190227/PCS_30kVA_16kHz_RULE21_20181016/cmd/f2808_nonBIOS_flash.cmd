/**********************************************************************
* File: f2808_nonBIOS_flash.cmd -- Linker command file for non-DSP/BIOS
* code with DSP in Boot to Flash bootmode.
*
* History: 10/26/04 - original (D. Alter)
**********************************************************************/

MEMORY
{
PAGE 0:    /* Program Memory */
   BEGIN_M0    : origin = 0x000000, length = 0x000002     /* Part of M0SARAM.  Used for "Boot to M0" bootloader mode. */
   L0SARAM     : origin = 0x008000, length = 0x001000     /* 4Kw L0 SARAM */
   H0SARAM     : origin = 0x00C002, length = 0x002000     /* 8Kw H0 SARAM */
   OTP         : origin = 0x3D7800, length = 0x000400     /* 1Kw OTP */
   FLASH_ABCD  : origin = 0x3E8000, length = 0x00FF80     /* 64kW FLASH, Sectors A, B, C, and D combined */
   CSM_RSVD    : origin = 0x3F7F80, length = 0x000076     /* Part of FLASH Sector A.  Reserved when CSM is in use. */
   BEGIN_FLASH : origin = 0x3F7FF6, length = 0x000002     /* Part of FLASH Sector A.  Used for "boot to flash" bootloader mode. */
   PASSWORDS   : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASH Sector A.  CSM password locations. */
   BOOTROM     : origin = 0x3FF000, length = 0x000FC0     /* 4Kw Boot ROM */
   RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of Boot ROM */

 PAGE 1 :   /* Data Memory */
   M0SARAM     : origin = 0x000002, length = 0x0003FE     /* 1Kw M0 SARAM */
   M1SARAM     : origin = 0x000400, length = 0x000400     /* 1Kw M1 SARAM */
   L1SARAM     : origin = 0x009000, length = 0x001000     /* 4Kw L1 SARAM */
}

 
SECTIONS
{
/*** Compiler Required Sections ***/
  /* Program memory (PAGE 0) sections */
   .text             : > FLASH_ABCD,        PAGE = 0
   .cinit            : > FLASH_ABCD,        PAGE = 0
   .const            : > FLASH_ABCD,        PAGE = 0
   .econst           : > FLASH_ABCD,        PAGE = 0      
   .pinit            : > FLASH_ABCD,        PAGE = 0
   .reset            : > RESET,             PAGE = 0, TYPE = DSECT  /* We are not using the .reset section */
   .switch           : > FLASH_ABCD,        PAGE = 0

  /* Data Memory (PAGE 1) sections */
   .cio              : > L1SARAM,           PAGE = 1
   .bss              : > L1SARAM,           PAGE = 1
   .ebss             : > L1SARAM,           PAGE = 1
   .stack            : > M1SARAM,           PAGE = 1
   .sysmem           : > M0SARAM,           PAGE = 1
   .esysmem          : > L1SARAM,           PAGE = 1

/*** User Defined Sections ***/
   codestart         : > BEGIN_FLASH,       PAGE = 0                /* Used by file CodeStartBranch.asm */
   csm_rsvd          : > CSM_RSVD,          PAGE = 0                /* Used by file passwords.asm */
   passwords         : > PASSWORDS,         PAGE = 0                /* Used by file passwords.asm */
   secureRamFuncs    :   LOAD = FLASH_ABCD, PAGE = 0                /* Used by InitFlash() in SysCtrl.c */ 
                         RUN = L0SARAM,     PAGE = 0 
                         LOAD_START(_secureRamFuncs_loadstart),
                         LOAD_END(_secureRamFuncs_loadend),
                         RUN_START(_secureRamFuncs_runstart)
}

/******************* end of file ************************/

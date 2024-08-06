/**********************************************************************
* File: f28335_nonBIOS_ram.cmd -- Linker command file for non-DSP/BIOS
* code with DSP in Boot to M0 SARAM boot mode.
*
* History: 09/18/07 - original (D. Alter)
**********************************************************************/

MEMORY
{
PAGE 0:    /* Program Memory */
   BEGIN_M0        : origin = 0x000000, length = 0x000002     /* Part of M0SARAM.  Used for "Boot to M0" bootloader mode. */
   L0123SARAM      : origin = 0x008000, length = 0x004000     /* SARAM, L0 through L3 combined.  CSM secure */
   FLASH_ABCDEFGH  : origin = 0x300000, length = 0x03FF80     /* On-chip FLASH */
   CSM_RSVD        : origin = 0x33FF80, length = 0x000076     /* Part of FLASH Sector A.  Reserved when CSM is in use. */
   BEGIN_FLASH     : origin = 0x33FFF6, length = 0x000002     /* Part of FLASH Sector A.  Used for "Jump to flash" bootloader mode. */
   PASSWORDS       : origin = 0x33FFF8, length = 0x000008     /* Part of FLASH Sector A.  CSM password locations. */
   ADC_CAL         : origin = 0x380080, length = 0x000009     /* ADC_cal function in Reserved memory */
   OTP             : origin = 0x380400, length = 0x000400     /* 1Kw OTP */
   IQTABLES        : origin = 0x3FE000, length = 0x000B50     /* Part of Boot ROM */
   IQTABLES2       : origin = 0x3FEB50, length = 0x00008C     /* Part of Boot ROM */
   FPUTABLES       : origin = 0x3FEBDC, length = 0x0006A0     /* Part of Boot ROM */
   BOOTROM         : origin = 0x3FF27C, length = 0x000D44     /* 8Kw Boot ROM */
   RESET           : origin = 0x3FFFC0, length = 0x000002     /* part of Boot ROM */

 PAGE 1 :   /* Data Memory */
   M0SARAM         : origin = 0x000002, length = 0x0003FE     /* 1Kw M0 SARAM */
   M1SARAM         : origin = 0x000400, length = 0x000400     /* 1Kw M1 SARAM */
   L4SARAM         : origin = 0x00C000, length = 0x001000     /* 4Kw L4 SARAM, DMA accessible */
   L5SARAM         : origin = 0x00D000, length = 0x001000     /* 4Kw L5 SARAM, DMA accessible */
   L6SARAM         : origin = 0x00E000, length = 0x001000     /* 4Kw L6 SARAM, DMA accessible, 1 WS prog access */
   L7SARAM         : origin = 0x00F000, length = 0x001000     /* 4Kw L7 SARAM, DMA accessible 1 WS prog access */
}

 
SECTIONS
{
/*** Compiler Required Sections ***/

  /* Program memory (PAGE 0) sections */
   .text             : > L0123SARAM,            PAGE = 0
   .cinit            : > L0123SARAM,            PAGE = 0
   .const            : > L0123SARAM,            PAGE = 0
   .econst           : > L0123SARAM,            PAGE = 0      
   .pinit            : > L0123SARAM,            PAGE = 0
   .reset            : > RESET,                 PAGE = 0, TYPE = DSECT  /* We are not using the .reset section */
   .switch           : > L0123SARAM,            PAGE = 0

  /* Data Memory (PAGE 1) sections */
   .bss              : > L4SARAM,               PAGE = 1
   .ebss             : > L4SARAM,               PAGE = 1
   .cio              : > L4SARAM,               PAGE = 1
   .stack            : > M1SARAM,               PAGE = 1
   .sysmem           : > L4SARAM,               PAGE = 1
   .esysmem          : > L4SARAM,               PAGE = 1

/*** User Defined Sections ***/
   codestart         : > BEGIN_M0,              PAGE = 0                /* Used by file CodeStartBranch.asm */
   internalMemFuncs  : > L0123SARAM,            PAGE = 0                /* Used by Xintf.c.  Link to internal memory */
   secureRamFuncs    : > L0123SARAM,            PAGE = 0                /* Used by file SysCtrl.c.  Link to CSM secure RAM */ 
}

/******************* end of file ************************/

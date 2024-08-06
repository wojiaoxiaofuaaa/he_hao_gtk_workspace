/**********************************************************************
* File: f28335_nonBIOS_flash.cmd -- Linker command file for non-DSP/BIOS
* code with DSP in Boot to Flash boot mode.
*
* History: 09/18/07 - original (D. Alter)
**********************************************************************/

MEMORY
{
PAGE 0:    /* Program Memory */
   BEGIN_M0        : origin = 0x000000, length = 0x000002     /* Part of M0SARAM.  Used for "Boot to M0" bootloader mode. */
   L0123SARAM      : origin = 0x008000, length = 0x004000     /* SARAM, L0 through L3 combined.  CSM secure */
   FLASH_CDEFGH    : origin = 0x320004, length = 0x017FFC     /* On-chip FLASH */
   FLASH_SAFECRC   : origin = 0x338000, length = 0x000001
   FLASH_SAFE      : origin = 0x338001, length = 0x003FFD
   CSM_RSVD        : origin = 0x33FF80, length = 0x000076     /* Part of FLASH Sector A.  Reserved when CSM is in use. */
   BEGIN_FLASH     : origin = 0x320002, length = 0x000002     /* Part of FLASH Sector A.  Used for "Jump to flash" bootloader mode. */
   PASSWORDS       : origin = 0x33FFF8, length = 0x000008     /* Part of FLASH Sector A.  CSM password locations. */
   ADC_CAL         : origin = 0x380080, length = 0x000009     /* ADC_cal function in Reserved memory */
   OTP             : origin = 0x380400, length = 0x000400     /* 1Kw OTP */
   IQTABLES        : origin = 0x3FE000, length = 0x000B50     /* Part of Boot ROM */
   IQTABLES2       : origin = 0x3FEB50, length = 0x00008C     /* Part of Boot ROM */
   FPUTABLES       : origin = 0x3FEBDC, length = 0x0006A0     /* Part of Boot ROM */
   BOOTROM         : origin = 0x3FF27C, length = 0x000D44     /* 8Kw Boot ROM */
   RESET           : origin = 0x3FFFC0, length = 0x000002     /* part of Boot ROM */
   FLASH_HEAD      : origin = 0x320000, length = 0x000002
   FLASH_TRAIL     : origin = 0x33BFFE, length = 0x000002 

 PAGE 1 :   /* Data Memory */
   M0SARAM         : origin = 0x000006, length = 0x0003FA     /* 1Kw M0 SARAM */
   //M0SARAM         : origin = 0x000000, length = 0x000400
   M1SARAM         : origin = 0x000400, length = 0x000400     /* 1Kw M1 SARAM */
   //L3SARAM         : origin = 0x00B000, length = 0x001000     /* 4Kw L4 SARAM, DMA accessible */
   L4SARAM         : origin = 0x00C000, length = 0x001000     /* 4Kw L4 SARAM, DMA accessible */
   L56SARAM        : origin = 0x00D000, length = 0x002000     /* 4Kw L5 SARAM, DMA accessible */
   //L6SARAM         : origin = 0x00E000, length = 0x001000     /* 4Kw L6 SARAM, DMA accessible, 1 WS prog access */
   RAMSafety       : origin = 0x00F000, length = 0x001000     /* 4Kw L7 SARAM, DMA accessible 1 WS prog access */
}

 
SECTIONS
{
/*** Compiler Required Sections ***/
  /* Program memory (PAGE 0) sections */
   .text             : > FLASH_CDEFGH,          PAGE = 0
   .cinit            : > FLASH_CDEFGH,          PAGE = 0
   .const            : > FLASH_CDEFGH,          PAGE = 0
   .econst           : > FLASH_CDEFGH,          PAGE = 0      
   .pinit            : > FLASH_CDEFGH,          PAGE = 0
   .reset            : > RESET,                 PAGE = 0, TYPE = DSECT  /* We are not using the .reset section */
   .switch           : > FLASH_CDEFGH,          PAGE = 0

  /* Data Memory (PAGE 1) sections */
   .bss              : > L4SARAM,               PAGE = 1
   .ebss             : > L56SARAM,              PAGE = 1
   .cio              : > L4SARAM,               PAGE = 1
   .stack            : > M0SARAM,               PAGE = 1
   .sysmem           : > L4SARAM,               PAGE = 1
   .esysmem          : > L4SARAM,               PAGE = 1

/*** User Defined Sections ***/
   codestart         : > BEGIN_FLASH,           PAGE = 0                /* Used by file CodeStartBranch.asm */
   csm_rsvd          : > CSM_RSVD,              PAGE = 0                /* Used by file passwords.asm */
   internalMemFuncs  : > FLASH_CDEFGH,          PAGE = 0                /* Used by file Xintf.c.  Link to internal memory */
   FlashSafetyCRC    : > FLASH_SAFECRC          PAGE = 0
   FlashSafety       : > FLASH_SAFE             PAGE = 0 
   passwords         : > PASSWORDS,             PAGE = 0                /* Used by file passwords.asm */

   FlashHead         : > FLASH_HEAD,            PAGE = 0
   FlashTrail        : > FLASH_TRAIL,           PAGE = 0
   
   RamSafetyProt     : > RAMSafety,             PAGE = 1
   /* Section secureRamFuncs used by file SysCtrl.c. */
   secureRamFuncs    :   LOAD = FLASH_CDEFGH, PAGE = 0                  /* Should be Flash */ 
                         RUN = L0123SARAM,      PAGE = 0                /* Must be CSM secured RAM */
                         LOAD_START(_secureRamFuncs_loadstart),
                         LOAD_END(_secureRamFuncs_loadend),
                         RUN_START(_secureRamFuncs_runstart)

   AdcIsrRamFuncs    :   LOAD = FLASH_CDEFGH, PAGE = 0                 /* Used by InitFlash() in SysCtrl.c */ 
                         RUN = L0123SARAM,     PAGE = 0 
                         LOAD_START(_AdcIsrRamFuncs_loadstart),
                         LOAD_END(_AdcIsrRamFuncs_loadend),
                         RUN_START(_AdcIsrRamFuncs_runstart) 

  sMpptStrategyRamFuncs    :   LOAD = FLASH_CDEFGH, PAGE = 0                /* Used by InitFlash() in SysCtrl.c */ 
                         RUN = L0123SARAM,     PAGE = 0 
                         LOAD_START(_sMpptStrategyRamFuncs_loadstart),
                         LOAD_END(_sMpptStrategyRamFuncs_loadend),
                         RUN_START(_sMpptStrategyRamFuncs_runstart)
  
  sInvControlRamFuncs      :   LOAD = FLASH_CDEFGH, PAGE = 0                
                         RUN = L0123SARAM,     PAGE = 0 
                         LOAD_START(_sInvControlRamFuncs_loadstart),
                         LOAD_END(_sInvControlRamFuncs_loadend),
                         RUN_START(_sInvControlRamFuncs_runstart)

  CAPRamFuncs        :   LOAD = FLASH_CDEFGH, PAGE = 0                
                         RUN = L0123SARAM,     PAGE = 0 
                         LOAD_START(_CAPRamFuncs_loadstart),
                         LOAD_END(_CAPRamFuncs_loadend),
                         RUN_START(_CAPRamFuncs_runstart)

  ProtectRamFuncs    :   LOAD = FLASH_CDEFGH, PAGE = 0                
                         RUN = L0123SARAM,     PAGE = 0 
                         LOAD_START(_ProtectRamFuncs_loadstart),
                         LOAD_END(_ProtectRamFuncs_loadend),
                         RUN_START(_ProtectRamFuncs_runstart) 

  eCanRamFuncs       :   LOAD = FLASH_CDEFGH, PAGE = 0                
                         RUN = L0123SARAM,     PAGE = 0 
                         LOAD_START(_eCanRamFuncs_loadstart),
                         LOAD_END(_eCanRamFuncs_loadend),
                         RUN_START(_eCanRamFuncs_runstart)

  BiDirDCCtrFuncs       :   LOAD = FLASH_CDEFGH, PAGE = 0                
                         RUN = L0123SARAM,     PAGE = 0 
                         LOAD_START(_BiDirDCCtrFuncs_loadstart),
                         LOAD_END(_BiDirDCCtrFuncs_loadend),
                         RUN_START(_BiDirDCCtrFuncs_runstart)

}

/******************* end of file ************************/

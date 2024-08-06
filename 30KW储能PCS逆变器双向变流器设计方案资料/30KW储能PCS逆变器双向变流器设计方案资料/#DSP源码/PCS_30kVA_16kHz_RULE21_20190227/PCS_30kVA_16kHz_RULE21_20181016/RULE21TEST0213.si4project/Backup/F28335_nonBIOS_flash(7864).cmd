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
   
   FLASH_CDEFGH    : origin = 0x320004, length = 0x012FFC     /* On-chip FLASH */
   FLASH_RTS       : origin = 0x333000, length = 0x005000     /* On-chip FLASH */
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
   //RAMSafety       : origin = 0x00F000, length = 0x001000     /* 4Kw L7 SARAM, DMA accessible 1 WS prog access */
  // Protect Data
	RAMSafetyProtect1    : origin = 0x00F000, length = 0x000010
	RAMSafetyProtect2    : origin = 0x00F010, length = 0x000010
	RAMSafetyProtect3    : origin = 0x00F020, length = 0x000010
	RAMSafetyProtect4    : origin = 0x00F030, length = 0x000010
	RAMSafetyProtect5    : origin = 0x00F040, length = 0x00003C
	
	RAMSafetyProtect6    : origin = 0x00F0B8, length = 0x00003C
	RAMSafetyProtect7    : origin = 0x00F0F4, length = 0x00003C
	RAMSafetyProtect8    : origin = 0x00F130, length = 0x00003C
	
	RAMSafetyProtect9    : origin = 0x00F16C, length = 0x000002
	RAMSafetyProtect10    : origin = 0x00F16E, length = 0x000002
	
	RAMSafetyProtect11    : origin = 0x00F170, length = 0x000004
	RAMSafetyProtect12    : origin = 0x00F174, length = 0x000004
	RAMSafetyProtect13    : origin = 0x00F178, length = 0x000004
	RAMSafetyProtect14    : origin = 0x00F17C, length = 0x000004
	
	RAMSafetyProtect15    : origin = 0x00F180, length = 0x000002
	RAMSafetyProtect16    : origin = 0x00F182, length = 0x000002
	RAMSafetyProtect17    : origin = 0x00F184, length = 0x000002
	RAMSafetyProtect18    : origin = 0x00F186, length = 0x000002
	RAMSafetyProtect19    : origin = 0x00F188, length = 0x000002
	RAMSafetyProtect20    : origin = 0x00F18A, length = 0x000002
	RAMSafetyProtect21    : origin = 0x00F18C, length = 0x000002

	
	RAMSafetyProtect22    : origin = 0x00F18E, length = 0x000004
	RAMSafetyProtect23    : origin = 0x00F192, length = 0x000004
	
	RAMSafetyProtect24    : origin = 0x00F196, length = 0x000002
	RAMSafetyProtect25    : origin = 0x00F198, length = 0x000002
	RAMSafetyProtect26    : origin = 0x00F19A, length = 0x000002
	RAMSafetyProtect27    : origin = 0x00F19C, length = 0x000002
	RAMSafetyProtect28    : origin = 0x00F19E, length = 0x000002
	RAMSafetyProtect29    : origin = 0x00F1A0, length = 0x000002
	RAMSafetyProtect30    : origin = 0x00F1A2, length = 0x000002
	RAMSafetyProtect31    : origin = 0x00F1A4, length = 0x000002
	RAMSafetyProtect32    : origin = 0x00F1A6, length = 0x000002
	RAMSafetyProtect33    : origin = 0x00F1A8, length = 0x000002
	RAMSafetyProtect34    : origin = 0x00F1AA, length = 0x000002
	RAMSafetyProtect35    : origin = 0x00F1AE, length = 0x000002
	RAMSafetyProtect36    : origin = 0x00F1B0, length = 0x000002
	RAMSafetyProtect37    : origin = 0x00F1B2, length = 0x000002
	

	
  //  IIC
	RAMSafetyIIC0    : origin = 0x00F200, length = 0x000190
	RAMSafetyIIC1    : origin = 0x00F390, length = 0x000190
  // SYSCTR
    RAMSafetySYSCTR1 : origin = 0x00F590, length = 0x000002
    RAMSafetySYSCTR2 : origin = 0x00F592, length = 0x000002
    RAMSafetySYSCTR3 : origin = 0x00F594, length = 0x000002
    RAMSafetySYSCTR4 : origin = 0x00F596, length = 0x000002
    RAMSafetySYSCTR5 : origin = 0x00F598, length = 0x000002
    RAMSafetySYSCTR6 : origin = 0x00F59A, length = 0x000002
    RAMSafetySYSCTR7 : origin = 0x00F59C, length = 0x000002
    RAMSafetySYSCTR8 : origin = 0x00F59E, length = 0x000002
    RAMSafetySYSCTR9 : origin = 0x00F5A0, length = 0x000010
    
    RAMSafetySYSCTR10 : origin = 0x00F5B0, length = 0x000002
    RAMSafetySYSCTR11 : origin = 0x00F5B2, length = 0x000002
    RAMSafetySYSCTR12 : origin = 0x00F5B4, length = 0x000002
    RAMSafetySYSCTR13 : origin = 0x00F5B6, length = 0x000002
    RAMSafetySYSCTR14 : origin = 0x00F5B8, length = 0x000002
    RAMSafetySYSCTR15 : origin = 0x00F5BA, length = 0x000002
    RAMSafetySYSCTR16 : origin = 0x00F5BC, length = 0x000002
    
    RAMSafetySYSCTR17 : origin = 0x00F5C0, length = 0x000004
    RAMSafetySYSCTR18 : origin = 0x00F5C4, length = 0x000004
    RAMSafetySYSCTR19 : origin = 0x00F5C8, length = 0x000004
    
    RAMSafetySYSCTR20 : origin = 0x00F5CC, length = 0x000002
    RAMSafetySYSCTR21 : origin = 0x00F5CE, length = 0x000002
    RAMSafetySYSCTR22 : origin = 0x00F5D0, length = 0x000002
    RAMSafetySYSCTR23 : origin = 0x00F5D2, length = 0x000002
  // AD
	RAMSafetyAD1 : origin = 0x00F5E0, length = 0x000002
	RAMSafetyAD2 : origin = 0x00F5E2, length = 0x000002
	RAMSafetyAD3 : origin = 0x00F5E4, length = 0x000002
	RAMSafetyAD4 : origin = 0x00F5E6, length = 0x000002
	RAMSafetyAD5 : origin = 0x00F5E8, length = 0x000002
	RAMSafetyAD6 : origin = 0x00F5EA, length = 0x000002
	RAMSafetyAD7 : origin = 0x00F5EC, length = 0x000002
	RAMSafetyAD8 : origin = 0x00F5F2, length = 0x000006
	RAMSafetyAD9 : origin = 0x00F5F8, length = 0x000006
	RAMSafetyAD10 : origin = 0x00F5FE, length = 0x000006
	RAMSafetyAD11 : origin = 0x00F604, length = 0x000002
	RAMSafetyAD12 : origin = 0x00F608, length = 0x000002
  // BiDir
    RAMSafetyBiDir : origin = 0x00F610, length = 0x000002
  // InvCon
    RAMSafetyInvCon1  : origin = 0x00F640, length = 0x000002
    RAMSafetyInvCon2  : origin = 0x00F642, length = 0x000002
    RAMSafetyInvCon3  : origin = 0x00F644, length = 0x000002
    RAMSafetyInvCon4  : origin = 0x00F646, length = 0x000002
  // TFLAG
    RAMSafetytFlag0       : origin = 0x00F800, length = 0x000100
    RAMSafetytFlag1       : origin = 0x00F900, length = 0x000100
 // ELSE
    RAMSafety       : origin = 0x00FA00, length = 0x000800
}

 
SECTIONS
{
/*** Compiler Required Sections ***/
  /* Program memory (PAGE 0) sections */ 
  // .text		     : {rts2800_ml.lib<.text>} >>FLASH_RTS PAGE = 0
   
  
   .text             : > FLASH_CDEFGH,          PAGE = 0
   .cinit            : > FLASH_CDEFGH,          PAGE = 0
   .const            : > FLASH_CDEFGH,          PAGE = 0
   .econst           : > FLASH_CDEFGH,          PAGE = 0      
   .pinit            : > FLASH_CDEFGH,          PAGE = 0
   .reset            : > RESET,                 PAGE = 0, TYPE = DSECT  /* We are not using the .reset section */
   .switch           : > FLASH_CDEFGH,          PAGE = 0

  // .rtsmath { -l rts2800_ml.lib (.text)}  > L0123SARAM PAGE = 0
   IQmathTables     : > IQTABLES,    PAGE = 0, type = NOLOAD
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

   //rts2800_ml.lib    : > FLASH_FPU,             PAGE = 0

   
   //RamSafetyProt     : > RAMSafety,             PAGE = 1
   //protect data
   RAMSafetyProtectProt1     : > RAMSafetyProtect1,             PAGE = 1
   RAMSafetyProtectProt2     : > RAMSafetyProtect2,             PAGE = 1
   RAMSafetyProtectProt3     : > RAMSafetyProtect3,             PAGE = 1
   RAMSafetyProtectProt4     : > RAMSafetyProtect4,             PAGE = 1
   RAMSafetyProtectProt5     : > RAMSafetyProtect5,             PAGE = 1
   RAMSafetyProtectProt6     : > RAMSafetyProtect6,             PAGE = 1
   RAMSafetyProtectProt7     : > RAMSafetyProtect7,             PAGE = 1
   RAMSafetyProtectProt8     : > RAMSafetyProtect8,             PAGE = 1
   RAMSafetyProtectProt9     : > RAMSafetyProtect9,             PAGE = 1
   RAMSafetyProtectProt10     : > RAMSafetyProtect10,             PAGE = 1
   RAMSafetyProtectProt11     : > RAMSafetyProtect11,             PAGE = 1
   RAMSafetyProtectProt12     : > RAMSafetyProtect12,             PAGE = 1
   RAMSafetyProtectProt13     : > RAMSafetyProtect13,             PAGE = 1
   RAMSafetyProtectProt14     : > RAMSafetyProtect14,             PAGE = 1
   RAMSafetyProtectProt15     : > RAMSafetyProtect15,             PAGE = 1
   RAMSafetyProtectProt16     : > RAMSafetyProtect16,             PAGE = 1
   RAMSafetyProtectProt17     : > RAMSafetyProtect17,             PAGE = 1
   RAMSafetyProtectProt18     : > RAMSafetyProtect18,             PAGE = 1
   RAMSafetyProtectProt19     : > RAMSafetyProtect19,             PAGE = 1
   RAMSafetyProtectProt20     : > RAMSafetyProtect20,             PAGE = 1
   RAMSafetyProtectProt21     : > RAMSafetyProtect21,             PAGE = 1
   RAMSafetyProtectProt22     : > RAMSafetyProtect22,             PAGE = 1
   RAMSafetyProtectProt23     : > RAMSafetyProtect23,             PAGE = 1
   RAMSafetyProtectProt24     : > RAMSafetyProtect24,             PAGE = 1
   RAMSafetyProtectProt25     : > RAMSafetyProtect25,             PAGE = 1
   RAMSafetyProtectProt26     : > RAMSafetyProtect26,             PAGE = 1
   RAMSafetyProtectProt27     : > RAMSafetyProtect27,             PAGE = 1
   RAMSafetyProtectProt28     : > RAMSafetyProtect28,             PAGE = 1
   RAMSafetyProtectProt29     : > RAMSafetyProtect29,             PAGE = 1
   RAMSafetyProtectProt30     : > RAMSafetyProtect30,             PAGE = 1
   RAMSafetyProtectProt31     : > RAMSafetyProtect31,             PAGE = 1
   RAMSafetyProtectProt32     : > RAMSafetyProtect32,             PAGE = 1
   RAMSafetyProtectProt33     : > RAMSafetyProtect33,             PAGE = 1
   RAMSafetyProtectProt34     : > RAMSafetyProtect34,             PAGE = 1
   RAMSafetyProtectProt35     : > RAMSafetyProtect35,             PAGE = 1
   RAMSafetyProtectProt36     : > RAMSafetyProtect36,             PAGE = 1
   RAMSafetyProtectProt37     : > RAMSafetyProtect37,             PAGE = 1
  
  //  IIC
   RAMSafetyIICProt0     : > RAMSafetyIIC0,             PAGE = 1
   RAMSafetyIICProt1     : > RAMSafetyIIC1,             PAGE = 1
  // SYSCTR
   RAMSafetySYSCTRProt1     : > RAMSafetySYSCTR1,             PAGE = 1
   RAMSafetySYSCTRProt2     : > RAMSafetySYSCTR2,             PAGE = 1
   RAMSafetySYSCTRProt3     : > RAMSafetySYSCTR3,             PAGE = 1
   RAMSafetySYSCTRProt4     : > RAMSafetySYSCTR4,             PAGE = 1
   RAMSafetySYSCTRProt5     : > RAMSafetySYSCTR5,             PAGE = 1
   RAMSafetySYSCTRProt6     : > RAMSafetySYSCTR6,             PAGE = 1
   RAMSafetySYSCTRProt7     : > RAMSafetySYSCTR7,             PAGE = 1
   RAMSafetySYSCTRProt8     : > RAMSafetySYSCTR8,             PAGE = 1
   RAMSafetySYSCTRProt9     : > RAMSafetySYSCTR9,             PAGE = 1

   RAMSafetySYSCTRProt10     : > RAMSafetySYSCTR10,             PAGE = 1
   RAMSafetySYSCTRProt11     : > RAMSafetySYSCTR11,             PAGE = 1
   RAMSafetySYSCTRProt12     : > RAMSafetySYSCTR12,             PAGE = 1
   RAMSafetySYSCTRProt13     : > RAMSafetySYSCTR13,             PAGE = 1
   RAMSafetySYSCTRProt14     : > RAMSafetySYSCTR14,             PAGE = 1
   RAMSafetySYSCTRProt15     : > RAMSafetySYSCTR15,             PAGE = 1
   RAMSafetySYSCTRProt16     : > RAMSafetySYSCTR16,             PAGE = 1

   RAMSafetySYSCTRProt17     : > RAMSafetySYSCTR17,             PAGE = 1
   RAMSafetySYSCTRProt18     : > RAMSafetySYSCTR18,             PAGE = 1
   RAMSafetySYSCTRProt19     : > RAMSafetySYSCTR19,             PAGE = 1
   RAMSafetySYSCTRProt20     : > RAMSafetySYSCTR20,             PAGE = 1
   RAMSafetySYSCTRProt21     : > RAMSafetySYSCTR21,             PAGE = 1
   RAMSafetySYSCTRProt22     : > RAMSafetySYSCTR22,             PAGE = 1
   RAMSafetySYSCTRProt23     : > RAMSafetySYSCTR22,             PAGE = 1
  //AD
    RAMSafetyADProt1     : > RAMSafetyAD1,             PAGE = 1
    RAMSafetyADProt2     : > RAMSafetyAD2,             PAGE = 1
    RAMSafetyADProt3     : > RAMSafetyAD3,             PAGE = 1
    RAMSafetyADProt4     : > RAMSafetyAD4,             PAGE = 1
    RAMSafetyADProt5     : > RAMSafetyAD5,             PAGE = 1
    RAMSafetyADProt6     : > RAMSafetyAD6,             PAGE = 1
    RAMSafetyADProt7     : > RAMSafetyAD7,             PAGE = 1
    RAMSafetyADProt8     : > RAMSafetyAD8,             PAGE = 1
    RAMSafetyADProt9     : > RAMSafetyAD9,             PAGE = 1
    RAMSafetyADProt10     : > RAMSafetyAD10,             PAGE = 1
    RAMSafetyADProt11     : > RAMSafetyAD11,             PAGE = 1
    RAMSafetyADProt12     : > RAMSafetyAD12,             PAGE = 1
  //BiDir
    RAMSafetyBiDirProt    : > RAMSafetyBiDir,             PAGE = 1
  //InvCon
    RAMSafetyInvConProt1    : > RAMSafetyInvCon1,             PAGE = 1
    RAMSafetyInvConProt2    : > RAMSafetyInvCon2,             PAGE = 1
    RAMSafetyInvConProt3    : > RAMSafetyInvCon3,             PAGE = 1
    RAMSafetyInvConProt4    : > RAMSafetyInvCon4,             PAGE = 1
  // TFLAG
    RAMSafetytFlagProt0    : > RAMSafetytFlag0,             PAGE = 1
    RAMSafetytFlagProt1    : > RAMSafetytFlag1,             PAGE = 1
    
  // ELSE
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

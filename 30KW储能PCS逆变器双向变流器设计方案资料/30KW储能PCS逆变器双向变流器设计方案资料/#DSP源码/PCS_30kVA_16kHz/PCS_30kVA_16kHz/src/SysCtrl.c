/**********************************************************************
* File: SysCtrl.c
* Devices: TMS320F2808, TMS320F2806, TMS320F2801
* Author: David M. Alter, Texas Instruments Inc.
* History:
*   02/11/05 - original (D. Alter)
**********************************************************************/

#include "..\DSP2833x_headers\include\DSP2833x_Device.h"
// EXAMPLE_BIOS or EXAMPLE_NONBIOS are defined in the CCS project build options
#include "..\include\example_nonBIOS.h"

//---------------------------------------------------------------------------
// Example: KickDog: 
//---------------------------------------------------------------------------
// This function resets the watchdog timer.
// Enable this function for using KickDog in the application 

void KickDog(void)
{
    EALLOW;
    SysCtrlRegs.WDKEY = 0x0055;
    SysCtrlRegs.WDKEY = 0x00AA;
    EDIS;
}

//---------------------------------------------------------------------------
// Example: DisableDog: 
//---------------------------------------------------------------------------
// This function disables the watchdog timer.

void DisableDog(void)
{
    EALLOW;
    SysCtrlRegs.WDCR= 0x0068;
    EDIS;
}

/**********************************************************************
* Function: InitSysCtrl()
*
* Description: Initializes the F280x CPU.
**********************************************************************/
void InitSysCtrl(void)
{
volatile Uint16 i;						// General purpose Uint16
volatile int16 dummy;					// General purpose volatile int16

// Disable the watchdog        
	DisableDog();

	asm(" EALLOW");						// Enable EALLOW protected register access

/*** Memory Protection Configuration ***/
	DevEmuRegs.PROTSTART = 0x0100;		// Write default value to protection start register
	DevEmuRegs.PROTRANGE = 0x00FF;		// Write default value to protection range register

/*** Unlock the Code Security Module if CSM not in use ***/
/* Unlocking the CSM will allow code running from non-secure memory
   to access code and data in secure memory.  One would only want to
   unsecure the CSM if code security were not desired, and therefore
   the CSM is not in use (otherwise, unlocking the CSM will compromise
   the security of user code).  If the CSM is not in use, the best
   thing to do is leave the password locations programmed to 0xFFFF,
   which is the flash ERASED state.  When all passwords are 0xFFFF,
   all that is required to unlock the CSM are dummy reads of the
   PWL locations.
*/
	dummy = CsmPwl.PSWD0;				// Dummy read of PWL locations
	dummy = CsmPwl.PSWD1;				// Dummy read of PWL locations
	dummy = CsmPwl.PSWD2;				// Dummy read of PWL locations
	dummy = CsmPwl.PSWD3;				// Dummy read of PWL locations
	dummy = CsmPwl.PSWD4;				// Dummy read of PWL locations
	dummy = CsmPwl.PSWD5;				// Dummy read of PWL locations
	dummy = CsmPwl.PSWD6;				// Dummy read of PWL locations
	dummy = CsmPwl.PSWD7;				// Dummy read of PWL locations

/*** Disable the Watchdog Timer ***/
	SysCtrlRegs.WDCR = 0x00E8;
/*
 bit 15-8      0's:    reserved
 bit 7         1:      WDFLAG, write 1 to clear
 bit 6         1:      WDDIS, 1=disable WD
 bit 5-3       101:    WDCHK, WD check bits, always write as 101b
 bit 2-0       000:    WDPS, WD prescale bits, 000: WDCLK=OSCCLK/512/1
*/

/*** System and Control Register ***/
	SysCtrlRegs.SCSR = 0x0000;
/*
 bit 15-3      0's:    reserved
 bit 2         0:      WDINTS, WD interrupt status bit (read-only)
 bit 1         0:      WDENINT, 0=WD causes reset, 1=WD causes WDINT
 bit 0         0:      WDOVERRIDE, write 1 to disable disabling of the WD (clear-only)
*/

/*** Configure the PLL ***/

// Note: The DSP/BIOS configuration tool can also be used to intialize the PLL
// instead of doing the initialization here.

	// Make sure the PLL is not running in limp mode
	if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 1)
	{													// PLL is not running in limp mode
		SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;				// Turn off missing clock detect before changing PLLCR
		SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;				// DIVSEL must be 0 or 1  (/4 CLKIN mode) before changing PLLCR
		SysCtrlRegs.PLLCR.bit.DIV = 0x000A;				// PLLx10/4 (because DIVSEL is /4)
   
		// Wait for PLL to lock (Optional).
		// During this time the CPU will run at OSCCLK/2 until the PLL is stable.
		// Once the PLL is stable the CPU will automatically switch to the new PLL value.
		// Code is not required to sit and wait for the PLL to lock.  However, 
		// if the code does anything that is timing critical (e.g. something that
		// relies on the CPU clock frequency to be at speed), then it is best to wait
		// until PLL lock is complete.  The watchdog should be disabled before this loop
		// (e.g., as was done above), or fed within the loop.
		while(SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1)		// Wait for PLLLOCKS bit to set
		{
			SysCtrlRegs.WDKEY = 0x0055;					// Service the watchdog while waiting
			SysCtrlRegs.WDKEY = 0x00AA;					//   in case the user enabled it.
		}

		SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;				// Enable missing clock detect circuitry
		DelayUs(20/2);									// Wait 20 us (just an example).  Remember we're running
														// at half-speed here, so divide function argument by 2.
		SysCtrlRegs.PLLSTS.bit.DIVSEL = 0x2;			// Change to /2 mode
	}
	else
	{													// PLL is running in limp mode
	// User should replace the below with a call to an appropriate function,
	// for example shutdown the system (since something is very wrong!).
		asm(" ESTOP0");
	}

/*** Configure the clocks ***/
	SysCtrlRegs.HISPCP.all = 0x0000;		// Hi-speed periph clock prescaler, HSPCLK=SYSCLKOUT/1
#if (!SCI_SCOPE) 
	SysCtrlRegs.LOSPCP.all = 0x0002;		// Lo-speed periph clock prescaler, LOSPCLK=SYSCLKOUT/4
#endif
#if SCI_SCOPE
    SysCtrlRegs.LOSPCP.all = 0x0000;        // Lo-speed periph clock prescaler, LOSPCLK=SYSCLKOUT
#endif

    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;	// GPIO input module is clocked
    SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 0;		// XINTF module is clocked
    SysCtrlRegs.PCLKCR3.bit.DMAENCLK = 0;		// SYSCLKOUT to DMA enabled
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 0;	// SYSCLKOUT to CPU Timer2
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1;	// SYSCLKOUT to CPU Timer1 enabled
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1;	// SYSCLKOUT to CPU Timer0 enabled 
  
    SysCtrlRegs.PCLKCR1.bit.EQEP2ENCLK = 0;	// SYSCLKOUT to eQEP2 enabled
    SysCtrlRegs.PCLKCR1.bit.EQEP1ENCLK = 0;	// SYSCLKOUT to eQEP1 enabled
    SysCtrlRegs.PCLKCR1.bit.ECAP6ENCLK = 0;		// SYSCLKOUT to eCAP6 enabled
    SysCtrlRegs.PCLKCR1.bit.ECAP5ENCLK = 0;		// SYSCLKOUT to eCAP5 enabled
    SysCtrlRegs.PCLKCR1.bit.ECAP4ENCLK = 0;	// SYSCLKOUT to eCAP4 enabled
	SysCtrlRegs.PCLKCR1.bit.ECAP3ENCLK = 1;	// SYSCLKOUT to eCAP3 enabled
	SysCtrlRegs.PCLKCR1.bit.ECAP2ENCLK = 1;	// SYSCLKOUT to eCAP2 enabled
	SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 1;	// SYSCLKOUT to eCAP1 enabled
	SysCtrlRegs.PCLKCR1.bit.EPWM6ENCLK = 1;	// SYSCLKOUT to ePWM6 enabled
	SysCtrlRegs.PCLKCR1.bit.EPWM5ENCLK = 1;	// SYSCLKOUT to ePWM5 enabled
	SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;	// SYSCLKOUT to ePWM4 enabled
	SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;	// SYSCLKOUT to ePWM3 enabled
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;	// SYSCLKOUT to ePWM2 enabled
	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;	// SYSCLKOUT to ePWM1 enabled

	SysCtrlRegs.PCLKCR0.bit.ECANBENCLK = 1;	// SYSCLKOUT to eCAN-B enabled
	SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 1;	// SYSCLKOUT to eCAN-A enabled
	SysCtrlRegs.PCLKCR0.bit.MCBSPAENCLK = 0;	// LSPCLK to McBSP-A enabled	
	SysCtrlRegs.PCLKCR0.bit.MCBSPBENCLK = 0;	// LSPCLK to McBSP-B enabled
	SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;	// LSPCLK to SCI-B enabled
	SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;	// LSPCLK to SCI-A enabled
	SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 0;		// LSPCLK to SPI-A enabled
	SysCtrlRegs.PCLKCR0.bit.SCICENCLK = 0;		// LSPCLK to SCI-C enabled
	SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 1;		// LSPCLK to I2C-A enabled	
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;	// HSPCLK to ADC enabled

// The PCLKCR0.TBCLKSYNC bit is handled seperately in main() since
// it affects synchronization of the ePWM counters.

/*** Configure the low-power modes ***/
	SysCtrlRegs.LPMCR0.all = 0x00FC;		// LPMCR0 set to default value

//--- Finish up
	asm(" EDIS");						// Disable EALLOW protected register access

} //end InitSysCtrl()


/**********************************************************************
* Function: InitFlash()
* Description: Initializes the F280x flash timing registers.
* Notes:
*  1) This function MUST be executed out of RAM.  Executing it out of
*     OTP/FLASH will produce unpredictable results.
*  2) The flash registers are code security module protected.  Therefore,
*     you must either run this function from L0/L1 RAM, or you must
*     first unlock the CSM.  Note that unlocking the CSM as part of
*     the program flow can compromise the code security.
* 3) The latest datasheet for the particular device of interest should
*    be consulted to confirm the flash timing specifications.
**********************************************************************/
#pragma CODE_SECTION(InitFlash, "secureRamFuncs")
void InitFlash(void)
{
	asm(" EALLOW");									// Enable EALLOW protected register access
	FlashRegs.FPWR.bit.PWR = 3;						// Pump and bank set to active mode
	FlashRegs.FSTATUS.bit.V3STAT = 1;				// Clear the 3VSTAT bit
	FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;	// Sleep to standby transition cycles
	FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;	// Standby to active transition cycles
	FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;			// Random access waitstates
	FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5;			// Paged access waitstates
	FlashRegs.FOTPWAIT.bit.OTPWAIT = 8;				// OTP waitstates
	FlashRegs.FOPT.bit.ENPIPE = 1;					// Enable the flash pipeline
	asm(" EDIS");									// Disable EALLOW protected register access

/*** Force a complete pipeline flush to ensure that the write to the last register
     configured occurs before returning.  Safest thing is to wait 8 full cycles. ***/

    asm(" RPT #6 || NOP");

} //end of InitFlash()




/*** end of file *****************************************************/

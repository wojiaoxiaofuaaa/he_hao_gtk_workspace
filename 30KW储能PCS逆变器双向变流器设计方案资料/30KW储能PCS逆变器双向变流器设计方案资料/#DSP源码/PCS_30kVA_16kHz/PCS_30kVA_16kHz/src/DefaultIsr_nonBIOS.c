/**********************************************************************
* File: DefaultIsr_nonBIOS.c
* Devices: TMS320F2808, TMS320F2806, TMS320F2801
* Author: David M. Alter, Texas Instruments Inc.
* History:
*   02/11/05 - original (D. Alter)
**********************************************************************/

#include "..\DSP280x_headers\include\DSP280x_Device.h"

#include "..\include\example_nonBIOS.h"

#include "..\FunSrc\SCI.h"
#include "..\FunSrc\AD.h"
#include "..\FunSrc\TIMER.h"
#include "..\FunSrc\SPI.h"



/*********************************************************************/
interrupt void INT13_ISR(void)				// 0x000D1A  INT13 - XINT13 (or CPU Timer1, reserved for TI)
{TimerIsr();
// Next two lines for debug only - remove after inserting your ISR
	//asm (" ESTOP0");						// Emulator Halt instruction
//	while(1);
}

/*********************************************************************/
interrupt void INT14_ISR(void)				// 0x000D1C  INT14 - CPU Timer2, reserved for TI
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void DATALOG_ISR(void)			// 0x000D1E  DATALOG - CPU data logging interrupt
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void RTOSINT_ISR(void)			// 0x000D20  RTOSINT - CPU RTOS interrupt
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EMUINT_ISR(void) 			// 0x000D22  EMUINT - CPU emulation interrupt
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void NMI_ISR(void)				// 0x000D24  NMI - XNMI interrupt
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ILLEGAL_ISR(void)			// 0x000D26  ILLEGAL - illegal operation trap
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER1_ISR(void)				// 0x000D28  USER1 - software interrupt #1
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER2_ISR(void)				// 0x000D2A  USER2 - software interrupt #2
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER3_ISR(void)				// 0x000D2C  USER3 - software interrupt #3
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER4_ISR(void)				// 0x000D2E  USER4 - software interrupt #4
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER5_ISR(void)				// 0x000D30  USER5 - software interrupt #5
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER6_ISR(void)				// 0x000D32  USER6 - software interrupt #6
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER7_ISR(void)				// 0x000D34  USER7 - software interrupt #7
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER8_ISR(void)				// 0x000D36  USER8 - software interrupt #8
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER9_ISR(void)				// 0x000D38  USER9 - software interrupt #9
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER10_ISR(void)				// 0x000D3A  USER10 - software interrupt #10
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER11_ISR(void)				// 0x000D3C  USER11 - software interrupt #11
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void USER12_ISR(void)				// 0x000D3E  USER12 - software interrupt #12
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SEQ1INT_ISR(void)			// 0x000D40  SEQ1INT (ADC SEQ1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SEQ2INT_ISR(void)			// 0x000D42  SEQ2INT (ADC SEQ2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000D44 reserved

/*********************************************************************/
interrupt void XINT1_ISR(void)				// 0x000D46  XINT1
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}     

/*********************************************************************/
interrupt void XINT2_ISR(void)				// 0x000D48  XINT2
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}


/*********************************************************************/
interrupt void ADCINT_ISR(void)				// 0x000D4A  ADCINT (ADC)
{
    ADCIsr();
	 
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;		// Must acknowledge the PIE group


}

/*********************************************************************/
interrupt void TINT0_ISR(void)				// 0x000D4C  TINT0 (CPU TIMER 0)
{
	
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void WAKEINT_ISR(void)			// 0x000D4E  WAKEINT (LPM/WD)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group
  
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM1_TZINT_ISR(void)		// 0x000D50  EPWM1_TZINT (EPWM1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM2_TZINT_ISR(void)		// 0x000D52  EPWM2_TZINT (EPWM2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM3_TZINT_ISR(void)		// 0x000D54  EPWM3_TZINT (EPWM3)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group
  
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM4_TZINT_ISR(void)		// 0x000D56  EPWM4_TZINT (EPWM4)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group
  
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM5_TZINT_ISR(void)		// 0x000D58  EPWM5_TZINT (EPWM5)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM6_TZINT_ISR(void)		// 0x000D5A  EPWM6_TZINT (EPWM6)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000D5C reserved
											// 0x000D5E reserved
     
/*********************************************************************/
interrupt void EPWM1_INT_ISR(void)			// 0x000D60  EPWM1_INT (EPWM1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

	EPwm1Regs.ETCLR.bit.INT = 1;

	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM2_INT_ISR(void)			// 0x000D62  EPWM2_INT (EPWM2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM3_INT_ISR(void)			// 0x000D64  EPWM3_INT (EPWM3)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM4_INT_ISR(void)			// 0x000D66  EPWM4_INT (EPWM4)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM5_INT_ISR(void)			// 0x000D68  EPWM5_INT (EPWM5)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EPWM6_INT_ISR(void)			// 0x000D6A  EPWM6_INT (EPWM6)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000D6C reserved
											// 0x000D6E reserved

/*********************************************************************/
interrupt void ECAP1_INT_ISR(void)			// 0x000D70  ECAP1_INT (ECAP1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group
	
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ECAP2_INT_ISR(void)			// 0x000D72  ECAP2_INT (ECAP2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ECAP3_INT_ISR(void)			// 0x000D74  ECAP3_INT (ECAP3)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ECAP4_INT_ISR(void)			// 0x000D76  ECAP4_INT (ECAP4)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000D78 reserved
											// 0x000D7A reserved
											// 0x000D7C reserved
											// 0x000D7E reserved

/*********************************************************************/
interrupt void EQEP1_INT_ISR(void)			// 0x000D80  EQEP1_INT (EQEP1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;	// Must acknowledge the PIE group
 
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void EQEP2_INT_ISR(void)			// 0x000D82  EQEP2_INT (EQEP2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000D84 reserved
											// 0x000D86 reserved
											// 0x000D88 reserved
											// 0x000D8A reserved
											// 0x000D8C reserved
											// 0x000D8E reserved

/*********************************************************************/
interrupt void SPIRXINTA_ISR(void)			// 0x000D90  SPIRXINTA (SPI-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group
    SPI_receiveIsr();
// Next two lines for debug only - remove after inserting your ISR
//	asm (" ESTOP0");						// Emulator Halt instruction
//	while(1);
}

/*********************************************************************/
interrupt void SPITXINTA_ISR(void)			// 0x000D92  SPITXINTA (SPI-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SPIRXINTB_ISR(void)			// 0x000D94  SPIRXINTB (SPI-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SPITXINTB_ISR(void)			// 0x000D96  SPITXINTB (SPI-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SPIRXINTC_ISR(void)			// 0x000D98  SPIRXINTC (SPI-C)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SPITXINTC_ISR(void)			// 0x000D9A  SPITXINTC (SPI-C)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SPIRXINTD_ISR(void)			// 0x000D9C  SPIRXINTD (SPI-D)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SPITXINTD_ISR(void)			// 0x000D9E  SPITXINTD (SPI-D)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000DA0 reserved
											// 0x000DA2 reserved
											// 0x000DA4 reserved
											// 0x000DA6 reserved
											// 0x000DA8 reserved
											// 0x000DAA reserved
											// 0x000DAC reserved
											// 0x000DAE reserved

/*********************************************************************/
interrupt void I2CINT1A_ISR(void)			// 0x000DB0  I2CINT1A (I2C-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void I2CINT2A_ISR(void)			// 0x000DB2  I2CINT2A (I2C-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000DB4 reserved
											// 0x000DB6 reserved
											// 0x000DB8 reserved
											// 0x000DBA reserved
											// 0x000DBC reserved
											// 0x000DBE reserved

/*********************************************************************/
interrupt void SCIRXINTA_ISR(void)			// 0x000DC0  SCIRXINTA (SCI-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

	SciaRxIsr();
// Next two lines for debug only - remove after inserting your ISR
	//asm (" ESTOP0");						// Emulator Halt instruction
//	while(1);
}

/*********************************************************************/
interrupt void SCITXINTA_ISR(void)			// 0x000DC2  SCITXINTA (SCI-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SCIRXINTB_ISR(void)			// 0x000DC4  SCIRXINTB (SCI-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void SCITXINTB_ISR(void)			// 0x000DC6  SCITXINTB (SCI-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ECAN0INTA_ISR(void)			// 0x000DC8  ECAN0_INTA (ECAN-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ECAN1INTA_ISR(void)			// 0x000DCA  ECAN1_INTA (ECAN-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ECAN0INTB_ISR(void)			// 0x000DCC  ECAN0_INTB (ECAN-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void ECAN1INTB_ISR(void)			// 0x000DCE  ECAN1_INTB (ECAN-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
											// 0x000DD0 reserved
											// 0x000DD2 reserved
											// 0x000DD4 reserved
											// 0x000DD6 reserved
											// 0x000DD8 reserved
											// 0x000DDA reserved
											// 0x000DDC reserved
											// 0x000DDE reserved

/*********************************************************************/
											// 0x000DE0 reserved
											// 0x000DE2 reserved
											// 0x000DE4 reserved
											// 0x000DE6 reserved
											// 0x000DE8 reserved
											// 0x000DEA reserved
											// 0x000DEC reserved
											// 0x000DEE reserved

/*********************************************************************/
											// 0x000DF0 reserved
											// 0x000DF2 reserved
											// 0x000DF4 reserved
											// 0x000DF6 reserved
											// 0x000DF8 reserved
											// 0x000DFA reserved
											// 0x000DFC reserved
											// 0x000DFE reserved

/**********************************************************************
* Reserved ISRs
**********************************************************************/

interrupt void PIE_RESERVED(void)			// Reserved PIE base vector.
{
// This ISR is for reserved PIE base vectors.  It should never be reached by
// properly executing code.  If you get here, it means something is wrong.

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}

/*********************************************************************/
interrupt void rsvd_ISR(void)				// Reserved PIE group vector
{
// This ISR is for reserved PIE group vectors.  It should never be reached by
// properly executing code.  If you get here, it means something is wrong.

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);
}


/*** end of file *****************************************************/

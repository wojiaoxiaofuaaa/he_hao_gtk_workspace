/**********************************************************************
* File: PieVect_nonBIOS.c
* Devices: TMS320F2808, TMS320F2806, TMS320F2801
* Author: David M. Alter, Texas Instruments Inc.
* History:
*   02/11/05 - original (D. Alter)
**********************************************************************/

#include "..\DSP280x_headers\include\DSP280x_Device.h"

#include "..\include\example_nonBIOS.h"


/**********************************************************************
* The PIE vector initialization table for the F280x.
**********************************************************************/
const struct PIE_VECT_TABLE PieVectTableInit = {

/*** Base vectors ***/
    PIE_RESERVED,                   // 0x000D00  reserved
    PIE_RESERVED,                   // 0x000D02  reserved
    PIE_RESERVED,                   // 0x000D04  reserved
    PIE_RESERVED,                   // 0x000D06  reserved
    PIE_RESERVED,                   // 0x000D08  reserved
    PIE_RESERVED,                   // 0x000D0A  reserved
    PIE_RESERVED,                   // 0x000D0C  reserved
    PIE_RESERVED,                   // 0x000D0E  reserved
    PIE_RESERVED,                   // 0x000D10  reserved
    PIE_RESERVED,                   // 0x000D12  reserved
    PIE_RESERVED,                   // 0x000D14  reserved
    PIE_RESERVED,                   // 0x000D16  reserved
    PIE_RESERVED,                   // 0x000D18  reserved
    INT13_ISR,                      // 0x000D1A  INT13 - XINT13 (or CPU Timer1, reserved for TI)
    INT14_ISR,                      // 0x000D1C  INT14 - CPU Timer2, reserved for TI
    DATALOG_ISR,                    // 0x000D1E  DATALOG - CPU data logging interrupt
    RTOSINT_ISR,                    // 0x000D20  RTOSINT - CPU RTOS interrupt
    EMUINT_ISR,                     // 0x000D22  EMUINT - CPU emulation interrupt
    NMI_ISR,                        // 0x000D24  NMI - XNMI interrupt
    ILLEGAL_ISR,                    // 0x000D26  ILLEGAL - illegal operation trap
    USER1_ISR,                      // 0x000D28  USER1 - software interrupt #1
    USER2_ISR,                      // 0x000D2A  USER2 - software interrupt #2
    USER3_ISR,                      // 0x000D2C  USER3 - software interrupt #3
    USER4_ISR,                      // 0x000D2E  USER4 - software interrupt #4
    USER5_ISR,                      // 0x000D30  USER5 - software interrupt #5
    USER6_ISR,                      // 0x000D32  USER6 - software interrupt #6
    USER7_ISR,                      // 0x000D34  USER7 - software interrupt #7
    USER8_ISR,                      // 0x000D36  USER8 - software interrupt #8
    USER9_ISR,                      // 0x000D38  USER9 - software interrupt #9
    USER10_ISR,                     // 0x000D3A  USER10 - software interrupt #10
    USER11_ISR,                     // 0x000D3C  USER11 - software interrupt #11
    USER12_ISR,                     // 0x000D3E  USER12 - software interrupt #12

/*** Core interrupt #1 re-map ***/
    SEQ1INT_ISR,                    // 0x000D40  ADC Sequencer 1 INT
    SEQ2INT_ISR,                    // 0x000D42  ADC Sequencer 2 INT
    rsvd_ISR,                       // 0x000D44  reserved
    XINT1_ISR,                      // 0x000D46  XINT1
    XINT2_ISR,                      // 0x000D48  XINT2
    ADCINT_ISR,                     // 0x000D4A  ADCINT (ADC)
    TINT0_ISR,                      // 0x000D4C  TINT0 (CPU TIMER 0)
    WAKEINT_ISR,                    // 0x000D4E  WAKEINT (LPM/WD)

/*** Core interrupt #2 re-map ***/
    EPWM1_TZINT_ISR,                // 0x000D50  EPWM1TZINT
    EPWM2_TZINT_ISR,                // 0x000D52  EPWM2TZINT
    EPWM3_TZINT_ISR,                // 0x000D54  EPWM3TZINT
    EPWM4_TZINT_ISR,                // 0x000D56  EPWM4TZINT
    EPWM5_TZINT_ISR,                // 0x000D58  EPWM5TZINT
    EPWM6_TZINT_ISR,                // 0x000D5A  EPWM6TZINT
    rsvd_ISR,                       // 0x000D5C  reserved
    rsvd_ISR,                       // 0x000D5E  reserved

/*** Core interrupt #3 re-map ***/
    EPWM1_INT_ISR,                  // 0x000D60  EPWM1INT
    EPWM2_INT_ISR,                  // 0x000D62  EPWM2INT
    EPWM3_INT_ISR,                  // 0x000D64  EPWM3INT
    EPWM4_INT_ISR,                  // 0x000D66  EPWM4INT
    EPWM5_INT_ISR,                  // 0x000D68  EPWM5INT
    EPWM6_INT_ISR,                  // 0x000D6A  EPWM6INT
    rsvd_ISR,                       // 0x000D6C  reserved
    rsvd_ISR,                       // 0x000D6E  reserved

/*** Core interrupt #4 re-map ***/
    ECAP1_INT_ISR,                  // 0x000D70  ECAP1INT
    ECAP2_INT_ISR,                  // 0x000D72  ECAP2INT
    ECAP3_INT_ISR,                  // 0x000D74  ECAP3INT
    ECAP4_INT_ISR,                  // 0x000D76  ECAP4INT
    rsvd_ISR,                       // 0x000D78  reserved
    rsvd_ISR,                       // 0x000D7A  reserved
    rsvd_ISR,                       // 0x000D7C  reserved
    rsvd_ISR,                       // 0x000D7E  reserved

/*** Core interrupt #5 re-map ***/
    EQEP1_INT_ISR,                  // 0x000D80  EQEP1INT
    EQEP2_INT_ISR,                  // 0x000D82  EQEP2INT
    rsvd_ISR,                       // 0x000D84  reserved
    rsvd_ISR,                       // 0x000D86  reserved
    rsvd_ISR,                       // 0x000D88  reserved
    rsvd_ISR,                       // 0x000D8A  reserved
    rsvd_ISR,                       // 0x000D8C  reserved
    rsvd_ISR,                       // 0x000D8E  reserved

/*** Core interrupt #6 re-map ***/
    SPIRXINTA_ISR,                  // 0x000D90  SPIRXINTA
    SPITXINTA_ISR,                  // 0x000D92  SPIRXINTA
    SPIRXINTB_ISR,                  // 0x000D94  SPIRXINTB
    SPITXINTB_ISR,                  // 0x000D96  SPIRXINTB
    SPIRXINTC_ISR,                  // 0x000D98  SPIRXINTC
    SPITXINTC_ISR,                  // 0x000D9A  SPIRXINTC
    SPIRXINTD_ISR,                  // 0x000D9C  SPIRXINTD
    SPITXINTD_ISR,                  // 0x000D9E  SPIRXINTD

/*** Core interrupt #7 re-map ***/
    rsvd_ISR,                       // 0x000DA0  reserved
    rsvd_ISR,                       // 0x000DA2  reserved
    rsvd_ISR,                       // 0x000DA4  reserved
    rsvd_ISR,                       // 0x000DA6  reserved
    rsvd_ISR,                       // 0x000DA8  reserved
    rsvd_ISR,                       // 0x000DAA  reserved
    rsvd_ISR,                       // 0x000DAC  reserved
    rsvd_ISR,                       // 0x000DAE  reserved

/*** Core interrupt #8 re-map ***/
    I2CINT1A_ISR,                   // 0x000DB0  I2CINT1A (I2C-A)
    I2CINT2A_ISR,                   // 0x000DB2  I2CINT2A (I2C-A)
    rsvd_ISR,                       // 0x000DB4  reserved
    rsvd_ISR,                       // 0x000DB6  reserved
    rsvd_ISR,                       // 0x000DB8  reserved
    rsvd_ISR,                       // 0x000DBA  reserved
    rsvd_ISR,                       // 0x000DBC  reserved
    rsvd_ISR,                       // 0x000DBE  reserved

/*** Core interrupt #9 re-map ***/
    SCIRXINTA_ISR,                  // 0x000DC0  SCIRXINTA (SCI-A)
    SCITXINTA_ISR,                  // 0x000DC2  SCITXINTA (SCI-A)
    SCIRXINTB_ISR,                  // 0x000DC4  SCIRXINTB (SCI-B)
    SCITXINTB_ISR,                  // 0x000DC6  SCITXINTB (SCI-B)
    ECAN0INTA_ISR,                  // 0x000DC8  ECAN0INTA (eCAN-A)
    ECAN1INTA_ISR,                  // 0x000DCA  ECAN1INTA (eCAN-A)
    ECAN0INTB_ISR,                  // 0x000DCC  ECAN0INTB (eCAN-B)
    ECAN1INTB_ISR,                  // 0x000DCE  ECAN1INTB (eCAN-B)

/*** Core interrupt #10 re-map ***/
    rsvd_ISR,                       // 0x000DD0  reserved
    rsvd_ISR,                       // 0x000DD2  reserved
    rsvd_ISR,                       // 0x000DD4  reserved
    rsvd_ISR,                       // 0x000DD6  reserved
    rsvd_ISR,                       // 0x000DD8  reserved
    rsvd_ISR,                       // 0x000DDA  reserved
    rsvd_ISR,                       // 0x000DDC  reserved
    rsvd_ISR,                       // 0x000DDE  reserved

/*** Core interrupt #11 re-map ***/
    rsvd_ISR,                       // 0x000DE0  reserved
    rsvd_ISR,                       // 0x000DE2  reserved
    rsvd_ISR,                       // 0x000DE4  reserved
    rsvd_ISR,                       // 0x000DE6  reserved
    rsvd_ISR,                       // 0x000DE8  reserved
    rsvd_ISR,                       // 0x000DEA  reserved
    rsvd_ISR,                       // 0x000DEC  reserved
    rsvd_ISR,                       // 0x000DEE  reserved


/*** Core interrupt #12 re-map ***/
    rsvd_ISR,                       // 0x000DF0  reserved
    rsvd_ISR,                       // 0x000DF2  reserved
    rsvd_ISR,                       // 0x000DF4  reserved
    rsvd_ISR,                       // 0x000DF6  reserved
    rsvd_ISR,                       // 0x000DF8  reserved
    rsvd_ISR,                       // 0x000DFA  reserved
    rsvd_ISR,                       // 0x000DFC  reserved
    rsvd_ISR,                       // 0x000DFE  reserved

}; //end PieVectTableInit{}


/*** end of file *****************************************************/

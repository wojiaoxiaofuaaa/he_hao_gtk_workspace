// TI File $Revision: /main/8 $
// Checkin $Date: July 2, 2007   16:41:57 $
//###########################################################################
//
// FILE:   F2833x_Boot.h
//
// TITLE:  F2833x Boot ROM Definitions.
//
//###########################################################################
// $TI Release: 2833x/2823x Boot ROM V2 $
// $Release Date: March 4, 2008 $
//###########################################################################
#ifndef TMS320X2833X_BOOT_H
#define TMS320X2833X_BOOT_H
//---------------------------------------------------------------------------
// Function Prototypes
//
extern Uint32 SCI_Boot(void);
extern void InitBoot(void);
//---------------------------------------------------------------------------
// Fixed boot entry points:
//
#define FLASH_ENTRY_POINT 0x320002
#define OTP_ENTRY_POINT   0x380400
#define RAM_ENTRY_POINT   0x000000
#define XINTF_ENTRY_POINT 0x100000
#define PASSWORD_LOCATION 0x33FFF6

#define ERROR 1
#define NO_ERROR 0
#define EIGHT_BIT 8
#define SIXTEEN_BIT 16
#define EIGHT_BIT_HEADER 0x08AA
#define SIXTEEN_BIT_HEADER 0x10AA

typedef Uint16 (* uint16fptr)();
extern uint16fptr GetWordData;
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern void main(void);
extern Uint32 InitSysCtrl_POINT;

#endif // end of TMS320x2833x_BOOT_H definition
 

***********************************************************************
* File: SetDBGIER.asm
* Devices: TMS320F2808, TMS320F2806, TMS320F2801
* Author: David M. Alter, Texas Instruments Inc.
* History:
*   02/11/05 - original (D. Alter)
***********************************************************************


**********************************************************************
* Function: SetDBGIER()                                              *
* Description: Sets the DBGIER register (for realtime emulation).    *
* DSP: TMS320F2808, TMS320F2806, TMS320F2801                         *
* Last Modified: 02/11/05                                            *
* Include files: none                                                *
* Function Prototype: void SetDBGIER(unsigned int)                   *
* Useage: SetDBGIER(value);                                          *
* Input Parameters: Uint16 value = value to put in DBGIER register.  *
* Return Value: none                                                 *
* Notes: none                                                        *
**********************************************************************
		.def _SetDBGIER
		.text
		
_SetDBGIER:
		MOV 	*SP++,AL
		POP 	DBGIER
		LRETR

; end of function SetDBGIER() ****************************************


;end of file SetDBGIER.asm
       .end

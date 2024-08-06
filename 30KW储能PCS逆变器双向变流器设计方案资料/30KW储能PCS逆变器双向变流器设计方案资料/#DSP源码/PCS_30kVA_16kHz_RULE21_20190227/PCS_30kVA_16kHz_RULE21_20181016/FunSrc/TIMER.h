/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef TIMER_H
#define TIMER_H


typedef struct
{
    INT16U    t1msFlag;
    INT16U    t10msFlag;
    INT16U    t20msFlag;
    INT16U    t100msFlag;
    INT16U    t500msFlag;
    INT16U    t1msTimerCnt;
    INT16U    t10msTimerCnt;
    INT16U    t20msTimerCnt;
    INT16U    t100msTimerCnt;
    INT16U    t500msTimerCnt;
}T_TIMEROBJECT;


//---------------------------------------------------------------------------
// Function Prototypes
//
extern void InitTimer(void);
extern void InitTimerVar(void);
extern void TimerIsr(void);
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern T_TIMEROBJECT tTimer;


//---------------------------------------------------------------------------
// Global symbols defined in source files
//
#define TimerFre   1000//1000   //1kHZ

#define ft1ms      tTimer.t1msFlag
#define ft10ms     tTimer.t10msFlag
#define ft20ms     tTimer.t20msFlag
#define ft100ms    tTimer.t100msFlag
#define ft500ms    tTimer.t500msFlag
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

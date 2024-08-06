/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  TIMER.c 
Description: 产生定时时基 
Version :  V1.0
Author:Zhou Xu	 
Date:2009-11-11
*********************************************************************/
#include "..\FunSrc\Funheader.h"

T_TIMEROBJECT tTimer;


#pragma CODE_SECTION(sTimerDeal, "AdcIsrRamFuncs")
/**********************************************************************
* Function: InitTime()
*
* Description: Initializes the Timer0 on the F280x.
**********************************************************************/
void InitTimer(void)
{	
	CpuTimer1Regs.PRD.all  = CPUFre/TimerFre;
	CpuTimer1Regs.TPR.all  = 0;
	CpuTimer1Regs.TPRH.all = 0;
	CpuTimer1Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	CpuTimer1Regs.TCR.bit.TRB = 1;   
	CpuTimer1Regs.TCR.bit.TIE = 1;          	
	CpuTimer1Regs.TCR.bit.TSS = 0;
/*** Enable the Timer 0 interrupt ***/ 
 // PieCtrlRegs.PIEIER1.bit.INTx7 = 1;	// Enable TINT0 in PIE group 1
    IER |= M_INT13;						// Enable INT1 in IER to enable PIE group  		
} // end InitTime()

void InitTimerVar(void)
{
    ft1ms = 0;
	ft10ms = 0;
	ft20ms = 0;
	ft100ms = 0;
	ft500ms = 0;
    tTimer.t1msTimerCnt = 0;
	tTimer.t10msTimerCnt = 0;
	tTimer.t20msTimerCnt = 0;
	tTimer.t100msTimerCnt = 0;
	tTimer.t500msTimerCnt = 0;
}

/*********************************************************************
Function name:  void sTimerDeal(void)
Description:  判断目标定时时间是否到
Calls:        	 
Called By:  TimerIsr(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sTimerDeal(void)
{
    tTimer.t1msTimerCnt++;
	tTimer.t10msTimerCnt++;
	tTimer.t20msTimerCnt++;
	tTimer.t100msTimerCnt++;
	tTimer.t500msTimerCnt++;
	sCalLVRTValue();
    
	if(tTimer.t1msTimerCnt >= 1)
	{
	    ft1ms = 1;
        tTimer.t1msTimerCnt = 0;
	}

	if(tTimer.t10msTimerCnt >= 10)
	{
	    ft10ms = 1;
        tTimer.t10msTimerCnt = 0;
	}

	if(tTimer.t20msTimerCnt >= 20)
	{
	    ft20ms = 1;
        tTimer.t20msTimerCnt = 0;
	}

	if(tTimer.t100msTimerCnt >= 100)
	{
	    ft100ms = 1;
        tTimer.t100msTimerCnt = 0;
	}
	
	if(tTimer.t500msTimerCnt >= 500)
	{
	    ft500ms = 1;
        tTimer.t500msTimerCnt = 0;
	}
}
/*********************************************************************
Function name:  void sBoostParamInit(void)
Description:  Initialize global varibles for boost control
Calls:        	 
Called By:  sPreChkMode(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void TimerIsr(void)
{
    sTimerDeal();   //产生定时时基  
} //end TimeIsr
/*** end of file *****************************************************/

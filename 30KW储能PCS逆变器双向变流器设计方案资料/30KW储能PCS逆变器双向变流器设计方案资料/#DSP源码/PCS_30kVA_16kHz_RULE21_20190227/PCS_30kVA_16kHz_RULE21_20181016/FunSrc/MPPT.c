/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  boostcontroller.c 
Description:  This file gives definitions and implementation of boost control functions 
Version :  V1.0
Author:Zhou Xu	 
Date:2009-11-09
*********************************************************************/
/**** head-files including****/
#include "..\FunSrc\Funheader.h"

//macro definitions constant for MPPT

#define cBus5V              80 //80=16*5V
#define cBus20V            320 //  320 = 16* 20V

//MPPT global variables in this file 
INT32S dwPVVoltSum = 0;
INT32S dwPVPowerSum = 0;

INT16S wMpptPvVoltMPPrev = 0;
INT32S dwMpptPvPowerMPPrev = 0;
INT16S wMpptPvVoltMP = 0;
INT16S wMpptCount;        //MPPT Count
INT16S fLoadDerating = 0; // 1: normal;-1 derating;0,do noting
INT16U uwMpptVolMin = 0;
INT16S wMpptPVCurrOffset = 0;

#pragma CODE_SECTION(sMpptStrategy, "sMpptStrategyRamFuncs")

/**** global function ****/
/*********************************************************************
Function name:  void sMPPTParamInit(void)
Description:  Initialize global varibles for MPPT
Calls:        	 
Called By:  maybe sPreChkMode(void) or others
Parameters:  void
Return:  void      
 *********************************************************************/
void sMPPTParamInit(void)
{
    dwPVVoltSum = 0;   
    dwPVPowerSum = 0;   
    wMpptPvVoltMPPrev = 0;
    dwMpptPvPowerMPPrev = 0;
    wMpptCount = 0;
    wMpptPvVoltMP = 8800;   
	fLoadDerating = 0;
	wMpptPVCurrOffset = 0;
 }
 
 /*********************************************************************************
Function name:void sMpptStrategy(INT16S wPVVolMppt,INT16S wPVCurrMppt,INT16S wDCPowerMppt)
Description: In boost interrupt
Calls:  void
Called By:  100us period  in interrupt                                                              
Parameters: INT16S,INT16S
Return: void		
Freq: 6kHz
**********************************************************************************/
void sMpptStrategy(INT16S wPVVolMppt,INT16S wPVCurrMppt,INT16S wDCPowerMppt)
{	
	INT16S wPvVoltAvg = 0;  
	INT32S dwPvPowerAvg = 0; 
	INT32S dwDeltaPvPower = 0; 
	INT16S wDeltaPvVolt = 0;  
	INT16S wPOstep = 24;
	static INT16U uwMiniCurrCnt = 0;
    
	dwPVVoltSum = dwPVVoltSum + wPVVolMppt;
	dwPVPowerSum = dwPVPowerSum + wDCPowerMppt;
	
	if(++wMpptCount >= 8)
	{
	    wMpptCount = 0;

        wPvVoltAvg = dwPVVoltSum>>3;                          
	    dwPvPowerAvg = dwPVPowerSum>>3;
	    dwPVVoltSum = 0;
	    dwPVPowerSum = 0;
	    
	    if((wPVCurrMppt - wMpptPVCurrOffset)> 32) //64=4A*Q4
	    {	   
	        dwDeltaPvPower = (dwPvPowerAvg - dwMpptPvPowerMPPrev); 
	        wDeltaPvVolt = wPvVoltAvg - wMpptPvVoltMPPrev;  
			//===for debug====
	        uwDebugPvVoltAvg = wDeltaPvVolt; 
	        uwDebugPvPower = dwDeltaPvPower >> 4;					
	       
	        dwMpptPvPowerMPPrev = dwPvPowerAvg;
	        wMpptPvVoltMPPrev = wPvVoltAvg;
	        
	        if(wDCPowerMppt < 2176)
	        {
	        	wPOstep = 24;
	        }
	        else if(wDCPowerMppt < 4352)
	        {
	        	wPOstep = 24;
	        }
	        else
	        {
	        	wPOstep = 24;
	        }
	        

			if(abs(wPvVoltAvg - wMpptPvVoltMP) < cBus5V)
			{
				if (dwDeltaPvPower > 0)
				{
					if (wDeltaPvVolt > 0)
					{
						wMpptPvVoltMP = wMpptPvVoltMP + wPOstep;
					}
					else if (wDeltaPvVolt < 0)
					{
						wMpptPvVoltMP = wMpptPvVoltMP - wPOstep;
					}
				}
				else
				{
					if (wDeltaPvVolt > 0)
					{
						wMpptPvVoltMP = wMpptPvVoltMP - wPOstep;
					}
					else if (wDeltaPvVolt < 0)
					{
						wMpptPvVoltMP = wMpptPvVoltMP + wPOstep;
					}
					else
				    {
					}
				}
	        }
			uwMiniCurrCnt = 0;
	    }
	    else
	    {
		    //===for accelarator MPPT====
		    if (++uwMiniCurrCnt < 10 )  //  10*320ms = 3.2s
		    {
		        wMpptPvVoltMP = wMpptPvVoltMP - cBus5V;
		    }
		    else
		    {
		        wMpptPvVoltMP = wMpptPvVoltMP - cBus20V;
		        uwMiniCurrCnt = 10;
		    }
	    }
    }  
         
    //limit of mppt voltage
    if (wMpptPvVoltMP > 13600)
    {
        wMpptPvVoltMP = 13600;
    }
    else if (wMpptPvVoltMP < uwMpptVolMin)
    {
        wMpptPvVoltMP = uwMpptVolMin;
    }   
}
/*********************************************************************
Function name:  void  sSetMPVolt(INT16S wVolt)
Description:  set wVolt as the MP voltage
Calls:         	 
Called By:  maybe sPreChkMode(void) or others.Before calling suwMPPTStrategy(),call this function 
Parameters:  void
Return:  void      
 *********************************************************************/
void sSetMPVolt(INT16S wVolt)
{
    wMpptPvVoltMP = wVolt;
}




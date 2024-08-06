/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  boostcontroller.c 
Description:  This file gives definitions and implementation of boost control functions 
Version :  V1.0
Author:  Zhou Xu
Date:2016.07.13
*********************************************************************/
/**** head-files including****/
#include "..\FunSrc\Funheader.h"

/**** macro definition****/
#define cVolt5V                    80

#define cBusDivBattery             7099//780/450
#define cDCBusVoltFiltGainNum      717//410
#define cDCBusVoltFiltGainDen      3379//3686
#define cDCBusVoltKp               5147//5147
#define cDCBusVoltKi               7//12
#define cDCBusVoltKaw              5//10

#define cBatteryVoltFiltGainNum    338//208
#define cBatteryVoltFiltGainDen    3758//3888
#define cBatteryVoltKp             5147//5147
#define cBatteryVoltKi             15//24
#define cBatteryVoltKaw            12//19 

#define cDCCurrFiltGainNum         4009//3363 
#define cDCCurrFiltGainDen         87//733
#define cDCCurrKp                  10294//7824
#define cDCCurrKi                  264//449
#define cDCCurrKaw                 105//235

#define cDCPwmCarr                 1344 //8192/(780*16)  Q11
#define cDCPwmCarrInv              1560 //1/cDCPwmCarr*2048 Q10
#define cDCModuLimitHigh           7372 //Boost IGBT not allowed 100% duty,littler than 2us duty ,can't be out
#define cDCModuLimitLow            1    //we limited 90%

INT16U fDCModeSwitch = 1;//0:Disable 1:Switch 1,2 Or Switch 3,4  2:Switch1,2,3,4.
INT16U fDCModeSwitchEn = 0;
INT16S wDCCtrCoeff = 8;
INT16S wBatteryCurrRef = 0;
INT16S wDCCtrBusVolRef = 0;
INT16S wBatteryLimitCurrHigh = cDCCurrMax;
INT16S wBatteryLimitCurrLow = cDCCurrMin;
INT16S wBatteryChargeCurr = cDCCurrMin;
INT16S wBatteryDisChargeCurr = cDCCurrMax;

INT16S wBatVolLimtHigh = 9600;
INT16S wBatVolLimtLow = 6400;
INT16S wBiDCCtrModu1,wBiDCCtrModu2,wBiDCCtrModu3,wBiDCCtrModu4;
INT16S wUdDivBatCoeff = 4816;
INT16U fDCVoltCtrDeal = 0;
INT32S dwDeltaDCCurr = 0;
INT16S wDCPwmCarrFrwd = cDCPwmCarr;
INT16U fCPLDShutDCDeal = 0;
INT16U uwCPLDShutDCDelay = 0;

INT16S wDCBusVoltErrFiltPrev = 0;
INT32S dwDCBusVoltIngCtrl = 0;
INT32S dwDCBusVoltSatErr = 0;

INT16S wBatteryChargeVoltErrFiltPrev = 0;
INT32S dwBatteryChargeVoltIngCtrl = 0;
INT32S dwBatteryChargeVoltSatErr = 0;

INT16S wBatteryDischargeVoltErrFiltPrev = 0;
INT32S dwBatteryDischargeVoltIngCtrl = 0;
INT32S dwBatteryDischargeVoltSatErr = 0;

INT16S wDCCurrErrFiltPrev1st = 0;
INT32S dwDCCurrIngCtrl1st = 0;
INT32S dwDCModuSatErr1st = 0;

INT16S wDCCurrErrFiltPrev2nd = 0;
INT32S dwDCCurrIngCtrl2nd = 0;
INT32S dwDCModuSatErr2nd = 0;

INT16S wDCCurrErrFiltPrev3rd = 0;
INT32S dwDCCurrIngCtrl3rd = 0;
INT32S dwDCModuSatErr3rd = 0;

INT16S wDCCurrErrFiltPrev4th = 0;
INT32S dwDCCurrIngCtrl4th = 0;
INT32S dwDCModuSatErr4th = 0;

INT16S wDebugDeltaDCCurr = 0;
INT16S wDebugDeltaDCCurrCharge = 0;
INT16S wDebugDeltaDCCurrDisCharge = 0;
INT16S wDebugCPLDShutDC = 0;

INT16U uwDCCtrStartDelay = 0;

#pragma CODE_SECTION(swBiDirDCVoltCtrl, "BiDirDCCtrFuncs")
#pragma CODE_SECTION(sBiDirBatteryVoltCtr, "BiDirDCCtrFuncs")
#pragma CODE_SECTION(swBiDirectionalDCCurrCtr1st, "BiDirDCCtrFuncs")
#pragma CODE_SECTION(swBiDirectionalDCCurrCtr2nd, "BiDirDCCtrFuncs")
#pragma CODE_SECTION(swBiDirectionalDCCurrCtr3rd, "BiDirDCCtrFuncs")
#pragma CODE_SECTION(swBiDirectionalDCCurrCtr4th, "BiDirDCCtrFuncs")
#pragma CODE_SECTION(sBiDirDCCon1And3Controller, "BiDirDCCtrFuncs")
#pragma CODE_SECTION(sBiDirDCCon2And4Controller, "BiDirDCCtrFuncs")

//------------------------------Data Ram-----------------------------------
#pragma DATA_SECTION(fCPLDShutDCDeal,"RamSafetyProt");
#pragma DATA_SECTION(uwCPLDShutDCDelay,"RamSafetyProt");

/*********************************************************************
Function name:  void sBoostParamInit(void)
Description:  Initialize global varibles for boost control
Calls:        	 
Called By:  sPreChkMode(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sBiDirDCCtrParamInit(void)
{
    fDCModeSwitch = 1;
    fDCModeSwitchEn = 0;  
    wDCCtrCoeff = 8;
    fDCVoltCtrDeal = 0;
    wBatteryCurrRef = 0;
    wDCCtrBusVolRef = 8000;
    wBatteryLimitCurrHigh = cDCCurrMax;
    wBatteryLimitCurrLow = cDCCurrMin;
    wBatteryChargeCurr = cDCCurrMin;
    wBatteryDisChargeCurr = cDCCurrMax;
    wBiDCCtrModu1 = 1;
    wBiDCCtrModu2 = 1;
    wBiDCCtrModu3 = 1;
    wBiDCCtrModu4 = 1;
    wUdDivBatCoeff = 4816;
    dwDeltaDCCurr = 0;
    
    wDCBusVoltErrFiltPrev = 0;
    dwDCBusVoltIngCtrl = 0;
    dwDCBusVoltSatErr = 0;
    
    wBatteryChargeVoltErrFiltPrev = 0;
    dwBatteryChargeVoltIngCtrl = 0;
    dwBatteryChargeVoltSatErr = 0;
    
    wBatteryDischargeVoltErrFiltPrev = 0;
    dwBatteryDischargeVoltIngCtrl = 0;
    dwBatteryDischargeVoltSatErr = 0;
    
    wDCCurrErrFiltPrev1st = 0;
    dwDCCurrIngCtrl1st = 0;
    dwDCModuSatErr1st = 0;
    
    wDCCurrErrFiltPrev2nd = 0;
    dwDCCurrIngCtrl2nd = 0;
    dwDCModuSatErr2nd = 0;
    
    wDCCurrErrFiltPrev3rd = 0;
    dwDCCurrIngCtrl3rd = 0;
    dwDCModuSatErr3rd = 0;
    
    wDCCurrErrFiltPrev4th = 0;
    dwDCCurrIngCtrl4th = 0;
    dwDCModuSatErr4th = 0;
    
    fCPLDShutDCDeal = 0;
    uwCPLDShutDCDelay = 0;
    uwDCCtrStartDelay = 0;
}
 
/******************************************************************************************
Function name: INT16S swBiDirDCVoltCtrl(INT16S wDCVoltRef,INT16S wDCVolt) 
Description: BiDirection DC voltage closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  INT16S      
 *****************************************************************************************/
INT16S swBiDirDCVoltCtrl(INT16S wDCVoltRef,INT16S wPOVolt,INT16S wONVolt)  
{	
INT32S dwDCBusVoltErr;
INT16S wDCBusVoltErrFilt;
INT32S dwDCBusVoltPICtrl;
INT32S dwDCBusCurrRefPreSat;
INT16S wDCCurrRef;
INT32S dwDeltaDCCurrTmp;  
    
    //*****************************Bi-Directional voltage control*********************************
    dwDCBusVoltErr = ((INT32S)((INT32S)wDCVoltRef - wPOVolt - wONVolt) * cBusDivBattery)>>12;// 780/450 Q4
    dwDCBusVoltErr = dwDCBusVoltErr * wDCCtrCoeff;//4 DC
    if(dwDCBusVoltErr > 32000)
    {
    	dwDCBusVoltErr = 32000;
    }
    else if(dwDCBusVoltErr < -32000)
    {
    	dwDCBusVoltErr = -32000;
    }
    //LPF 
    wDCBusVoltErrFilt = (INT16S)(((INT32S)wDCBusVoltErrFiltPrev * cDCBusVoltFiltGainDen + dwDCBusVoltErr * cDCBusVoltFiltGainNum) >> 12);
    wDCBusVoltErrFiltPrev = wDCBusVoltErrFilt;
    
    if(fDCChargeToDisCharge == 1)
    {
    	if(wDCCtrCoeff == 8)
    	{
    	    dwDeltaDCCurrTmp = (INT32S)wInvCurrResolving * wUdDivBatCoeff;//(INT32S)512 * wUdDivBatCoeff;//4A 1024,But 2 DC Runing = 1024/2
    	}
    	else
    	{
    		dwDeltaDCCurrTmp = ((INT32S)wInvCurrResolving * wUdDivBatCoeff)>>1;//(INT32S)256 * wUdDivBatCoeff;//4A 1024,But 4 DC Runing = 1024/4
    	}
    	wDebugDeltaDCCurrDisCharge = dwDeltaDCCurrTmp>>12;
    	wDCBusVoltErrFiltPrev = 0;
    	dwDCBusVoltSatErr = 0;
    	dwDCBusVoltIngCtrl = dwDCBusVoltIngCtrl + dwDeltaDCCurrTmp;
    	fDCChargeToDisCharge = 0;
    }
    else if(fDCDisChargeToCharge == 1)
    {
    	if(wDCCtrCoeff == 8)
    	{
    	    dwDeltaDCCurrTmp = (INT32S)wInvCurrResolving * wUdDivBatCoeff;//(INT32S)512 * wUdDivBatCoeff;//4A 1024,But 2 DC Runing = 1024/2
    	}
    	else
    	{
    		dwDeltaDCCurrTmp = ((INT32S)wInvCurrResolving * wUdDivBatCoeff)>>1;//(INT32S)256 * wUdDivBatCoeff;//4A 1024,But 4 DC Runing = 1024/4
    	}
    	wDebugDeltaDCCurrCharge = dwDeltaDCCurrTmp>>12;
    	wDCBusVoltErrFiltPrev = 0;
    	dwDCBusVoltSatErr = 0;
    	dwDCBusVoltIngCtrl = dwDCBusVoltIngCtrl - dwDeltaDCCurrTmp;
    	fDCDisChargeToCharge = 0;
    }
    
    if(fDCVoltCtrDeal == 1)
    {  	
    	if(wDCCtrCoeff == 8)
    	{
    		dwDeltaDCCurrTmp = dwDeltaDCCurr>>1;
    	}
    	else
    	{
    		dwDeltaDCCurrTmp = dwDeltaDCCurr>>2;
    	}  
    	wDebugDeltaDCCurr = dwDeltaDCCurrTmp>>12;  
    	wDCBusVoltErrFiltPrev = 0;
    	dwDCBusVoltSatErr = 0;
    	//wDCBusVoltErrFilt = 0;	
    	dwDCBusVoltIngCtrl = dwDCBusVoltIngCtrl + dwDeltaDCCurrTmp;
    	fDCVoltCtrDeal = 0;
    }

    //integral 
    dwDCBusVoltIngCtrl = dwDCBusVoltIngCtrl + (INT32S)wDCBusVoltErrFilt * cDCBusVoltKi + (INT32S)dwDCBusVoltSatErr * cDCBusVoltKaw;
    //PI
    dwDCBusVoltPICtrl = (((INT32S)wDCBusVoltErrFilt * cDCBusVoltKp + dwDCBusVoltIngCtrl) >> 12);
    //boost current reference 
    dwDCBusCurrRefPreSat = dwDCBusVoltPICtrl;	
    //limit of boost current reference
    if (dwDCBusCurrRefPreSat > wBatteryLimitCurrHigh) 
    {		
        wDCCurrRef = wBatteryLimitCurrHigh;
    }
    else if (dwDCBusCurrRefPreSat < wBatteryLimitCurrLow)
    {		
    	wDCCurrRef = wBatteryLimitCurrLow;
    }
    else
    {
    	wDCCurrRef = dwDCBusCurrRefPreSat;
    }
    //saturation error for anti-windup
    dwDCBusVoltSatErr = (INT32S)wDCCurrRef - dwDCBusCurrRefPreSat;
    
    if((fInvEnOut == 1)&&(fRunInvAhdChkRequire == 1))//avoid bat voltage limit set lower or higher than bat voltage,fInvEnout = 1,the Inv can limit bus voltage
    {
        if(fCharge == 1)
        {   	
        	if(wDCCurrRef < wBatteryChargeCurr)
        	{
        		wDCCurrRef = wBatteryChargeCurr;
        	}
        }
        else
        {    	
        	if(wDCCurrRef > wBatteryDisChargeCurr)
        	{
        		wDCCurrRef = wBatteryDisChargeCurr;
        	}
        }
    }
    
    if(fCPLDShutDCPWM == 0)
    {
    	fCPLDShutDCDeal = 1;
        uwCPLDShutDCDelay = 0;
    }
    
    if(fCPLDShutDCDeal == 1)
    {
    	wDCCurrRef = 0;    	
    	wDCBusVoltErrFiltPrev = 0;
    	dwDCBusVoltSatErr = 0;
    	dwDCBusVoltIngCtrl = 0;
    	wDebugCPLDShutDC++;
    	
    	if(++uwCPLDShutDCDelay >= 4)
    	{
    		uwCPLDShutDCDelay = 4;
    		fCPLDShutDCDeal = 0;
    	}
    }
    else
    {
    	uwCPLDShutDCDelay = 0;
    }
    
    return(wDCCurrRef);
}

/******************************************************************************************
Function name: void sBiDirBatteryVoltCtr(INT16S wBatVolt) 
Description: BiDirection Battery voltage closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  void      
 *****************************************************************************************/
void sBiDirBatteryVoltCtr(INT16S wBatVolt)
{	
INT32S dwBatteryVoltErr;
INT16S wBatteryVolErrFilt;
INT32S dwBatteryVolPICtrl;
INT32S dwBatteryHighRefPreSat;
INT32S dwBatteryLowRefPreSat;
INT16S wBatteryCurrLow,wBatteryCurrHigh;
    
    //*****************************Battery Volt upper level control*********************************    
    dwBatteryVoltErr = (INT32S)((INT32S)wBatVolLimtHigh - wBatVolt) * wDCCtrCoeff;
    if(dwBatteryVoltErr > 32000)
    {
    	dwBatteryVoltErr = 32000;
    }
    else if(dwBatteryVoltErr < -32000)
    {
    	dwBatteryVoltErr = -32000;
    }       
    //LPF 
    wBatteryVolErrFilt = (INT16S)(((INT32S)wBatteryChargeVoltErrFiltPrev * cBatteryVoltFiltGainDen + dwBatteryVoltErr * cBatteryVoltFiltGainNum) >> 12);
    wBatteryChargeVoltErrFiltPrev = wBatteryVolErrFilt;
    //integral 
    dwBatteryChargeVoltIngCtrl = dwBatteryChargeVoltIngCtrl + (INT32S)wBatteryVolErrFilt * cBatteryVoltKi + (INT32S)dwBatteryChargeVoltSatErr * cBatteryVoltKaw;
    //PI
    dwBatteryVolPICtrl = ((INT32S)wBatteryVolErrFilt * cBatteryVoltKp + dwBatteryChargeVoltIngCtrl) >> 12;
    //boost current reference 
    dwBatteryLowRefPreSat = dwBatteryVolPICtrl;	
    //limit of boost current reference
    if (dwBatteryLowRefPreSat > -cDCCurrMin) 
    {		
        wBatteryCurrLow = -cDCCurrMin;
    }
    else if (dwBatteryLowRefPreSat < -cDCCurrResolving)//cDCCurrMax must little than AC min Current * GridVolt/BatteryVolt
    {		
    	wBatteryCurrLow = -cDCCurrResolving;//cDCCurrMax
    }
    else
    {
    	wBatteryCurrLow = (INT16S)dwBatteryLowRefPreSat;
    }
    //saturation error for anti-windup
    dwBatteryChargeVoltSatErr = (INT32S)wBatteryCurrLow - dwBatteryLowRefPreSat;    
    wBatteryChargeCurr = - wBatteryCurrLow;
    
    //*****************************Battery Volt lower level control*********************************
    dwBatteryVoltErr = (INT32S)((INT32S)wBatVolLimtLow - wBatVolt) * wDCCtrCoeff;
    if(dwBatteryVoltErr > 32000)
    {
    	dwBatteryVoltErr = 32000;
    }
    else if(dwBatteryVoltErr < -32000)
    {
    	dwBatteryVoltErr = -32000;
    } 
    
    //LPF 
    wBatteryVolErrFilt = (INT16S)(((INT32S)wBatteryDischargeVoltErrFiltPrev * cBatteryVoltFiltGainDen + dwBatteryVoltErr * cBatteryVoltFiltGainNum) >> 12);
    wBatteryDischargeVoltErrFiltPrev = wBatteryVolErrFilt;
    
    //integral 
    dwBatteryDischargeVoltIngCtrl = dwBatteryDischargeVoltIngCtrl + (INT32S)wBatteryVolErrFilt * cBatteryVoltKi + (INT32S)dwBatteryDischargeVoltSatErr * cBatteryVoltKaw;
    //PI
    dwBatteryVolPICtrl = ((INT32S)wBatteryVolErrFilt * cBatteryVoltKp + dwBatteryDischargeVoltIngCtrl) >> 12;
    //boost current reference 
    dwBatteryHighRefPreSat = dwBatteryVolPICtrl;	
    //limit of boost current reference
    if (dwBatteryHighRefPreSat > cDCCurrResolving)//-cDCCurrMin
    {		
        wBatteryCurrHigh = cDCCurrResolving;//-cDCCurrMin
    }
    else if (dwBatteryHighRefPreSat < -cDCCurrMax)
    {		
    	wBatteryCurrHigh = -cDCCurrMax;
    }
    else
    {
    	wBatteryCurrHigh = (INT16S)dwBatteryHighRefPreSat;
    }
    //saturation error for anti-windup
    dwBatteryDischargeVoltSatErr = (INT32S)wBatteryCurrHigh - dwBatteryHighRefPreSat;
    wBatteryDisChargeCurr = - wBatteryCurrHigh;   
}
/******************************************************************************************
Function name:INT16S swBiDirectionalDCCurrCtr1st(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
Description: First BiDirection DC Converter Current closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  INT16S      
 *****************************************************************************************/
INT16S swBiDirectionalDCCurrCtr1st(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
{
INT16S wDCCurrModu;
INT16S wDCCurrErr;
INT16S wDCCurrErrFilt;
INT32S dwDCCurrPICtrl;
INT32S dwDCModuPreSat;
INT32S dwDCModuFedFrwd;
    
    wDCCurrErr = wDCCurrRef - wDCCurr;//Q8
    
    if(fCPLDShutDCDeal == 1)
    {
        wDCCurrErr = 0;
        wDCCurrErrFiltPrev1st = 0;
        dwDCCurrIngCtrl1st = 0;
        dwDCModuSatErr1st = 0;
    }
    //LPF 
    wDCCurrErrFilt = (INT16S)(((INT32S)wDCCurrErrFiltPrev1st * cDCCurrFiltGainDen + (INT32S)wDCCurrErr * cDCCurrFiltGainNum) >> 12);
    wDCCurrErrFiltPrev1st = wDCCurrErrFilt;
    
    //integral 
    dwDCCurrIngCtrl1st = dwDCCurrIngCtrl1st + (INT32S)wDCCurrErrFilt * cDCCurrKi + (INT32S)dwDCModuSatErr1st * cDCCurrKaw;
        
    //PI
    dwDCCurrPICtrl = ((INT32S)wDCCurrErrFilt * cDCCurrKp + dwDCCurrIngCtrl1st) >> 12;
    
    dwDCModuFedFrwd = (INT32S)8192 - (((INT32S)wDCVolt * wDCPwmCarrFrwd)>>11);
    
    if(dwDCModuFedFrwd < 0)
    {
    	dwDCModuFedFrwd = 0;
    }

    dwDCModuPreSat = (((INT64S)dwDCCurrPICtrl * cDCPwmCarr)>>15) + dwDCModuFedFrwd;
    
    //limit of boost modulation
    if (dwDCModuPreSat > cDCModuLimitHigh)
    {
        wDCCurrModu = cDCModuLimitHigh;
    }
    else if (dwDCModuPreSat < cDCModuLimitLow)
    {
        wDCCurrModu  = cDCModuLimitLow;
    }
    else
    {
        wDCCurrModu = dwDCModuPreSat;
    }
    //compute saturation error for anti-windup
    dwDCModuSatErr1st = ((((INT32S)wDCCurrModu - dwDCModuPreSat) * cDCPwmCarrInv)>>6);
    
    wDCCurrModu = ((INT32S)wDCCurrModu * uwBiDirDCCtrPWMPrd)>>13;
    
    if(wDCCurrModu < 1)//225
    {
    	wDCCurrModu = 1;//boost IGBT's duty little than 3us,don't out
    }
    //return boost modulation value
    return(wDCCurrModu); 
}
/******************************************************************************************
Function name:INT16S swBiDirectionalDCCurrCtr1st(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
Description: Sencond BiDirection DC Converter Current closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  INT16S      
 *****************************************************************************************/
INT16S swBiDirectionalDCCurrCtr2nd(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
{
INT16S wDCCurrModu;
INT16S wDCCurrErr;
INT16S wDCCurrErrFilt;
INT32S dwDCCurrPICtrl;
INT32S dwDCModuPreSat;
INT32S dwDCModuFedFrwd;
    
    wDCCurrErr = wDCCurrRef - wDCCurr;
    
    if(fCPLDShutDCDeal == 1)
    {
        wDCCurrErr = 0;
        wDCCurrErrFiltPrev2nd = 0;
        dwDCCurrIngCtrl2nd = 0;
        dwDCModuSatErr2nd = 0;
    }
    //LPF 
    wDCCurrErrFilt = (INT16S)(((INT32S)wDCCurrErrFiltPrev2nd * cDCCurrFiltGainDen + (INT32S)wDCCurrErr * cDCCurrFiltGainNum) >> 12);
    wDCCurrErrFiltPrev2nd = wDCCurrErrFilt;
    
    //integral 
    dwDCCurrIngCtrl2nd = dwDCCurrIngCtrl2nd + (INT32S)wDCCurrErrFilt * cDCCurrKi + (INT32S)dwDCModuSatErr2nd * cDCCurrKaw;
    
    //PI
    dwDCCurrPICtrl = ((INT32S)wDCCurrErrFilt * cDCCurrKp + dwDCCurrIngCtrl2nd) >> 12;

    dwDCModuFedFrwd = (INT32S)8192 - (((INT32S)wDCVolt * wDCPwmCarrFrwd)>>11);
    
    if(dwDCModuFedFrwd < 0)
    {
    	dwDCModuFedFrwd = 0;
    }

    dwDCModuPreSat = (((INT64S)dwDCCurrPICtrl * cDCPwmCarr)>>15) + dwDCModuFedFrwd;
    
    //limit of boost modulation
    if (dwDCModuPreSat > cDCModuLimitHigh)
    {
        wDCCurrModu = cDCModuLimitHigh;
    }
    else if (dwDCModuPreSat < cDCModuLimitLow)
    {
        wDCCurrModu  = cDCModuLimitLow;
    }
    else
    {
        wDCCurrModu = dwDCModuPreSat;
    }
    //compute saturation error for anti-windup
    dwDCModuSatErr2nd = ((((INT32S)wDCCurrModu - dwDCModuPreSat) * cDCPwmCarrInv)>>6);
    
    wDCCurrModu = ((INT32S)wDCCurrModu * uwBiDirDCCtrPWMPrd)>>13;
    if(wDCCurrModu < 1)
    {
    	wDCCurrModu = 1;//boost IGBT's duty little than 3us,don't out
    }
    //return boost modulation value
    return(wDCCurrModu); 
}
/******************************************************************************************
Function name:INT16S swBiDirectionalDCCurrCtr1st(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
Description: Third BiDirection DC Converter Current closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  INT16S      
 *****************************************************************************************/
INT16S swBiDirectionalDCCurrCtr3rd(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
{
INT16S wDCCurrModu;
INT16S wDCCurrErr;
INT16S wDCCurrErrFilt;
INT32S dwDCCurrPICtrl;
INT32S dwDCModuPreSat;
INT32S dwDCModuFedFrwd;
    
    wDCCurrErr = wDCCurrRef - wDCCurr;
    
    if(fCPLDShutDCDeal == 1)
    {
        wDCCurrErr = 0;
        wDCCurrErrFiltPrev3rd = 0;
        dwDCCurrIngCtrl3rd = 0;
        dwDCModuSatErr3rd = 0;
    }
    //LPF 
    wDCCurrErrFilt = (INT16S)(((INT32S)wDCCurrErrFiltPrev3rd * cDCCurrFiltGainDen + (INT32S)wDCCurrErr * cDCCurrFiltGainNum) >> 12);
    wDCCurrErrFiltPrev3rd = wDCCurrErrFilt;
    
    //integral 
    dwDCCurrIngCtrl3rd = dwDCCurrIngCtrl3rd + (INT32S)wDCCurrErrFilt * cDCCurrKi + (INT32S)dwDCModuSatErr3rd * cDCCurrKaw;
    
    //PI
    dwDCCurrPICtrl = ((INT32S)wDCCurrErrFilt * cDCCurrKp + dwDCCurrIngCtrl3rd) >> 12;

    dwDCModuFedFrwd = (INT32S)8192 - (((INT32S)wDCVolt * wDCPwmCarrFrwd)>>11);
    
    if(dwDCModuFedFrwd < 0)
    {
    	dwDCModuFedFrwd = 0;
    }

    dwDCModuPreSat = (((INT64S)dwDCCurrPICtrl * cDCPwmCarr)>>15) + dwDCModuFedFrwd;
    
    //limit of boost modulation
    if (dwDCModuPreSat > cDCModuLimitHigh)
    {
        wDCCurrModu = cDCModuLimitHigh;
    }
    else if (dwDCModuPreSat < cDCModuLimitLow)
    {
        wDCCurrModu  = cDCModuLimitLow;
    }
    else
    {
        wDCCurrModu = dwDCModuPreSat;
    }
    //compute saturation error for anti-windup
    dwDCModuSatErr3rd = ((((INT32S)wDCCurrModu - dwDCModuPreSat) * cDCPwmCarrInv)>>6);
    
    wDCCurrModu = ((INT32S)wDCCurrModu * uwBiDirDCCtrPWMPrd)>>13;
    if(wDCCurrModu < 1)
    {
    	wDCCurrModu = 1;//boost IGBT's duty little than 3us,don't out
    }
    //return boost modulation value
    return(wDCCurrModu); 
}
/******************************************************************************************
Function name:INT16S swBiDirectionalDCCurrCtr1st(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
Description: Fourth BiDirection DC Converter Current closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  INT16S      
 *****************************************************************************************/
INT16S swBiDirectionalDCCurrCtr4th(INT16S wDCCurrRef,INT16S wDCCurr,INT16S wDCVolt)
{
INT16S wDCCurrModu;
INT16S wDCCurrErr;
INT16S wDCCurrErrFilt;
INT32S dwDCCurrPICtrl;
INT32S dwDCModuPreSat;
INT32S dwDCModuFedFrwd;
    
    wDCCurrErr = wDCCurrRef - wDCCurr;
    
    if(fCPLDShutDCDeal == 1)
    {
        wDCCurrErr = 0;
        wDCCurrErrFiltPrev4th = 0;
        dwDCCurrIngCtrl4th = 0;
        dwDCModuSatErr4th = 0;
    }
    //LPF 
    wDCCurrErrFilt = (INT16S)(((INT32S)wDCCurrErrFiltPrev4th * cDCCurrFiltGainDen + (INT32S)wDCCurrErr * cDCCurrFiltGainNum) >> 12);
    wDCCurrErrFiltPrev4th = wDCCurrErrFilt;
    
    //integral 
    dwDCCurrIngCtrl4th = dwDCCurrIngCtrl4th + (INT32S)wDCCurrErrFilt * cDCCurrKi + (INT32S)dwDCModuSatErr4th * cDCCurrKaw;
    
    //PI
    dwDCCurrPICtrl = ((INT32S)wDCCurrErrFilt * cDCCurrKp + dwDCCurrIngCtrl4th) >> 12;

    dwDCModuFedFrwd = (INT32S)8192 - (((INT32S)wDCVolt * wDCPwmCarrFrwd)>>11);
    
    if(dwDCModuFedFrwd < 0)
    {
    	dwDCModuFedFrwd = 0;
    }

    dwDCModuPreSat = (((INT64S)dwDCCurrPICtrl * cDCPwmCarr)>>15) + dwDCModuFedFrwd;
    
    //limit of boost modulation
    if (dwDCModuPreSat > cDCModuLimitHigh)
    {
        wDCCurrModu = cDCModuLimitHigh;
    }
    else if (dwDCModuPreSat < cDCModuLimitLow)
    {
        wDCCurrModu  = cDCModuLimitLow;
    }
    else
    {
        wDCCurrModu = dwDCModuPreSat;
    }
    //compute saturation error for anti-windup
    dwDCModuSatErr4th = ((((INT32S)wDCCurrModu - dwDCModuPreSat) * cDCPwmCarrInv)>>6);
    
    wDCCurrModu = ((INT32S)wDCCurrModu * uwBiDirDCCtrPWMPrd)>>13;
    if(wDCCurrModu < 1)
    {
    	wDCCurrModu = 1;//boost IGBT's duty little than 3us,don't out
    }
    //return boost modulation value
    return(wDCCurrModu); 
}
/******************************************************************************************
Function name:Void sBiDirDCCon1And3Controller(INT16S wBatVolt,INT16S wBatCurr1,INT16S wBatCurr3,INT16U fDCOutEn)
Description: Fourth BiDirection DC Converter Current closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  INT16S      
 *****************************************************************************************/
void sBiDirDCCon1And3Controller(INT16S wBatVolt,INT16S wBatCurr1,INT16S wBatCurr3,INT16U fDCOutEn)
{
	if(fDCOutEn == 1)
	{
		wDCCtrCoeff = 8;
        sBiDirBatteryVoltCtr(wBatVolt);		
	    wBiDCCtrModu1 = swBiDirectionalDCCurrCtr1st(wBatteryCurrRef,wBatCurr1,wBatVolt);
	    
	    wDCCurrErrFiltPrev3rd = 0;
        dwDCCurrIngCtrl3rd = 0;
        dwDCModuSatErr3rd = 0;
        wBiDCCtrModu3 = 1;
	}
	else if(fDCOutEn == 2)
	{
		wDCCtrCoeff = 8;
		sBiDirBatteryVoltCtr(wBatVolt);
	    wBiDCCtrModu3 = swBiDirectionalDCCurrCtr3rd(wBatteryCurrRef,wBatCurr3,wBatVolt);
	    
	    wDCCurrErrFiltPrev1st = 0;
        dwDCCurrIngCtrl1st = 0;
        dwDCModuSatErr1st = 0;
        wBiDCCtrModu1 = 1;
	}
	else if(fDCOutEn == 3)
	{
		wDCCtrCoeff = 4;
		sBiDirBatteryVoltCtr(wBatVolt);
	    wBiDCCtrModu1 = swBiDirectionalDCCurrCtr1st(wBatteryCurrRef,wBatCurr1,wBatVolt);
	    wBiDCCtrModu3 = swBiDirectionalDCCurrCtr3rd(wBatteryCurrRef,wBatCurr3,wBatVolt);
	}
	else
	{
		wDCCtrCoeff = 4;
		sBiDirBatteryVoltCtr(wBatVolt);
		
		wDCCurrErrFiltPrev1st = 0;
        dwDCCurrIngCtrl1st = 0;
        dwDCModuSatErr1st = 0;
        
        wDCCurrErrFiltPrev3rd = 0;
        dwDCCurrIngCtrl3rd = 0;
        dwDCModuSatErr3rd = 0;
        
        fDCVoltCtrDeal = 0;
        
        //wBatteryChargeVoltErrFiltPrev = 0;
        //dwBatteryChargeVoltIngCtrl = 0;
        //dwBatteryChargeVoltSatErr = 0;
        //
        //wBatteryDischargeVoltErrFiltPrev = 0;
        //dwBatteryDischargeVoltIngCtrl = 0;
        //dwBatteryDischargeVoltSatErr = 0;
        
        wBiDCCtrModu1 = 1;
        wBiDCCtrModu3 = 1;
        //wBatteryLimitCurrHigh = cDCCurrMax;
        //wBatteryLimitCurrLow = cDCCurrMin;
        fCPLDShutDCDeal = 0;
        uwCPLDShutDCDelay = 0;
        uwDCCtrStartDelay = 0;
	}
}
/******************************************************************************************
Function name:Void sBiDirDCCon2And4Controller(INT16S wBatVolt,INT16S wPOVolt,INT16S wONVolt,INT16S wBatCurr2,INT16S wBatCurr4,INT16U fDCOutEn)
Description: Fourth BiDirection DC Converter Current closed-loop control algorithm 
Calls: ADISR        	 
Called By: 
Parameters:  INT16S
Return:  INT16S      
 *****************************************************************************************/
void sBiDirDCCon2And4Controller(INT16S wBatVolt,INT16S wPOVolt,INT16S wONVolt,INT16S wBatCurr2,INT16S wBatCurr4,INT16S wBatCurr,INT16U fDCOutEn)
{
static INT16U fDCOutEnBak = 1;
    if(fDCOutEn == 1)
    {
    	wDCCtrCoeff = 8;
    	
    	++uwDCCtrStartDelay;   	
    	if(uwDCCtrStartDelay >= 1)//3
    	{
    		uwDCCtrStartDelay = 1;//3
    		if(fDCOutEnBak == 0)
    	    {
    	    	wDCBusVoltErrFiltPrev = 0;
                dwDCBusVoltIngCtrl = (INT32S)wBatCurr<<11;//wBatCurr/2 * 4096
                dwDCBusVoltSatErr = 0;
                wBatteryCurrRef = wBatCurr>>1;
    	    }
    	    else if(fDCOutEnBak == 3)
    	    {
    	    	wDCBusVoltErrFiltPrev = 0;
                dwDCBusVoltIngCtrl = (INT32S)wBatteryCurrRef<<13;//wBatCurr/2 * 4096
                dwDCBusVoltSatErr = 0;
                wBatteryCurrRef = wBatteryCurrRef<<1;
    	    }
    	    else
    	    {   	
    	        wBatteryCurrRef = swBiDirDCVoltCtrl(wDCCtrBusVolRef,wPOVolt,wONVolt);
    	    }
    	}
    	else
    	{
    		wDCBusVoltErrFiltPrev = 0;
            dwDCBusVoltIngCtrl = 0;
            dwDCBusVoltSatErr = 0;
            wBatteryCurrRef = 0;
    	}
    	    	
    	wBiDCCtrModu2 = swBiDirectionalDCCurrCtr2nd(wBatteryCurrRef,wBatCurr2,wBatVolt);
    	
    	if(fDCOutEnBak == 3)
    	{
    		wDCCurrErrFiltPrev1st = wDCCurrErrFiltPrev2nd;
	    	dwDCCurrIngCtrl1st = dwDCCurrIngCtrl2nd;
	    	dwDCModuSatErr1st = dwDCModuSatErr2nd;
	    	wBiDCCtrModu1 = wBiDCCtrModu2;
    	}
                
        wDCCurrErrFiltPrev4th = 0;
        dwDCCurrIngCtrl4th = 0;
        dwDCModuSatErr4th = 0;
        wBiDCCtrModu4 = 1;
    }
    else if(fDCOutEn == 2)
    {
    	wDCCtrCoeff = 8;
    	
    	++uwDCCtrStartDelay;   	
    	if(uwDCCtrStartDelay >= 1)//3
    	{
    		uwDCCtrStartDelay = 1;//3
    	    if(fDCOutEnBak == 0)
    	    {
    	    	wDCBusVoltErrFiltPrev = 0;
                dwDCBusVoltIngCtrl = (INT32S)wBatCurr<<11;//wBatCurr/2 * 4096
                dwDCBusVoltSatErr = 0;
                wBatteryCurrRef = wBatCurr>>1;
    	    }
    	    else if(fDCOutEnBak == 3)
    	    {
    	    	wDCBusVoltErrFiltPrev = 0;
                dwDCBusVoltIngCtrl = (INT32S)wBatteryCurrRef<<13;//wBatCurr/2 * 4096
                dwDCBusVoltSatErr = 0;
                wBatteryCurrRef = wBatteryCurrRef<<1;
    	    }
    	    else
    	    {
    	    	wBatteryCurrRef = swBiDirDCVoltCtrl(wDCCtrBusVolRef,wPOVolt,wONVolt);
    	    } 
    	}
    	else
    	{
    		wDCBusVoltErrFiltPrev = 0;
            dwDCBusVoltIngCtrl = 0;
            dwDCBusVoltSatErr = 0;
            wBatteryCurrRef = 0;
    	}
    	    	
	    wBiDCCtrModu4 = swBiDirectionalDCCurrCtr4th(wBatteryCurrRef,wBatCurr4,wBatVolt);
	    
	    if(fDCOutEnBak == 3)
    	{
    		wDCCurrErrFiltPrev3rd = wDCCurrErrFiltPrev4th;
	    	dwDCCurrIngCtrl3rd = dwDCCurrIngCtrl4th;
	    	dwDCModuSatErr3rd = dwDCModuSatErr4th;
	    	wBiDCCtrModu3 = wBiDCCtrModu4;
    	}
	    
	    wDCCurrErrFiltPrev2nd = 0;
        dwDCCurrIngCtrl2nd = 0;
        dwDCModuSatErr2nd = 0;
        wBiDCCtrModu2 = 1;
    }
    else if(fDCOutEn == 3)
    {
    	wDCCtrCoeff = 4;
    	
    	++uwDCCtrStartDelay;   	
    	if(uwDCCtrStartDelay >= 1)//3
    	{
    		uwDCCtrStartDelay = 1;//3    		 	
    	    if(fDCOutEnBak == 0)
    	    {
    	    	wDCBusVoltErrFiltPrev = 0;
                dwDCBusVoltIngCtrl = (INT32S)wBatCurr<<10;//wBatCurr/4 * 4096
                dwDCBusVoltSatErr = 0;
                wBatteryCurrRef = wBatCurr>>2; 
    	    }
    	    else if((fDCOutEnBak == 1)||(fDCOutEnBak == 2))
    	    {
    	    	wDCBusVoltErrFiltPrev = 0;
                dwDCBusVoltIngCtrl = (INT32S)wBatteryCurrRef<<11;//wBatCurr/4 * 4096
                dwDCBusVoltSatErr = 0;
                wBatteryCurrRef = wBatteryCurrRef>>1; 
    	    }
    	    else
    	    {
	            wBatteryCurrRef = swBiDirDCVoltCtrl(wDCCtrBusVolRef,wPOVolt,wONVolt);
	        }
	    }
	    else
	    {
	    	wDCBusVoltErrFiltPrev = 0;
            dwDCBusVoltIngCtrl = 0;
            dwDCBusVoltSatErr = 0;
            wBatteryCurrRef = 0;
	    }
	    
	    wBiDCCtrModu2 = swBiDirectionalDCCurrCtr2nd(wBatteryCurrRef,wBatCurr2,wBatVolt);
	    wBiDCCtrModu4 = swBiDirectionalDCCurrCtr4th(wBatteryCurrRef,wBatCurr4,wBatVolt);
	    
	    if(fDCOutEnBak == 1)
	    {	    	
	    	wDCCurrErrFiltPrev1st = wDCCurrErrFiltPrev2nd;
	    	dwDCCurrIngCtrl1st = dwDCCurrIngCtrl2nd;
	    	dwDCModuSatErr1st = dwDCModuSatErr2nd;
	    	wBiDCCtrModu1 = wBiDCCtrModu2;
	    	
	    	wDCCurrErrFiltPrev3rd = wDCCurrErrFiltPrev2nd; 
	    	dwDCCurrIngCtrl3rd = dwDCCurrIngCtrl2nd;
	    	dwDCModuSatErr3rd = dwDCModuSatErr2nd;
	    	wBiDCCtrModu3 = wBiDCCtrModu2;
	    	
	    	wDCCurrErrFiltPrev4th = wDCCurrErrFiltPrev2nd; 
	    	dwDCCurrIngCtrl4th = dwDCCurrIngCtrl2nd;
	    	dwDCModuSatErr4th = dwDCModuSatErr2nd;
	    	wBiDCCtrModu4 = wBiDCCtrModu2;
	    }
	    else if(fDCOutEnBak == 2)
	    {
	    	wDCCurrErrFiltPrev1st = wDCCurrErrFiltPrev4th;
	    	dwDCCurrIngCtrl1st = dwDCCurrIngCtrl4th;
	    	dwDCModuSatErr1st = dwDCModuSatErr4th;
	    	wBiDCCtrModu1 = wBiDCCtrModu4;
	    	
	    	wDCCurrErrFiltPrev2nd = wDCCurrErrFiltPrev4th; 
	    	dwDCCurrIngCtrl2nd = dwDCCurrIngCtrl4th;
	    	dwDCModuSatErr2nd = dwDCModuSatErr4th;
	    	wBiDCCtrModu2 = wBiDCCtrModu4;
	    	
	    	wDCCurrErrFiltPrev3rd = wDCCurrErrFiltPrev4th; 
	    	dwDCCurrIngCtrl3rd = dwDCCurrIngCtrl4th;
	    	dwDCModuSatErr3rd = dwDCModuSatErr4th;
	    	wBiDCCtrModu3 = wBiDCCtrModu4;
	    }
    }
    else
    {
    	wDCCtrCoeff = 4;
    	
    	wDCCurrErrFiltPrev2nd = 0;
        dwDCCurrIngCtrl2nd = 0;
        dwDCModuSatErr2nd = 0;
        
        wDCCurrErrFiltPrev4th = 0;
        dwDCCurrIngCtrl4th = 0;
        dwDCModuSatErr4th = 0;
        
        wDCBusVoltErrFiltPrev = 0;
        dwDCBusVoltIngCtrl = 0;
        dwDCBusVoltSatErr = 0;
        
        wBiDCCtrModu2 = 1;
        wBiDCCtrModu4 = 1;
        wBatteryCurrRef = 0;
        fDCVoltCtrDeal = 0;
        fCPLDShutDCDeal = 0;
        uwCPLDShutDCDelay = 0;
        uwDCCtrStartDelay = 0;
    }
    fDCOutEnBak = fDCOutEn;
}
/*** end of file *****************************************************/


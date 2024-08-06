/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  CAP.c 
Description:  电网频率捕获测量 
Version :  V1.0
Author:Zhou Xu	 
Date:2009-11-03
*********************************************************************/
#include "..\FunSrc\Funheader.h"

INT32U udwRCapVal=0,udwSCapVal=0,udwTCapVal=0;
INT32U udwRCapValBak=0,udwSCapValBak=0,udwTCapValBak=0;
T_CAPEVENTFLAG tCapEvent,tLstPhase;
INT16U fFreDetect;
INT16U fCapIntEvent;
INT16U uwfPhaseSequencChkOk;
INT16U uwfSTExchange; 
INT16U uwfFreCalEnable; 

INT16U uwRSGridOffsetBak = 2048;
INT16U uwSTGridOffsetBak = 2048;
INT16U uwTRGridOffsetBak = 2048;

INT32U udwRSOffsetBak = 134217728;
INT32U udwSTOffsetBak = 134217728;
INT32U udwTROffsetBak = 134217728;

#define cCapCoeff  CPUFre/8          
#define fRCapEvent tCapEvent.RCap   
#define fSCapEvent tCapEvent.SCap      
#define fTCapEvent tCapEvent.TCap

#define cPrd18R86ms         22110//14740   //18.86ms  53HZ
#define cPrd21R27ms         24933//16622   //21.27ms  47HZ
#define cPrd15R87ms         18601//12400   //15.87ms  63HZ
#define cPrd17R54ms         20559//13706   //17.54ms  57HZ

#define cFreq60Hz        15360   //60*16
#define cFreq50Hz        12800   //50*16

#define cOffsetDen      30772 
#define cOffsetNum      1996

#pragma CODE_SECTION(CAPIsr, "CAPRamFuncs")
#pragma CODE_SECTION(sCapDeal, "CAPRamFuncs")
#pragma CODE_SECTION(sCapIsrDeal, "CAPRamFuncs")
#pragma CODE_SECTION(sRCapDeal, "CAPRamFuncs")
#pragma CODE_SECTION(sSCapDeal, "CAPRamFuncs")
#pragma CODE_SECTION(sTCapDeal, "CAPRamFuncs")
#pragma CODE_SECTION(suwGetMinGridFreq, "CAPRamFuncs")


void InitCapVar(void)
{
    udwRCapVal = 0;
    udwSCapVal = 0;
    udwTCapVal = 0;
    udwRCapValBak = 0;
    udwSCapValBak = 0;
    udwTCapValBak = 0;
    fRCapEvent = 0;
    fSCapEvent = 0;
    fTCapEvent = 0;
    fRLstPhase = 0;
    fSLstPhase = 0;
    fTLstPhase = 0;
    fCapIntEvent = 0;
    fFreDetect = 0;
    uwfPhaseSequencChkOk = 0;
    uwfSTExchange = 0;
    uwfFreCalEnable = 1;

}

/**********************************************************************
* Function: InitECap()
*
* Description: Initializes the Enhanced Capture modules on the F280x.
**********************************************************************/
void InitECap(void)
{


/************************************************************/
/*** Configure eCAP1 unit for capture                     ***/
/************************************************************/

	ECap1Regs.ECEINT.all = 0;					// Disable all eCAP interrupts
	ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
	ECap1Regs.ECCTL1.bit.CAPLDEN = 0;			// Disabled loading of capture results
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;			// Stop the counter

	ECap1Regs.TSCTR = 0;						// Clear the counter
	ECap1Regs.CTRPHS = 0;						// Clear the counter phase register

	ECap1Regs.ECCTL2.all = 0x0098;				// ECAP control register 2
    /*
    bit 15-11     00000:  reserved
    bit 10        0:      APWMPOL, don't care
    bit 9         0:      CAP/APWM, 0 = capture mode, 1 = APWM mode
    bit 8         0:      SWSYNC, 0 = no action (no s/w synch)
    bit 7-6       10:     SYNCO_SEL, 10 = disable sync out signal
    bit 5         0:      SYNCI_EN, 0 = disable Sync-In
    bit 4         1:      TSCTRSTOP, 1 = enable counter
    bit 3         1:      RE-ARM, 0 = don't re-arm, 1 = re-arm
    bit 2-1       00:     STOP_WRAP, 11 = wrap after 4 captures
    bit 0         0:      CONT/ONESHT, 0 = continuous mode
    */

	ECap1Regs.ECCTL1.all = 0xC144;				// ECAP control register 1
    /*
     bit 15-14     11:     FREE/SOFT, 11 = ignore emulation suspend
     bit 13-9      00001:  CPU Fre / 2
     bit 8         1:      CAPLDEN, 1 = enable capture results load
     bit 7         0:      CTRRST4, 0 = do not reset counter on CAP4 event
     bit 6         1:      CAP4POL, 0 = rising edge, 1 = falling edge
     bit 5         0:      CTRRST3, 0 = do not reset counter on CAP3 event
     bit 4         0:      CAP3POL, 0 = rising edge, 1 = falling edge
     bit 3         0:      CTRRST2, 0 = do not reset counter on CAP2 event
     bit 2         1:      CAP2POL, 0 = rising edge, 1 = falling edge
     bit 1         0:      CTRRST1, 0 = do not reset counter on CAP1 event
     bit 0         0:      CAP1POL, 0 = rising edge, 1 = falling edge
    */

	ECap1Regs.ECEINT.all = 0x0002;				// Enable desired eCAP interrupts
    /*
    bit 15-8      0's:    reserved
    bit 7         0:      CTR=CMP, 0 = compare interrupt disabled
    bit 6         0:      CTR=PRD, 0 = period interrupt disabled
    bit 5         0:      CTROVF, 0 = overflow interrupt disabled
    bit 4         0:      CEVT4, 0 = event 4 interrupt disabled
    bit 3         0:      CEVT3, 0 = event 3 interrupt disabled
    bit 2         0:      CEVT2, 0 = event 2 interrupt disabled
    bit 1         1:      CEVT1, 1 = event 1 interrupt enabled
    bit 0         0:      reserved
    */

    ECap2Regs.ECEINT.all = 0;					// Disable all eCAP interrupts
	ECap2Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
	ECap2Regs.ECCTL1.bit.CAPLDEN = 0;			// Disabled loading of capture results
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;			// Stop the counter

	ECap2Regs.TSCTR = 0;						// Clear the counter
	ECap2Regs.CTRPHS = 0;						// Clear the counter phase register

	ECap2Regs.ECCTL2.all = 0x0098;				// ECAP control register 2
	ECap2Regs.ECCTL1.all = 0xC144;
    ECap2Regs.ECEINT.all = 0x0000;

	ECap3Regs.ECEINT.all = 0;					// Disable all eCAP interrupts
	ECap3Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
	ECap3Regs.ECCTL1.bit.CAPLDEN = 0;			// Disabled loading of capture results
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;			// Stop the counter
        
	ECap3Regs.TSCTR = 0;						// Clear the counter
	ECap3Regs.CTRPHS = 0;						// Clear the counter phase register
        
	ECap3Regs.ECCTL2.all = 0x0098;				// ECAP control register 2
	ECap3Regs.ECCTL1.all = 0xC144;
    ECap3Regs.ECEINT.all = 0x0000;

	PieCtrlRegs.PIEIER4.bit.INTx1 = 1;	// Enable ECAP1_INT in PIE group 4
	IER |= M_INT4;						// Enable INT4 in IER to enable PIE group 4

} // end InitECap()

/*********************************************************************
Function name: void sfPhaseSequJudge(INT16S swLineVoltSAtRScz,INT16S swLineVoltTAtRScz)
Description: Phase sequence check  
Calls:          
Called By:   in Power On Mode
Parameters: INT16S swLineVoltSAtRcz,INT16S swLineVoltTAtRcz.  means RS line and ST line voltage when RS line voltage cross zero
Return:      void    
 *********************************************************************/  
INT16U sfPhaseSequJudge(INT16S swLineVoltSAtRScz,INT16S swLineVoltTAtRScz)
{
static INT16U uwPostSeqCnt = 0;
static INT16U uwNegtSeqCnt = 0; 

    if(uwfPhaseSequencChkOk == 0)
    {
    	uwPostSeqCnt = 0;
    	uwNegtSeqCnt = 0; 
    	uwfPhaseSequencChkOk = 1;
    }
    else if(uwfPhaseSequencChkOk == 1)
    {
        if (swLineVoltSAtRScz > swLineVoltTAtRScz)  // When Negative Voltage Across Zero,Capturing Rise Edge Pulse
        {
            uwPostSeqCnt++;
            uwNegtSeqCnt = 0;
            if (uwPostSeqCnt > 10) //10 times
            { 
                uwPostSeqCnt = 0;
                uwfSTExchange = 0;//don't exchange S phase and T phase
                uwfPhaseSequencChkOk = 2;
            }
        }
        else if(swLineVoltTAtRScz > swLineVoltSAtRScz)   
        {
            uwNegtSeqCnt++;
            uwPostSeqCnt = 0;
            if (uwNegtSeqCnt > 10) //10 times
            { 
                uwNegtSeqCnt = 0;
                uwfSTExchange = 1; //exchange S phase and T phase
                uwfPhaseSequencChkOk = 2;
            }
        }
        else
        {
            uwNegtSeqCnt = 0;
            uwPostSeqCnt = 0;
        }     
    }
    if(uwfPhaseSequencChkOk <= 1) return(0);
    else
    {	 
        uwPostSeqCnt = 0;
        uwNegtSeqCnt = 0;
        return(uwfSTExchange+1);
	}
}
/*********************************************************************
Function name:  void sCapIsrDeal(void)
Description:  捕获中断内需处理的函数
Calls:        	 
Called By:  
Parameters:  void
Return:  void    
*********************************************************************/
void sCapIsrDeal(void)
{
static INT16U uwCapDelay = 0;
    if((fPhaseSequ != 0)&&(fCSVSSwitch == 0))
    {
        if((fRLstPhase == 0) && (fSLstPhase == 0) && (fTLstPhase == 0) && (fLVRTValue > uwLVRTActiveValue)&&(fHVRTValue < uwHVRTActiveValue))//14730
        { 
            if(++uwCapDelay>52)
            {   
                uwRSGridOffsetBak = suwGetAvg(&udwRSGridVolOffsetSum,&uwRSGridVolOffsetSumCnt,&fRSGridVolOffsetSumClear);
                uwSTGridOffsetBak = suwGetAvg(&udwSTGridVolOffsetSum,&uwSTGridVolOffsetSumCnt,&fSTGridVolOffsetSumClear);
                uwTRGridOffsetBak = suwGetAvg(&udwTRGridVolOffsetSum,&uwTRGridVolOffsetSumCnt,&fTRGridVolOffsetSumClear);
                
                udwRSOffsetBak = ((INT64U)udwRSOffsetBak*cOffsetDen + ((INT64U)uwRSGridOffsetBak<<16)*cOffsetNum)>>15;
                if(udwRSOffsetBak > 140902400)
                {
                    udwRSOffsetBak = 140902400;
                }
                else if(udwRSOffsetBak < 127533056)
                {
                    udwRSOffsetBak = 127533056;
                }
                uwRSGridVolOffset = udwRSOffsetBak>>16;
                
                udwSTOffsetBak = ((INT64U)udwSTOffsetBak*cOffsetDen + ((INT64U)uwSTGridOffsetBak<<16)*cOffsetNum)>>15;
                if(udwSTOffsetBak > 140902400)
                {
                    udwSTOffsetBak = 140902400;
                }
                else if(udwSTOffsetBak < 127533056)
                {
                    udwSTOffsetBak = 127533056;
                }
                uwSTGridVolOffset = udwSTOffsetBak>>16;
                
                udwTROffsetBak = ((INT64U)udwTROffsetBak*cOffsetDen + ((INT64U)uwTRGridOffsetBak<<16)*cOffsetNum)>>15;
                if(udwTROffsetBak > 140902400)
                {
                    udwTROffsetBak = 140902400;
                }
                else if(udwTROffsetBak < 127533056)
                {
                    udwTROffsetBak = 127533056;
                }
                uwTRGridVolOffset = udwTROffsetBak>>16;
                uwCapDelay = 52;
            } 
            else
            {
                fRSGridVolOffsetSumClear = 1;
                fRSGridVolOffsetSumClear = 1;
                fRSGridVolOffsetSumClear = 1;
            }
        }
        else
        {
            uwCapDelay = 0;
        }
    }
    else
    {
        uwCapDelay = 0;
        if(fStartOrNormal == 1)
        {
            udwRSGridVolFiltoffset = (INT32U)wEepromRSGridVolOffset<<16;
            udwSTGridVolFiltoffset = (INT32U)wEepromSTGridVolOffset<<16;
            udwTRGridVolFiltoffset = (INT32U)wEepromTRGridVolOffset<<16;
            
            udwRSOffsetBak = (INT32U)wEepromRSGridVolOffset<<16;
            udwSTOffsetBak = (INT32U)wEepromSTGridVolOffset<<16;
            udwTROffsetBak = (INT32U)wEepromTRGridVolOffset<<16;
        }
    }
    sDciControl(fInvEnOut); //DCI抑制
    
    if(!CFCICALCFLAG)wGFCIAvgCurr = swGetAvg(&dwGFCIAvgCurrSum,&uwGFCIAvgCurrSumCnt,&fGFCIAvgCurrSumClear);    
    sCalRmsValue(); //计算有效值    
    
    fCapIntEvent = 1;
}
    
/*********************************************************************
Function name:  void CAPIsr(void)
Description:  电网RS线电压周期捕获中断
Calls:        	 
Called By:  
Parameters:  void
Return:  void    
*********************************************************************/
void CAPIsr(void)
{
INT32U udwDeltaRCapVal,udwDeltaSCapVal,udwDeltaTCapVal;

	udwRCapVal = (INT32U)ECap1Regs.CAP1;
	udwSCapVal = (INT32U)ECap2Regs.CAP1;
    udwTCapVal = (INT32U)ECap3Regs.CAP1;
    
    if(ECap1Regs.ECFLG.bit.CEVT1 == 1) fRCapEvent = 1;
    if(ECap2Regs.ECFLG.bit.CEVT1 == 1) fSCapEvent = 1;
    if(ECap3Regs.ECFLG.bit.CEVT1 == 1) fTCapEvent = 1;
    	
    udwDeltaRCapVal = (labs((INT32S)(udwRCapVal - udwRCapValBak)))>>7;
    udwDeltaSCapVal = (labs((INT32S)(udwSCapVal - udwSCapValBak)))>>7;
    udwDeltaTCapVal = (labs((INT32S)(udwTCapVal - udwTCapValBak)))>>7;
    
    if(fFreDetect == 1)
    {
    	if(uwSysFreq == cFreq50Hz)
    	{
    	static INT16U uwCapCrossZeroFiltNum = 0;
    		if((udwDeltaRCapVal > cPrd18R86ms) && (udwDeltaSCapVal > cPrd18R86ms) && (udwDeltaTCapVal > cPrd18R86ms) && 
               (udwDeltaRCapVal < cPrd21R27ms) && (udwDeltaSCapVal < cPrd21R27ms) && (udwDeltaTCapVal < cPrd21R27ms))
            {
            	sCapIsrDeal();
            	uwCapCrossZeroFiltNum = 0;
            	uwfFreCalEnable = 1;
            }
            else
            {
            	if(++uwCapCrossZeroFiltNum > 5)
            	{
            		uwCapCrossZeroFiltNum = 5;
            		sCapIsrDeal();
            		uwfFreCalEnable = 1;
            	}
            	else 
            	{
            		uwfFreCalEnable = 0;
            	}
            }
    	}
    	else
    	{
    	static INT16U uwCapCrossZeroFiltNum = 0;
    		if((udwDeltaRCapVal > cPrd15R87ms) && (udwDeltaSCapVal > cPrd15R87ms) && (udwDeltaTCapVal > cPrd15R87ms) && 
               (udwDeltaRCapVal < cPrd17R54ms) && (udwDeltaSCapVal < cPrd17R54ms) && (udwDeltaTCapVal < cPrd17R54ms))
            {
            	sCapIsrDeal();
            	uwCapCrossZeroFiltNum = 0;
            	uwfFreCalEnable = 1;
            }
            else
            {
            	if(++uwCapCrossZeroFiltNum > 5)
            	{
            	    uwCapCrossZeroFiltNum = 5;
            	    sCapIsrDeal();	
            	    uwfFreCalEnable = 1;
            	}
            	else
            	{
            		uwfFreCalEnable = 0;
            	}
            }
    	}
    }
    else
    {
    	sCapIsrDeal();
    	uwfFreCalEnable = 1;
    }
     
    wSTGridVolBak = wSTGridVol;  //相序检测使用
    wTRGridVolBak = wTRGridVol;
    
	ECap1Regs.ECCLR.bit.CEVT1 = 1;			// Clear the CEVT1 flag
	ECap2Regs.ECCLR.bit.CEVT1 = 1;
	ECap3Regs.ECCLR.bit.CEVT1 = 1;

	ECap1Regs.ECCLR.bit.INT = 1;			// Clear the ECAP1 interrupt flag
    ECap1Regs.ECCTL2.bit.REARM = 1;

}//end CAPIsr()

/*********************************************************************
Function name:  void sRCapDeal(INT16U *pFre,INT16U *pLstPhaseFlag)
Description:  电网RS线电压频率计算和掉线判断
Calls:        	 
Called By:  sCapDeal(void)
Parameters:  INT16U *pFre,INT16U *pLstPhaseFlag
Return:  void    
*********************************************************************/
void sRCapDeal(INT16U *pFre,INT16U *pLstPhaseFlag)
{
static INT16U uwCapOverTimerCnt=0;
    if((++uwCapOverTimerCnt) > 12) //捕获超时
    {
        uwCapOverTimerCnt = 0;
        *pFre = 0; 
        *pLstPhaseFlag = 1; 
        
        if(fRCapEvent == 1)
        {
        	fRCapEvent = 0;
        	udwRCapValBak = udwRCapVal;       	
        }   
    }
    else
    {    	    	     
    	if(fRCapEvent == 1)
    	{
    	INT32U i = 0;   
    	    *pLstPhaseFlag = 0;  	
    		uwCapOverTimerCnt = 0;
    		fRCapEvent = 0;
    		if(udwRCapVal > udwRCapValBak)
				i = udwRCapVal - udwRCapValBak;
    		else
				i = 0x100000000 - udwRCapValBak + udwRCapVal;  
			
    		if(i < 1500000)   // CPUFre/100Hz= 1000000
				i = 1500000;
		    else if(i > 30000000)  //CPUFre/5Hz=20000000
				i = 30000000;
			i = i>>5;  // for Q4
            if(uwfFreCalEnable == 1)
            {
			    *pFre = FourCPUFre/i; // frequence=[5Hz~100Hz], Q8
            }
            udwRCapValBak = udwRCapVal;   		
    	}
    }	
}
/*********************************************************************
Function name:  void sSCapDeal(INT16U *pFre,INT16U *pLstPhaseFlag)
Description:  电网ST线电压频率计算和掉线判断
Calls:        	 
Called By:  sCapDeal(void)
Parameters:  INT16U *pFre,INT16U *pLstPhaseFlag
Return:  void    
*********************************************************************/
void sSCapDeal(INT16U *pFre,INT16U *pLstPhaseFlag)
{
static INT16U uwCapOverTimerCnt=0;
    if((++uwCapOverTimerCnt) > 12) //捕获超时
    {
        uwCapOverTimerCnt = 0;
        *pFre = 0; 
        *pLstPhaseFlag = 1; 
        
        if(fSCapEvent == 1)
        {
        	fSCapEvent = 0;
        	udwSCapValBak = udwSCapVal;       	
        }   
    }
    else
    {
    	if(fSCapEvent == 1)
    	{
    	INT32U i = 0;    
    	    *pLstPhaseFlag = 0; 	
    		uwCapOverTimerCnt = 0;
    		fSCapEvent = 0;
    		if(udwSCapVal > udwSCapValBak) i = udwSCapVal - udwSCapValBak;
    		else i = 0x100000000 - udwSCapValBak + udwSCapVal;    		
    		if(i < 1500000)   // CPUFre/100Hz= 1000000
				i = 1500000;
		    else if(i > 30000000)  //CPUFre/5Hz=20000000
				i = 30000000;
			i = i>>5;  // for Q4
            if(uwfFreCalEnable == 1)
            {
		    	*pFre = FourCPUFre/i; // frequence=[5Hz~100Hz], 
            }
            udwSCapValBak = udwSCapVal;   		
    	}
    }	
}
/*********************************************************************
Function name:  void sTCapDeal(INT16U *pFre,INT16U *pLstPhaseFlag)
Description:  电网TR线电压频率计算和掉线判断
Calls:        	 
Called By:  sCapDeal(void)
Parameters:  INT16U *pFre,INT16U *pLstPhaseFlag
Return:  void    
*********************************************************************/
void sTCapDeal(INT16U *pFre,INT16U *pLstPhaseFlag)
{
static INT16U uwCapOverTimerCnt=0;
    if((++uwCapOverTimerCnt) > 12) //捕获超时
    {
        uwCapOverTimerCnt = 0;
        *pFre = 0; 
        *pLstPhaseFlag = 1; 
        
        if(fTCapEvent == 1)
        {
        	fTCapEvent = 0;
        	udwTCapValBak = udwTCapVal;       	
        }   
    }
    else
    {
    	if(fTCapEvent == 1)
    	{
    	INT32U i = 0;   
    	    *pLstPhaseFlag = 0;  	
    		uwCapOverTimerCnt = 0;
    		fTCapEvent = 0;
    		if(udwTCapVal > udwTCapValBak) i = udwTCapVal - udwTCapValBak;
    		else i = 0x100000000 - udwTCapValBak + udwTCapVal;    		
    		if(i < 1500000)   // CPUFre/100Hz= 1000000
				i = 1500000;
		    else if(i > 30000000)  //CPUFre/5Hz=20000000
				i = 30000000;
			i = i>>5;  // for Q4
            if(uwfFreCalEnable == 1)
            {
		    	*pFre = FourCPUFre/i; // frequence=[5Hz~100Hz], 
            }
            udwTCapValBak = udwTCapVal;   		
    	}
    }	
}
/*********************************************************************
Function name:  suwGetMinGridFreq();
Description:  电网线电压频率最大值
Calls:       	 
Called By:  Invcontrol()
Parameters:  void
Return:  INT16U    
*********************************************************************/
INT16U suwGetMinGridFreq()
{
INT16U uwswapMin;
    uwswapMin = uwRFreq;
    if (uwSFreq <= uwswapMin)
    {
        uwswapMin = uwSFreq;
    }
    if (uwTFreq <= uwswapMin)
    {
        uwswapMin = uwTFreq;
    }
    return(uwswapMin);
}
/*********************************************************************
Function name:  suwGetMaxGridFreq();
Description:  电网线电压频率最大值
Calls:       	 
Called By:  Invcontrol()
Parameters:  void
Return:  INT16U    
*********************************************************************/
INT16U suwGetMaxGridFreq()
{
INT16U uwswapMax;
    uwswapMax = uwRFreq;
    if (uwSFreq >= uwswapMax)
    {
        uwswapMax = uwSFreq;
    }
    if (uwTFreq >= uwswapMax)
    {
        uwswapMax = uwTFreq;
    }
    return(uwswapMax);
}
/*********************************************************************
Function name:  suwGetMidGridFreq();
Description:  电网线电压频率最大值
Calls:       	 
Called By:  Invcontrol()
Parameters:  void
Return:  INT16U    
*********************************************************************/
INT16U suwGetMidGridFreq()   //tiger zhang 091228 
{
INT16U uwswapMid;
        
    if(((uwSFreq > uwRFreq)&&(uwSFreq < uwTFreq))||((uwSFreq < uwRFreq)&&(uwSFreq > uwTFreq)))
    {
    	uwswapMid = uwSFreq;
    }
    else if(((uwTFreq > uwRFreq)&&(uwTFreq < uwSFreq))||((uwTFreq < uwRFreq)&&(uwTFreq > uwSFreq)))
    {
    	uwswapMid = uwTFreq;
    }
    else
    {
    	uwswapMid = uwRFreq;
    }
    return(uwswapMid);
}
/*********************************************************************
Function name:  sCapDeal(void)
Description:  电网线电压频率计算、有效值计算
Calls:  sRCapDeal(&uwRFre,&fRLstPhase),sSCapDeal(&uwRFre,&fRLstPhase),sTCapDeal(&uwRFre,&fRLstPhase)      	 
Called By:  sCapDeal(void)
Parameters:  void
Return:  void    
*********************************************************************/
void sCapDeal(void) //5ms
{
    sRCapDeal(&uwRFreq,&fRLstPhase);
    sSCapDeal(&uwSFreq,&fSLstPhase);
    sTCapDeal(&uwTFreq,&fTLstPhase);

    uwRFre = uwRFreq>>4;
    uwSFre = uwSFreq>>4;
    uwTFre = uwTFreq>>4;
	
    if((fRLstPhase == 1) || (fSLstPhase == 1) || (fTLstPhase == 1)) //捕获超时，此时需要令有效值为零 
    {
    	sRmsClearZero(); //有效值清零     
        sInvShortPowerClearZero();     
    }
}


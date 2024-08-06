/*********************************************************************
Copyright 2009, Chint Power System Ltd. Co.
File Name:  MAIN.c 
Description:  MAIN主函数 
Version :  V1.0
Author:Zhou Xu	 
Date:2009-11-03
*********************************************************************/
#include "..\FunSrc\Funheader.h"

#define cSqrt2                 23170   //Q14 
#define cVolt5V                80
#define cVolt10V               160
#define cVolt20V               320
#define cFreq50Hz              12800
#define cFreq60Hz              15360



#pragma DATA_SECTION(cFlashCheckHead,"FlashHead");
const INT16S cFlashCheckHead[] = {0x1a0a,0xe5f5};
#pragma DATA_SECTION(cFlashCheckTrail,"FlashTrail");
const INT16S cFlashCheckTrail[] = {0x1a0a,0xe5f5};

/*********************************************************************
Function name:  void InitVar(void)
Description:  变量初始化
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void InitVar(void)
{
    InitTimerVar();
    InitAdcVar();
	InitECanVar();
    InitCapVar();
    InitI2cVar();
    sPLLParamInit();
    sInvParamInit();
    sBiDirDCCtrParamInit();
    InitPWMVar();
    InitSysCtrVar();
    InitProtectVar();
	//sMPPTParamInit();
    sRestartVarInit();
    {
    INT16U j=0;    
	    for(j=0;j<sizeof(tSendData.uwData0);j++)
	    {
            tSendData.uwData0[j] = 0;
            tSendData.uwData1[j] = 0;
            tSendData.uwData2[j] = 0;
	    }
    }
}
/*********************************************************************
Function name:  suwFlashCheck(Uint16 *pPtr_Head,Uint16 *pPtr_Trail)
Description:  flash check
Calls:        	 
Called By:  main
Parameters:  INT16U *pPtr_Head,INT16U *pPtr_Trail
Return:  INT16U      
 *********************************************************************/
INT16U suwFlashCheck(INT16U *pPtr_Head,INT16U *pPtr_Trail)  //=1 App Ok
{
INT16U fFlashStatus;
    if((*pPtr_Head == 0x1a0a) && (*(++pPtr_Head) == 0xe5f5) &&
       (*pPtr_Trail == 0x1a0a) && (*(++pPtr_Trail) == 0xe5f5))
    {
        fFlashStatus = 1;
    }
	else 
    {
        fFlashStatus = 0;
    }

	return(fFlashStatus);
}
/********************************************************************* 
Function name:  void sBootLoaderSchedule(void)                                        
Description:  Bootloader引导时序程序，5ms执行一次                                               
Calls:        	                                                       
Called By:  main主程序,sSysMode,                                                           
Parameters:  void                                                      
Return:  void                                                          
 *********************************************************************/
void sBootLoaderSchedule(void)
{
    if(uwBootloaderStep == 0)
    {
        if(fEepromRdWrHalt == 1) uwBootloaderStep = 1;  //停止正常Eeprom读写
    }
    else if(uwBootloaderStep == 1)       
    {
    static INT16U uwDelayCnt = 0;
        if(++uwDelayCnt > 400) //等待2s，保证串口数据发送完毕
        {
            uwDelayCnt = 400;
            sChipReset();
        }
    }     
}

/*********************************************************************
Function name:  void sSysMode(void)
Description:  
Calls:        	 
Called By:  main(void)
Parameters:  void
Return:  void      
 *********************************************************************/
void sSysMode(void)
{
	switch(uwSysMode)
	{
	static INT16U uwDelay400ms = 0;
	static INT16U fCSVSSwitchBak = 0;
	
		case PowerOnMode:
		    {
			static INT16U uwDelay1s = 0;
                if(fRunOrder == CLEAR)  //无运行命令
			    {
			        sRestartVarInit();
			        sGridRelayOpen();
			        sInvRelayOpen();
			        sPERelayOpen();
			        sAuxiRelayOpen();
			        sIsoRelayOpen();
			        sDCSoftRelayOpen();
			        sDisDCSps();
			        sDisInvSps();		    
			    }
			    else   
			    {
			    	uwDelay400ms = 0;
			    	
			    	if(udwPowerOnWaitTime == 0)
			    	{   
			    	    if((uwAuxiRelayDelay == 0)&&(uwDCSoftRelayDelay == 0))
			    	    {
			    	        if(fBatteryInitiStateOK <= 1) 
			    	        {
                                sBatteryInitiStateJudge(wBatteryOutAvgVol);
			    	        }
			    	        else 
			    	        {
			    	            if(fPhaseSequ == CLEAR) //=0，检测没成功，=1相序正常，=2相序相反
			    	            {
			    	                if(fCSVSSwitch == 1)
			    	                {
			    	                    if((wRSGridRmsVol > 3200)&&(wSTGridRmsVol > 3200)&&(wTRGridRmsVol > 3200))
			    	                    {
			    	                        fVSRunByGrid = 1;
			    	                        fPhaseSequ = sfPhaseSequJudge(wSTGridVolBak,wTRGridVolBak);
			    	                        if(fPhaseSequ != CLEAR)
			    	                        {
			    	                        	fPhaseSequBak = fPhaseSequ;
			    	                        	sPLLParamInit();
			    	                        }
			    	                    }
			    	                    else
			    	                    {
			    	                        if(fVSRunByGrid == 1)
			    	                        {
			    	                            sRestartVarInit();
			    	        	    			sGridRelayOpen();
			                                    sInvRelayOpen();
			                                    sPERelayOpen();
			                                    sAuxiRelayOpen();
			                                    sIsoRelayOpen();
			    	    		    			sDCSoftRelayOpen();
			    	                        }
			    	                        else
			    	                        {
			    	                            fPhaseSequ = fPhaseSequBak;
			    	                        	sPLLParamInit();
			    	                        }
			    	                    }
			    	                }
			    	                else
			    	                {
			    	                    fPhaseSequ = sfPhaseSequJudge(wSTGridVolBak,wTRGridVolBak);
			    	                    if(fPhaseSequ != CLEAR)
			    	                    {
			    	                    	fPhaseSequBak = fPhaseSequ;
			    	                    	sPLLParamInit();
			    	                    }
			    	                }
			    	            }
			    	            else
			    	            {
			    	                if(fIsoStatue == 0)
			    	                {
			    	                    if((wEepromISOEnable == 1)&&(fBatteryInitiStateOK == 2)&&(fATSForceStop == 0))
			    	        		    {
			    	        		    	wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
			    	        		    	
			    	        		    	if((fBatteryReady == 1)&&(wBatteryOutAvgVol >= wBatVolLimtLow)&&(fFactoryFastStart == 0))
			    	        		    	{
			    	        		    	    fIsoStatue = sfIsoChk(&fIsoLowErr,&fIsoLowErrEvent);
			    	        		    	}
			    	        		    	else
			    	        		    	{
			    	        		    		sRestartVarInit();
			    	        		    		sGridRelayOpen();
			                                    sInvRelayOpen();
			                                    sPERelayOpen();
			                                    sAuxiRelayOpen();
			                                    sIsoRelayOpen();
			    	    			    		sDCSoftRelayOpen();
			    	        		    	}
			    	        		    }
			    	        		    else
			    	        		    {
			    	        		    	fIsoStatue = 1;
			    	        		    	fATSForceStop = 0;
			    	        		    }
			    	                }
			    	                else
			    	        	    {
			    	        	    	if(fGFCICTStatue == 0)
                                        {
                                        	if((wEepromGFCICTChkEn == 1)&&(fFactoryFastStart == 0))
                                        	{
                                        		fGFCICTStatue = sfGFCICTChk(wGFCIAvgCurr,&fGFCICTErr,&fGFCICTErrEvent);
                                        	}
                                        	else
                                        	{
                                        		fGFCICTStatue = 1;
                                        	}
                                        }
			    	        	    }
			    	            }
			    	        }			    	       
			    	    }
			    	}
			    	else
			    	{
			    	    if(fFactoryFastStart == 1)
			            {
			                udwPowerOnWaitTime = 0;
			            }
			            else
			            {
			                if(fGridPretect == 1)
			                {
			                    fGridPretect = 0;
			                    udwPowerOnWaitTime = (INT32U)wEepromPowerOnWaitTime * 100;
			                }
			                else
			                {
			                    udwPowerOnWaitTime--;
			                }
			            }
			    	}    
                     
			        if((fBatteryInitiStateOK >= 2)&&(fPhaseSequ != 0)&&(fIsoStatue == 1)&&(fGFCICTStatue == 1))
			        {
			            uwDelay1s++;
			            if(uwDelay1s > 100)
			            {	
			                uwDelay1s = 0;
			                wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
			                if((fBatteryReady == 1)&&(wBatteryOutAvgVol >= wBatVolLimtLow))//DC Relay close
			                {
			                	fStartByDCOrAC = 0;
			                	fCSVSSwitchBak = fCSVSSwitch;
			                	uwSysModePrepare = PreCheckMode;
			                }
			                else if((fBattDarkStart == 1)&&(fCSVSSwitch == 0))
			                {
			                	fStartByDCOrAC = 1;
			                	fCSVSSwitchBak = fCSVSSwitch;
			                	uwSysModePrepare = PreCheckMode;
			                }
			                else
			                {
			                	sRestartVarInit();
			        	    	sGridRelayOpen();
			                    sInvRelayOpen();
			                    sPERelayOpen();
			                    sAuxiRelayOpen();
			                    sIsoRelayOpen();
			        			sDCSoftRelayOpen();
			                }
                        }
                    }
			        else
			        {
			            uwDelay1s = 0;
			        }
			    } 
			}              
		    sErrFaultDeal();
		    break;

        case PreCheckMode:    
			if(fRunOrder == CLEAR)  //无运行命令
	        {
	            sRestartVarInit();
	            sDCOCPDisable();
	            sGridRelayOpen();
			    sInvRelayOpen();
			    sPERelayOpen();
			    sAuxiRelayOpen();
			    sIsoRelayOpen();
			    sDCSoftRelayOpen();
	            uwSysModePrepare = PowerOnMode;
	        }
	        else
	        {	            
                sRelayPowerLPSOn();
                uwDelay400ms = 0;
                if(fStartByDCOrAC == 0)
                {   
                	fBattDarkStart = 0;                 	
                	if(fDCCtrBusSoftSta == 0)
                	{
                	    fDCCtrBusSoftSta = sfBiDirDCCtrSoftStart(wBatteryInAvgVol,wBatteryOutAvgVol);
                	}
                	else
                	{
                	    if((fCSVSSwitch == 0)&&(fCSVSSwitchBak == 0))
                	    {
                		    wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
                            wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
                		    wDCCtrBusVolRef = wStdBusVolRef + cVolt20V;
                		    
                		    if(fRelayChkRequire == 0)
                            {
                                fRelayChkRequire = sfRelayChk(&fGridRelayErr,&fGridRelayErrEvent,&fInvRelayErr,&fInvRelayErrEvent);
                            }
                            else
                            {
                            	if(fRunInvAhdChkRequire == 0)
                            	{
                            		fRunInvAhdChkRequire = sfRunInvAhdChk();
                            	}
                            }
                        }
                        else if((fCSVSSwitch == 1)&&(fCSVSSwitchBak == 1))
                        {
                            wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
                            wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
                		    wDCCtrBusVolRef = wStdBusVolRef + cVolt20V;
                		    
                            if(fRelayChkRequire == 0)
                            {
                                fRelayChkRequire = sfRelayChk(&fGridRelayErr,&fGridRelayErrEvent,&fInvRelayErr,&fInvRelayErrEvent);
                            }
                            else
                            {
                                if(fRunInvAhdChkRequire == 0)
                            	{
                            		fRunInvAhdChkRequire = sfRunInvAhdChkByVS();
                            	}
                            }
                        }
                        else
                        {
                            sRestartVarInit();
                		    sDCOCPDisable();
	                        sGridRelayOpen();
			                sInvRelayOpen();
			                sPERelayOpen();
			                sAuxiRelayOpen();
			                sIsoRelayOpen();
			                sDCSoftRelayOpen();
	                        uwSysModePrepare = PowerOnMode;
                        }         		
                	}
                }
                else
                {
                	if(fBattDarkStart == 1)
                	{
                	    if(fRelayChkRequire == 0)
                        {
                            fRelayChkRequire = sfRelayChk(&fGridRelayErr,&fGridRelayErrEvent,&fInvRelayErr,&fInvRelayErrEvent);
                        }
                	    else
                	    {
                	    	if(fRunInvAhdChkRequire == 0)
                            {
                            	fRunInvAhdChkRequire = sfRunInvAhdChkByAC();
                            }
                            else
                            {
                            	if(fDCCtrBusSoftSta == 0)
                	            {
                	                fDCCtrBusSoftSta = sfBiDirDCCtrSoftStartByAC(wBatteryInAvgVol,wBatteryOutAvgVol);
                	            }
                            }
                	    }
                	}
                	else
                	{
                		sRestartVarInit();
                		sDCOCPDisable();
	                    sGridRelayOpen();
			            sInvRelayOpen();
			            sPERelayOpen();
			            sAuxiRelayOpen();
			            sIsoRelayOpen();
			            sDCSoftRelayOpen();
	                    uwSysModePrepare = PowerOnMode;
                	}
                }
                	                                               
			    if((fRelayChkRequire == 1)&&(fRunInvAhdChkRequire == 1)&&(fDCCtrBusSoftSta == 1))
			    {	
			        if(fError == 0)
			        {
			            fInvEnOut = 1;				            
				        uwSysModePrepare = InvRunMode;
				        fBattDarkStart = 0;
				    }
                }  			
			}			    
            sErrFaultDeal();   
            break;
        
        case InvRunMode:
            if(fRunOrder == 0)
            {
                sRestartVarInit();
                sDCOCPDisable();
                sGridRelayOpen();
				sInvRelayOpen();
				sPERelayOpen();
				sAuxiRelayOpen();
				sIsoRelayOpen();
				sDCSoftRelayOpen();				
                uwSysModePrepare = PowerOnMode;
            }
            else
            {
            	//if(++uwDelay400ms >= 30)
            	//{
            	//	uwDelay400ms = 30;
            	//	sRelayPowerLPSOff();
            	//}
            	            	
            	wBatteryLimitCurrHigh = cDCCurrMax;
                wBatteryLimitCurrLow = cDCCurrMin;
                    
            	//wBatVolLimtHigh = ((INT32S)wBatChargeVoltLimt * 26214)>>14;
            	sBatteryChargeDeal(wBatteryInAvgVol,wBatteryAvgCurr);
            	wBatVolLimtLow = ((INT32S)wBatDisChargeVoltLimt * 26214)>>14;
            	wDCCtrBusVolRef = wStdBusVolRef + cVolt20V;
            	
            	if(fCSVSSwitch == 0)
            	{
            	    if((fBiDirDCEnOut == 3)&&(fDCModeSwitch == 0))
            	    {
            	    	if(fCharge == 1)
            	    	{
            	    	    wInvCtrBusVolRef = wBatVolLimtHigh;
            	    	}
            	    	else
            	    	{
            	    		if(wBatVolLimtLow > wStdBusVolRef)
            	    		{
            	    			wInvCtrBusVolRef = wBatVolLimtLow;
            	    		}
            	    		else
            	    		{
            	    			wInvCtrBusVolRef = wStdBusVolRef;
            	    		}
            	    	}
            	    }
            	    else
            	    {
            	    	if(fCharge == 1)
            	    	{
            	    		wInvCtrBusVolRef = wDCCtrBusVolRef + cVolt20V;//cVolt10V
            	    	}
            	    	else
            	    	{
            	    		wInvCtrBusVolRef = wDCCtrBusVolRef - cVolt20V;//cVolt10V
            	    	}           	    	            	    	
            	    }	
            	    
            	    sSysSleepChk();
            	}
            	else
            	{
            	    if((fBiDirDCEnOut == 3)&&(fDCModeSwitch == 0))
            	    {
            	        if(wBatVolLimtLow > wStdBusVolRef)
            	    	{
            	    		wBusDisChargeVoltRef = wBatVolLimtLow;
            	    	}
            	    	else
            	    	{
            	    		wBusDisChargeVoltRef = wStdBusVolRef;
            	    	}

            	    	wBusChargeVoltRef = wBatVolLimtHigh;
            	    }
            	    else
            	    {
            	        wBusDisChargeVoltRef = wDCCtrBusVolRef - cVolt20V;           	    	
            	        wBusChargeVoltRef = wDCCtrBusVolRef + cVolt20V;
            	    }            	    
            	}         	    
            }
            sErrFaultDeal();
            break;

        case FaultMode:
        {
	        static INT16U uwDelay5s = 0;
	        static INT16U uwErrManualClearDelay10s = 0;
			
            if(suwGetFaultGrade() <= 1)
            {
            	if(++uwDelay5s >= 1000)
            	{
            		uwDelay5s = 0;
            		sRestartVarInit();
            		fError = 0;
            		uwSysModePrepare = PowerOnMode;
            	}
            	uwErrManualClearDelay10s = 0;           			 
            }
            else
            {
                uwDelay5s = 0;
                
                if(fIsoLowErrEvent||fGridRelayErrEvent||fRunInvAhdErrEvent||fDampResistanceErrEvent||fGFCICTErrEvent
                   ||fInvRelayErrEvent||fAuxiRelayErrEvent||fDCRelayErrEvent||fDCPWMErrEvent||fDCSoftRelayErrEvent||fDCPWMPreChkErrEvent||fACPWMPreChkErrEvent)
                {
                	uwErrManualClearDelay10s++;
                	if(uwErrManualClearDelay10s >= 2000)  //10s filter 2000
                	{
					    uwErrManualClearDelay10s = 0;
	                    fGridRelayErrEvent = 0;
	                    fGridRelayErr = 0;
	                    fDCRelayErrEvent = 0;
	                    fDCRelayErr = 0;
	                    fDCRelayBurn = 0;
						fInvRelayErrEvent = 0;
	                    fInvRelayErr = 0;
	                    fAuxiRelayLost = 0;
	                    fAuxiRelayErrEvent = 0;
	                    fAuxiRelayErr = 0;
	                    fIsoLowErrEvent = 0;
	                    fIsoLowErr = 0;
	                    fRunInvAhdErr = 0;
	                    fRunInvAhdErrEvent = 0;
	                    fDampResistanceErr = 0;
	                    fDampResistanceErrEvent = 0;
	                    fDCPWMLost = 0;
	                    fDCPWMErr = 0;
						fDCPWMLostPreChk=0;
						fDCPWMPreChkErr=0;
						fACPWMLostPreChk=0;
	                    fACPWMPreChkErr=0;
						
	                    fDCSoftRelayLost = 0;
	                    fDCSoftRelayErr = 0;
	                    fDCSoftRelayErrEvent = 0; 
	                    fGFCICTErr = 0; 
	                    fGFCICTErrEvent = 0;	                                 
				        uwFaultCode[0] = 0;
	                    uwFaultCode[1] = 0;
	                    uwFaultGrade = 0;
	                    sRestartVarInit();
	                    fError = 0;
	            	    uwSysModePrepare = PowerOnMode;
                	}
                }
                else
                {	
                    uwErrManualClearDelay10s = 0;
                    if(fDeCapEnergy != 1)
                    {  	
                        sErroringDeal();
                        sDisDCSps();
                        sDisInvSps();                        
                    }                                             
                    uwSysModePrepare = FaultMode;                   
                }
            }
        } 	
        sErrFaultDeal();	
        break;
			
        case ShutMode:   
        {
            static INT16U uwDelay5s = 0;      
            
            if((uwPFCode == 0) && (fQuitShutModeOrder == 1))  //强制退出ShutMode
            {
            	if(++uwDelay5s >= 1000)
            	{
            		uwDelay5s = 0;
            		sRestartVarInit();
            		fError = 0;
            		fQuitShutModeOrder = 0;
                
            		uwSysModePrepare = PowerOnMode;
            		uwFaultGrade = 0;
            	}       			 
            }
        	else //if(suwGetFaultGrade() >= 2)
        	{  
                uwDelay5s = 0;
                
                sErroringDeal();              
                uwSysModePrepare = ShutMode;
                uwSysMode =	ShutMode;
                sDisDCSps();
                sDisInvSps();
                
                if(fGridRelayFaultEvent == 1)
                {  
                    //fGridRelayFaultEvent = 0;
                    fGridRelayErrEvent = 0;
                    fGridRelayErr = 0;
                    //uwFaultCode[0] = uwFaultCode[0]&0xdfff;
                }
                
                if(fInvRelayFaultEvent == 1)
                {  
                    fInvRelayErrEvent = 0;
                    fInvRelayErr = 0;
                }
                 
                if(fInvCurrHighFaultEvent == 1) // fInvCurrHighErrEvent
                {  
                    //fInvCurrHighFaultEvent = 0;
                    //fInvCurrHighErrEvent = 0;
                    fInvCurrHighErr = 0;
                    //uwFaultCode[0] = uwFaultCode[0]&0xfdff;
                }      	
                if(fOCPFaultEvent == 1)    
                {
                	sPWMTZClr();
                	//fOCPFaultEvent = 0;
                	//fOCPErrEvent = 0;
					fOCPErr = 0;
                	//uwFaultCode[1] = uwFaultCode[1]&0xfeff;
                }  
                if(fPhaseRmsCurFaultEvent == 1) // fPhaseRmsCurErrEvent
                {  
                    //fPhaseRmsCurFaultEvent = 0;
                    //fPhaseRmsCurErrEvent = 0;
                    fPhaseRmsCurErr = 0;
                    //uwFaultCode[1] = uwFaultCode[1]&0xf7ff;//0xefff
                }
                if(fRunInvAhdFaultEvent == 1)
                {
                    //fRunInvAhdFaultEvent = 0;
                    fRunInvAhdErrEvent = 0;
                    fRunInvAhdErr = 0;
                    //uwFaultCode[1] = uwFaultCode[1]&0x7fff;
                }
                if(fDCIHighFaultEvent == 1)
                {
                    //fDCIHighFaultEvent = 0;
                    //fDCIHighErrEvent = 0;
                    fDCIHighErr = 0;
                    //uwFaultCode[1] = uwFaultCode[1]&0xfffb;
                }  
                
                if(fDampResistanceFaultEvent == 1)
                {
                	fDampResistanceErrEvent = 0;
                    fDampResistanceErr = 0;
                }                  
            }
        }
        sErrFaultDeal();
        break;


        case BootloaderMode: 
        {
            sRestartVarInit();
            uwSysModePrepare = BootloaderMode;
            sDisDCSps();
            sDisInvSps();  
            
            sBootLoaderSchedule();                        
        }
        break;
    }
	uwSysMode = uwSysModePrepare;
}

extern void KickDog(void);
extern void DisableDog(void);

/*********************************************************************
Function name:  void main(void)
Description:  main主程序
Calls:        	 
Called By:  
Parameters:  void
Return:  void      
 *********************************************************************/
void main(void)
{
    DINT;
    IFR = 0x0000;
    IER = 0x0000;
    
    /*** CPU Initialization ***/
    InitSysCtrl();						// Initialize the CPU (FILE: SysCtrl.c)
    InitGpio();							// Initialize the shared GPIO pins (FILE: Gpio.c)
    InitPieCtrl();						// Initialize and enable the PIE (FILE: PieCtrl.c)
    
    CPURegister_check();
    if(wRegistercheckFlag == 1)
    {
    	sChipReset();
    }
    sRamChk();
   /*** Copy all FLASH sections that need to run from RAM (use memcpy() from RTS library) ***/
   // Section secureRamFuncs contains user defined code that runs from CSM secured RAM
    memcpy(&secureRamFuncs_runstart,
           &secureRamFuncs_loadstart,
           &secureRamFuncs_loadend - &secureRamFuncs_loadstart);

    memcpy(&AdcIsrRamFuncs_runstart,
           &AdcIsrRamFuncs_loadstart,
           &AdcIsrRamFuncs_loadend - &AdcIsrRamFuncs_loadstart);
           
    memcpy(&sMpptStrategyRamFuncs_runstart,
           &sMpptStrategyRamFuncs_loadstart,
           &sMpptStrategyRamFuncs_loadend - &sMpptStrategyRamFuncs_loadstart);        
    
    memcpy(&sInvControlRamFuncs_runstart,
           &sInvControlRamFuncs_loadstart,
           &sInvControlRamFuncs_loadend - &sInvControlRamFuncs_loadstart);  
             
    memcpy(&CAPRamFuncs_runstart,
           &CAPRamFuncs_loadstart,
           &CAPRamFuncs_loadend - &CAPRamFuncs_loadstart);      
                             
    memcpy(&ProtectRamFuncs_runstart,
           &ProtectRamFuncs_loadstart,
           &ProtectRamFuncs_loadend - &ProtectRamFuncs_loadstart);  
           
    memcpy(&eCanRamFuncs_runstart,
           &eCanRamFuncs_loadstart,
           &eCanRamFuncs_loadend - &eCanRamFuncs_loadstart); 
           
    memcpy(&BiDirDCCtrFuncs_runstart,
           &BiDirDCCtrFuncs_loadstart,
           &BiDirDCCtrFuncs_loadend - &BiDirDCCtrFuncs_loadstart);                        
  
    /*** Initialize the FLASH ***/
    InitFlash();						// Initialize the FLASH (FILE: SysCtrl.c)
    
    if(suwFlashCheck((Uint16 *)0x320000,(Uint16 *)0x33BFFE) == 0)  
	{
	    sChipReset();   
	}

    /*** VAR Initialization ***/
    InitVar();
    
    InitAdc();							// Initialize the ADC (FILE: Adc.c)
    InitEPwm();							// Initialize the Event Manager (FILE: EPwm.c)
    InitECana();
    InitECap();							// Initialize the Capture units (FILE: ECap.c)
    InitScia();
    InitScib();
    InitTimer();
    InitI2c();

/*** Enable the clocks to the ePWM module ***/
// Note: this should be done after all ePWM modules are configured
// to ensure synchronization between the ePWM modules.
    asm(" EALLOW");							// Enable EALLOW protected register access
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;	// HSPCLK to ePWM modules enabled
    asm(" EDIS");							// Disable EALLOW protected register access

    {
	    INT16U i = 0;
	    for(i = 0;i < 300;i++) DelayUs(5000);  //直流量检测需延时，滤波时间长
    }
 /*** Enable interrupts ***/
    SetDBGIER(IER);						// Configure the DBGIER for realtime debug
  
	
 	//sEnInvSps(); // shutdown AuxiPower 
	//sEnDCSps();
 	sInFanOff();
 	ERTM;
    EINT;
	KickDog();
      
    //Enable the watchdog
	EALLOW;
	SysCtrlRegs.WDCR = 0x0028;   
	EDIS;
    
    sACPowerEnable();
	sGridRelayOpen();
	sInvRelayOpen();
	sAuxiRelayOpen();
	sIsoRelayOpen();
	sPERelayOpen();
	sDCRelayPOpen();
	sQFMRelayClose();
	sDCSoftRelayOpen();
	    
    while(fStartOrNormal == 0)	
	{
	static INT16U uwDelay500ms = 0;
	    KickDog();
	
        if(ft10ms == 1)
        {
        	ft10ms = 0;
        	sPWMTZClr();  
        	if(uwSysMode == BootloaderMode)  //执行过(++uwDelay500ms > 25)，Eeprom常规萘取过了
            {
                sBootLoaderSchedule();
            }
     		sErrFaultDeal();
        }
        else if(ft20ms == 1)
        {
            ft20ms = 0;
  	       
  	        sEepromHandle();
  	        if(uwI2CBlockState.bit.fEpRdOnPwStep == 0)
            {
                uwDelay500ms = 0;
                if((uwI2CBlockState.bit.fEpImptErr != 0) || (uwI2CBlockState.bit.fNoStandardStatus == 1))
                {                    
                    sCANConfigID();
                    sCANIntEnable();
                }
            }
            else if(uwI2CBlockState.bit.fEpRdOnPwStep == 1)
            {
                sInitLCDImptData();                                            
                //sCANBConfigID();
                //sCANIntEnable();
                uwDelay500ms = 0;
            } 
            else if(uwI2CBlockState.bit.fEpRdOnPwStep == 2)
            {
                fSysCtrSignal.all = wEepromSysCtrSignal;
  	            sCANConfigID();
  	            sCANIntEnable();
  	            sCANDataMange();
  	            
  	            if((wEepromStandardValue == cIEEE1547Standard)||((wEepromStandardValue == cRule21Standard)))
                {
                    udwPowerOnWaitTime = (INT32U)wEepromPowerOnWaitTime * 100;
                    if(wEepromRatedFreqSel == 1)
					{
                        uwSysFreq = cFreq60Hz;
                        sPLLParamInit();
					}
					else
					{
						uwSysFreq = cFreq50Hz;
                        sPLLParamInit();
					}
                }
                
                wBatChargeVoltLimt = wEepromChargeVoltLimt;
                wBatDisChargeVoltLimt = wEepromDisChargeVoltLimt;
                wBatChargeCurrLimt = wEepromChargeCurrLimt;
                wBatDisChargeCurrLimt = wEepromDisChargeCurrLimt;
                wBatFloatChargeVolt = wEepromFloatChargeVolt;
                wBatSToFChargeCurr = wEepromSToFChargeCurr;
                
                wSOCChargeVoltLimit = wBatChargeVoltLimt;
                wSOCDisChargeVoltLimit = wBatDisChargeVoltLimt;
                wSOCChargeCurrLimt = wBatChargeCurrLimt;
                wSOCDisChargeCurrLimit = wBatDisChargeCurrLimt;
                wSOCFloatChargeVolt = wBatFloatChargeVolt;
                wSOCSToFChargeCurr = wBatSToFChargeCurr;
				
                //uwI2CBlockState.bit.fEpErr = sfEepromRunDataChange();       
	      				 
                if(++uwDelay500ms > 25)
                {
                    uwDelay500ms = 0;
			    
                    uwRInvCurrOffset = suwGetAvg(&udwRInvCurrOffsetSum,&uwRInvCurrOffsetSumCnt,&fRInvCurrOffsetSumClear);
			        uwSInvCurrOffset = suwGetAvg(&udwSInvCurrOffsetSum,&uwSInvCurrOffsetSumCnt,&fSInvCurrOffsetSumClear);
                    uwTInvCurrOffset = suwGetAvg(&udwTInvCurrOffsetSum,&uwTInvCurrOffsetSumCnt,&fTInvCurrOffsetSumClear); 
                    
		            uwRDCICurrOffset = suwGetAvg(&udwRDCICurrOffsetSum,&uwRDCICurrOffsetSumCnt,&fRDCICurrOffsetSumClear);
                    uwSDCICurrOffset = suwGetAvg(&udwSDCICurrOffsetSum,&uwSDCICurrOffsetSumCnt,&fSDCICurrOffsetSumClear);
                    uwTDCICurrOffset = suwGetAvg(&udwTDCICurrOffsetSum,&uwTDCICurrOffsetSumCnt,&fTDCICurrOffsetSumClear);
                    
                    uwBatCurr1Offset = suwGetAvg(&udwBatCurr1OffsetSum,&uwBatCurr1OffsetSumCnt,&fBatCurr1OffsetSumClear);
                    uwBatCurr2Offset = suwGetAvg(&udwBatCurr2OffsetSum,&uwBatCurr2OffsetSumCnt,&fBatCurr2OffsetSumClear);
                    uwBatCurr3Offset = suwGetAvg(&udwBatCurr3OffsetSum,&uwBatCurr3OffsetSumCnt,&fBatCurr3OffsetSumClear);
                    uwBatCurr4Offset = suwGetAvg(&udwBatCurr4OffsetSum,&uwBatCurr4OffsetSumCnt,&fBatCurr4OffsetSumClear);
                    
                    uwGFCICurrOffset = suwGetAvg(&udwGFCICurrOffsetSum,&uwGFCICurrOffsetSumCnt,&fGFCICurrOffsetSumClear);
                    
                    if(abs((INT16S)uwBatCurr1Offset - wEepromBatteryCurr1Offset) > 60) uwBatCurr1Offset = wEepromBatteryCurr1Offset;
                    if(abs((INT16S)uwBatCurr2Offset - wEepromBatteryCurr2Offset) > 60) uwBatCurr2Offset = wEepromBatteryCurr2Offset; 
                    if(abs((INT16S)uwBatCurr3Offset - wEepromBatteryCurr3Offset) > 60) uwBatCurr3Offset = wEepromBatteryCurr3Offset;
                    if(abs((INT16S)uwBatCurr4Offset - wEepromBatteryCurr4Offset) > 60) uwBatCurr4Offset = wEepromBatteryCurr4Offset;
                    if(abs((INT16S)uwGFCICurrOffset - wEepromGFCICurrOffset) > 60) uwGFCICurrOffset = wEepromGFCICurrOffset;
			        
                    if(fDisBootloaderOrder == 0) uwSysModePrepare = BootloaderMode;
                    
                    uwSysMode = uwSysModePrepare;            
		            DelayUs(20000);               //Delay for DcOffsetCheck
                    if(uwSysMode == PowerOnMode)
                    {
                        if(fError == 1)
			            {
			                fStartOrNormal = 0;
                        }
			            else
			            { 
			                if(wEepromStandardValue != cNoStandard)
			                {
				                InitTimerVar();
				                fStartOrNormal = 1;				             
                            }
                            else
                            {
                                fStartOrNormal = 0;
                            }
                        }
		            }
		            else if(uwSysMode == FaultMode)  
		            {
		                if(suwGetFaultGrade() <= 1)
			            {
			                uwSysModePrepare = PowerOnMode;
			          	    uwSysMode = PowerOnMode; 
			          	    fError = 0; 				
			            }
			            fStartOrNormal = 0; 
		            }
		            else if(uwSysMode == ShutMode)
		            {
		                if((suwGetFaultGrade() <= 1) && (fQuitShutModeOrder == 1))
			            {
			                uwSysModePrepare = PowerOnMode;
			                uwSysMode = PowerOnMode;
  			                fQuitShutModeOrder = 0;
                            fError = 0;
			            }
			            fStartOrNormal = 0;
                    }
                    else if(uwSysMode == BootloaderMode)	 
                    {
                        if(fError == 1) fStartOrNormal = 0;
                        sRestartVarInit();                       
                    }
                    //fStartOrNormal = 1;
                }
            }
            sEepromChk(&fEepromErr,&fEepromErrEvent);                          	    
            #if (!SCI_SCOPE)
                SCI_TX(&tSendData.uwData0[0],uwSendNum);		
                SciaRxDeal(); 
            #endif                
        }
    }
    
//====Main Loop ===//
	while(1)							// Dummy loop.  Wait for an interrupt.
    {
        //KickDog();

        if(ft10ms == 1)
        { 
        static INT16U uwGridChkDlyCnt = 0;
        static INT16U uwDelay50ms = 0;  
        				
        	ft10ms = 0;		    
            sCapDeal();
            if(fCapIntEvent == 1)
            {
            	fCapIntEvent = 0;
                sCalRmsPrdValueSum();
            }
                      
            if(++uwGridChkDlyCnt >= 10) //100ms
            {
                uwGridChkDlyCnt = 10;
                sLineRmsVolChk(&wRSGridRmsVol,&wSTGridRmsVol,&wTRGridRmsVol,&fLineGridRmsVolErr,&fLineGridRmsVolErrEvent);
            }

            if(fDCOCPEnable == 0)
            {
                ++uwDelay50ms;
                if(uwDelay50ms >= 5)
                {
                    uwDelay50ms = 0;
                    sDCOCPEnable();
                }
            }
            else
            {
                uwDelay50ms = 0;
                sDCOCPEnable();
            }
			      	            
            sIGBTHaltStateChk(&fIGBTHaltErr,&fIGBTHaltErrEvent);

			sACOCPStateChk(&fOCPErr,&fOCPErrEvent);
			sDCOCPStateChk(&fDCOCPErr,&fDCOCPErrEvent);
            sCtrPowerChk(&fCtrPowerErr,&fCtrPowerErrEvent);
            sCPLDOscilChk(&fCPLDOsciErr,&fCPLDOsciErrEvent);
            sDecCapEnergyChk(&fDecCapEnergyErr,&fDecCapEnergyErrEvent);			
			sGFCIDynamicHighChk(wGFCICurrRms,wGFCIAvgCurr,&fGFCIHighErr,&fGFCIHighErrEvent);
			sGFCISteadyHighChk(&wGFCICurrRms,&wGFCIAvgCurr,&fGFCISteadyErr,&fGFCISteadyErrEvent);
			sDCPWMChk(&fDCPWMErr,&fDCPWMErrEvent);
			sDCSoftRelayChk(&fDCSoftRelayErr,&fDCSoftRelayErrEvent);
			sRelayDelayDeal();

			sDCPWMChkPreChk(&fDCPWMPreChkErr,&fDCPWMPreChkErrEvent);
			sACPWMChkPreChk(&fACPWMPreChkErr,&fACPWMPreChkErrEvent);
			
            sTZStateClear(); 
            
            if(fDeCapEnergy != 1)
            {
                if(wEepromRemoteSel == 1)
                {
                	fCANRunOrder = fRemotePCSComdOn;
                	//fLocalPCSComdOnOff = fCANRunOrder;
                }
                else
                {
                	fCANRunOrder = fLocalPCSComdOnOff;
                	//fRemotePCSComdOn = fCANRunOrder;
                }
            }
            
            if(wEepromRemoteSel == 1)
            {
                wBatChargeVoltLimt = wEepromChargeVoltLimt;
                wBatDisChargeVoltLimt = wEepromDisChargeVoltLimt;
                wBatChargeCurrLimt = wSOCChargeCurrLimt;
                wBatDisChargeCurrLimt = wSOCDisChargeCurrLimit;
                wBatFloatChargeVolt = wEepromFloatChargeVolt;
                wBatSToFChargeCurr = wEepromSToFChargeCurr; 
            }
            else
            {
                wBatChargeVoltLimt = wEepromChargeVoltLimt;
                wBatDisChargeVoltLimt = wEepromDisChargeVoltLimt;
                wBatFloatChargeVolt = wEepromFloatChargeVolt;
                wBatSToFChargeCurr = wEepromSToFChargeCurr; 
                
                if(fBMSActive == 0)
                {                    
                    wBatChargeCurrLimt = wEepromChargeCurrLimt;
                    wBatDisChargeCurrLimt = wEepromDisChargeCurrLimt;                    
                }
                else
                {
                    wBatChargeCurrLimt = wSOCChargeCurrLimt;
                    wBatDisChargeCurrLimt = wSOCDisChargeCurrLimit;
                }               
            }            
                      
            fRunOrder = fCANRunOrder && fSciRunOrder && fDisBootloaderOrder;
            
            sSysMode();           
            
            if(fDisBootloaderOrder == 0)
            {
            	uwSysModePrepare = BootloaderMode;
            	uwSysMode = uwSysModePrepare;
            }            		    		    
        }
        else if(ft20ms == 1)
        {
            INT16U fDECapState = 0;           
                        
            ft20ms = 0;
            //********************************* DC Avg Cal*******************************
            wPOBusAvgVol = swGetAvg(&dwPOBusVolSum,&uwPOBusVolCnt,&fPOBusVolClear);
		    wONBusAvgVol = swGetAvg(&dwONBusVolSum,&uwONBusVolCnt,&fONBusVolClear);
            wBusAvgVol = wPOBusAvgVol + wONBusAvgVol;

			wBatteryOutAvgVol = swGetAvg(&dwBatteryOutVolSum,&uwBatteryOutVolCnt,&fBatteryOutVolClear);
			wBatteryInAvgVol = swGetAvg(&dwBatteryInVolSum,&uwBatteryInVolCnt,&fBatteryInVolClear);

			wBatteryAvgCurr1 = swGetAvg(&dwBatteryCurr1Sum,&uwBatteryCurr1Cnt,&fBatteryCurr1Clear);
			wBatteryAvgCurr2 = swGetAvg(&dwBatteryCurr2Sum,&uwBatteryCurr2Cnt,&fBatteryCurr2Clear);
			wBatteryAvgCurr3 = swGetAvg(&dwBatteryCurr3Sum,&uwBatteryCurr3Cnt,&fBatteryCurr3Clear);
			wBatteryAvgCurr4 = swGetAvg(&dwBatteryCurr4Sum,&uwBatteryCurr4Cnt,&fBatteryCurr4Clear);
            wBatteryAvgCurr = wBatteryAvgCurr1 + wBatteryAvgCurr2 + wBatteryAvgCurr3 + wBatteryAvgCurr4;            
            wIsoAvgVol = swGetAvg(&dwIsoVolSum,&uwIsoVolCnt,&fIsoVolClear);
            
            sACPowerDeal(wBatteryOutAvgVol);
            sDCRelayChk(&fDCRelayErr,&fDCRelayErrEvent);	        
	        sBusVolHighChk(&wBusAvgVol,&fBusHighErr,&fBusHighErrEvent);
		    sVCapHighChk(&wPOBusAvgVol,&wONBusAvgVol,&fVCapHighErr,&fVCapHighErrEvent);
		    sVCapDiffChk(&wPOBusAvgVol,&wONBusAvgVol,&fVCapDiffErr,&fVCapDiffErrEvent);
		    sBatVolHighChk(&wBatteryOutAvgVol,&wBatteryInAvgVol,&fBatVolHighErr,&fBatVolHighErrEvent);
		    sBusDiffBatVolChk(&wBusAvgVol,&wBatteryInAvgVol,&fBusDiffBatVolErr,&fBusDiffBatVolErrEvent);      
            sBatRevConnectChk(&wBatteryOutAvgVol,&fBattRevConnectErr,&fBattRevConnectErrEvent);//need, but no debug
	        
            sPhaseRmsCurChk(&wRInvRmsCurr,&wSInvRmsCurr,&wTInvRmsCurr,&fPhaseRmsCurErr,&fPhaseRmsCurErrEvent);
            sDCIHighChk(&wRDCIAvgCurr,&wSDCIAvgCurr,&wTDCIAvgCurr,&fDCIHighErr,&fDCIHighErrEvent); 
            uwGridFreq = suwMidGridFreqExtract(uwRFreq,uwSFreq,uwTFreq,&dwAngleIngSub);
            wIqRefBakToLVRT = swIqRefExtract(&tInvCurrDqoRef);
			wpuRmsCurrBakToLVRT = swRmsCurrExtract(&twInvRmsCurr);
			sFreHighChk(&uwRFreq,&uwSFreq,&uwTFreq,&fFreHighErr,&fFreHighErrEvent);
            sFreLowChk(&uwRFreq,&uwSFreq,&uwTFreq,&fFreLowErr,&fFreLowErrEvent);
            sAIFreqChk(&uwRFreq,&uwSFreq,&uwTFreq,&fAIFreqErr,&fAIFreqErrEvent);
            sAuxiRelayChk(&fAuxiRelayErr,&fAuxiRelayErrEvent);    
			sCalBusVolRef(&wRSGridRmsVol,&wSTGridRmsVol,&wTRGridRmsVol);
			sSysRatedCoeffChangeInit();
			wUdDivBatCoeff = swGirdDivBatteryVolCoeff(wBatteryInAvgVol);
			
			sCSPowerComdCal();
			sBiDirDCModeSwitch(wBatteryInAvgVol,wBatteryAvgCurr,dwDCPower);					                      
			sCalIdLimit();
			sCalIqRef();			            
			sAiCurrRefCal();
			sCANDataMange();
            sCANActiveChk(&fCANNoAcitveErr,&fCANNoAcitveErrEvent); 
            sDCCurrBalanceChk(&fDCCurrBalanceErr,&fDCCurrBalanceErrEvent);
            sVSPowerComdCal();
            sVSCoeffCal();
            sVSGridPretectCheckEn();
            SCIB_TX(&uwScibTxData[0],uwScibSendNum);
            
            sEepromHandle();
            sEepromChk(&fEepromErr,&fEepromErrEvent);
            
            #if (!SCI_SCOPE)
                SCI_TX(&tSendData.uwData0[0],uwSendNum);
                SciaRxDeal();
            #endif
                        
            uwInvPwmKFeedBack = suwInvAndDCPwmKCal(wBusAvgVol); 
                     
			if(fDeCapEnergy == 1)
			{
			    fDECapState = sDecCapEnergy(&wBusAvgVol);
			    if(fDECapState == 1)
			    {
			        fDeCapEnergy = 0;
			    }
			    else if(fDECapState == 2)
			    {
			        fDeCapEnergy = 2;
			    }
			}
			sFlashChk();
			sADSamplChk();
			StatusPowerCalc();
		}
        else if(ft100ms == 1)
        {
		    ft100ms = 0;
                    			
            sCalInvActivePowerValue(&ddwInvActivePowerSum[0],&uwInvActivePowerSumCnt,&fInvActivePowerSumClear,&dwInvActivePower[0]);
            sCalInvReactivePowerValue(&ddwInvReactivePowerSum[0],&uwInvReactivePowerSumCnt,&fInvReactivePowerSumClear,&dwInvReactivePower[0],&wRSGridRmsVol,&wSTGridRmsVol,&wTRGridRmsVol);  
           
            sPowerAdjust(&dwInvActivePower[0],&dwInvReactivePower[0]); 
            sCalInvApparentPowerValue(&dwInvActivePower[0],&dwInvReactivePower[0],&udwApparentPower[0]);  
            sCalDCPowerValue(&ddwDCPowerSum,&uwDCPowerSumCnt,&fDCPowerSumClear,&dwDCPower);          
           
            uwPowerEff = swEfficiencyFit(dwInvActivePower[3],wBatteryInAvgVol);
            sPowerFactor(&dwInvActivePower[0],&udwApparentPower[0],&wPowerFactor[0]);
            sBatteryDataUpdateChk(wBatteryPortVolt);

            sCalLongAvg();          
			
			sPowerDisMatchChk(&dwDCPower,&dwInvActivePower[3],&fPowerDisMatchErr,&fPowerDisMatchEvent);//PowerIn!=PowerOut			  
			sDCSwitchChk(wBatteryInAvgVol,&fDCSwitchErr,&fDCSwitchErrEvent);			
			sBatteryCoeffChk(&fBatteryCoeffErr,&fBatteryCoeffErrEvent);
			uwLVRTActiveValue = ((INT32S)wEepromLVRTTripVol * 16777)>>10;
			uwHVRTActiveValue = ((INT32S)wEepromHVRTTripVol * 16777)>>10;			
        }
        else if(ft500ms == 1)
        {       
			INT16S wMaxRmsCurr;
			INT16U i;
            
            ft500ms = 0;
            
            //========= Temperature Sensor Fitting =======  
            wEnvTmpAvg = swGetAvg(&dwEnvTmpSum,&uwEnvTmpSumCnt,&fEnvTmpSumClear);          
			wHeatTmpAvg = swGetAvg(&dwHeatTmpSum,&uwHeatTmpSumCnt,&fHeatTmpSumClear);
            wEnvFitTemp = swModuleEnvTempFit(wEnvTmpAvg);
            wHeatFitTemp = swModuleHeatTempFit(wHeatTmpAvg);
            
            sCalRmsAvgValue();
            sCalRmsFifoAvgValue();
			
            sTmpHighChk(&wHeatFitTemp,&wEnvFitTemp,&fOTErr,&fOTErrEvent); 
            
            sTmpLoseChk(&wHeatFitTemp,&wEnvFitTemp,&fTempSensorErr,&fTempSensorErrEvent);
            
            //sSpdChk(&fSpdErr,&fSpdErrEvent);

            if(wRInvRmsFifoAvgCurr > wSInvRmsFifoAvgCurr) 
			{
			    wMaxRmsCurr = wRInvRmsFifoAvgCurr;
		    }
            else
            { 
			    wMaxRmsCurr = wSInvRmsFifoAvgCurr;
			}
            if(wTInvRmsFifoAvgCurr > wMaxRmsCurr)
            { 
			    wMaxRmsCurr = wTInvRmsFifoAvgCurr;
			}
			wInvMaxRmsCurr = wMaxRmsCurr;
			
            fFanStatus = sFanOpenLoopCtrl(wEnvFitTemp,wHeatFitTemp,fTempdisable,dwInvActivePower[3],dwInvReactivePower[3],udwApparentPower[3]);

            uwCurrMax = suMaxOutputCurrCal(wEnvFitTemp,wHeatFitTemp,wBusAvgVol,wInvMaxRmsCurr);//suMaxApparentPowerCal();

            sFanFdbChk(&fFanStatus,&fFanErr,&fFanErrEvent);
                        
            uwPowerEffAvg = suwGetAvg(&udwPowerEffSum,&uwPowerEffSumCnt,&fPowerEffSumClear);
            
            for(i=0;i<4;i++)
            {
                wPowerFactorAvg[i] = swGetAvg(&dwPowerFactorSum[i],&uwPowerFactorSumCnt[i],&fPowerFactorSumClear[i]);
                if(dwInvReactivePower[i] < 0)
                {
                    wPowerFactorAvg[i] = -wPowerFactorAvg[i];
                }
            }
            sAdjustPF();            
            wGridVolBalanceDegree =  swCalBalanceDegree(&twGridLineRmsAvgVol);
			sGridVoltBalanceDegreeChk(&wGridVolBalanceDegree,&fGridVoltBalanceDegreeErr,&fGridVoltBalanceDegreeErrEvent);
			sDC12And34OptSwitch();
			sEEPROMProRangeChk();
        }          
    }  
} //end of main()

/*** end of file *****************************************************/

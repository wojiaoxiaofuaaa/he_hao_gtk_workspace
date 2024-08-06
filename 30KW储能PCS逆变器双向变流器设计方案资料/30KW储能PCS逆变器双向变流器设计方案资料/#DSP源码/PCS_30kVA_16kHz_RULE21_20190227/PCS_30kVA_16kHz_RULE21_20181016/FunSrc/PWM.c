/**********************************************************************
* File: EPwm.c
* Devices: TMS320F2808, TMS320F2806, TMS320F2801
* Author:Zhou Xu
* History:
*   02/11/05 - original (D. Alter)
**********************************************************************/
#include "..\FunSrc\Funheader.h"

INT16U uwInvCtrPWMPrd;
INT16U uwBiDirDCCtrPWMPrd;
INT16U uwFanDuty;

void InitPWMVar(void)
{
	uwInvCtrPWMPrd = 0;
	uwBiDirDCCtrPWMPrd = 0;
	uwFanDuty = 0;
}
/**********************************************************************
* Function: InitEPwm()
*
* Description: Initializes the Enhanced PWM modules on the F280x.
**********************************************************************/
void InitEPwm(void)
{
    /************************************************************/
    //*** Must disable the clock to the ePWM modules if you    ***/
    //*** want all ePMW modules synchronized.                  ***/
    //*** EPwm1 For Three-level Inverter A Phase
    //*** EPwm2 For Three-level Inverter B Phase
    //*** EPwm3 For Three-level Inverter C Phase
    //*** EPwm4 For Bi-Direction DC Converter 1 And 3
    //*** EPwm5 For Bi-Direction DC Converter 2 And 4
    //*** EPwm6 For ADTrip
    /************************************************************/
	asm(" EALLOW");						        //Enable EALLOW protected register access
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      
	asm(" EDIS");						        //Disable EALLOW protected register access
	
    //-------------------------------------EPWM1------------------------------------------                                            
    EPwm1Regs.TBCTL.bit.CTRMODE = 0x3;          //stop-freeze counter opteration
    EPwm1Regs.TBCTL.bit.FREE_SOFT = 0x3;        //Configure timer control register
    EPwm1Regs.TBCTL.bit.PHSDIR = 0x0;           //0x0 before
    EPwm1Regs.TBCTL.bit.CLKDIV = 0x0;           
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x0;        
    EPwm1Regs.TBCTL.bit.SWFSYNC = 0x0;                                                   
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 0x1;     //CTR == 0 ,SYNC out
    EPwm1Regs.TBCTL.bit.PRDLD = 0x0;        
	EPwm1Regs.TBCTL.bit.PHSEN = 0x0;        
	                                  
    EPwm1Regs.TBCTR = 0x0000;				    //Clear timer counter
	EPwm1Regs.TBPRD = 4688;//CPUFre/PWM_Fre/2;	        //Set timer period
	EPwm1Regs.TBPHS.half.TBPHS = 0x0000;        //Set timer phase
                                                
	EPwm1Regs.ETPS.all = 0x000;			        //Configure SOCA
	EPwm1Regs.ETSEL.all = 0x000;			    //Configure SOCA

	EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD; //Set PWM duty cycle
    EPwm1Regs.CMPB = 0;//EPwm1Regs.TBPRD;        	//Set PWM duty cycle
	EPwm1Regs.CMPCTL.all = 0x0000;			    //CMPA Load on ctr=zro,CMPB load on ctr = zro;	  
	
	EPwm1Regs.AQCTLA.all = 0x0060;		        //Action-qualifier control register A  UP->High,Down->Low
    EPwm1Regs.AQCTLB.all = 0x0900;		        //Action-qualifier control register B  UP->Low,Down->High
	EPwm1Regs.AQSFRC.all = 0x00ED;              //Force clear
	EPwm1Regs.AQCSFRC.all = 0x0005; 	        //INV PWM FORCE LOW
	
    EPwm1Regs.DBCTL.bit.OUT_MODE = 0;	        //Deadband disabled
    EPwm1Regs.PCCTL.bit.CHPEN = 0;		        //PWM chopper unit disabled
	EALLOW;	
	EPwm1Regs.TZSEL.bit.OSHT1 = 1;
	EPwm1Regs.TZCTL.all = 0x000A;		        //Trip action disabled for output A ,TZ Force Low
	EDIS;

    //-------------------------------------EPWM2------------------------------------------
    EPwm2Regs.TBCTL.bit.CTRMODE = 0x3;          //stop-freeze counter opteration
    EPwm2Regs.TBCTL.bit.FREE_SOFT = 0x3;		//Configure timer control register
    EPwm2Regs.TBCTL.bit.PHSDIR = 0x0;           //0x0 before
    EPwm2Regs.TBCTL.bit.CLKDIV = 0x0;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x0;
    EPwm2Regs.TBCTL.bit.SWFSYNC = 0x0;  
    EPwm2Regs.TBCTL.bit.SYNCOSEL = 0x0;
    EPwm2Regs.TBCTL.bit.PRDLD = 0x0;
	EPwm2Regs.TBCTL.bit.PHSEN = 0x1;		    //Configure timer control register

    EPwm2Regs.TBCTR = 0x0000;				    //Clear timer counter
	EPwm2Regs.TBPRD = 4688;//CPUFre/PWM_Fre/2;	        //Set timer period
	EPwm2Regs.TBPHS.half.TBPHS = 0x0000;	    //Set timer phase

	EPwm2Regs.ETPS.all = 0x0000;			    //Configure SOCA
	EPwm2Regs.ETSEL.all = 0x0000;			    //Configure SOCA

	EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.TBPRD; //Set PWM duty cycle
    EPwm2Regs.CMPB = 0;//EPwm2Regs.TBPRD;	        //Set PWM duty cycle
	EPwm2Regs.CMPCTL.all = 0x0000;			    //CMPA Load on ctr=zro,CMPB load on ctr = zro;

	EPwm2Regs.AQCTLA.all = 0x0060;		        //Action-qualifier control register A  UP->High,Down->Low
    EPwm2Regs.AQCTLB.all = 0x0900;		        //Action-qualifier control register B  UP->Low,Down->High
	EPwm2Regs.AQSFRC.all = 0x00ED;
    EPwm2Regs.AQCSFRC.all = 0x0005;	            //INV PWM FORCE LOW

    EPwm2Regs.DBCTL.bit.OUT_MODE = 0;	        //Deadband disabled
    EPwm2Regs.PCCTL.bit.CHPEN = 0;		        //PWM chopper unit disabled
	EALLOW;
	EPwm2Regs.TZSEL.bit.OSHT1 = 1;
	EPwm2Regs.TZCTL.all = 0x000A;		        //Trip action disabled for output A ,TZ Force Low
	EDIS;
    
    //-------------------------------------EPWM3------------------------------------------
    EPwm3Regs.TBCTL.bit.CTRMODE = 0x3;          //stop-freeze counter opteration
    EPwm3Regs.TBCTL.bit.FREE_SOFT = 0x3;		//Configure timer control register
    EPwm3Regs.TBCTL.bit.PHSDIR = 0x0;           //0x0 before
    EPwm3Regs.TBCTL.bit.CLKDIV = 0x0;
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0x0;
    EPwm3Regs.TBCTL.bit.SWFSYNC = 0x0;
    
    EPwm3Regs.TBCTL.bit.SYNCOSEL = 0x0;
    EPwm3Regs.TBCTL.bit.PRDLD = 0x0;
	EPwm3Regs.TBCTL.bit.PHSEN = 0x1;		    //Configure timer control register

    EPwm3Regs.TBCTR = 0x0000;				    //Clear timer counter
	EPwm3Regs.TBPRD = 4688;//CPUFre/PWM_Fre/2;	        //Set timer period,10000表示100MHZ系统频率
	EPwm3Regs.TBPHS.half.TBPHS = 0x0000;	    //Set timer phase

	EPwm3Regs.ETPS.all = 0x0000;			    //Configure SOCA
	EPwm3Regs.ETSEL.all = 0x0000;			    //Configure SOCA

	EPwm3Regs.CMPA.half.CMPA = EPwm3Regs.TBPRD; //Set PWM duty cycle
    EPwm3Regs.CMPB = 0;//EPwm3Regs.TBPRD;	        //Set PWM duty cycle
	EPwm3Regs.CMPCTL.all = 0x0000;			    //CMPA Load on ctr=zro,CMPB load on ctr = zro;

	EPwm3Regs.AQCTLA.all = 0x0060;		        //Action-qualifier control register A  UP->High,Down->Low
    EPwm3Regs.AQCTLB.all = 0x0900;		        //Action-qualifier control register B  UP->Low,Down->High
	EPwm3Regs.AQSFRC.all = 0x00ED;
    EPwm3Regs.AQCSFRC.all = 0x0005;	            //INV PWM FORCE LOW

    EPwm3Regs.DBCTL.bit.OUT_MODE = 0;	        //Deadband disabled
    EPwm3Regs.PCCTL.bit.CHPEN = 0;		        //PWM chopper unit disabled
	EALLOW;
	EPwm3Regs.TZSEL.bit.OSHT1 = 1;
	EPwm3Regs.TZCTL.all = 0x000A;		        //Trip action disabled for output A ,TZ Force Low
	EDIS;
	
	//-------------------------------------EPWM4------------------------------------------
    EPwm4Regs.TBCTL.bit.CTRMODE = 0x3;          //stop-freeze counter opteration
    EPwm4Regs.TBCTL.bit.FREE_SOFT = 0x3;		//Configure timer control register
    EPwm4Regs.TBCTL.bit.PHSDIR = 0x1;           //Logic NO,Down to Up
    EPwm4Regs.TBCTL.bit.CLKDIV = 0x0;
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0x0;
    EPwm4Regs.TBCTL.bit.SWFSYNC = 0x0;        
    EPwm4Regs.TBCTL.bit.SYNCOSEL = 0x0;
    EPwm4Regs.TBCTL.bit.PRDLD = 0x0;
	EPwm4Regs.TBCTL.bit.PHSEN = 0x1;		    //Configure timer control register
    EPwm4Regs.TBCTR = 0x0000;				    //Clear timer counter
	EPwm4Regs.TBPRD = 4688;//CPUFre/PWM_Fre/2;	        //Set timer period
	EPwm4Regs.TBPHS.half.TBPHS = 2288;//2448;

	EPwm4Regs.ETPS.all = 0x0000;			    //Configure SOCA
	EPwm4Regs.ETSEL.all = 0x0000;			    //Configure SOCA

	EPwm4Regs.CMPA.half.CMPA = EPwm4Regs.TBPRD; //Set 100% Duty
	EPwm4Regs.CMPB = EPwm4Regs.TBPRD;
	EPwm4Regs.CMPCTL.all = 0x0000;			    //Compare control register

	EPwm4Regs.AQCTLA.all = 0x0060;		        //Action-qualifier control register A  UP->High,Down->Low
	EPwm4Regs.AQCTLB.all = 0x0600;
	EPwm4Regs.AQSFRC.all = 0x00ED;
    EPwm4Regs.AQCSFRC.all = 0x0005;	            //INV PWM FORCE LOW

	EPwm4Regs.DBCTL.bit.OUT_MODE = 0;	        //Deadband disabled
	EPwm4Regs.PCCTL.bit.CHPEN = 0;		        //PWM chopper unit disabled
	
	EALLOW;
	EPwm4Regs.TZSEL.bit.OSHT1 = 1;              //EPWM4`s IO are GPIO and SYN
	EPwm4Regs.TZCTL.all = 0x000A;		        //Trip action disabled for output A
	EDIS;
	
    //-------------------------------------EPWM5------------------------------------------
    EPwm5Regs.TBCTL.bit.CTRMODE = 0x3;          //stop-freeze counter opteration
	EPwm5Regs.TBCTL.bit.FREE_SOFT = 0x3;		//Configure timer control register
    EPwm5Regs.TBCTL.bit.PHSDIR = 0x0;
    EPwm5Regs.TBCTL.bit.CLKDIV = 0x0;
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0x0;
    EPwm5Regs.TBCTL.bit.SWFSYNC = 0x0;
    EPwm5Regs.TBCTL.bit.SYNCOSEL = 0x0;
    EPwm5Regs.TBCTL.bit.PRDLD = 0x0;
	EPwm5Regs.TBCTL.bit.PHSEN = 0x1;     
    EPwm5Regs.TBCTR = 0x0000;				    //Clear timer counter
	EPwm5Regs.TBPRD = 4688;//CPUFre/PWM_Fre/2;	        //Set timer period
	EPwm5Regs.TBPHS.half.TBPHS = 2400;//2240;//EPwm5Regs.TBPRD;	    //Set timer phase
    EPwm5Regs.ETPS.all = 0x0000; 			
	EPwm5Regs.ETSEL.all = 0x0000;			

    EPwm5Regs.CMPA.half.CMPA = EPwm5Regs.TBPRD;	//Set PWM duty cycle
    EPwm5Regs.CMPB = EPwm5Regs.TBPRD;
	EPwm5Regs.CMPCTL.all = 0x0000;			    //Compare control register
	EPwm5Regs.AQCTLA.all = 0x0060;		        //Action-qualifier control register A
	EPwm5Regs.AQCTLB.all = 0x0600;
	
	EPwm5Regs.AQSFRC.all = 0x00ED;
    EPwm5Regs.AQCSFRC.all = 0x0005;	    

	EPwm5Regs.DBCTL.bit.OUT_MODE = 0;	        //Deadband disabled
    EPwm5Regs.PCCTL.bit.CHPEN = 0;		        //PWM chopper unit disabled

    EALLOW;
	EPwm5Regs.TZSEL.bit.OSHT1 = 1;              //fan always run even TZ happens
	EPwm5Regs.TZCTL.all = 0x000A;		        //Trip action disabled for output A
    EDIS;

    //-------------------------------------EPWM6------------------------------------------
    EPwm6Regs.TBCTL.bit.CTRMODE = 0x3;          //stop-freeze counter opteration
	EPwm6Regs.TBCTL.bit.FREE_SOFT = 0x3;		//Configure timer control register
    EPwm6Regs.TBCTL.bit.PHSDIR = 0x0;
    EPwm6Regs.TBCTL.bit.CLKDIV = 0x0;
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0x0;
    EPwm6Regs.TBCTL.bit.SWFSYNC = 0x0;    
    EPwm6Regs.TBCTL.bit.SYNCOSEL = 0x0;
    EPwm6Regs.TBCTL.bit.PRDLD = 0x0;
	EPwm6Regs.TBCTL.bit.PHSEN = 0x1;
     
    EPwm6Regs.TBCTR = 0x0000;				      //Clear timer counter
	EPwm6Regs.TBPRD = 2344;//CPUFre/Boost_Fre/2;  //Double of EPwm1
	EPwm6Regs.TBPHS.half.TBPHS = 0; //Set timer phase
	
	EPwm6Regs.ETPS.all = 0x0100;                //Configure SOCA
	EPwm6Regs.ETSEL.all = 0x0900;			    //Configure SOCA

    EPwm6Regs.CMPA.half.CMPA = EPwm6Regs.TBPRD;
    EPwm6Regs.CMPB = EPwm6Regs.TBPRD;
	EPwm6Regs.CMPCTL.all = 0x0000;

	EPwm6Regs.AQCTLA.all = 0x0060;		        //Action-qualifier control register A
	EPwm6Regs.AQCTLB.all = 0x0600;		        //CBD Clear: force EPWMxB output low.CBU Set: force EPWMxB output high.@0813
	EPwm6Regs.AQSFRC.all = 0x00ED;
    EPwm6Regs.AQCSFRC.all = 0x0000;	

	EPwm6Regs.DBCTL.bit.OUT_MODE = 0;	        //Deadband disabled
	EPwm6Regs.PCCTL.bit.CHPEN = 0;		        //PWM chopper unit disabled
    EALLOW;
	EPwm6Regs.TZSEL.bit.OSHT1 = 0;              //EPWM4`s IO are GPIO and SYN
	EPwm6Regs.TZCTL.all = 0x000F;		        //Trip action disabled for output A
    EDIS;
    
    uwInvCtrPWMPrd = 4688;//CPUFre/PWM_Fre/2;
    uwBiDirDCCtrPWMPrd = 4688;
    EPwm1Regs.TBCTL.bit.CTRMODE = 0x2;	        //Enable the timer in count up/down mode
    EPwm2Regs.TBCTL.bit.CTRMODE = 0x2;	        //Enable the timer in count up/down mode
    EPwm3Regs.TBCTL.bit.CTRMODE = 0x2;	        //Enable the timer in count up/down mode
    EPwm4Regs.TBCTL.bit.CTRMODE = 0x2;	        //Enable the timer in count up/down mode
    EPwm5Regs.TBCTL.bit.CTRMODE = 0x2;	        //Enable the timer in count up/down mode
    EPwm6Regs.TBCTL.bit.CTRMODE = 0x2; 	        //Enable the timer in count up/down mode

} // end InitEPwm()

/*** end of file *****************************************************/

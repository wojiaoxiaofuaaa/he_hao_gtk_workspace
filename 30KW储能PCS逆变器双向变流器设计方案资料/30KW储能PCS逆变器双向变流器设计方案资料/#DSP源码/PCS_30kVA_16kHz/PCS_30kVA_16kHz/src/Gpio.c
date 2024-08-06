/**********************************************************************
* File: Gpio.c
* Devices: TMS320F2808, TMS320F2806, TMS320F2801
* Author: Zhouxu
* History:
*   02/11/05 - original (D. Alter)
**********************************************************************/
#include "..\FunSrc\Funheader.h"
/**********************************************************************
* Function: InitGpio()
*
* Description: Initializes the shared GPIO pins on the F280x.
**********************************************************************/
void InitGpio(void)
{
	asm(" EALLOW");								// Enable EALLOW protected register access

/*** Group A pins ***/
	GpioCtrlRegs.GPACTRL.all = 0x0000;			// QUALPRD = SYSCLKOUT for all group A GPIO
	
	GpioCtrlRegs.GPAQSEL1.all = 0x0000;			// No qualification for all group A GPIO 0-15
	GpioCtrlRegs.GPAQSEL2.all = 0x0000;			// No qualification for all group A GPIO 16-31
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 2;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 2;
	GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 2;
	GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 2;
	GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 2;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 2;
	GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 2;
	GpioCtrlRegs.GPAQSEL2.bit.GPIO31 = 2;
	

	GpioCtrlRegs.GPADIR.all = 0x0000;			// All group A GPIO are inputs
	
	//GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;			//epwm1,2,3,IGBT Drive
    //GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;
    //GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;
    //GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;
    //GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;
    //GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;

    //GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;          //sync signal
    	        
    //GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;		// =0 pullups enabled,Fan Pwm
    //GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;      // fan control
    //GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;
    //GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;
    //GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;
    
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;    //Tz1
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;    //Mcu feedback
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    //AuxiRelayCtr
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    //ACOCP
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;    //DCSps
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;    //AD Channel select
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;    //CANRXA
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;    //CANTXA
    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;    //CANTXB
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;    //CANRXB
    GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;    //SCITXDB
    GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;    //SCIRXDB
    GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;    //eCap1
    GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;    //eCap2
    GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;    //eCap3
    GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;    //NC
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    //NC
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    //NC
    GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;    //NC
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;    //fCtrPowerState
        
	GpioCtrlRegs.GPAMUX1.bit.GPIO0  = 1;		// 0=GPIO, 1=EPWM1A    2=rsvd       3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 1;		// 0=GPIO, 1=EPWM1B    2=ECAP6      3=MFSRB
	GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 1;		// 0=GPIO, 1=EPWM2A    2=rsvd       3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 1;		// 0=GPIO, 1=EPWM2B    2=ECAP5      3=MCLKRB
	GpioCtrlRegs.GPAMUX1.bit.GPIO4  = 1;		// 0=GPIO, 1=EPWM3A    2=rsvd       3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO5  = 1;		// 0=GPIO, 1=EPWM3B    2=MFSRA      3=ECAP1
	GpioCtrlRegs.GPAMUX1.bit.GPIO6  = 1;		// 0=GPIO, 1=EPWM4A    2=EPWMSYNCI  3=EPWMSYNCO
	GpioCtrlRegs.GPAMUX1.bit.GPIO7  = 1;		// 0=GPIO, 1=EPWM4B    2=MCLKRA     3=ECAP2
	GpioCtrlRegs.GPAMUX1.bit.GPIO8  = 1;		// 0=GPIO, 1=EPWM5A    2=CANTXB     3=ADCSOCAO
	GpioCtrlRegs.GPAMUX1.bit.GPIO9  = 1;		// 0=GPIO, 1=EPWM5B    2=SCITXB     3=ECAP3
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;		// 0=GPIO, 1=EPWM6A    2=CANRXB     3=ADCSOCBO 
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;		// 0=GPIO, 1=EPWM6B    2=SCIRXB     3=ECAP4
	
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;		// 0=GPIO, 1=TZ1       2=CANTXB     3=MDXB

	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;		// 0=GPIO, 1=TZ2       2=CANRXB     3=MDRB 
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;		// 0=GPIO, 1=TZ3       2=SCITXB     3=MCLKXB  
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;		// 0=GPIO, 1=TZ4       2=SCIRXB     3=MFSXB
	GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;		// 0=GPIO, 1=SPISIMOA  2=CANTXB     3=TZ5
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;		// 0=GPIO, 1=SPISOMIA  2=CANRXB     3=TZ6
	
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;		// 0=GPIO, 1=SPICLKA   2=SCITXB     3=CANRXA
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;		// 0=GPIO, 1=SPISTEA   2=SCIRXB     3=CANTXA
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 3;		// 0=GPIO, 1=EQEP1A    2=MDXA       3=CANTXB
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 3;		// 0=GPIO, 1=EQEP1B    2=MDRA       3=CANRXB  

	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;		// 0=GPIO, 1=EQEP1S    2=MCLKXA     3=SCITXB
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;		// 0=GPIO, 1=EQEP1I    2=MFSXA      3=SCIRXB
	
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 1;		// 0=GPIO, 1=ECAP1     2=EQEP2A     3=MDXB
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 1;		// 0=GPIO, 1=ECAP2     2=EQEP2B     3=MDRB
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 1;		// 0=GPIO, 1=ECAP3     2=EQEP2I     3=MCLKXB
	
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;		// 0=GPIO, 1=ECAP4     2=EQEP2S     3=MFSXB
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 0;		// 0=GPIO, 1=SCIRXDA   2=XZCS6      3=XZCS6
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;		// 0=GPIO, 1=SCITXDA   2=XA19       3=XA19
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0;		// 0=GPIO, 1=CANRXA    2=XA18       3=XA18
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;		// 0=GPIO, 1=CANTXA    2=XA17       3=XA17

/*** Group B pins ***/
	GpioCtrlRegs.GPBCTRL.all = 0x0000;			// QUALPRD = SYSCLKOUT for all group B GPIO
	
	GpioCtrlRegs.GPBQSEL1.all = 0x0000;			// No qualification for all group B GPIO 32-35
	GpioCtrlRegs.GPBQSEL2.all = 0x0000; 
	GpioCtrlRegs.GPBQSEL1.bit.GPIO34 = 2;       //IGBTHAlt
	GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 2; 
	GpioCtrlRegs.GPBQSEL1.bit.GPIO37 = 2;       //DCOCP	  
	GpioCtrlRegs.GPBQSEL1.bit.GPIO38 = 2;       //CPLDShutInvPWM
	GpioCtrlRegs.GPBQSEL1.bit.GPIO43 = 2;       //CPLDShutDCPWM
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 2;
    GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 2;

	GpioCtrlRegs.GPBDIR.all = 0x0000;			// All group B GPIO are inputs
	GpioCtrlRegs.GPBPUD.all = 0x0000;			// All group B pullups enabled

	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;		// 0=GPIO, 1=SDAA      2=EPWMSYNCI  3=ADCSOCAO
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;		// 0=GPIO, 1=SCLA      2=EPWMSYNCO  3=ADCSOCBO
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;		// 0=GPIO, 1=rsvd      2=rsvd       3=rsvd
	GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 1;		// 0=GPIO, 1=SCITXDA   2=XR/W       3=XR/W
	GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 1;		// 0=GPIO, 1=SCIRXDA   2=XZCS0      3=XZCS0
	GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 0;		// 0=GPIO, 1=rsvd      2=XZCS7      3=XZCS7
	GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 0;		// 0=GPIO, 1=rsvd      2=XWE0       3=XWE0
    GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;		// 0=GPIO, 1=rsvd      2=XA16       3=XA16
	GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0;		// 0=GPIO, 1=rsvd      2=XA0/XWE1   3=XA0/XWE1
	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;		// 0=GPIO, 1=rsvd      2=XA1        3=XA1
	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0;		// 0=GPIO, 1=rsvd      2=XA2        3=XA2
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0;		// 0=GPIO, 1=rsvd      2=XA3        3=XA3
	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0;		// 0=GPIO, 1=rsvd      2=XA4        3=XA4
	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 0;		// 0=GPIO, 1=rsvd      2=XA5        3=XA5
	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 0;		// 0=GPIO, 1=rsvd      2=XA6        3=XA6
    GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 0;		// 0=GPIO, 1=rsvd      2=XA7        3=XA7   
	GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;		// 0=GPIO, 1=eCap      2=XD31       3=XD31
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;		// 0=GPIO, 1=eCap      2=XD30       3=XD30
	GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;		// 0=GPIO, 1=eQEP      2=XD29       3=XD29
	GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;		// 0=GPIO, 1=eQEP      2=XD28       3=XD28
	GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;		// 0=GPIO, 1=eQEP      2=XD27       3=XD27
	GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;		// 0=GPIO, 1=eQEP      2=XD26       3=XD26
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1;		// 0=GPIO, 1=SPISIMOA  2=XD25       3=XD25
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1;		// 0=GPIO, 1=SPISOMIA  2=XD24       3=XD24
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1;		// 0=GPIO, 1=SPICLKA   2=XD23       3=XD23
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 1;		// 0=GPIO, 1=SPISTEA   2=XD22       3=XD22
	GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;		// 0=GPIO, 1=MCLKRA    2=XD21       3=XD21
	GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;		// 0=GPIO, 1=MFSRA     2=XD20       3=XD20
	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;		// 0=GPIO, 1=MCLKRB    2=XD19       3=XD19
	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;		// 0=GPIO, 1=MFSRB     2=XD18       3=XD18
	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;		// 0=GPIO, 1=SCIRXDC   2=XD17       3=XD17
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;		// 0=GPIO, 1=SCITXDC   2=XD16       3=XD16
/*** Group C pins ***/
    GpioCtrlRegs.GPCDIR.all = 0x0000;			// All group C GPIO are inputs
    
   	GpioCtrlRegs.GPCPUD.all = 0x0000;			// All group C pullups enabled
	GpioCtrlRegs.GPCPUD.bit.GPIO76 = 1;
	GpioCtrlRegs.GPCPUD.bit.GPIO77 = 1;	
	GpioCtrlRegs.GPCPUD.bit.GPIO78 = 1;
	GpioCtrlRegs.GPCPUD.bit.GPIO79 = 1;
			
    GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;		// 0=GPIO, 1=GPIO      2=XD15       3=XD15
	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;		// 0=GPIO, 1=GPIO      2=XD14       3=XD14
	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;		// 0=GPIO, 1=GPIO      2=XD13       3=XD13
	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;		// 0=GPIO, 1=GPIO      2=XD12       3=XD12
	GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 0;		// 0=GPIO, 1=GPIO      2=XD11       3=XD15
	GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 0;		// 0=GPIO, 1=GPIO      2=XD10       3=XD14
	GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 0;		// 0=GPIO, 1=GPIO      2=XD9        3=XD9
	GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 0;		// 0=GPIO, 1=GPIO      2=XD8        3=XD8	
	GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 0;		// 0=GPIO, 1=GPIO      2=XD7        3=XD7
	GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 0;		// 0=GPIO, 1=GPIO      2=XD6        3=XD6
	GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 0;		// 0=GPIO, 1=GPIO      2=XD5        3=XD5
	GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 0;		// 0=GPIO, 1=GPIO      2=XD4        3=XD4
	GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 0;		// 0=GPIO, 1=GPIO      2=XD3        3=XD3
	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 0;		// 0=GPIO, 1=GPIO      2=XD2        3=XD2
	GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 0;		// 0=GPIO, 1=GPIO      2=XD1        3=XD1
	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 0;		// 0=GPIO, 1=GPIO      2=XD0        3=XD0
	
	GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 0;		// 0=GPIO, 1=GPIO      2=XA8        3=XA8
	GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 0;		// 0=GPIO, 1=GPIO      2=XA9        3=XA9
	GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 0;		// 0=GPIO, 1=GPIO      2=XA10       3=XA10
	GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 0;		// 0=GPIO, 1=GPIO      2=XA11       3=XA11
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 0;		// 0=GPIO, 1=GPIO      2=XA12       3=XA12
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 0;		// 0=GPIO, 1=GPIO      2=XA13       3=XA13
	GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 0;		// 0=GPIO, 1=GPIO      2=XA14       3=XA14
	GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;		// 0=GPIO, 1=GPIO      2=XA15       3=XA15
	
/*** Output I/O Config***/
    GpioDataRegs.GPASET.bit.GPIO14 = 1;         //AuxiRelayCtr         0,close         1,open
    GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;       //DCSps                0,Diable        1,Enable
    GpioDataRegs.GPASET.bit.GPIO17 = 1;         //ADSLECT              0,select a      1,slect b
    GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;       //NC
    GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;       //NC
    GpioDataRegs.GPBSET.bit.GPIO39 = 1;         //GridRelay            0,close         1,open
    GpioDataRegs.GPBCLEAR.bit.GPIO40 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO41 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO42 = 1;       //DC3,4CPLDEnable      0,Disable       1,Enable
    GpioDataRegs.GPBSET.bit.GPIO44 = 1;         //DCOCPEnable
    GpioDataRegs.GPBSET.bit.GPIO45 = 1;         //InvRelay             0,close         1,open 
    GpioDataRegs.GPBSET.bit.GPIO46 = 1;         //DCSps                0,Enable        1,Disable  
    GpioDataRegs.GPBSET.bit.GPIO47 = 1;         //ACSps                0,Enable        1,Disable 
    GpioDataRegs.GPBSET.bit.GPIO48 = 1;         //PERelay/GFCICT       0,close         1,open
    GpioDataRegs.GPBSET.bit.GPIO49 = 1;         //NC                   must be set
    GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;       //NC
    GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;       //NC
    GpioDataRegs.GPCSET.bit.GPIO64 = 1;         //485REorDE            0,active        1,disable
    GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;       //NC
    GpioDataRegs.GPCSET.bit.GPIO66 = 1;         //DCRealyNCtr          0,Close         1,Open
    GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;       //NC
    GpioDataRegs.GPCSET.bit.GPIO69 = 1;         //DCSoftRelayCtr       0,Close         1,Open
    GpioDataRegs.GPCSET.bit.GPIO72 = 1;         //ACPowerRelayCtr      0,Close         1,Open      
    GpioDataRegs.GPCCLEAR.bit.GPIO74 = 1;       //IntFan out           0,disable       1,active
    GpioDataRegs.GPCCLEAR.bit.GPIO75 = 1;       //RelaySps             0,Iso_disable PwSave_enable    1,Iso_enable PwSave_disable 
    GpioDataRegs.GPCSET.bit.GPIO80 = 1;         //Unlock               0,              1,
    GpioDataRegs.GPCSET.bit.GPIO81 = 1;         //DCRelayCtr           0,close         1,open
    GpioDataRegs.GPCCLEAR.bit.GPIO82 = 1;       //DC1,2CPLDEnable      0,Disable       1,Enable
    GpioDataRegs.GPCCLEAR.bit.GPIO83 = 1;       //InvCpldOutCtr        1,enable        0,disable    
/*** IO DIR select***/  
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;         //AuxiRelayCtr         0,close         1,open  
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;         //DCSps                0,Enable        1,Disable
    GpioCtrlRegs.GPADIR.bit.GPIO17 = 1;         //ADSLECT              0,select a      1,slect b
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;         //NC  
    GpioCtrlRegs.GPADIR.bit.GPIO30 = 1;         //NC   
    GpioCtrlRegs.GPBDIR.bit.GPIO39 = 1;         //GridRelay            0,close         1,open 
    GpioCtrlRegs.GPBDIR.bit.GPIO40 = 1;         //NC
    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 1;         //NC
    GpioCtrlRegs.GPBDIR.bit.GPIO42 = 1;         //DC3,4CPLDEnable      0,Disable       1,Enable    
    GpioCtrlRegs.GPBDIR.bit.GPIO44 = 1;         //NC 
    GpioCtrlRegs.GPBDIR.bit.GPIO45 = 1;         //InvRelay             0,close         1,open 
    GpioCtrlRegs.GPBDIR.bit.GPIO46 = 1;         //DCSps                0,Enable        1,Disable
    GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1;         //ACSps                0,Enable        1,Disable 
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;         //PERelay/GFCICT       0,close         1,open
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;         //NC                   1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;         //NC                   1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;         //NC                   1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;         //NC                   1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 1;         //NC                   1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;         //NC                   1,Output        0,Input
    GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1;         //NC                   1,Output        0,Input
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;         //NC                   1,Output        0,Input
    GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;         //NC                   1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO64 = 1;         //485REorDE            1,Output        0,Input 
    GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;         //NC                   1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1;         //DCRealyNCtr          1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;         //NC                   1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO69 = 1;         //DCSoftRelayCtr       1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO72 = 1;         //ACPowerRelayCtr      1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO74 = 1;         //IntFan               1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO75 = 1;         //RelaySps
    GpioCtrlRegs.GPCDIR.bit.GPIO80 = 1;         //Unlock               1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO81 = 1;         //DCRelayCtr           1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO82 = 1;         //DC1,2CPLDEnable      1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO83 = 1;         //InvCpldOutCtr        1,Output        0,Input 
    
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;         //Mcu feedback         1,Output        0,Input
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;         //ACOCP                1,Output        0,Input
    GpioCtrlRegs.GPADIR.bit.GPIO28 = 0;         //wake up              1,Output        0,Input	
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 0;         //CtrPowerState        1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0;         //IGBTHalt             1,Output        0,Input     
    GpioCtrlRegs.GPBDIR.bit.GPIO37 = 0;         //DCOCP                1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO38 = 0;         //CPLDShutInvPwm       1,Output        0,Input 
    GpioCtrlRegs.GPBDIR.bit.GPIO43 = 0;         //CPLDShutDCPwm        1,Output        0,Input     
    GpioCtrlRegs.GPCDIR.bit.GPIO68 = 0;         //fFanState            1,Output        0,Input          
    GpioCtrlRegs.GPCDIR.bit.GPIO70 = 0;         //fDCRelayState        1,Output        0,Input          
    GpioCtrlRegs.GPCDIR.bit.GPIO71 = 0;         //fSPDState            1,Output        0,Input         
    GpioCtrlRegs.GPCDIR.bit.GPIO73 = 0;         //fBUSOVP              1,Output        0,Input
    GpioCtrlRegs.GPCDIR.bit.GPIO76 = 0;         //Addr                 1,Output        0,Input          
    GpioCtrlRegs.GPCDIR.bit.GPIO77 = 0;         //Addr                 1,Output        0,Input 
    GpioCtrlRegs.GPCDIR.bit.GPIO78 = 0;         //Addr                 1,Output        0,Input           
    GpioCtrlRegs.GPCDIR.bit.GPIO79 = 0;         //Addr                 1,Output        0,Input 
    GpioCtrlRegs.GPCDIR.bit.GPIO84 = 0;         //BootMode switch      1,Output        0,Input          
    GpioCtrlRegs.GPCDIR.bit.GPIO85 = 0;         //BootMode switch      1,Output        0,Input  
    GpioCtrlRegs.GPCDIR.bit.GPIO86 = 0;         //BootMode switch      1,Output        0,Input            
    GpioCtrlRegs.GPCDIR.bit.GPIO87 = 0;         //BootMode switch      1,Output        0,Input
    asm(" EDIS");								// Disable EALLOW protected register access
    
} //end InitGpio()


/*** end of file *****************************************************/

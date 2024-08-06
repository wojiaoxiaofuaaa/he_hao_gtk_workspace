#include "..\FunSrc\Funheader.h"



T_PROTECTOBJECT tErrFlag;

void InitProtectVar(void)
{


}


void sErroringDeal()
{   
	fError = 1;       
    fInvEnOut = 0;     
    fBiDirDCEnOut = 0;
    fDCModeSwitchEn = 0;
    
    sDisBiDirDC1And2CpldOut();
    sDisBiDirDC3And4CpldOut();
    sDisBiDirDC1PWMOut();
    sDisBiDirDC2PWMOut();
    sDisBiDirDC3PWMOut();
    sDisBiDirDC4PWMOut();               
    sDisInvCpldOut();            
    sDisInvPWMOut();     //断开接触器接口。 实际上就是 。 RUN FLAG state的变化。 flag run cmd 和 flag run state 
    if((uwSysMode == InvRunMode)||(uwSysMode == PreCheckMode))
    {
        sDCOCPDisable();
    }
 	sGridRelayOpen();
 	sInvRelayOpen();
 	sAuxiRelayOpen();
    sIsoRelayOpen();    
    sPERelayOpen();
	sDCSoftRelayOpen();  // 变频器和pcs不一样在于可以变频器需要的开关只有一个  pcs需要打开很多开关才能实现 ，第一步要实现的就是 目前处于什么模式？ 
    
    if(wEepromDCRelayActProt == 1)
    {
    	sDCRelayOpenAct();//sDCRelayPOpen();
    }
    fMpptEnable = 0;	
    //fLoadDerating = 0;    
}
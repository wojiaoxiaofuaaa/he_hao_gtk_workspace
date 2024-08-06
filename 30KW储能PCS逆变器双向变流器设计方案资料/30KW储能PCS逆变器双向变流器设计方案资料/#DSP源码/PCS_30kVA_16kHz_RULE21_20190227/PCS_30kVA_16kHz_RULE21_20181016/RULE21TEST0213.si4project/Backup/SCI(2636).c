/**********************************************************************
* File: EPwm.c
* Devices: TMS320F2808, TMS320F2806, TMS320F2801
* Author: Zhou Xu
* History:
*   02/11/05 - original (D. Alter)
**********************************************************************/

#include "..\FunSrc\Funheader.h"

T_SENDDATA tSendData;
T_RECWORDS tReceData[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};    // Receive data for SCI-A
INT16U uwDataSci[9]={1,0,0,1,0,0,0,0,0};//{1,0,0,1,0,0,0,0,0};
T_RECWORDS tScibRxData[6] = {0,0,0,0,0,0};
INT16U uwScibTxData[3]={0,0,0};
INT16U fScibRxOk = 0;
INT16U fScibTxEn = 0;
INT16U uwScibSendNum = 0;
INT16U uwReceFlag = 0;
INT16U uwSendFlag = 0;
INT16U uwSendNum = 0;
INT16U uwSciSendNum = 0;
INT16S *pSciRamData = 0;
//=========================MONITOR======================================
INT8U  ubRxCnt = 0;              //index of received data
INT8U  ubCheckSum = 0;           //sum crc
INT8U  ubRxBuf[SCI_RxCountMax]; //receive buffer
INT16U uwTxCnt=0;                //Transmit data count,data[TxCnt] will be sent
INT8U  ubTxBuf[10]={0xaa,0x55,0,0,0,0,0,0,0,0};
INT32U dwpAddr1,dwpAddr2,dwpAddr3,dwpAddr4;
INT16U uwMonCnt=0;
INT16S ParaU16_Online=0;

T_SWORDOBJECT tChxData[4];
//=========================MONITOR======================================
INT16U uwDebugPvVoltAvg=0,uwDebugPvPower=0,uwDebugDeltaPvPower=0;
#define cHexToDec     640//  =10/16*1024 = 640 
#define cVolt3V       48
#define cCurr2A       32


/**********************************************************************
* Function: InitScic()
* used for communication to Computer
* Description: Initializes the SCIA modules on the F28235.
**********************************************************************/
void InitScia(void)										
{
    Uint16 BRR;
    //================= MONITOR ===========================
    dwpAddr1 = 0x00000000;
    dwpAddr2 = 0x00000000;
    dwpAddr3 = 0x00000000; 
    dwpAddr4 = 0x00000000;
    //==================MONITOR=============================
    
    SciaRegs.SCICCR.bit.STOPBITS = SCI_StopNum;
    if (SCI_Check == 0) SciaRegs.SCICCR.bit.PARITYENA = 0;	
    else if (SCI_Check == 1)
    {
        SciaRegs.SCICCR.bit.PARITYENA = 1;
        SciaRegs.SCICCR.bit.PARITY = 0;
    }
    else
    {
        SciaRegs.SCICCR.bit.PARITYENA = 1;
        SciaRegs.SCICCR.bit.PARITY = 1;
    }
    SciaRegs.SCICCR.bit.SCICHAR = SCI_DataNum;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0;
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
    
    SciaRegs.SCICTL1.all = 0x0003;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;
    
    //================= MONITOR ===========================  
    #if (!SCI_SCOPE) 
        SciaRegs.SCICTL2.bit.TXINTENA = 0;
        BRR = CPUFre/(SysCtrlRegs.LOSPCP.all*2)/8/SCI_BRD - 1;
    #endif
    #if SCI_SCOPE
        SciaRegs.SCICTL2.bit.TXINTENA = 1;
        BRR = CPUFre/(1*1)/8/SCI_BRD - 1;
    #endif
    //================= MONITOR ===========================  
    SciaRegs.SCIHBAUD = BRR>>8;
    SciaRegs.SCILBAUD = BRR&0xFF;
    
    SciaRegs.SCICCR.bit.LOOPBKENA = 0; // TEST MODE 
    //================= MONITOR ===========================
    #if (!SCI_SCOPE)
    SciaRegs.SCIFFTX.all = 0xC040;
    SciaRegs.SCIFFRX.all = 0xC061;
    SciaRegs.SCIFFCT.all = 0x0000;
    
    SciaRegs.SCICTL1.all =0x0063;     // Relinquish SCI from Reset 
    SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;	 
    
    /*** Enable the SCIARX interrupt ***/
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;	// Enable SCIRXINTA in PIE group 9
    IER |= M_INT9;						// Enable INT9 in IER to enable PIE group 
    #endif
    
    #if SCI_SCOPE
    SciaRegs.SCIFFTX.all = 0x8060;//0xC060; // Tx Int Enable
    SciaRegs.SCIFFRX.all = 0xC041;//0xC061; // Rx Int Enable
    SciaRegs.SCIFFCT.all = 0x0000;
    
    SciaRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset 
    SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;//1;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;//1;	 
    
    /*** Enable the SCIARX interrupt ***/
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;	// Enable SCIRXINTA in PIE group 9
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1;	// Enable SCITXINTA in PIE group 9
    IER |= M_INT9;						// Enable INT9 in IER to enable PIE group  	
    #endif  		      									    									
}  

/**********************************************************************
* Function: InitScib()
* use for communications to MCU
* Description: Initializes the SCIA modules on the F28235.
**********************************************************************/
void InitScib(void)	
{
    Uint16 BRR;
    ScibRegs.SCICCR.bit.STOPBITS = SCI_StopNum;
    if (SCI_Check == 0) ScibRegs.SCICCR.bit.PARITYENA = 0;	
    else if (SCI_Check == 1)
    {
        ScibRegs.SCICCR.bit.PARITYENA = 1;
        ScibRegs.SCICCR.bit.PARITY = 0;
    }
    else
    {
        ScibRegs.SCICCR.bit.PARITYENA = 1;
        ScibRegs.SCICCR.bit.PARITY = 1;
    }
    ScibRegs.SCICCR.bit.SCICHAR = SCI_DataNum;
    ScibRegs.SCICCR.bit.LOOPBKENA = 0;
    ScibRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
    
    ScibRegs.SCICTL1.all = 0x0003;
    ScibRegs.SCICTL2.bit.RXBKINTENA = 1;

    ScibRegs.SCICTL2.bit.TXINTENA = 0;
    BRR = CPUFre/(SysCtrlRegs.LOSPCP.all*2)/8/9600 - 1;//9600
  
    ScibRegs.SCIHBAUD = BRR>>8;
    ScibRegs.SCILBAUD = BRR&0xFF;
    
    ScibRegs.SCICCR.bit.LOOPBKENA = 0; // TEST MODE 
    ScibRegs.SCIFFTX.all = 0xC040;
    ScibRegs.SCIFFRX.all = 0xC061;
    ScibRegs.SCIFFCT.all = 0x0000;
        
    ScibRegs.SCICTL1.all =0x0063;     // Relinquish SCI from Reset 
    ScibRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
    ScibRegs.SCIFFRX.bit.RXFIFORESET = 1;	 
  
    /*** Enable the SCIBRX interrupt ***/
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;	// Disable SCIRXINTB in PIE group 9
    IER |= M_INT9;						// Enable INT9 in IER to enable PIE group 
}

/**********************************************************************
* Function: SciaRxIsr()
* used for communication to Computer
* Description: Initializes the SCIA modules on the F28235.
**********************************************************************/
void SciaRxIsr(void)
{   

#if (!SCI_SCOPE)  
static INT16U j=0;
INT16U i=0;
  
   if(SciaRegs.SCIRXST.bit.RXERROR == 1)
   {
        SciaRegs.SCICTL1.bit.SWRESET = 0;     
        asm(" RPT #6 || NOP"); 
        j = 0;                
        SciaRegs.SCICTL1.bit.SWRESET = 1;            

   }
   else
   { 
       i = SciaRegs.SCIRXBUF.all;
	   if(i == 0xAA) 
	   {
	       if(j >= 6) j = 0;
       
	       if((j&1) == 0) {tReceData[j/2].bit.byte1 = i;j++;}
	       else {tReceData[j/2].bit.byte0 = i;j++;}		
       }
	   else
	   {
	       if((j == 1)||(j == 0)) {j = 0;tReceData[0].word = 0;}
	   	   else if(j < 6) 
	   	   {
	   	       if((j&1) == 0) {tReceData[j/2].bit.byte1 = i;j++;}
	           else {tReceData[j/2].bit.byte0 = i;j++;}
	   	   }
	   	   else j=0;
	   }

	if (j == 6) uwReceFlag = 1;
   }	
    
	SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;   // Clear Overflow flag	
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;   // Clear Interrupt flag
#endif

//==================== MONITOR ==================
#if SCI_SCOPE
  
  if(SciaRegs.SCIRXST.bit.RXERROR == 1)
  {
      SciaRegs.SCICTL1.bit.SWRESET = 0;     
      asm(" RPT #6 || NOP"); 
      SciaRegs.SCICTL1.bit.SWRESET = 1;            
  }
  else
  { 
  
      ubRxBuf[ubRxCnt] = SciaRegs.SCIRXBUF.all;//(INT8U)((SciaRegs.SCIRXBUF.all)&0xFF);                //acquire data
 
      if(ubRxCnt<=7)
      {
          ubCheckSum += ubRxBuf[ubRxCnt];  //CRC sum calculation
          ubRxCnt++;
      }
      else
      {  //receive  sum data
          if((ubCheckSum & 0xFF) == ubRxBuf[8]) 
          {            //receive valid command
	          if((ubRxBuf[0] == 0xAA)&&(ubRxBuf[1] == 0xAA))
	          {
	      	      if((ubRxBuf[2] == 0x00)&&(ubRxBuf[3] == 0x52))
	      	      { //Read One Data
	      		      switch(ubRxBuf[6])
	      		      {
	      			      case 0x01:	
	      			          dwpAddr1 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|(ubRxBuf[4]&0xff);
	      			          dwpAddr2 = 0x00000000;
	      			          dwpAddr3 = 0x00000000;
	      			          dwpAddr4 = 0x00000000;	
	      			          break;
	      			      case 0x02:	
	      			          dwpAddr1 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|(ubRxBuf[4]&0xff);
	      			          dwpAddr2 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|((ubRxBuf[4]+0x01)&0xff);
	      			          dwpAddr3 = 0x00000000;
	      			          dwpAddr4 = 0x00000000;
	      			          break;
	      			      case 0x03:	
	      			          dwpAddr1 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|(ubRxBuf[4]&0xff);
	      			          dwpAddr2 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|((ubRxBuf[4]+0x01)&0xff);
	      			          dwpAddr3 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|((ubRxBuf[4]+0x02)&0xff);
	      			          dwpAddr4 = 0x00000000;
	      			          break;
	      			      case 0x04:	
	      			          dwpAddr1 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|(ubRxBuf[4]&0xff);
	      			          dwpAddr2 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|((ubRxBuf[4]+0x01)&0xff);
	      			          dwpAddr3 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|((ubRxBuf[4]+0x02)&0xff);
	      			          dwpAddr4 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|((ubRxBuf[4]+0x03)&0xff);
	      			          break;
                          
	      			      default:  ;
	      		      }
	      		  }
	      		  else if((ubRxBuf[2] == 0x00)&&(ubRxBuf[3] == 0x57))
	      		  {     //Write One Data
	      		      dwpAddr1 = 0x0000ffff&((INT16U)(ubRxBuf[5])<<8)|(ubRxBuf[4]&0xff);
	      		      dwpAddr2 = dwpAddr1;
	      		      dwpAddr3 = dwpAddr1;
	      		      dwpAddr4 = dwpAddr1;	 
	      		      *(int *)dwpAddr1 = ((INT16U)(ubRxBuf[7])<<8)|(ubRxBuf[6]&0xff);
	      		      *(int *)dwpAddr2 = *(int *)dwpAddr1;
	      		      *(int *)dwpAddr3 = *(int *)dwpAddr1;
	      		      *(int *)dwpAddr4 = *(int *)dwpAddr1;
	      		  }
	      		  else
	      		  {
	      		      dwpAddr1 = 0x00000000;
  	      			  dwpAddr2 = 0x00000000;
  	      			  dwpAddr3 = 0x00000000; 
  	      			  dwpAddr4 = 0x00000000;
	      		  }
	      	  }
	      	  else
	      	  {
                  //handle Chn1 address  
                  dwpAddr1 = 0x0000ffff & ((INT16U)(ubRxBuf[1])<<8)|(ubRxBuf[0]&0xff);  //avoid access odd address
                  
                  //handle Chn2 address  
                  dwpAddr2 = 0x0000ffff & ((INT16U)(ubRxBuf[3])<<8)|(ubRxBuf[2]&0xff);  //avoid access odd address
                  
                  //handle Chn3 address
                  dwpAddr3 = 0x0000ffff & ((INT16U)(ubRxBuf[5])<<8)|(ubRxBuf[4]&0xff);  //avoid access odd address
                  
                  //handle Chn4 address
                  dwpAddr4 = 0x0000ffff & ((INT16U)(ubRxBuf[7])<<8)|(ubRxBuf[6]&0xff);  //avoid access odd address
              } 
	          ubCheckSum = 0;
          }
          ubRxCnt = 0;  
      }
  }
#endif	
}


void SCI_TX(INT16U *data,INT16U size)   //size 1~16
{        	
#if (!SCI_SCOPE)     	
static INT16U uwSendFrameCnt = 0;
    if(uwSendFlag == 1)
    {
	    if(size > uwSendFrameCnt)
	    {
			while(SciaRegs.SCICTL2.bit.TXEMPTY == 0 || SciaRegs.SCIFFTX.bit.TXFFST != 0);
	        
            SciaRegs.SCITXBUF = (*(data+uwSendFrameCnt))>>8;   //SCITXBUF是8位的
            SciaRegs.SCITXBUF = (*(data+uwSendFrameCnt))&0xFF;

			uwSendFrameCnt++;
        }
		else
		{
		    uwSendFlag = 0;
		    uwSendFrameCnt = 0;
		}  		   	       
    }
#endif

//================= MONITOR ==============================
#if SCI_SCOPE
    if(uwTxCnt <= 9)
    {	        
         SciaRegs.SCITXBUF = ubTxBuf[uwTxCnt];   //SCITXBUF是8位的
         uwTxCnt++;
    }
    else
    {
         uwTxCnt = 10;
    }
#endif 
	
}

void SciaRxDeal(void)
{
    if(uwReceFlag == 1)
    {
        if(tReceData[0].word == 0xaaaa)  //数据头
        {
            if(tReceData[1].bit.byte1 == 'W')  //W  ASCII码 87
            {
            U_MachineCoeff wMachineCoeffMax,wMachineCoeffMin; 
            U_RatedCoeff wRatedCoeffMax,wRatedCoeffMin; 
		        wMachineCoeffMax.all = wEepromDefaultImptMaxArray[1];
                wMachineCoeffMin.all = wEepromDefaultImptMinArray[1];
                wRatedCoeffMax.all = wEepromDefaultImptMaxArray[2];
                wRatedCoeffMin.all = wEepromDefaultImptMinArray[2];
                    
                if(tReceData[1].bit.byte0 == 0) uwDataSci[0] = tReceData[2].word;   // uwDataSci[0] : fSciRunOrder 
                else if(tReceData[1].bit.byte0 == 1) uwDataSci[1] = tReceData[2].word;
                else if(tReceData[1].bit.byte0 == 2) uwDataSci[2] = tReceData[2].word;
                else if(tReceData[1].bit.byte0 == 3) uwDataSci[3] = tReceData[2].word; 
                else if(tReceData[1].bit.byte0 == 4) uwDataSci[4] = tReceData[2].word;  //被使用了 
                else if(tReceData[1].bit.byte0 == 5) uwDataSci[5] = tReceData[2].word;
                else if(tReceData[1].bit.byte0 == 6) uwDataSci[6] = tReceData[2].word;
                else if(tReceData[1].bit.byte0 == 7) uwDataSci[7] = tReceData[2].word;
                else if(tReceData[1].bit.byte0 == 8) uwDataSci[8] = tReceData[2].word;
                else if((tReceData[1].bit.byte0 >= 10) && (tReceData[1].bit.byte0 <= 16))
		        {
				    if(tReceData[1].bit.byte0 == 11)
					{
					    if((tReceData[2].word <= wMachineCoeffMax.bit.MachineVersion)&&(tReceData[2].word >= wMachineCoeffMin.bit.MachineVersion))
		            	{
		            	    wEepromMachineVersion = tReceData[2].word;
		            	    wEepromMachineConfig = fMachineCoeff.all;
		            	    wLCDImptData[1] = wEepromMachineConfig;
		            	}
					}			
		        	else if(tReceData[1].bit.byte0 != 12)
		        	{		        
		                	wEepromImptData[(tReceData[1].bit.byte0-10)]= tReceData[2].word;
		                	wLCDImptData[(tReceData[1].bit.byte0-10)] = tReceData[2].word;
		                	//写法规，恢复出厂值
		                	if((tReceData[1].bit.byte0 == 16) &&  (wEepromImptData[6] != 0)) 
		                	{	
		                		sEepromProtectDataSetDefaultVaule(84);
		                		//sEepromProtectSupplDataSetDefaultVaule(70);
		                		if(uwI2CBlockState.bit.fEpRdOnPwStep != 2)fSciStandChanged = 1;	
		                	}
		                	fEepromOperateRequire = 1;
		                	fHoldRegisterStatus = 2; 
		            }		            	              
		        }
		        else if(tReceData[1].bit.byte0 == 19)
		        {
		        	if((tReceData[2].word <= wMachineCoeffMax.bit.InverterNum)&&(tReceData[2].word >= wMachineCoeffMin.bit.InverterNum))
		            {
		                wEepromInverterNum = tReceData[2].word;	
		                wEepromMachineConfig = fMachineCoeff.all;            	    
		            }
		        }
		        else if(tReceData[1].bit.byte0 == 20)
		        {
		        	if((tReceData[2].word <= wMachineCoeffMax.bit.GroupNO)&&(tReceData[2].word >= wMachineCoeffMin.bit.GroupNO))
		            {
		                wEepromGroupNO = tReceData[2].word;	
		                wEepromMachineConfig = fMachineCoeff.all;	            	    
		            }
		        }
		        else if((tReceData[1].bit.byte0 >= 21) && (tReceData[1].bit.byte0 <= 25))
		        {
		            if(tReceData[1].bit.byte0 == 21)
		            {
		            	if((tReceData[2].word <= wRatedCoeffMax.bit.RatedFre)&&(tReceData[2].word >= wRatedCoeffMin.bit.RatedFre))
		            	{
		            	    wEepromRatedFreqSel = tReceData[2].word;		            	    
		            	}		            	
		            }
		            else if(tReceData[1].bit.byte0 == 22)
		            {
		            	if((tReceData[2].word <= wRatedCoeffMax.bit.RatedVol)&&(tReceData[2].word >= wRatedCoeffMin.bit.RatedVol))
		            	{
		            	    wEepromRatedVolSel = tReceData[2].word;		            	    
		            	}
		            }
		            else if(tReceData[1].bit.byte0 == 23)
		            {
		            	if((tReceData[2].word <= wRatedCoeffMax.bit.RatedPower)&&(tReceData[2].word >= wRatedCoeffMin.bit.RatedPower))
		            	{
		            	    wEepromRatedPowerSel = tReceData[2].word;		            	    
		            	}
		            }
		            else if(tReceData[1].bit.byte0 == 24)
		            {
		            	if((tReceData[2].word <= wRatedCoeffMax.bit.CurrOVEn)&&(tReceData[2].word >= wRatedCoeffMin.bit.CurrOVEn))
		            	{
		            	    wEepromCurrOVEn = tReceData[2].word;		            	    
		            	}
		            }
		             else if(tReceData[1].bit.byte0 == 25)
		            {
		            	if((tReceData[2].word <= wRatedCoeffMax.bit.PowerOVEn)&&(tReceData[2].word >= wRatedCoeffMin.bit.PowerOVEn))
		            	{
		            	    wEepromPowerOVEn = tReceData[2].word;		            	    
		            	}
		            }
		            wEepromRatedConfig = fRatedCoeff.all;
		            wLCDImptData[2] = wEepromRatedConfig;
		            fEepromOperateRequire = 1;
		        }
		        else if((tReceData[1].bit.byte0 >= 26) && (tReceData[1].bit.byte0 <= 95))
		        {
		        INT16U i,uwSum=0;
		            wEepromProtectSupplData[(tReceData[1].bit.byte0-26)]= tReceData[2].word;
		            for(i=0;i<6;i++)
		            {
		                uwSum += wEepromProtectSupplData[(tReceData[1].bit.byte0-26)/7*7+i];
		            }
		            wEepromProtectSupplData[(tReceData[1].bit.byte0-26)/7*7+6] = uwSum;
		            fEepromOperateRequire = 1;
		            fHoldRegisterStatus = 2;
		        }  
                else if((tReceData[1].bit.byte0 >= 100) && (tReceData[1].bit.byte0 <= 169)) // Only for factory Test, 20100421 
		        {
		        INT16U i,uwSum=0;		         
			        //wSCIWriteEepromAdcAdjustData[(tReceData[1].bit.byte0-100)]= tReceData[2].word;
		            wEepromAdcAdjustData[(tReceData[1].bit.byte0-100)] = tReceData[2].word;						
		            for(i=0;i<6;i++)
		            {
		                uwSum += wEepromAdcAdjustData[(tReceData[1].bit.byte0-100)/7*7+i];
		            }
		            wEepromAdcAdjustData[(tReceData[1].bit.byte0-100)/7*7+6] = uwSum;
		            fEepromOperateRequire = 1;						
		        }
		        else if((tReceData[1].bit.byte0 >= 170) && (tReceData[1].bit.byte0 <= 253))	
		        {
		        INT16U i,uwSum=0;
		            wEepromProtectData[(tReceData[1].bit.byte0-170)]= tReceData[2].word;
		            for(i=0;i<6;i++)
		            {
		                uwSum += wEepromProtectData[(tReceData[1].bit.byte0-170)/7*7+i];
		            }
		            wEepromProtectData[(tReceData[1].bit.byte0-170)/7*7+6] = uwSum;
		            fEepromOperateRequire = 1;
		            fSysCtrSignal.all = wEepromSysCtrSignal;
		            fHoldRegisterStatus = 2;
		        }
	        }
		    else if(tReceData[1].bit.byte1 == 'R') //R ASCII码 82 
			{
			    switch(tReceData[1].bit.byte0)
			    {
			    	case 0:
			    	{
			    	    uwSendNum = 300;
			    	    uwSciSendNum = tReceData[2].bit.byte1;
			    	    uwDataSci[4] = 1;
			    	    break;
			    	} 
			    	case 1:
			    	{
			    	    tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;
                        tSendData.uwData0[2] =  fStartOrNormal; 
                        tSendData.uwData0[3] =  fRunOrder;
                        tSendData.uwData0[4] =  fBatteryInitiStateOK;
                        tSendData.uwData0[5] =  fPhaseSequ;       
                        tSendData.uwData0[6] =  fIsoStatue;   
                        tSendData.uwData0[7] =  fStartByDCOrAC;
                        tSendData.uwData0[8] =  fDCCtrBusSoftSta;    
                        tSendData.uwData0[9] =  fRelayChkRequire;   
                        tSendData.uwData0[10] = fCSVSSwitch;//fRunInvAhdChkRequire;
                        tSendData.uwData0[11] = fVSRunByGrid;//wInvDsCurrLimitHigh;
                        tSendData.uwData0[12] = wInvDsCurrLimitHigh;//-wInvDsCurrLimitLow;
                        tSendData.uwData0[13] = uwFaultCode[0];
                        tSendData.uwData0[14] = uwFaultCode[1];
                        tSendData.uwData0[15] = uwFaultCode[2];
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
                    }  
			    	case 2:
			        {	
			    	    tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;
                        tSendData.uwData0[2] =  fLVRTValue;
                        tSendData.uwData0[3] =  fImbalanceLVRT;
                        tSendData.uwData0[4] =  fLVRTStartUpdate;
                        tSendData.uwData0[5] =  fSoftLVRTEn;       
                        tSendData.uwData0[6] =  fLVRTStartOrEnd;    
                        tSendData.uwData0[7] =  wPOBusAvgVol;
                        tSendData.uwData0[8] =  wONBusAvgVol;  
                        tSendData.uwData0[9] =  tInvPosCurrDqoRef.q;  
                        tSendData.uwData0[10] = tInvNosCurrDqoRef.q;
                        tSendData.uwData0[11] = wIqRefBakToLVRT;
                        tSendData.uwData0[12] = wpuRmsCurrBakToLVRT;//wUdDivBatCoeff;
                        tSendData.uwData0[13] = fFVRTType;
                        tSendData.uwData0[14] = wDebugCPLDShutInv;
                        tSendData.uwData0[15] = fCharge;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
                    }  
			    	case 3:
			        {	
			    	    tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;
                        tSendData.uwData0[2] =  fBatteryReady;
                        tSendData.uwData0[3] =  fUqUnconv;//wBatteryOutAvgVol;
                        tSendData.uwData0[4] =  uwIsoChkStep;
                        tSendData.uwData0[5] =  wIsoAvgVol;      
                        tSendData.uwData0[6] =  fGFCICTStatue; 
                        tSendData.uwData0[7] =  uwfGFCICTChkFinished; 
                        tSendData.uwData0[8] =  uwGFCICTChkStep;    
                        tSendData.uwData0[9] =  wGFCICurrRms;  
                        tSendData.uwData0[10] = wGFCIAvgCurr;
                        tSendData.uwData0[11] = wEepromISOEnable;
                        tSendData.uwData0[12] = wEepromGFCICTChkEn;
                        tSendData.uwData0[13] = uwFaultCode[0];
                        tSendData.uwData0[14] = uwFaultCode[1];
                        tSendData.uwData0[15] = uwFaultCode[2];
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
                    }   
			    	case 4:
			        {	
			    	    tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;
                        tSendData.uwData0[2] =  wBatteryInAvgVol; 
                        tSendData.uwData0[3] =  wIsoRlyOpenVngAvgTemp;
                        tSendData.uwData0[4] =  wIsoRlyCloseVngAvgTemp;
                        tSendData.uwData0[5] =  wStdBusVolRef;      
                        tSendData.uwData0[6] =  wDCCtrBusVolRef;  
                        tSendData.uwData0[7] =  fDCModeSwitchEn; 
                        tSendData.uwData0[8] =  fDCModeSwitch;    
                        tSendData.uwData0[9] =  fBiDirDCEnOut;  
                        tSendData.uwData0[10] = uwRFreq;
                        tSendData.uwData0[11] = uwSFreq;
                        tSendData.uwData0[12] = uwTFreq;
                        tSendData.uwData0[13] = fACPowerCtr;//wDebugTest5;//wEnvFitTemp;
                        tSendData.uwData0[14] = wDebugTest6;//wHeatFitTemp;
                        tSendData.uwData0[15] = wIsoImp;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
                    }                                                 
			    	case 5:
			    	{
			    		tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;
                        tSendData.uwData0[2] =  fDCModeSwitchEn;
                        tSendData.uwData0[3] =  fDCModeSwitch;
                        tSendData.uwData0[4] =  fBiDirDCEnOut;
                        tSendData.uwData0[5] =  wBatteryInAvgVol;   
                        tSendData.uwData0[6] =  wBatteryAvgCurr;
                        tSendData.uwData0[7] =  wDCCtrBusVolRef; 
                        tSendData.uwData0[8] =  wInvCtrBusVolRef;    
                        tSendData.uwData0[9] =  wBatteryPortVolt;//wBusAvgVol;  
                        tSendData.uwData0[10] = wDebugDeltaDCCurr;  
                        tSendData.uwData0[11] = wDebugDeltaDCCurrCharge;
                        tSendData.uwData0[12] = wDebugDeltaDCCurrDisCharge;//wUdDivBatCoeff;
                        tSendData.uwData0[13] = uwFaultCode[0];
                        tSendData.uwData0[14] = uwFaultCode[1];
                        tSendData.uwData0[15] = uwFaultCode[2];
                        uwSendNum = 16;
                        uwSendFlag = 1;  
                        break;
			    	}
			    	case 6:
			    	{
			    		tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wDCCtrBusVolRef;
                        tSendData.uwData0[2] =  wBusAvgVol;
                        tSendData.uwData0[3] =  wDebugInvDsCurrRef;
                        tSendData.uwData0[4] =  wDebugInvDCCurrRef;
                        tSendData.uwData0[5] =  wDebugTest6;//wDCCurrSet;//wInvCurrResolving;//wEepromRemoteSel;     
                        tSendData.uwData0[6] =  fActiveMode;
                        tSendData.uwData0[7] =  wDCCurrAim;
                        tSendData.uwData0[8] =  wActivePowerPer; 
                        tSendData.uwData0[9] =  uwpuApparentPowerMax;                 
                        tSendData.uwData0[10] = uwpuReactivePowerMax; 
                        tSendData.uwData0[11] = dwActivePowerAim>>4; 
                        tSendData.uwData0[12] = tInvCurrDqo.d;//wInvDsCurrLimitHigh;                 
                        tSendData.uwData0[13] = tInvCurrDqo.q;//wInvDsCurrLimitLow;
                        tSendData.uwData0[14] = tInvCurrDqoRef.d;
                        tSendData.uwData0[15] = tInvCurrDqoRef.q;//fCharge;//tInvCurrDqoRef.q;
                        uwSendNum = 16;
                        uwSendFlag = 1;  
                        break;
			    	}
			    	
			    	case 7:
			    	{
			    		tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;
                        tSendData.uwData0[2] =  wDebugAIStart;
                        tSendData.uwData0[3] =  wDebugAIEnd;
                        tSendData.uwData0[4] =  wDebugAIDeltaIqRef;
                        tSendData.uwData0[5] =  uwRFreq;     
                        tSendData.uwData0[6] =  wStdBusVolRef;
                        tSendData.uwData0[7] =  wDCCtrBusVolRef;     
                        tSendData.uwData0[8] =  wInvCtrBusVolRef;    
                        tSendData.uwData0[9] =  wBatteryInAvgVol;               
                        tSendData.uwData0[10] = fDCModeSwitchEn; 
                        tSendData.uwData0[11] = fDCModeSwitch; 
                        tSendData.uwData0[12] = fBiDirDCEnOut;                 
                        tSendData.uwData0[13] = uwFaultCode[0];
                        tSendData.uwData0[14] = uwFaultCode[1];
                        tSendData.uwData0[15] = uwFaultCode[2];
                        uwSendNum = 16;
                        uwSendFlag = 1;  
                        break;
			    	}
			        case 8:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  fCANActive;           //1,Open         0,Close
                        tSendData.uwData0[2] =  fDCRelayState;        //1,Close        0,Open
                        tSendData.uwData0[3] =  fMCUActState;         //1,Open         0,Close
                        tSendData.uwData0[4] =  fDCOCPEnable;//fWakeUp;              //1,Fault        0,OK
                        tSendData.uwData0[5] =  fCtrPowerState;       //1,OK           0,Fault
                        tSendData.uwData0[6] =  fIGBTHalt;            //1,OK           0,Open
                        tSendData.uwData0[7] =  fDCOCPState;          //1,OK           0,Fault
                        tSendData.uwData0[8] =  fCPLDShutInvPWM;      //1,OK           0,Sighle
                        tSendData.uwData0[9] =  fCPLDShutDCPWM;       //1,OK           0,Sighle
                        tSendData.uwData0[10] = fACPowerCtr;//fArcingState;         //1,OVP          0,NO OVP
                        tSendData.uwData0[11] = fFanState;            //1,Fault        0,OK
                        tSendData.uwData0[12] = fATSState;            //1,Fault        0,OK
                        tSendData.uwData0[13] = fBUSOVPState;         //1,Fault        0,OK
                        tSendData.uwData0[14] = fPhyAddrState;        //1,Fault        0,OK
                        tSendData.uwData0[15] = fACOCPState;          //1,Fault        0,OK
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 9:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wActivePowerPer;
                        tSendData.uwData0[2] =  fActiveMode;
                        tSendData.uwData0[3] =  wDCCurrSet;
                        tSendData.uwData0[4] =  wReactivePowerPer;
                        tSendData.uwData0[5] =  fReactiveMode;
                        tSendData.uwData0[6] =  wInvDsCurrLimitHigh;
                        tSendData.uwData0[7] =  wInvDsCurrLimitLow;
                        tSendData.uwData0[8] =  wBatteryOutAvgVol;
                        tSendData.uwData0[9] =  wBatteryInAvgVol;
                        tSendData.uwData0[10] = wBatVolLimtHigh;
                        tSendData.uwData0[11] = wBatVolLimtLow;
                        tSendData.uwData0[12] = fHotBackup;
                        tSendData.uwData0[13] = uwFaultCode[0];
                        tSendData.uwData0[14] = uwFaultCode[1];
                        tSendData.uwData0[15] = uwFaultCode[2];
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
					}
			        case 0xA:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wRInvRmsCurr;     
                        tSendData.uwData0[2] =  wSInvRmsCurr;    
                        tSendData.uwData0[3] =  wTInvRmsCurr;
                        tSendData.uwData0[4] =  (INT16S)(((INT32S)((INT32S)wRInvRmsCurr + wSInvRmsCurr + wTInvRmsCurr)*5461)>>14);
                        tSendData.uwData0[5] =  wBusAvgVol;       
                        tSendData.uwData0[6] =  wRInvRmsVol;
                        tSendData.uwData0[7] =  wSInvRmsVol;
                        tSendData.uwData0[8] =  wTInvRmsVol;
                        tSendData.uwData0[9] =  wBatteryInAvgVol;
                        tSendData.uwData0[10] = wBatteryOutAvgVol;
                        tSendData.uwData0[11] = wRSGridRmsVol;
                        tSendData.uwData0[12] = wSTGridRmsVol;
                        tSendData.uwData0[13] = wTRGridRmsVol;
                        tSendData.uwData0[14] = wPOBusAvgVol;
                        tSendData.uwData0[15] = wONBusAvgVol;
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0xB:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wBatteryAvgCurr;     
                        tSendData.uwData0[2] =  wBatteryAvgCurr1;    
                        tSendData.uwData0[3] =  wBatteryAvgCurr2;
                        tSendData.uwData0[4] =  wBatteryAvgCurr3;
                        tSendData.uwData0[5] =  wBatteryAvgCurr4;       
                        tSendData.uwData0[6] =  uwSysFreq;
                        tSendData.uwData0[7] =  fCharge;
                        tSendData.uwData0[8] =  wDebugCPLDShutDC;
                        tSendData.uwData0[9] =  wDebugTest4;
                        tSendData.uwData0[10] = wDebugTest5;
                        tSendData.uwData0[11] = wInvCurrResolving;//wRatedInvCurrInv;
                        tSendData.uwData0[12] = dwRatedPower;
                        tSendData.uwData0[13] = wRatedGridLineVol;
                        tSendData.uwData0[14] = wInvCurrPeakMax;
                        tSendData.uwData0[15] = wRatedInvCurrPeak;
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0xC:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  fReactiveMode;                       
                        tSendData.uwData0[3] =  wPFSet;              
                        tSendData.uwData0[4] =  wPowerFactorAvg[3];                
                        tSendData.uwData0[5] =  wReactivePowerPer;                    
                        tSendData.uwData0[6] =  dwReactivePowerAim>>8;              
                        tSendData.uwData0[7] =  dwInvReactivePower[3]>>8;       
                        tSendData.uwData0[8] =  wDebugTest2;
                        tSendData.uwData0[9] =  wDebugTest1;
                        tSendData.uwData0[10] = tInvCurrPowerReguRefDqo.q;
                        tSendData.uwData0[11] = tInvCurrDqoRef.q;  
                        tSendData.uwData0[12] = wAdjustStep;       
                        tSendData.uwData0[13] = fAdjustQEn;      
                        tSendData.uwData0[14] = wAdjustPF; 
                        tSendData.uwData0[15] = wReactiveCurrErr;          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }	
			        case 0xD:
			        {
			        	tSendData.uwData0[0] = 0xaaaa;
                        tSendData.uwData0[1] = uwSysMode;                            
                        tSendData.uwData0[2] = fDCCtrBusSoftSta;                       
                        tSendData.uwData0[3] = wStdBusVolRef;              
                        tSendData.uwData0[4] = fDCModeSwitchEn;                
                        tSendData.uwData0[5] = fDCModeSwitch;                    
                        tSendData.uwData0[6] = fBiDirDCEnOut;              
                        tSendData.uwData0[7] = wBusAvgVol;       
                        tSendData.uwData0[8] = fRelayChkRequire;
                        tSendData.uwData0[9] = fRelayCheckStep;
                        tSendData.uwData0[10] = fRunInvAhdChkRequire;
                        tSendData.uwData0[11] = fRunInvAhdChkRestart;
                        tSendData.uwData0[12] = wDebugRunInvChkStep;     
                        tSendData.uwData0[13] =fDCPWMLostPreChk;// tInvVolDqo.d;//uwFaultCode[0];      
                        tSendData.uwData0[14] =fACPWMLostPreChk;//  tInvVolDqo.q;//uwFaultCode[1];
                        tSendData.uwData0[15] = uwFaultCode[3];          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }	
			        case 0xe:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  dwInvActivePower[3]>>4;                       
                        tSendData.uwData0[3] =  dwInvReactivePower[3]>>4;              
                        tSendData.uwData0[4] =  udwApparentPower[3]>>4;                
                        tSendData.uwData0[5] =  wPowerFactorAvg[3];              
                        tSendData.uwData0[6] =  dwDCPower>>4;       
                        tSendData.uwData0[7] =  wBatteryDisChargeCurr; 
                        tSendData.uwData0[8] =  -wBatteryChargeCurr;
                        tSendData.uwData0[9] =  wInvDsCurrLimitHigh;
                        tSendData.uwData0[10] = wInvDsCurrLimitLow;
                        tSendData.uwData0[11] = tInvCurrDqoRef.d;
                        tSendData.uwData0[12] = tInvCurrDqoRef.q;       
                        tSendData.uwData0[13] = wBatteryLimitCurrHigh;      
                        tSendData.uwData0[14] = -wBatteryLimitCurrLow; 
                        tSendData.uwData0[15] = fPhaseSequBak;          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0xf:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  fRunInvAhdChkRestart;                       
                        tSendData.uwData0[3] =  wDebugRunInvChkStep;              
                        tSendData.uwData0[4] =  uwDCSoftStep;                                                     
                        tSendData.uwData0[5] =  wStdBusVolRef;       
                        tSendData.uwData0[6] =  wDCCtrBusVolRef; 
                        tSendData.uwData0[7] =  wInvCtrBusVolRef;
                        tSendData.uwData0[8] =  wBusAvgVol;
                        tSendData.uwData0[9] =  wBatteryInAvgVol;
                        tSendData.uwData0[10] = wBatteryOutAvgVol; 
                        tSendData.uwData0[11] = fDCModeSwitch;
                        tSendData.uwData0[12] = fInvEnOut;//wDCPwmCarrFrwd;//fStartByDCOrAC;//tInvVolDqo.q;       
                        tSendData.uwData0[13] = uwFaultCode[0];      
                        tSendData.uwData0[14] = uwFaultCode[1]; 
                        tSendData.uwData0[15] = uwFaultCode[2];          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x10:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  wEepromRemoteSel;                       
                        tSendData.uwData0[3] =  fRemotePCSComdOn;              
                        tSendData.uwData0[4] =  fLocalPCSComdOnOff;                
                        tSendData.uwData0[5] =  fCANRunOrder;                    
                        tSendData.uwData0[6] =  fSciRunOrder;              
                        tSendData.uwData0[7] =  fDisBootloaderOrder;       
                        tSendData.uwData0[8] =  fRunOrder;
                        tSendData.uwData0[9] =  fDeCapEnergy;
                        tSendData.uwData0[10] = dwGFCIAvgActSum;
                        tSendData.uwData0[11] = dwGFCIRmsActSum;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x11:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  uwHalfCycSampNum;                       
                        tSendData.uwData0[3] =  uwHalfCysRatedVolSqure;              
                        tSendData.uwData0[4] =  udwHalfCysRatedVolSqure0R81>>13;                
                        tSendData.uwData0[5] =  udwHalfCysRatedVolSqure0R2>>13;                    
                        tSendData.uwData0[6] =  udwHalfCysRatedVolSqure0R01>>13;              
                        tSendData.uwData0[7] =  cNotch8KNum1;       
                        tSendData.uwData0[8] =  cNotch8KNum2;
                        tSendData.uwData0[9] =  cNotch8KNum3;
                        tSendData.uwData0[10] = uwDECapEnergyStep;
                        tSendData.uwData0[11] = fPhaseSequBak;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x12:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  uwSysModePrepare;                       
                        tSendData.uwData0[3] =  fDisBootloaderOrder;              
                        tSendData.uwData0[4] =  uwI2CBlockState.bit.fEpRdOnPwStep;                
                        tSendData.uwData0[5] =  uwPowerEffAvg;                    
                        tSendData.uwData0[6] =  GpioDataRegs.GPBDAT.bit.GPIO40;//fRunInvAhdChkRequire;              
                        tSendData.uwData0[7] =  fInvLittleVolOk;       
                        tSendData.uwData0[8] =  wBatteryLimitCurrHigh;
                        tSendData.uwData0[9] =  wBatteryLimitCurrLow;
                        tSendData.uwData0[10] = wDebugPLLDAxis;
                        tSendData.uwData0[11] = wDebugPLLQAxis;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x13:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  dwInvActivePower[0]>>4;                            
                        tSendData.uwData0[2] =  dwInvActivePower[1]>>4;                       
                        tSendData.uwData0[3] =  dwInvActivePower[2]>>4;              
                        tSendData.uwData0[4] =  dwInvActivePower[3]>>4;                
                        tSendData.uwData0[5] =  dwInvReactivePower[0]>>4;                    
                        tSendData.uwData0[6] =  dwInvReactivePower[1]>>4;              
                        tSendData.uwData0[7] =  dwInvReactivePower[2]>>4;       
                        tSendData.uwData0[8] =  dwInvReactivePower[3]>>4;
                        tSendData.uwData0[9] =  udwApparentPower[0]>>4;
                        tSendData.uwData0[10] = udwApparentPower[1]>>4;
                        tSendData.uwData0[11] = udwApparentPower[2]>>4;
                        tSendData.uwData0[12] = udwApparentPower[3]>>4;       
                        tSendData.uwData0[13] = uwFaultCode[0];      
                        tSendData.uwData0[14] = uwFaultCode[1]; 
                        tSendData.uwData0[15] = uwFaultCode[2];          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }			        
			        case 0x14:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  fDCRelayPCtr;                       
                        tSendData.uwData0[3] =  fGridRelayCtr;              
                        tSendData.uwData0[4] =  fDCRelayBurn;                
                        tSendData.uwData0[5] =  fAuxiRelayLost;                    
                        tSendData.uwData0[6] =  fDCSoftRelayCtr;              
                        tSendData.uwData0[7] =  wBatteryOutAvgVol;       
                        tSendData.uwData0[8] =  fDCPWMLost;
                        tSendData.uwData0[9] =  fDCSoftRelayLost;
                        tSendData.uwData0[10] = uwDCSoftRelayDelay;
                        tSendData.uwData0[11] = uwAuxiRelayDelay;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];         
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x15:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  fRelayChkRequire;                       
                        tSendData.uwData0[3] =  fRelayCheckStep;              
                        tSendData.uwData0[4] =  wBusAvgVol;                
                        tSendData.uwData0[5] =  wRInvRmsVol - abs(wRInvAvgVol);                    
                        tSendData.uwData0[6] =  wSInvRmsVol - abs(wSInvAvgVol);              
                        tSendData.uwData0[7] =  wTInvRmsVol - abs(wTInvAvgVol);       
                        tSendData.uwData0[8] =  wRInvAvgVol;
                        tSendData.uwData0[9] =  wSInvAvgVol;
                        tSendData.uwData0[10] = wTInvAvgVol;
                        tSendData.uwData0[11] = fAuxiRelayCtr;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];//uwPFCode;          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }	
			        case 0x16:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wBatChargeVoltLimt;                            
                        tSendData.uwData0[2] =  wEepromChargeVoltLimt;                       
                        tSendData.uwData0[3] =  wSOCChargeVoltLimit;              
                        tSendData.uwData0[4] =  wBatDisChargeVoltLimt;                
                        tSendData.uwData0[5] =  wEepromDisChargeVoltLimt;                    
                        tSendData.uwData0[6] =  wSOCDisChargeVoltLimit;              
                        tSendData.uwData0[7] =  wBatChargeCurrLimt;       
                        tSendData.uwData0[8] =  wEepromChargeCurrLimt;
                        tSendData.uwData0[9] =  wSOCChargeCurrLimt;
                        tSendData.uwData0[10] = wBatDisChargeCurrLimt;
                        tSendData.uwData0[11] = wEepromDisChargeCurrLimt;
                        tSendData.uwData0[12] = wSOCDisChargeCurrLimit;       
                        tSendData.uwData0[13] = wBatFloatChargeVolt;      
                        tSendData.uwData0[14] = wEepromFloatChargeVolt; 
                        tSendData.uwData0[15] = wSOCFloatChargeVolt;         
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x17:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  fCSVSSwitch;                       
                        tSendData.uwData0[3] =  wVSVoltSet;              
                        tSendData.uwData0[4] =  wDebugCPLDShutInv;//wVSFreqSet;                
                        tSendData.uwData0[5] =  wVoltDAim;                   
                        tSendData.uwData0[6] =  wVoltQAim;             
                        tSendData.uwData0[7] =  fGridChkValue;    
                        tSendData.uwData0[8] =  wVSIdPrev;//wPOBusAvgVol;
                        tSendData.uwData0[9] =  wVSIqPrev;//wONBusAvgVol;
                        tSendData.uwData0[10] = fATSState;//wInvDisChargeCurrMax;
                        tSendData.uwData0[11] = fQFMRelayCtr;//wInvChargeCurrMin;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];         
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x18:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  fCSVSSwitch;                       
                        tSendData.uwData0[3] =  0;              
                        tSendData.uwData0[4] =  dwDampD;                
                        tSendData.uwData0[5] =  dwDroopKQ;                   
                        tSendData.uwData0[6] =  wDebugTest4;              
                        tSendData.uwData0[7] =  tVSInvCurrDqoRef.d;       
                        tSendData.uwData0[8] =  tVSInvCurrDqoRef.q;
                        tSendData.uwData0[9] =  wTorqueM;
                        tSendData.uwData0[10] = wTorqueE;
                        tSendData.uwData0[11] = wTorqueD;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];         
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x19:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode;                            
                        tSendData.uwData0[2] =  fCSVSSwitch;                       
                        tSendData.uwData0[3] =  wDebugTest3;              
                        tSendData.uwData0[4] =  wDebugTest5;                
                        tSendData.uwData0[5] =  wDebugTest1;                   
                        tSendData.uwData0[6] =  wDebugTest2;              
                        tSendData.uwData0[7] =  fActiveMode;       
                        tSendData.uwData0[8] =  wActivePowerPer;
                        tSendData.uwData0[9] =  fPStepSwitch;
                        tSendData.uwData0[10] = fQStepSwitch;
                        tSendData.uwData0[11] = fFreOvStartOrEnd;
                        tSendData.uwData0[12] = uwFaultCode[0];       
                        tSendData.uwData0[13] = uwFaultCode[1];      
                        tSendData.uwData0[14] = uwFaultCode[2]; 
                        tSendData.uwData0[15] = uwFaultCode[3];         
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }        
			        case 0x31:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wLCDImptData[0];                            
                        tSendData.uwData0[2] =  wLCDImptData[1];                       
                        tSendData.uwData0[3] =  wLCDImptData[2];              
                        tSendData.uwData0[4] =  wLCDImptData[3];                
                        tSendData.uwData0[5] =  wLCDImptData[4];                    
                        tSendData.uwData0[6] =  wLCDImptData[5];              
                        tSendData.uwData0[7] =  wEepromBlockMap;       
                        tSendData.uwData0[8] =  wEepromMachineVersion;
                        tSendData.uwData0[9] =  wEepromSerialNumberH;
                        tSendData.uwData0[10] = wEepromSerialNumberM;
                        tSendData.uwData0[11] = wEepromSerialNumberL;
                        tSendData.uwData0[12] = wEepromStandardValue;       
                        tSendData.uwData0[13] = uwI2CBlockState.bit.fNoStandardStatus;      
                        tSendData.uwData0[14] = uwI2CBlockState.bit.fEpErr; 
                        tSendData.uwData0[15] = uwI2CBlockState.bit.fEpRdOnPwStep;          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x32:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wInputRegister[0];                        
                        tSendData.uwData0[2] =  wInputRegister[1];                       
                        tSendData.uwData0[3] =  wInputRegister[2];              
                        tSendData.uwData0[4] =  wInputRegister[3];                
                        tSendData.uwData0[5] =  wInputRegister[4];                    
                        tSendData.uwData0[6] =  wInputRegister[5];              
                        tSendData.uwData0[7] =  wInputRegister[6];       
                        tSendData.uwData0[8] =  wInputRegister[7];
                        tSendData.uwData0[9] =  wInputRegister[8];
                        tSendData.uwData0[10] = wInputRegister[9];
                        tSendData.uwData0[11] = wInputRegister[10];
                        tSendData.uwData0[12] = wInputRegister[11];       
                        tSendData.uwData0[13] = wInputRegister[12];      
                        tSendData.uwData0[14] = wInputRegister[13]; 
                        tSendData.uwData0[15] = wInputRegister[14];          
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }			        				    	
			        case 0x33:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wInputRegister[15];
                        tSendData.uwData0[2] =  wInputRegister[16];
                        tSendData.uwData0[3] =  wInputRegister[17];
                        tSendData.uwData0[4] =  wInputRegister[18];
                        tSendData.uwData0[5] =  wInputRegister[19];       
                        tSendData.uwData0[6] =  wInputRegister[20];    
                        tSendData.uwData0[7] =  wInputRegister[21];				    
                        tSendData.uwData0[8] =  wInputRegister[22];        
                        tSendData.uwData0[9] =  wInputRegister[23];       
                        tSendData.uwData0[10] = wInputRegister[24];
                        tSendData.uwData0[11] = wInputRegister[25]; 
                        tSendData.uwData0[12] = wInputRegister[26]; 
                        tSendData.uwData0[13] = wInputRegister[27]; 
                        tSendData.uwData0[14] = wInputRegister[28]; 
                        tSendData.uwData0[15] = wInputRegister[29]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x34:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wInputRegister[30];
                        tSendData.uwData0[2] =  wInputRegister[31];
                        tSendData.uwData0[3] =  wInputRegister[32];
                        tSendData.uwData0[4] =  wInputRegister[33];
                        tSendData.uwData0[5] =  wInputRegister[34];       
                        tSendData.uwData0[6] =  wInputRegister[35];    
                        tSendData.uwData0[7] =  wInputRegister[36];				    
                        tSendData.uwData0[8] =  wInputRegister[37];        
                        tSendData.uwData0[9] =  wInputRegister[38];       
                        tSendData.uwData0[10] = wInputRegister[39];
                        tSendData.uwData0[11] = wInputRegister[40]; 
                        tSendData.uwData0[12] = wInputRegister[41]; 
                        tSendData.uwData0[13] = wInputRegister[42]; 
                        tSendData.uwData0[14] = wInputRegister[43]; 
                        tSendData.uwData0[15] = wInputRegister[44]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x35:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wInputRegister[45];  
                        tSendData.uwData0[2] =  wInputRegister[46];	
                        tSendData.uwData0[3] =  wInputRegister[47];  
                        tSendData.uwData0[4] =  wInputRegister[48];  
                        tSendData.uwData0[5] =  wInputRegister[49];         
                        tSendData.uwData0[6] =  wInputRegister[50];      
                        tSendData.uwData0[7] =  wInputRegister[51];  				    
                        tSendData.uwData0[8] =  wInputRegister[52];          
                        tSendData.uwData0[9] =  wInputRegister[53];         
                        tSendData.uwData0[10] = wRegistercheckFlag;        					    
                        tSendData.uwData0[11] = wDebugFlashSum; 
                        tSendData.uwData0[12] = wEepromMachineConfig; 
                        tSendData.uwData0[13] = wEepromRatedConfig; 
                        tSendData.uwData0[14] = wEepromInverterNum; 
                        tSendData.uwData0[15] = wEepromGroupNO; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x36:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[0];  
                        tSendData.uwData0[2] =  wHoldRegister[1];	
                        tSendData.uwData0[3] =  wHoldRegister[2];  
                        tSendData.uwData0[4] =  wHoldRegister[3];  
                        tSendData.uwData0[5] =  wHoldRegister[4];         
                        tSendData.uwData0[6] =  wHoldRegister[5];      
                        tSendData.uwData0[7] =  wHoldRegister[6];  				    
                        tSendData.uwData0[8] =  wHoldRegister[7];          
                        tSendData.uwData0[9] =  wHoldRegister[8];         
                        tSendData.uwData0[10] = wHoldRegister[9];        					    
                        tSendData.uwData0[11] = wHoldRegister[10]; 
                        tSendData.uwData0[12] = wHoldRegister[11];  
                        tSendData.uwData0[13] = wHoldRegister[12];  
                        tSendData.uwData0[14] = wHoldRegister[13];  
                        tSendData.uwData0[15] = wHoldRegister[14];  
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x37:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[15];  
                        tSendData.uwData0[2] =  wHoldRegister[16];	
                        tSendData.uwData0[3] =  wHoldRegister[17];  
                        tSendData.uwData0[4] =  wHoldRegister[18];  
                        tSendData.uwData0[5] =  wHoldRegister[19];         
                        tSendData.uwData0[6] =  wHoldRegister[20];     
                        tSendData.uwData0[7] =  wHoldRegister[21]; 				    
                        tSendData.uwData0[8] =  wHoldRegister[22];         
                        tSendData.uwData0[9] =  wHoldRegister[23];        
                        tSendData.uwData0[10] = wHoldRegister[24];       					    
                        tSendData.uwData0[11] = wHoldRegister[25]; 
                        tSendData.uwData0[12] = wHoldRegister[26];  
                        tSendData.uwData0[13] = wHoldRegister[27];  
                        tSendData.uwData0[14] = wHoldRegister[28];  
                        tSendData.uwData0[15] = wHoldRegister[29];  
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x38:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[30];  
                        tSendData.uwData0[2] =  wHoldRegister[31];	
                        tSendData.uwData0[3] =  wHoldRegister[32];  
                        tSendData.uwData0[4] =  wHoldRegister[33];  
                        tSendData.uwData0[5] =  wHoldRegister[34];         
                        tSendData.uwData0[6] =  wHoldRegister[35];     
                        tSendData.uwData0[7] =  wHoldRegister[36]; 				    
                        tSendData.uwData0[8] =  wHoldRegister[37];         
                        tSendData.uwData0[9] =  wHoldRegister[38];        
                        tSendData.uwData0[10] = wHoldRegister[39];       					    
                        tSendData.uwData0[11] = wHoldRegister[40]; 
                        tSendData.uwData0[12] = wHoldRegister[41];  
                        tSendData.uwData0[13] = wHoldRegister[42];  
                        tSendData.uwData0[14] = wHoldRegister[43];  
                        tSendData.uwData0[15] = wHoldRegister[44];  
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x39:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[45];  
                        tSendData.uwData0[2] =  wHoldRegister[46];	
                        tSendData.uwData0[3] =  wHoldRegister[47];  
                        tSendData.uwData0[4] =  wHoldRegister[48];  
                        tSendData.uwData0[5] =  wHoldRegister[49];         
                        tSendData.uwData0[6] =  wHoldRegister[50];     
                        tSendData.uwData0[7] =  wHoldRegister[51]; 				    
                        tSendData.uwData0[8] =  wHoldRegister[52];         
                        tSendData.uwData0[9] =  wHoldRegister[53];        
                        tSendData.uwData0[10] = wHoldRegister[54];       					    
                        tSendData.uwData0[11] = wHoldRegister[55];
                        tSendData.uwData0[12] = wHoldRegister[56]; 
                        tSendData.uwData0[13] = wHoldRegister[57]; 
                        tSendData.uwData0[14] = wHoldRegister[58]; 
                        tSendData.uwData0[15] = wHoldRegister[59]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x3A:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[60];  
                        tSendData.uwData0[2] =  wHoldRegister[61];	
                        tSendData.uwData0[3] =  wHoldRegister[62];  
                        tSendData.uwData0[4] =  wHoldRegister[63];  
                        tSendData.uwData0[5] =  wHoldRegister[64];         
                        tSendData.uwData0[6] =  wHoldRegister[65];     
                        tSendData.uwData0[7] =  wHoldRegister[66]; 				    
                        tSendData.uwData0[8] =  wHoldRegister[67];         
                        tSendData.uwData0[9] =  wHoldRegister[68];        
                        tSendData.uwData0[10] = wHoldRegister[69]; 
                        tSendData.uwData0[11] = wHoldRegister[70];
                        tSendData.uwData0[12] = wHoldRegister[71];
                        tSendData.uwData0[13] = wHoldRegister[72];
                        tSendData.uwData0[14] = wHoldRegister[73]; 
                        tSendData.uwData0[15] = wHoldRegister[74]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x3B:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[75];  
                        tSendData.uwData0[2] =  wHoldRegister[76];	
                        tSendData.uwData0[3] =  wHoldRegister[77];  
                        tSendData.uwData0[4] =  wHoldRegister[78];  
                        tSendData.uwData0[5] =  wHoldRegister[79];         
                        tSendData.uwData0[6] =  wHoldRegister[80];     
                        tSendData.uwData0[7] =  wHoldRegister[81]; 				    
                        tSendData.uwData0[8] =  wHoldRegister[82];         
                        tSendData.uwData0[9] =  wHoldRegister[83];        
                        tSendData.uwData0[10] = wHoldRegister[84];
                        tSendData.uwData0[11] = wHoldRegister[85];
                        tSendData.uwData0[12] = wHoldRegister[86];
                        tSendData.uwData0[13] = wHoldRegister[87];
                        tSendData.uwData0[14] = wHoldRegister[88]; 
                        tSendData.uwData0[15] = wHoldRegister[89]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x3C:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[90];  
                        tSendData.uwData0[2] =  wHoldRegister[91];	
                        tSendData.uwData0[3] =  wHoldRegister[92];  
                        tSendData.uwData0[4] =  wHoldRegister[93];  
                        tSendData.uwData0[5] =  wHoldRegister[94];         
                        tSendData.uwData0[6] =  wHoldRegister[95];     
                        tSendData.uwData0[7] =  wHoldRegister[96]; 				    
                        tSendData.uwData0[8] =  wHoldRegister[97];         
                        tSendData.uwData0[9] =  wHoldRegister[98];        
                        tSendData.uwData0[10] = wHoldRegister[99];       					    
                        tSendData.uwData0[11] = wHoldRegister[100];
                        tSendData.uwData0[12] = wHoldRegister[101];
                        tSendData.uwData0[13] = wHoldRegister[102];
                        tSendData.uwData0[14] = wHoldRegister[103]; 
                        tSendData.uwData0[15] = wHoldRegister[104]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x3D:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[105];    
                        tSendData.uwData0[2] =  wHoldRegister[106]; 	
                        tSendData.uwData0[3] =  wHoldRegister[107];    
                        tSendData.uwData0[4] =  wHoldRegister[108];    
                        tSendData.uwData0[5] =  wHoldRegister[109];           
                        tSendData.uwData0[6] =  wHoldRegister[110];      
                        tSendData.uwData0[7] =  wHoldRegister[111];
                        tSendData.uwData0[8] =  wHoldRegister[112];
                        tSendData.uwData0[9] =  wHoldRegister[113];
                        tSendData.uwData0[10] = wHoldRegister[114];
                        tSendData.uwData0[11] = wHoldRegister[115];
                        tSendData.uwData0[12] = wHoldRegister[116];
                        tSendData.uwData0[13] = wHoldRegister[117];
                        tSendData.uwData0[14] = wHoldRegister[118]; 
                        tSendData.uwData0[15] = wHoldRegister[119]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x3E:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wHoldRegister[120];    
                        tSendData.uwData0[2] =  wHoldRegister[121]; 	
                        tSendData.uwData0[3] =  wHoldRegister[122];    
                        tSendData.uwData0[4] =  wHoldRegister[123];    
                        tSendData.uwData0[5] =  wHoldRegister[124];           
                        tSendData.uwData0[6] =  wHoldRegister[125];      
                        tSendData.uwData0[7] =  wHoldRegister[126]; 		    
                        tSendData.uwData0[8] =  wHoldRegister[127];          
                        tSendData.uwData0[9] =  wHoldRegister[128];         
                        tSendData.uwData0[10] = wHoldRegister[129];        					    
                        tSendData.uwData0[11] = wHoldRegister[130];
                        tSendData.uwData0[12] = wHoldRegister[131];
                        tSendData.uwData0[13] = wHoldRegister[132];
                        tSendData.uwData0[14] = wHoldRegister[133]; 
                        tSendData.uwData0[15] = wHoldRegister[134]; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }	
			        case 0x3F:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wRemoteCSPPer;    
                        tSendData.uwData0[2] =  wRemoteCSPFSet; 	
                        tSendData.uwData0[3] =  wRemoteCSQPer;    
                        tSendData.uwData0[4] =  wEepromRemoteSel;    
                        tSendData.uwData0[5] =  wRemoteCSQCtrModeSel;           
                        tSendData.uwData0[6] =  wHoldRegister[173];      
                        tSendData.uwData0[7] =  fCallDefaultValueOrder;  				    
                        tSendData.uwData0[8] =  uwFVRTCurrSoftDelay;          
                        tSendData.uwData0[9] =  wDebugTest6;         
                        tSendData.uwData0[10] = uwLVRTActiveValue;//    
                        tSendData.uwData0[11] = uwHVRTActiveValue;
                        tSendData.uwData0[12] = wDebugTest4;
                        tSendData.uwData0[13] = wDebugTest5;
                        tSendData.uwData0[14] = wEepromRemoteSel; 
                        tSendData.uwData0[15] = fHoldRegisterStatus; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }  
			        case 0x40:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wRemoteCSVSSwitch;    
                        tSendData.uwData0[2] =  wRemoteVSCtrMode; 	
                        tSendData.uwData0[3] =  wRemoteVSFreqSet;    
                        tSendData.uwData0[4] =  wRemoteVSVoltSet;    
                        tSendData.uwData0[5] =  wRemoteVSPSet;           
                        tSendData.uwData0[6] =  wRemoteVSQSet;      
                        tSendData.uwData0[7] =  0;  				    
                        tSendData.uwData0[8] =  0;          
                        tSendData.uwData0[9] =  0;         
                        tSendData.uwData0[10] = wEnvTmpAvg;	    
                        tSendData.uwData0[11] = wHeatTmpAvg;
                        tSendData.uwData0[12] = fEepromProtSumErrNum;
                        tSendData.uwData0[13] = fEepromProtSuplSumErrNum;
                        tSendData.uwData0[14] = uwI2CBlockState.word; 
                        tSendData.uwData0[15] = fHoldRegisterStatus; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }      				    	
			        case 0x64:
			        {
			        	tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromPOBusVolScale;
                        tSendData.uwData0[2] =  wEepromPOBusVolOffset; 
                        tSendData.uwData0[3] =  wEepromONBusVolScale;
                        tSendData.uwData0[4] =  wEepromONBusVolOffset; 
                        tSendData.uwData0[5] =  wEepromBatteryCurr1Scale;       
                        tSendData.uwData0[6] =  wEepromBatteryCurr1Offset;    
                        tSendData.uwData0[7] =  wEepromADCofeeSum00; 					    
                        tSendData.uwData0[8] =  wEepromBatteryCurr2Scale;          
                        tSendData.uwData0[9] =  wEepromBatteryCurr2Offset;        
                        tSendData.uwData0[10] = wEepromBatteryCurr3Scale;        					    
                        tSendData.uwData0[11] = wEepromBatteryCurr3Offset; 
                        tSendData.uwData0[12] = wEepromBatteryCurr4Scale;  
                        tSendData.uwData0[13] = wEepromBatteryCurr4Offset;
                        tSendData.uwData0[14] = wEepromADCofeeSum01;   
                        tSendData.uwData0[15] = wEepromRSGridVolScale; 
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x65:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromRSGridVolOffset;
                        tSendData.uwData0[2] =  wEepromSTGridVolScale;  
                        tSendData.uwData0[3] =  wEepromSTGridVolOffset;
                        tSendData.uwData0[4] =  wEepromTRGridVolScale;  
                        tSendData.uwData0[5] =  wEepromTRGridVolOffset;      
                        tSendData.uwData0[6] =  wEepromADCofeeSum02; 
                        tSendData.uwData0[7] =  wEepromRInvVolScale;      
                        tSendData.uwData0[8] =  wEepromRInvVolOffset;   
                        tSendData.uwData0[9] =  wEepromSInvVolScale;   
                        tSendData.uwData0[10] = wEepromSInvVolOffset; 
                        tSendData.uwData0[11] = wEepromTInvVolScale;
                        tSendData.uwData0[12] = wEepromTInvVolOffset;  
                        tSendData.uwData0[13] = wEepromADCofeeSum03;  
                        tSendData.uwData0[14] = wEepromRDCICurrScale;  
                        tSendData.uwData0[15] = wEepromRDCICurrOffset; 
                               
                        uwSendNum = 16;
                        uwSendFlag = 1;
                        break;
			        }
			        case 0x66:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromSDCICurrScale;
                        tSendData.uwData0[2] =  wEepromSDCICurrOffset;
                        tSendData.uwData0[3] =  wEepromTDCICurrScale;
                        tSendData.uwData0[4] =  wEepromTDCICurrOffset;
                        tSendData.uwData0[5] =  wEepromADCofeeSum04;
                        tSendData.uwData0[6] =  wEepromRInvCurrScale;
                        tSendData.uwData0[7] =  wEepromRInvCurrOffset;
                        tSendData.uwData0[8] =  wEepromSInvCurrScale;
                        tSendData.uwData0[9] =  wEepromSInvCurrOffset;
                        tSendData.uwData0[10] = wEepromTInvCurrScale;
                        tSendData.uwData0[11] = wEepromTInvCurrOffset;
                        tSendData.uwData0[12] = wEepromADCofeeSum05;
                        tSendData.uwData0[13] = wEepromIsoVolScale;
                        tSendData.uwData0[14] = wEepromIsoVolOffset;
                        tSendData.uwData0[15] = wEepromBatteryOutVolScale;
                                                      
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			        case 0x67:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromBatteryOutVolOffset;
                        tSendData.uwData0[2] =  wEepromBatteryInVolScale;    
                        tSendData.uwData0[3] =  wEepromBatteryInVolOffset;   
                        tSendData.uwData0[4] =  wEepromADCofeeSum06;  				
                        tSendData.uwData0[5] =  wEepromGFCICurrScale;     
                        tSendData.uwData0[6] =  wEepromGFCICurrOffset;        
                        tSendData.uwData0[7] =  wEepromEnvOffset; 
                        tSendData.uwData0[8] =  wEepromHeatOffset;
                        tSendData.uwData0[9] =  wEepromXFMROffset; 
                        tSendData.uwData0[10] = wEepromTfTmpOffset;			
                        tSendData.uwData0[11] = wEepromADCofeeSum07;   			    
                        tSendData.uwData0[12] = wEepromSave1;  
                        tSendData.uwData0[13] = wEepromSave2;
                        tSendData.uwData0[14] = wEepromSave3;   
                        tSendData.uwData0[15] = wEepromSave4;   
                         
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			        case 0x68:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] = wEepromSave5;      
                        tSendData.uwData0[2] = wEepromSave6;  
                        tSendData.uwData0[3] = wEepromADCofeeSum08; 
                        tSendData.uwData0[4] = wEepromSave7;       
                        tSendData.uwData0[5] = wEepromSave8;
                        tSendData.uwData0[6] = wEepromSave9;    
                        tSendData.uwData0[7] = wEepromSave10;					    
                        tSendData.uwData0[8] = wEepromSave11;   
                        tSendData.uwData0[9] = wEepromSave12;  
                        tSendData.uwData0[10] = wEepromADCofeeSum09;				    
                        tSendData.uwData0[11] = 0;
                        tSendData.uwData0[12] = 0;
                        tSendData.uwData0[13] = 0;
                        tSendData.uwData0[14] = 0;
                        tSendData.uwData0[15] = fSciStandChanged;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;   
                        break;
			        }	                    
		            case 0x69:
			        {
				        tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] = uwRSGridVolOffset;      
                        tSendData.uwData0[2] = uwSTGridVolOffset;  
                        tSendData.uwData0[3] = uwTRGridVolOffset;
				        tSendData.uwData0[4] = uwRInvCurrOffset; 
                        tSendData.uwData0[5] = uwSInvCurrOffset;      
                        tSendData.uwData0[6] = uwTInvCurrOffset;    
                        tSendData.uwData0[7] = uwRDCICurrOffset;					    
                        tSendData.uwData0[8] = uwSDCICurrOffset;   
                        tSendData.uwData0[9] = uwTDCICurrOffset;  
                        tSendData.uwData0[10] = 0;				    
                        tSendData.uwData0[11] = uwGFCICurrOffset;
                        tSendData.uwData0[12] = uwBatCurr1Offset;
                        tSendData.uwData0[13] = uwBatCurr2Offset;
                        tSendData.uwData0[14] = uwBatCurr3Offset;
                        tSendData.uwData0[15] = uwBatCurr4Offset;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1;   
                        break;
			        }	
	       
                    case 0xC8:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromGridVolMax_1;   
                        tSendData.uwData0[2] =  wEepromVolMaxTripTime_1;   
                        tSendData.uwData0[3] =  wEepromGridVolMin_1;      
                        tSendData.uwData0[4] =  wEepromVolMinTripTime_1;      
                        tSendData.uwData0[5] =  wEepromGridVolMax_2;   
                        tSendData.uwData0[6] =  wEepromVolMaxTripTime_2;//wEepromReactivePowerPer; 
                        tSendData.uwData0[7] =  wEepromSum00;               
                        tSendData.uwData0[8] =  wEepromGridVolMin_2;        
                        tSendData.uwData0[9] =  wEepromVolMinTripTime_2;
                        tSendData.uwData0[10] = wEepromGridVolMax_3;           
                        tSendData.uwData0[11] = wEepromVolMaxTripTime_3;        
                        tSendData.uwData0[12] = wEepromGridVolMin_3;        
                        tSendData.uwData0[13] = wEepromVolMinTripTime_3;        
                        tSendData.uwData0[14] = wEepromSum01;           
                        tSendData.uwData0[15] = wEepromGridVolRecover_Max;           
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			        case 0xC9:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromGridVolRecover_Min;   
                        tSendData.uwData0[2] =  wEepromGridVolRecover_Time;         
                        tSendData.uwData0[3] =  wEepromGridVoltBalanceMax;       
                        tSendData.uwData0[4] =  wEepromGridFaultPowerStep;  
                        tSendData.uwData0[5] =  wEepromPowerOnWaitTime; //修改       
                        tSendData.uwData0[6] =  wEepromSum02;              
                        tSendData.uwData0[7] =  wEepromGridFreqMax_1;                 
                        tSendData.uwData0[8] =  wEepromFreqMaxTripTime_1;          
                        tSendData.uwData0[9] =  wEepromGridFreqMin_1;          
                        tSendData.uwData0[10] = wEepromFreqMinTripTime_1;               
                        tSendData.uwData0[11] = wEepromGridFreqMax_2;        
                        tSendData.uwData0[12] = wEepromFreqMaxTripTime_2;        
                        tSendData.uwData0[13] = wEepromSum03;              
                        tSendData.uwData0[14] = wEepromGridFreqMin_2;       
                        tSendData.uwData0[15] = wEepromFreqMinTripTime_2;//wEepromBusSoftStaTimer;     
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			        case 0xCA:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromGridFreqRecover_Max; 
                        tSendData.uwData0[2] =  wEepromGridFreqRecover_Min;    
                        tSendData.uwData0[3] =  wEepromGridFreqRecover_Time;    
                        tSendData.uwData0[4] =  wEepromSysCtrSignal;    
                        tSendData.uwData0[5] =  wEepromSum04;          
                        tSendData.uwData0[6] =  wEepromLVRTTripVol;
                        tSendData.uwData0[7] =  wEepromHVRTTripVol;	    
                        tSendData.uwData0[8] =  wEepromFVRTPosCurrK;
                        tSendData.uwData0[9] =  wEepromFVRTNegCurrK;
                        tSendData.uwData0[10] = wEepromFVRTPowerStep;    
                        tSendData.uwData0[11] = wEepromOvFWTripFreq;
                        tSendData.uwData0[12] = wEepromSum05;
                        tSendData.uwData0[13] = wEepromOvFWRatio;
                        tSendData.uwData0[14] = wEepromOvFWExitFreq;
                        tSendData.uwData0[15] = wEepromUnFWTripFreq;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			        case 0xCB:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromUnFWRatio;
                        tSendData.uwData0[2] =  wEepromUnFWExitFreq;
                        tSendData.uwData0[3] =  wEepromFWRecPowerStep;
                        tSendData.uwData0[4] =  wEepromSum06;    
                        tSendData.uwData0[5] =  wEepromFWActTime;          
                        tSendData.uwData0[6] =  wEepromVWActTime;
                        tSendData.uwData0[7] =  wEepromPF_PCurveActPw1;	    
                        tSendData.uwData0[8] =  wEepromPF_PCurvePF1;
                        tSendData.uwData0[9] =  wEepromPF_PCurveActPw2;
                        tSendData.uwData0[10] = wEepromPF_PCurvePF2;    
                        tSendData.uwData0[11] = wEepromSum07;
                        tSendData.uwData0[12] = wEepromPF_PCurveLockInV;
                        tSendData.uwData0[13] = wEepromPF_PCurveLockOutV;
                        tSendData.uwData0[14] = wEepromQ_UCurveVolt1s;
                        tSendData.uwData0[15] = wEepromQ_UCurveReactPw1s;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
					case 0xCC:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromQ_UCurveVolt2s;
                        tSendData.uwData0[2] =  wEepromQ_UCurveReactPw2s;
                        tSendData.uwData0[3] =  wEepromSum08;
                        tSendData.uwData0[4] =  wEepromQ_UCurveVolt1i;    
                        tSendData.uwData0[5] =  wEepromQ_UCurveReactPw1i;          
                        tSendData.uwData0[6] =  wEepromQ_UCurveVolt2i;
                        tSendData.uwData0[7] =  wEepromQ_UCurveReactPw2i;	    
                        tSendData.uwData0[8] =  wEepromQ_UCurveLockInP;
                        tSendData.uwData0[9] =  wEepromQ_UCurveLockOutP;
                        tSendData.uwData0[10] = wEepromSum09;    
                        tSendData.uwData0[11] = wEepromIsoImpMin;
                        tSendData.uwData0[12] = wEepromGFCIMax;
                        tSendData.uwData0[13] = wEepromDCIMax;
                        tSendData.uwData0[14] = wEepromOvVWStartVolt;
                        tSendData.uwData0[15] = wEepromOvVWStopVolt;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
					case 0xCD:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromOvVWRatio;
                        tSendData.uwData0[2] =  wEepromSum10;
                        tSendData.uwData0[3] =  wEepromUnVWStartVolt;
                        tSendData.uwData0[4] =  wEepromUnVWStopVolt;    
                        tSendData.uwData0[5] =  wEepromUnVWRatio;          
                        tSendData.uwData0[6] =  wEepromQUActTime;
                        tSendData.uwData0[7] =  wEepromIsoRes;	    
                        tSendData.uwData0[8] =  wEepromUserSave1;
                        tSendData.uwData0[9] =  wEepromSum11;
                        tSendData.uwData0[10] = wRSGridRmsVol;    
                        tSendData.uwData0[11] = wSTGridRmsVol;
                        tSendData.uwData0[12] = wTRGridRmsVol;
                        tSendData.uwData0[13] = UabTemp;
                        tSendData.uwData0[14] = UbcTemp;
                        tSendData.uwData0[15] = UcaTemp;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			        case 0xCE:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromChargeVoltLimt;
                        tSendData.uwData0[2] =  wEepromDisChargeVoltLimt;
                        tSendData.uwData0[3] =  wEepromChargeCurrLimt;
                        tSendData.uwData0[4] =  wEepromDisChargeCurrLimt;    
                        tSendData.uwData0[5] =  wEepromFloatChargeVolt;          
                        tSendData.uwData0[6] =  wEepromSToFChargeCurr;
                        tSendData.uwData0[7] =  wEepromSupplSum01;	    
                        tSendData.uwData0[8] =  wEepromBatteryVoltMax;
                        tSendData.uwData0[9] =  wEepromBatteryVoltMin;
                        tSendData.uwData0[10] = wEepromBatChargeCurrMax;    
                        tSendData.uwData0[11] = wEepromBatDisChargeCurrMax;
                        tSendData.uwData0[12] = wEepromDCRelayVoltDiff;
                        tSendData.uwData0[13] = wEepromBatPreChargeCurrMax;
                        tSendData.uwData0[14] = wEepromSupplSum02;
                        tSendData.uwData0[15] = wEepromRemoteSel;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
					case 0xCF:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromCSVSSwitch;
                        tSendData.uwData0[2] =  wEepromAutoCSVSSwitchEn;
                        tSendData.uwData0[3] =  wEepromVSCtrMode;
                        tSendData.uwData0[4] =  wEepromVSVoltSet;    
                        tSendData.uwData0[5] =  wEepromVSFreqSet;          
                        tSendData.uwData0[6] =  wEepromSupplSum03;
                        tSendData.uwData0[7] =  wEepromVSPSet;	    
                        tSendData.uwData0[8] =  wEepromVSQSet;
                        tSendData.uwData0[9] =  wEepromVSDroopEn;
                        tSendData.uwData0[10] = wEepromVSFreqDroopKByP;    
                        tSendData.uwData0[11] = wEepromVSVoltDroopKByQ;
                        tSendData.uwData0[12] = wEepromCSPCtrModeSel;
                        tSendData.uwData0[13] = wEepromSupplSum04;
                        tSendData.uwData0[14] = wEepromCSPPer;
                        tSendData.uwData0[15] = wEepromCSCurrSet;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
					case 0xD0:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromPActStep;
                        tSendData.uwData0[2] =  wEepromCSQCtrModeSel;
                        tSendData.uwData0[3] =  wEepromCSPFSet;
                        tSendData.uwData0[4] =  wEepromCSQPer;    
                        tSendData.uwData0[5] =  wEepromSupplSum05;          
                        tSendData.uwData0[6] =  wEepromQActStep;
                        tSendData.uwData0[7] =  wEepromSoftPowerStep;	    
                        tSendData.uwData0[8] =  wEepromSoftStopEn;
                        tSendData.uwData0[9] =  wEepromStopPowerStep;
                        tSendData.uwData0[10] = wEepromPVStartUpVol;    
                        tSendData.uwData0[11] = wEepromPVFitFactor;
                        tSendData.uwData0[12] = wEepromSupplSum06;
                        tSendData.uwData0[13] = wEepromISOEnable;
                        tSendData.uwData0[14] = wEepromGFCICTChkEn;
                        tSendData.uwData0[15] = wEepromAPFOption;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
					case 0xD1:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromArcingEn;
                        tSendData.uwData0[2] =  wEepromFullRangeMpptEnable;
                        tSendData.uwData0[3] =  wEepromFullRangeMpptCycle;
                        tSendData.uwData0[4] =  wEepromSupplSum07;    
                        tSendData.uwData0[5] =  wEepromInvCurrMax;          
                        tSendData.uwData0[6] =  wEepromDCIOffsetDiffMax;
                        tSendData.uwData0[7] =  wEepromOffsetDiffMax;	    
                        tSendData.uwData0[8] =  wEepromEnvironmentTmpMax;
                        tSendData.uwData0[9] =  wEepromPowerTmpMax;
                        tSendData.uwData0[10] = wEepromXFMRTmpMax;    
                        tSendData.uwData0[11] = wEepromSupplSum08;
                        tSendData.uwData0[12] = wEepromTransformerTmpMax;
                        tSendData.uwData0[13] = wEepromBusVolMax;
                        tSendData.uwData0[14] = wEepromCapVolMax;
                        tSendData.uwData0[15] = wEepromDCRelayActProt;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
				    case 0xD2:
			        {
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  wEepromBiDirDCSwitch;
                        tSendData.uwData0[2] =  wEepromBatteryType;
                        tSendData.uwData0[3] =  wEepromSupplSum09;
                        tSendData.uwData0[4] =  0;//wEepromClientSave4;    
                        tSendData.uwData0[5] =  wEepromClientSave5;         
                        tSendData.uwData0[6] =  wEepromClientSave6;
                        tSendData.uwData0[7] =  wEepromClientSave7;	    
                        tSendData.uwData0[8] =  wEepromClientSave8;
                        tSendData.uwData0[9] =  wEepromClientSave9;
                        tSendData.uwData0[10] = wEepromSupplSum10;    
                        tSendData.uwData0[11] = EPwm1Regs.TZFLG.bit.OST;
                        tSendData.uwData0[12] = EPwm2Regs.TZFLG.bit.OST;
                        tSendData.uwData0[13] = EPwm3Regs.TZFLG.bit.OST;
                        tSendData.uwData0[14] = wPLLFrequency;
                        tSendData.uwData0[15] = 0;
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
                    case 0xDE:
			        {
			            uwScibTxData[0] = 0xaaaa;
			            uwScibTxData[1] = (0x52<<8)+tReceData[2].bit.byte0;
			            uwScibTxData[2] = 0;
			            
			            uwScibSendNum = 3;
			            fScibTxEn = 1;
			            			            
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode; 
                        tSendData.uwData0[2] =  0;    
                        tSendData.uwData0[3] =  tScibRxData[0].word;    
                        tSendData.uwData0[4] =  0;    
                        tSendData.uwData0[5] =  tScibRxData[1].word;          
                        tSendData.uwData0[6] =  0;
                        tSendData.uwData0[7] =  tScibRxData[2].word;	    
                        tSendData.uwData0[8] =  0;
                        tSendData.uwData0[9] =  tScibRxData[3].word;
                        tSendData.uwData0[10] = 0;    
                        tSendData.uwData0[11] = tScibRxData[4].word;
                        tSendData.uwData0[12] = 0;
                        tSendData.uwData0[13] = tScibRxData[5].word;
                        tSendData.uwData0[14] = 0;
                        tSendData.uwData0[15] = 0;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			        case 0xDF:
			        {
			            uwScibTxData[0] = 0xaaaa;
			            uwScibTxData[1] = (0x43<<8)+tReceData[2].bit.byte0;
			            uwScibTxData[2] = 0;
			            
			            uwScibSendNum = 3;
			            fScibTxEn = 1;
			            			            
			            tSendData.uwData0[0] =  0xaaaa;
                        tSendData.uwData0[1] =  uwSysMode; 
                        tSendData.uwData0[2] =  0;    
                        tSendData.uwData0[3] =  tScibRxData[0].word;    
                        tSendData.uwData0[4] =  0;    
                        tSendData.uwData0[5] =  tScibRxData[1].word;          
                        tSendData.uwData0[6] =  0;
                        tSendData.uwData0[7] =  tScibRxData[2].word;	    
                        tSendData.uwData0[8] =  0;
                        tSendData.uwData0[9] =  tScibRxData[3].word;
                        tSendData.uwData0[10] = 0;    
                        tSendData.uwData0[11] = tScibRxData[4].word;
                        tSendData.uwData0[12] = 0;
                        tSendData.uwData0[13] = tScibRxData[5].word;
                        tSendData.uwData0[14] = 0;
                        tSendData.uwData0[15] = 0;
                        
                        uwSendNum = 16;
                        uwSendFlag = 1; 
                        break;
			        }
			    }
			}		 
			else if(tReceData[1].bit.byte1 == 'r') //r ASCII码 114
			{
				pSciRamData = (INT16S*)(((INT32S)(tReceData[1].bit.byte0)<<16) + tReceData[2].word);
			}
		}
		uwReceFlag = 0;
    }
}

void sSciTxArray(void)
{
static INT16U i=0,j=0;
    if(uwDataSci[4] == 1)
    { 
        if(i < uwSciSendNum)
        {
    	    i++;
        }
        else
        {
    	    i = 0;
              
            if(tReceData[2].bit.byte0==0)
            {       
                tSendData.uwData0[j] = tGridPhaseVoltAbc.a;
                tSendData.uwData1[j] = tGridPhaseVoltAbc.b;
                tSendData.uwData2[j] = tGridPhaseVoltAbc.c;
            }       
            else if(tReceData[2].bit.byte0==1)
            {       
                tSendData.uwData0[j] = wPOBusVol;
                tSendData.uwData1[j] = wONBusVol;
                tSendData.uwData2[j] = wBatteryCurr; 
            }       
            else if(tReceData[2].bit.byte0==2)
            {       
                tSendData.uwData0[j] = tGridPhaseVoltAbc.a;
                tSendData.uwData1[j] = wRSGridVol;
                tSendData.uwData2[j] = wPLLCosTheta;
            }       
            else if(tReceData[2].bit.byte0==3)
            {       
                tSendData.uwData0[j] = twInvPhaseVol.a;//tGridPhaseVoltAbc.a;//tInvCurrDqo.d;
                tSendData.uwData1[j] = twGridPhaseVol.a;//wRSGridVol;//tInvCurrDqo.q;
                tSendData.uwData2[j] = twInvPhaseVol.b;//wPLLCosTheta;
            }       
            else if(tReceData[2].bit.byte0==4)
            {       
                tSendData.uwData0[j] = tInvModuAbc.A.word0;
                tSendData.uwData1[j] = tInvModuAbc.B.word0;
                tSendData.uwData2[j] = tInvModuAbc.C.word0;
            }       
            else if(tReceData[2].bit.byte0==5)
            {       
                tSendData.uwData0[j] = tInvCurrDqo.d;//tInvCurrDqo.d;//wEnvFitTemp;
                tSendData.uwData1[j] = tInvCurrDqo.o;//tInvCurrDqo.q;//wXFMRFitTemp;
                tSendData.uwData2[j] = tInvCurrDqoRef.d;//wDebugInvDsCurrRef;//wHeatFitTemp;
            }       
            else if(tReceData[2].bit.byte0==6)
            {       
                tSendData.uwData0[j] = twInvPhaseVol.c;
                tSendData.uwData1[j] = twGridPhaseVol.b;
                tSendData.uwData2[j] = twGridPhaseVol.c;//wTRGridVol;
            }       
            else if(tReceData[2].bit.byte0==7)
            {       
                tSendData.uwData0[j] = tInvCurrDqo.q;
                tSendData.uwData1[j] = tInvNosCurrDqoRef.q;
                tSendData.uwData2[j] = tInvCurrDqoRef.q;
            }  
            else if(tReceData[2].bit.byte0==8)
            {       
                tSendData.uwData0[j] = wRInvCurr;
                tSendData.uwData1[j] = wSInvCurr;
                tSendData.uwData2[j] = wTInvCurr;
            } 
            else if(tReceData[2].bit.byte0==9)   // for AI debug
            {       
                tSendData.uwData0[j] = wRDCIAvgCurr<<1;//wRSGridRmsVol;//uwDebugMaxFre;
                tSendData.uwData1[j] = wSDCIAvgCurr<<1;//wSTGridRmsVol;//EPwm5Regs.CMPA.half.CMPA;//wAIDeltaIq;
                tSendData.uwData2[j] = wTDCIAvgCurr<<1;//wTRGridRmsVol;//wRSTInvVolAdd;
            } 
            else if(tReceData[2].bit.byte0==10)
            {       
                tSendData.uwData0[j] = wDCIReguR;//tInvModuAbc.A.word0;//tInvModuAfBtGm.Af;//fSelfCalibration;//uwRFre;//dwMpptFullLoadSpec>>4;//
                tSendData.uwData1[j] = wDCIReguS;//tInvModuDqo.d;//tInvModuAfBtGm.Bt;//uwSelfCalibrationCnt;//uwSFre;//udwPowerAvg>>4;//
                tSendData.uwData2[j] = wDCIReguT;//tInvModuDqo.q;//wPLLCosTheta;//uwTFre;//wMpptVol;//
            }
            else if(tReceData[2].bit.byte0==11)
            {       
                tSendData.uwData0[j] = wTorqueM;//wBatteryCurrRef;
                tSendData.uwData1[j] = wTorqueE;//wBatteryCurr1;
                tSendData.uwData2[j] = wTorqueD;//wBatteryCurr3;
            }  
            else if(tReceData[2].bit.byte0==12)
            {       
                tSendData.uwData0[j] = wRInvVol;//wDebugCopy1[j];//wRInvVol;//tInvVolDqo.d;
                tSendData.uwData1[j] = wSInvVol;//wDebugCopy2[j];//wSInvVol;//tInvVolDqo.q;
                tSendData.uwData2[j] = wTInvVol;//wDebugCopy3[j];//wTInvVol;//tInvVolDqo.o;
            } 
            else if(tReceData[2].bit.byte0==13)
            {       
                tSendData.uwData0[j] = wBatteryCurrRef;//tGridPhaseVoltAbc.c;//wTInvCurr;
                tSendData.uwData1[j] = wBatteryCurr2;//wTInvCurr<<1;//wSCurrAvg;
                tSendData.uwData2[j] = wBatteryCurr4;//wTGridCurr<<1;//wTCurrAvg;
            }        
            j++;    
            if (j==100)
            {       
                j=0;
                uwDataSci[4]=0;
                uwSendFlag = 1;
            }       
        }       
    }
    else    
    {       
        i=0;  
        j=0;  
    }       
}

void SciaTxDeal(void)
{
    INT16U i;
    // Monitor every once    
    uwMonCnt++;    
    if(uwMonCnt == 2)
    {   //begin to send data     //2
        tChxData[0].sw = *(int *)dwpAddr1;
        tChxData[1].sw = *(int *)dwpAddr2;
        tChxData[2].sw = *(int *)dwpAddr3;        
        tChxData[3].sw = *(int *)dwpAddr4;
        for(i=0;i<4;i++)
        {
            if(tChxData[i].sw == 0x55aa)
            {
                tChxData[i].sw = 0x55ab;
            }
            if(tChxData[i].sb.low == 0x55)
            {
                tChxData[i].sb.low = 0x54;
            }
            ubTxBuf[2*i+2] = tChxData[i].sb.low;     // ch[i] low byte
            ubTxBuf[2*i+3] = tChxData[i].sb.hi;      // ch[i] high byte  
        }

	    //SciaRegs.SCICTL2.bit.TXINTENA = 1;			//Enalbe TXD  interrupt
		//send first data    
        SciaRegs.SCITXBUF = ubTxBuf[0];             //send begin
     	uwTxCnt = 1;            
        uwMonCnt = 0;
    }
    else if(uwMonCnt > 2)
    {   //2
        uwMonCnt = 0;
    }  
}
/**********************************************************************
* Function: ScibRxIsr()
* used for communication to Computer
* Description: Initializes the SCIA modules on the F28235.
**********************************************************************/
void ScibRxIsr(void)
{   
static INT16U j=0;
INT16U i=0;
  
    if(ScibRegs.SCIRXST.bit.RXERROR == 1)
    {
        ScibRegs.SCICTL1.bit.SWRESET = 0;     
        asm(" RPT #6 || NOP"); 
        j = 0;                
        ScibRegs.SCICTL1.bit.SWRESET = 1;            
    }
    else
    { 
        i = ScibRegs.SCIRXBUF.all;
	    if(i == 0xAA) 
	    {
	        if(j >= 10) j = 0;//10
        
	        if((j&1) == 0) {tScibRxData[j/2].bit.byte1 = i;j++;}
	        else {tScibRxData[j/2].bit.byte0 = i;j++;}		
        }
	    else
	    {
	        if((j == 1)||(j == 0)) {j = 0;tScibRxData[0].word = 0;}
	    	else if(j < 10) 
	    	{
	    	    if((j&1) == 0) {tScibRxData[j/2].bit.byte1 = i;j++;}
	            else {tScibRxData[j/2].bit.byte0 = i;j++;}
	    	}
	    	else j=0;
	    }
	    if (j == 10)
	    {
	        fScibRxOk = 1;
	    }
    }	
    
	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;   // Clear Overflow flag	
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;   // Clear Interrupt flag
}
/**********************************************************************
* Function: SCIB_TX()
* used for communication to Computer
* Description: Initializes the SCIA modules on the F28235.
**********************************************************************/
void SCIB_TX(INT16U *data,INT16U size)   //size 1~16
{        	     	
static INT16U uwSendFrameCnt = 0;
    if(fScibTxEn == 1)
    {
	    if(size > uwSendFrameCnt)
	    {
			while(ScibRegs.SCICTL2.bit.TXEMPTY == 0 || ScibRegs.SCIFFTX.bit.TXFFST != 0);
	        
            ScibRegs.SCITXBUF = (*(data+uwSendFrameCnt))>>8;   //SCITXBUF是8位的
            ScibRegs.SCITXBUF = (*(data+uwSendFrameCnt))&0xFF;

			uwSendFrameCnt++;
        }
		else
		{
		    fScibTxEn = 0;
		    uwSendFrameCnt = 0;
		}  		   	       
    }
	else
	{
		uwScibTxData[0] = 0xaaaa;
        uwScibTxData[1] = (0x52<<8)+0x1;
        uwScibTxData[2] = 0;
        
        size=3;
		if(size > uwSendFrameCnt)
	    {
			while(ScibRegs.SCICTL2.bit.TXEMPTY == 0 || ScibRegs.SCIFFTX.bit.TXFFST != 0);
	        
            ScibRegs.SCITXBUF = (*(data+uwSendFrameCnt))>>8;   //SCITXBUF是8位的
            ScibRegs.SCITXBUF = (*(data+uwSendFrameCnt))&0xFF;

			uwSendFrameCnt++;
        }
		else
		{
		    fScibTxEn = 0;
		    uwSendFrameCnt = 0;
		}
	}
}

/*** end of file ********.all*********************************************/



//###########################################################################

#include "..\FunSrc\Funheader.h"

//#pragma CODE_SECTION(SCI_Boot, "secureRamFuncs")
//#pragma CODE_SECTION(SCIA_GetWordData, "secureRamFuncs")
//#pragma CODE_SECTION(CopyData, "secureRamFuncs")
//#pragma CODE_SECTION(GetLongData, "secureRamFuncs")
//#pragma CODE_SECTION(ReadReservedFn, "secureRamFuncs")
// Private functions
inline void SCIA_Init(void);
Uint16 SCIA_AutobaudLock(void);
Uint16 SCIA_GetWordData(void);
// External functions
extern void CopyData(void);
Uint32 GetLongData(void);
extern void ReadReservedFn(void);
uint16fptr GetWordData;
Uint32 InitSysCtrl_POINT = 0;
//#################################################
// Uint32 SCI_Boot(void)
//--------------------------------------------
// This module is the main SCI boot routine.
// It will load code via the SCI-A port.
//
// It will return a entry point address back
// to the InitBoot routine which in turn calls
// the ExitBoot routine.
//--------------------------------------------

Uint32 SCI_Boot()
{   
    Uint32 EntryAddr;
	Uint16 wsciboot=0;
        
    EALLOW;
    SysCtrlRegs.WDCR = 0x0028; // Enable watchdog module
    SysCtrlRegs.WDKEY = 0x55; // Clear the WD counter
    SysCtrlRegs.WDKEY = 0xAA;
    EDIS;

   EALLOW;

  // At reset we are in /4 mode.  Change to /2
  SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;

  // Set MUX for BOOT Select
  GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;
  GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 0;
  GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 0;
  GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 0;
  
  GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 0;		// 0=GPIO, 1=GPIO      2=XA11       3=XA11,breaker

  // Set DIR for BOOT Select
  GpioCtrlRegs.GPCDIR.bit.GPIO87 = 0;
  GpioCtrlRegs.GPCDIR.bit.GPIO86 = 0;
  GpioCtrlRegs.GPCDIR.bit.GPIO85 = 0;
  GpioCtrlRegs.GPCDIR.bit.GPIO84 = 0;
  
  GpioDataRegs.GPCSET.bit.GPIO83 = 1;         //BreakerCtr      0,Open         1,Reset
  GpioCtrlRegs.GPCDIR.bit.GPIO83 = 1;         //BreakerCtr      1,Output       0,Input
  
  EDIS;
    
    EALLOW;    
    SysCtrlRegs.WDCR = 0x0068; 
    EDIS;	
	// Assign GetWordData to the SCI-A version of the
    // function. GetWordData is a pointer to a function.
    GetWordData = SCIA_GetWordData;
    
    SCIA_Init();
    wsciboot = SCIA_AutobaudLock();
    if( wsciboot!= 0)
	{
	    InitSysCtrl_POINT=(Uint32)main;//InitSysCtrl;         
	    return InitSysCtrl_POINT;
	}
    // If the KeyValue was invalid, abort the load
    // and return the flash entry point.
    if(SCIA_GetWordData() != 0x08AA)return FLASH_ENTRY_POINT;
    
    ReadReservedFn();
    
    EntryAddr = GetLongData();
    CopyData();
    
    
    //EALLOW;
    //SysCtrlRegs.WDCR = 0x0028; // Enable watchdog module
    //SysCtrlRegs.WDKEY = 0x55; // Clear the WD counter
    //SysCtrlRegs.WDKEY = 0xAA;
    //EDIS;


    return EntryAddr;
}
//#################################################
// void CopyData(void)
//-----------------------------------------------------
// This routine copies multiple blocks of data from the host
// to the specified RAM locations. There is no error
// checking on any of the destination addresses.
// That is it is assumed all addresses and block size
// values are correct.
//
// Multiple blocks of data are copied until a block
// size of 00 00 is encountered.
//
//-----------------------------------------------------
void CopyData()
{
    struct HEADER {
    Uint16 BlockSize;
    Uint32 DestAddr;
    } BlockHeader;
    Uint16 wordData;
    Uint16 i;
    // Get the size in words of the first block
    BlockHeader.BlockSize = (*GetWordData)();
    // While the block size is > 0 copy the data
    // to the DestAddr. There is no error checking
    // as it is assumed the DestAddr is a valid
    // memory location
    while(BlockHeader.BlockSize != (Uint16)0x0000)
    {
        BlockHeader.DestAddr = GetLongData();
        for(i = 1; i <= BlockHeader.BlockSize; i++)
        {
            wordData = (*GetWordData)();
            *(Uint16 *)BlockHeader.DestAddr++ = wordData;
        }
        // Get the size of the next block
        BlockHeader.BlockSize = (*GetWordData)();
    }
return;
}
//#################################################
// void SCIA_Init(void)
//----------------------------------------------
// Initialize the SCI-A port for communications
// with the host.
//----------------------------------------------
inline void SCIA_Init()
{
    // Enable the SCI-A clocks
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK=1;
    SysCtrlRegs.LOSPCP.all = 0x0002;
    SciaRegs.SCIFFTX.all=0x8000;
    // 1 stop bit, No parity, 8-bit character
    // No loopback
    SciaRegs.SCICCR.all = 0x0007;
    // Enable TX, RX, Use internal SCICLK
    SciaRegs.SCICTL1.all = 0x0003;
    // Disable RxErr, Sleep, TX Wake,
    // Disable Rx Interrupt, Tx Interrupt
    SciaRegs.SCICTL2.all = 0x0000;
    // Relinquish SCI-A from reset
    SciaRegs.SCICTL1.all = 0x0023;
    // Enable pull-ups on SCI-A pins
    //GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;
    //GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO35 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO36 = 0;
    //GpioCtrlRegs.GPAPUD.all &= 0xCFFFFFFF;
    // Enable the SCI-A pins
    //GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;
    //GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 1;
    //GpioCtrlRegs.GPAMUX2.all |= 0x05000000;
    // Input qual for SCI-A RX is asynch
    //GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 3;
    EDIS;
    return;
}

//------------------------------------------------
// void SCIA_AutobaudLock(void)
//------------------------------------------------
// Perform autobaud lock with the host.
// Note that if autobaud never occurs
// the program will hang in this routine as there
// is no timeout mechanism included.
//------------------------------------------------
Uint16 SCIA_AutobaudLock()
{
    Uint16 uwDelayCount1=0;
    //Uint16 uwDelayCount2=0; 
    Uint16 byteData1,byteData2,byteData3;
    // Must prime baud register with >= 1
    SciaRegs.SCIHBAUD = 0;
    SciaRegs.SCILBAUD = 1;
    // Prepare for autobaud detection
    // Set the CDC bit to enable autobaud detection
    // and clear the ABD bit
    SciaRegs.SCIFFCT.bit.CDC = 1;
    SciaRegs.SCIFFCT.bit.ABDCLR = 1;
    // Wait until you correctly read an
    // 'A' or 'a' and lock
    while(SciaRegs.SCIFFCT.bit.ABD != 1) 
    {
		DelayUs(100);
        if(++uwDelayCount1 >= 10000)
        {
           uwDelayCount1 =0;
           //if(++uwDelayCount2 >= 50)
           //{   
           //    uwDelayCount2 = 50;
               SciaRegs.SCIFFCT.bit.CDC = 0;
               return 1; 
           //}
        }
    } 
	uwDelayCount1 =0;
    // After autobaud lock, clear the CDC bit
	SciaRegs.SCIFFCT.bit.ABDCLR = 1;
    SciaRegs.SCIFFCT.bit.CDC = 0;
    while(SciaRegs.SCIRXST.bit.RXRDY != 1) { }
    byteData1 = SciaRegs.SCIRXBUF.bit.RXDT;
    //SciaRegs.SCITXBUF = byteData;   
    while(SciaRegs.SCIRXST.bit.RXRDY != 1)
	{
		DelayUs(100);
        if(++uwDelayCount1 >= 10000)
        {
           uwDelayCount1 =0;
           return 1; 
        }
    }
	uwDelayCount1 =0;
    byteData2 = SciaRegs.SCIRXBUF.bit.RXDT;
    //SciaRegs.SCITXBUF = byteData;
    while(SciaRegs.SCIRXST.bit.RXRDY != 1)
	{
		DelayUs(100);
        if(++uwDelayCount1 >= 10000)
        {
           uwDelayCount1 =0;
           return 1; 
        }
    }
	uwDelayCount1 =0;
    byteData3 = SciaRegs.SCIRXBUF.bit.RXDT;
    if((byteData1 ==byteData2)&&(byteData1 ==byteData3))
    {
        SciaRegs.SCITXBUF = byteData1;
        return 0;
    }
    else
    {
        return 1; 
    }
}
//#################################################
// UINT16 SCIA_GetWordData(void)
//-----------------------------------------------
// This routine fetches two bytes from the SCI-A
// port and puts them together to form a single
// 16-bit value. It is assumed that the host is
// sending the data in the order LSB followed by MSB.
//-----------------------------------------------
Uint16 SCIA_GetWordData()
{
   // Uint16 i,j;
    Uint16 wordData;
    Uint16 byteData;
    wordData = 0x0000;
    byteData = 0x0000;
    // Fetch the LSB and verify back to the host
    while(SciaRegs.SCIRXST.bit.RXRDY != 1){}
    wordData = (Uint16)SciaRegs.SCIRXBUF.bit.RXDT;
    SciaRegs.SCITXBUF = wordData;
    // Fetch the MSB and verify back to the host
    while(SciaRegs.SCIRXST.bit.RXRDY != 1){}
    byteData = (Uint16)SciaRegs.SCIRXBUF.bit.RXDT;
    SciaRegs.SCITXBUF = byteData;
    
    // form the wordData from the MSB:LSB
    wordData |= (byteData << 8);
    return wordData;
}
//#################################################
// void Read_ReservedFn(void)
//-------------------------------------------------
// This function reads 8 reserved words in the header.
// None of these reserved words are used by the
// this boot loader at this time, they may be used in
// future devices for enhancements. Loaders that use
// these words use their own read function.
//-------------------------------------------------
void ReadReservedFn()
{
    Uint16 i;
    // Read and discard the 8 reserved words.
    for(i = 1; i <= 8; i++)
    {
        GetWordData();
    }
    return;
}

//#################################################
// Uint32 GetLongData(void)
//-----------------------------------------------------
// This routine fetches a 32-bit value from the peripheral
// input stream.
//-----------------------------------------------------
Uint32 GetLongData()
{
    Uint32 longData;
    // Fetch the upper ? of the 32-bit value
    longData = ( (Uint32)(*GetWordData)() << 16);
    // Fetch the lower ? of the 32-bit value
    longData |= (Uint32)(*GetWordData)();
    return longData;
}
/*** end of file *****************************************************/

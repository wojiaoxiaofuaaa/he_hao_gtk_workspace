/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef SCI_H
#define SCI_H

#include "..\FunSrc\SysDefine.h"


typedef struct
{
    INT16U    uwData0[100];	         /*  */
    INT16U    uwData1[100];
	INT16U    uwData2[100];
}T_SENDDATA;

typedef struct
{
    INT16U  byte0:8;          
    INT16U  byte1:8;         
}T_RECBITS;

typedef union 
{	 
    INT16U       word;	
 	T_RECBITS    bit;
}T_RECWORDS;
 
//============== MONITOR ==================== 
typedef struct
{
     INT16U low:8;
     INT16S hi:8;
}T_TXBITS;

typedef union 
{	 
    INT16U       all;	
 	T_TXBITS     bit;
}T_SCICData;

typedef union{
        T_TXBITS sb;
        INT16S sw;
}T_SWORDOBJECT;  

#define SCI_RxCountMax 10   //Max receive data buffer MONITOR
//#define SCI_SCOPE  0//1
//============== MONITOR ====================

//---------------------------------------------------------------------------
// Function Prototypes 

extern void InitScia(void);
extern void InitScib(void);
extern void SciaRxIsr(void);
extern void ScibRxIsr(void);
extern void SCI_TX(Uint16 *data,Uint16 size);
extern void SCIB_TX(Uint16 *data,Uint16 size);
extern void SciaRxDeal(void);
extern void sSciTxArray(void);

//============= MONITOR ======================
extern void SciaTxDeal(void);
//============= MONITOR ======================
//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
extern T_SENDDATA   tSendData;
extern T_RECWORDS   tReceData[];
extern T_RECWORDS   tScibRxData[]; 
extern INT16U uwDataSci[];
extern INT16U uwScibTxData[];
//extern INT16U       uwReceFlag;
extern INT16U       uwSendFlag;
extern INT16U       uwSendNum;
extern INT16U       uwScibSendNum;
//=================== MONITOR ====================================
extern INT8U  ubRxCnt;              //index of received data
extern INT8U  ubCheckSum;           //sum crc
extern T_SWORDOBJECT tChxData[];
//=================== MONITOR ====================================
extern INT16U uwDebugPvVoltAvg,uwDebugPvPower,uwDebugDeltaPvPower;  
extern INT16U fNoChekFst;
//---------------------------------------------------------------------------
// Global symbols defined in source files
//
#if (!SCI_SCOPE)
#define SCI_BRD 	  19200 
#endif
#if SCI_SCOPE
#define SCI_BRD 	  595200//576000//512000//115200//625000//9600  
#endif 
 
#define SCI_DataNum   7    //=0:1 char bit,=1:2 char bits,=2:3 char bits ... =7:8 char bits 
#define SCI_StopNum   0    //=0:1 stop bit,      =1:2 stop bits  
#define SCI_Check     0    //=0:None check bit,  =1:Odd check   ,=2:Even check

#define sRS485RecIntEnable()   {PieCtrlRegs.PIEIER8.bit.INTx5 = 1;}
#define sRS485RecIntDisable()  {PieCtrlRegs.PIEIER8.bit.INTx5 = 0;}
#define fSciRunOrder        uwDataSci[0]
#define uwSciMpptVol        uwDataSci[1]
#define fFactoryFastStart   uwDataSci[2]
//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

/**********************************************************************
* File: example_nonBIOS.h
* Device: TMS320F2808
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example non-BIOS project.  Include this
*   file in all C-source files.
* History:
*   10/26/04 - original (D. Alter)
**********************************************************************/

#ifndef SysDefine_H
#define SysDefine_H

//---------------------------------------------------------------------------
// Function Prototypes
//

//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
typedef char                  INT8S;
typedef int                   INT16S;
typedef long                  INT32S;
typedef long long             INT64S;
typedef unsigned char         INT8U;
typedef unsigned int          INT16U;
typedef unsigned long         INT32U;
typedef unsigned long long    INT64U;

typedef struct
{
    INT16U  bit0:1;          
    INT16U  bit1:1;          
    INT16U  bit2:1; 
    INT16U  bit3:1;          
    INT16U  bit4:1;          
    INT16U  bit5:1;
    INT16U  bit6:1;          
    INT16U  bit7:1; 
    INT16U  bit8:1;          
    INT16U  bit9:1;          
    INT16U  bit10:1; 
    INT16U  bit11:1;          
    INT16U  bit12:1;          
    INT16U  bit13:1;
    INT16U  bit14:1;          
    INT16U  bit15:1;       
}T_BITS;

typedef union 
{	 
    INT16U      flag;	
    T_BITS      bits;
}T_FLAGOBJECT;

typedef struct
{
    INT16S  word0;         
    INT16S  word1; 
}T_WORDS;

typedef union 
{	 
    INT32S        words;	
 	T_WORDS       bits;
}T_LWORDOBJECT;

/**** struct type definition ****/
typedef struct
{
    INT16S a;
    INT16S b;
    INT16S c;
}T_THREE_PHASE_ABC;

typedef struct
{
    INT32S a;
    INT32S b;
    INT32S c;
}T_THREE_PHASE_ABC_dw;

typedef struct
{
    INT16S Af;
    INT16S Bt;
    INT16S Gm;
}T_THREE_PHASE_AFBTGM;

typedef struct
{
    INT16S d;
    INT16S q;
    INT16S o;
}T_THREE_PHASE_DQO;

typedef struct
{
    T_WORDS A;
    T_WORDS B;
	T_WORDS C;
}T_THREE_DUTY_ABC;

typedef struct
{
    INT32S N0;
    INT32S N1;
    INT32S N2;
}T_NotchOut_Prev;

typedef struct
{
    INT16S N0;
    INT16S N1;
}T_NotchIn_Prev;
//---------------------------------------------------------------------------
// Global symbols defined in source files
//
#define setbit(var,varbit)  (var|=(0x01<<(varbit)))
#define clrbit(var,varbit)  (var&=(~(0x01<<(varbit))))
#define checkbit(var,varbit) (var&(0x01<<(varbit)))

#define CPUFre            150E6               //100MHZ 
#define FourCPUFre        1200E6              //Q3

#define cSoftwareVersion   102
#define cProductCode       1132

#define ConverterNum       1                   //并联的机器台数


#define cNoStandard              0
#define cVDE0126Standard         1
#define cVDE4105Standard         2
//#define cG59Standard             3
//#define cRD1663Standard          4
//#define cC10_11Standard          5
//#define cEN50438Standard         6
#define cCEIStandard             7
#define cCGCStandard             8
//#define cIEC61727Standard        9
#define cIEEE1547Standard        10  
#define cBDEWStandard            11    
//#define cBrazilStandard          12
//#define cThailand_MEAStandard    13
//#define cCGCStandard_480V        14
#define cRule21Standard          19
//#define cO_Ha_MaStandard         20
//#define cMolo_LanaStandard       21

//---------------------------------------------------------------------------
#endif  // end of EXAMPLE_H definition

/*** end of file *****************************************************/

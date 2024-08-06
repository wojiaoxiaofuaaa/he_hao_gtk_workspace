/**********************************************************************
Copyright 2009, Chint Power
File Name: I2C.c
Description:
Version: V1.0
Author:Zhou Xu
Data: 2009-10-27
**********************************************************************/
/* head-files including*/
#include "..\FunSrc\Funheader.h"

INT16S wI2cRxData;
INT16S wI2cTxData;

#define  cFreq50Hz      12800    //50*Q8
#define  cFreq60Hz      15360    //60*Q4 = 6032

#define cWriteOrder     0x50
#define cReadOrder      0x51

const INT16S wEepromDefaultImptMaxArray[ ] = 
{ 
    0x7fff,                     0x63ff,                    0x520d,                    0x7fff,                       
    //wEepromBlockMap           wEepromMachineConfig       wEepromRatedConfig         wEepromSerialNumberH         
    0x7fff,                     0x7fff,                    0x1f                                                    
    //wEepromSerialNumberM      wEepromSerialNumberL       wEepromStandardValue   
};

const INT16S wEepromDefaultImptMinArray[ ] = 
{ 
    0x10,                       0x0,                       0x0,                       0x8000,                       
    //wEepromBlockMap           wEepromMachineConfig       wEepromRatedConfig         wEepromSerialNumberH        
    0x8000,                     0x8000,                    0x0                                                    
    //wEepromSerialNumberM      wEepromSerialNumberL       wEepromStandardValue    
};

const INT16S wEepromDefaultHisDataArray[ ] =
{
    0x0,                        0xaaaa,                    0xaaaa,                    0x0,
    //wEepromHisPVLinkStatus    wEepromHisArcBoardStatus   wEepromHisArcingStatus     wEepromHisSave1
    0x0,                        0x0,                       0x5554
    //wEepromHisSave2           wEepromHisSave3            wEempromHisSum00
};

const INT16S wEepromDefaultAdcAdjustArray[ ]  = 
{
    10223,                        0,                           10223,                       0,              
    //wEepromPOBusVolScale        wEepromPOBusVolOffset        wEepromONBusVolScale         wEepromONBusVolOffset
    13184,                        2048,                        0x8B5E,                      26435,                                
    //wEepromBatteryCurr1Scale    wEepromBatteryCurr1Offset    wEepromADCofeeSum00          wEepromBatteryCurr2Scale                    
    2048,                         13184,                       2048,                        26435,                            
    //wEepromBatteryCurr2Offset   wEepromBatteryCurr3Scale     wEepromBatteryCurr3Offset    wEepromBatteryCurr4Scale  
    2048,                         0x1A06,                      16059,                       2048,                                     
    //wEepromBatteryCurr4Offset   wEepromADCofeeSum01          wEepromRSGridVolScale        wEepromRSGridVolOffset                       
    16059,                        2048,                        16059,                       2048,                 
    //wEepromSTGridVolScale       wEepromSTGridVolOffset       wEepromTRGridVolScale        wEepromTRGridVolOffset
    0xD431,                       12858,                       2048,                        12858,             
    //wEepromADCofeeSum02         wEepromRInvVolScale          wEepromRInvVolOffset         wEepromSInvVolScale
    2048,                         12858,                       2048,                        0xAEAE,        
    //wEepromSInvVolOffset        wEepromTInvVolScale          wEepromTInvVolOffset         wEepromADCofeeSum03
    26601,                        2048,                        26601,                       2048,             
    //wEepromRDCICurrScale        wEepromRDCICurrOffset        wEepromSDCICurrScale         wEepromSDCICurrOffset   
    26601,                        2048,                        0x4FBB,                      22282,               
    //wEepromTDCICurrScale        wEepromTDCICurrOffset        wEepromADCofeeSum04          wEepromRInvCurrScale
    2048,                         22282,                       2048,                        22282,
    //wEepromRInvCurrOffset       wEepromSInvCurrScale         wEepromSInvCurrOffset        wEepromTInvCurrScale
    2048,                         0x1D1E,                      16773,                       0,                     
    //wEepromTInvCurrOffset       wEepromADCofeeSum05          wEepromIsoVolScale           wEepromIsoVolOffset 
    18630,                        177,                         18706,                       0,                         
    //wEepromBatteryOutVolScale   wEepromBatteryOutVolOffset   wEepromBatteryInVolScale     wEepromBatteryInVolOffset
    0xD40E,                       14944,                       2048,                        0,                    
    //wEepromADCofeeSum06         wEepromGFCICurrScale         wEepromGFCICurrOffset        wEepromEnvOffset                  
    0,                            0,                           0,                           0x4260,               
    //wEepromHeatOffset           wEepromXFMROffset            wEepromTfTmpOffset           wEepromADCofeeSum07 
    0,                            0,                           0,                           0,
    //wEepromSave1                wEepromSave2                 wEepromSave3                 wEepromSave4
    0,                            0,                           0,                           0,
    //wEepromSave5                wEepromSave6                 wEepromADCofeeSum08          wEepromSave7  
    0,                            0,                           0,                           0,       
    //wEepromSave8                wEepromSave9                 wEepromSave10                wEepromSave11      
    0,                            0                                          
    //wEepromSave12               wEepromADCofeeSum09       
};

const INT16S wEepromDefaultAdcAdjMaxArray[ ] = 
{
    11245,                        100,                         11245,                       100,              
    //wEepromPOBusVolScale        wEepromPOBusVolOffset        wEepromONBusVolScale         wEepromONBusVolOffset
    14502,                        2148,                        0x99AC,                      29078,                                
    //wEepromBatteryCurr1Scale    wEepromBatteryCurr1Offset    wEepromADCofeeSum00          wEepromBatteryCurr2Scale                    
    2148,                         14502,                       2148,                        29078,                            
    //wEepromBatteryCurr2Offset   wEepromBatteryCurr3Scale     wEepromBatteryCurr3Offset    wEepromBatteryCurr4Scale  
    2148,                         0x34FE,                      17665,                       2148,                                     
    //wEepromBatteryCurr4Offset   wEepromADCofeeSum01          wEepromRSGridVolScale        wEepromRSGridVolOffset                       
    17665,                        2148,                        17665,                       2148,                 
    //wEepromSTGridVolScale       wEepromSTGridVolOffset       wEepromTRGridVolScale        wEepromTRGridVolOffset
    0xE82F,                       14144,                       2148,                        14144,             
    //wEepromADCofeeSum02         wEepromRInvVolScale          wEepromRInvVolOffset         wEepromSInvVolScale
    2148,                         14144,                       2148,                        0xBEEC,        
    //wEepromSInvVolOffset        wEepromTInvVolScale          wEepromTInvVolOffset         wEepromADCofeeSum03
    29261,                        2148,                        29261,                       2148,             
    //wEepromRDCICurrScale        wEepromRDCICurrOffset        wEepromSDCICurrScale         wEepromSDCICurrOffset   
    29261,                        2148,                        0x7013,                      24510,               
    //wEepromTDCICurrScale        wEepromTDCICurrOffset        wEepromADCofeeSum04          wEepromRInvCurrScale
    2148,                         24510,                       2148,                        24510,
    //wEepromRInvCurrOffset       wEepromSInvCurrScale         wEepromSInvCurrOffset        wEepromTInvCurrScale
    2148,                         0x3866,                      18450,                       100,                     
    //wEepromTInvCurrOffset       wEepromADCofeeSum05          wEepromIsoVolScale           wEepromIsoVolOffset 
    20493,                        277,                         20577,                       100,                         
    //wEepromBatteryOutVolScale   wEepromBatteryOutVolOffset   wEepromBatteryInVolScale     wEepromBatteryInVolOffset
    0xEA5D,                       16438,                       2148,                        100,                    
    //wEepromADCofeeSum06         wEepromGFCICurrScale         wEepromGFCICurrOffset        wEepromEnvOffset                  
    100,                          100,                         100,                         0x4A2A,               
    //wEepromHeatOffset           wEepromXFMROffset            wEepromTfTmpOffset           wEepromADCofeeSum07 
    0,                            0,                           0,                           0,
    //wEepromSave1                wEepromSave2                 wEepromSave3                 wEepromSave4
    0,                            0,                           0,                           0,
    //wEepromSave5                wEepromSave6                 wEepromADCofeeSum08          wEepromSave7  
    0,                            0,                           0,                           0,       
    //wEepromSave8                wEepromSave9                 wEepromSave10                wEepromSave11      
    0,                            0                                          
    //wEepromSave12               wEepromADCofeeSum09
};

const INT16S wEepromDefaultAdcAdjMinArray[ ] = 
{
    9201,                         -100,                        9201,                        -100,              
    //wEepromPOBusVolScale        wEepromPOBusVolOffset        wEepromONBusVolScale         wEepromONBusVolOffset
    11866,                        1948,                        0x7D10,                      23792,                                
    //wEepromBatteryCurr1Scale    wEepromBatteryCurr1Offset    wEepromADCofeeSum00          wEepromBatteryCurr2Scale                    
    1948,                         11866,                       1948,                        23792,                            
    //wEepromBatteryCurr2Offset   wEepromBatteryCurr3Scale     wEepromBatteryCurr3Offset    wEepromBatteryCurr4Scale  
    1948,                         0xFF0E,                      14453,                       1948,                                     
    //wEepromBatteryCurr4Offset   wEepromADCofeeSum01          wEepromRSGridVolScale        wEepromRSGridVolOffset                       
    14453,                        1948,                        14453,                       1948,                 
    //wEepromSTGridVolScale       wEepromSTGridVolOffset       wEepromTRGridVolScale        wEepromTRGridVolOffset
    0xC033,                       11572,                       1948,                        11572,             
    //wEepromADCofeeSum02         wEepromRInvVolScale          wEepromRInvVolOffset         wEepromSInvVolScale
    1948,                         11572,                       1948,                        0x9E70,        
    //wEepromSInvVolOffset        wEepromTInvVolScale          wEepromTInvVolOffset         wEepromADCofeeSum03
    23941,                        1948,                        23941,                       1948,             
    //wEepromRDCICurrScale        wEepromRDCICurrOffset        wEepromSDCICurrScale         wEepromSDCICurrOffset   
    23941,                        1948,                        0x2F63,                      20054,               
    //wEepromTDCICurrScale        wEepromTDCICurrOffset        wEepromADCofeeSum04          wEepromRInvCurrScale
    1948,                         20054,                       1948,                        20054,
    //wEepromRInvCurrOffset       wEepromSInvCurrScale         wEepromSInvCurrOffset        wEepromTInvCurrScale
    1948,                         0x1D6,                       15096,                       -100,                     
    //wEepromTInvCurrOffset       wEepromADCofeeSum05          wEepromIsoVolScale           wEepromIsoVolOffset 
    16767,                        77,                          16853,                       -100,                         
    //wEepromBatteryOutVolScale   wEepromBatteryOutVolOffset   wEepromBatteryInVolScale     wEepromBatteryInVolOffset
    0xBDD1,                       13450,                       1948,                        -100,                    
    //wEepromADCofeeSum06         wEepromGFCICurrScale         wEepromGFCICurrOffset        wEepromEnvOffset                  
    -100,                         -100,                        -100,                        0x3A96,               
    //wEepromHeatOffset           wEepromXFMROffset            wEepromTfTmpOffset           wEepromADCofeeSum07 
    0,                            0,                           0,                           0,
    //wEepromSave1                wEepromSave2                 wEepromSave3                 wEepromSave4
    0,                            0,                           0,                           0,
    //wEepromSave5                wEepromSave6                 wEepromADCofeeSum08          wEepromSave7  
    0,                            0,                           0,                           0,       
    //wEepromSave8                wEepromSave9                 wEepromSave10                wEepromSave11      
    0,                            0                                          
    //wEepromSave12               wEepromADCofeeSum09
};

const INT16S wEepromDefaultProtectArrayRule21[ ] = 
{ 
    0x44c,                              0x514,                             0x370,                         0x834,                       
    //wEepromGridVolMax_1               wEepromVolMaxTripTime_1            wEepromGridVolMin_1            wEepromVolMinTripTime_1         
    0x4b0,                              0x10,                              0x19C4,                        0x2bc,                                                           
    //wEepromGridVolMax_2               wEepromVolMaxTripTime_2            wEepromSum00                   wEepromGridVolMin_2 
    0x44c,                              0x4b0,                             0x10,                          0x1f4,
    //wEepromVolMinTripTime_2           wEepromGridVolMax_3                wEepromVolMaxTripTime_3        wEepromGridVolMin_3
    0x96,                               0xE52,                             0x44c,                         0x370,
    //wEepromVolMinTripTime_3           wEepromSum01                       wEepromGridVolRecover_Max      wEepromGridVolRecover_Min
    0x7530,                             0x28,                              0x10,                          0x2,
    //wEepromGridVolRecover_Time        wEepromGridVoltBalanceMax          wEepromGridFaultPowerStep      wEepromPowerOnWaitTime
    0x7D26,                             0x32,                              0x3a98,                        0xff6a,
    //wEempromSum02                     wEepromGridFreqMax_1               wEepromFreqMaxTripTime_1       wEepromGridFreqMin_1
    0x3a98,                             0xc8,                              0x6,                           0x759A,
	//wEepromFreqMinTripTime_1          wEepromGridFreqMax_2               wEepromFreqMaxTripTime_2       wEepromSum03
	0xfed4,                             0x6,                               0x32,                          0xff6a,
	//wEepromGridFreqMin_2              wEepromFreqMinTripTime_2           wEepromGridFreqRecover_Max     wEepromGridFreqRecover_Min
    0x7530,                             0x1033,                            0x83D9,                        0x33e,
    //wEepromGridFreqRecover_Time       wEepromSysCtrSignal                wEepromSum04                   wEepromLVRTTripVol
    0x44c,                              0xf,                               0x14,                          2710,
    //wEepromHVRTTripVol                wEepromFVRTPosCurrK                wEepromFVRTNegCurrK            wEepromFVRTPowerStep
    0xa,                                0x2EC7,                            0x1f4,                         0xc8,
    //wEepromOvFWTripFreq               wEepromSum05                       wEepromOvFWRatio               wEepromOvFWExitFreq
    0xfff6,                             0x1f4,                             0xff38,                        0x3e8,
    //wEepromUnFWTripFreq               wEepromUnFWRatio                   wEepromUnFWExitFreq            wEepromFWRecPowerStep
    0x7C6,                              0x1f4,                             0x1f4,                         0x1f4,
    //wEepromSum06                      wEepromFWActTime                   wEepromVWActTime               wEepromPF_PCurveActPw1
    0x3e8,                              0x3e8,                             0xfc7c,                        0xA28,
    //wEepromPF_PCurvePF1               wEepromPF_PCurveActPw2             wEepromPF_PCurvePF2            wEepromSum07
    0x3e8,                              0x384,                             0x409,                         0x0,
    //wEepromPF_PCurveLockInV           wEepromPF_PCurveLockOutV           wEepromQ_UCurveVolt1s          wEepromQ_UCurveReactPw1s
    0x42e,                              0xfed4,                            0xE77,                         0x3c7,
    //wEepromQ_UCurveVolt2s             wEepromQ_UCurveReactPw2s           wEepromSum08                   wEepromQ_UCurveVolt1i
    0x0,                                0x398,                             0x12c,                         0xc8,
    //wEepromQ_UCurveReactPw1i          wEepromQ_UCurveVolt2i              wEepromQ_UCurveReactPw2i       wEepromQ_UCurveLockInP
    0x32,                               0x985,                             0x8c,                          0x9c4,
    //wEepromQ_UCurveLockOutP           wEepromSum09                       wEepromIsoImpMin               wEepromGFCIMax
    0x70c,                              0x424,                             0x438,                         0x1f4,
    //wEepromDCIMax                     wEepromOvVWStartVolt               wEepromOvVWStopVolt            wEepromOvVWRatio
    0x1BAC,                             0x3ac,                             0x398,                         0x1f4,
    //wEepromSum10                      wEepromUnVWStartVolt               wEepromUnVWStopVolt            wEepromUnVWRatio
    0x1f4,                              0x385,                               0x0,                           0xDE8
    //wEepromQUActTime                  wEepromIsoRes                   wEepromUserSave1               wEepromSum11
};
const INT16S wEepromDefaultProtMaxArrayRule21[ ] = 
{
	0x514,                              0x1770,                            0x3e8,                         0x1770,                       
    //wEepromGridVolMax_1               wEepromVolMaxTripTime_1            wEepromGridVolMin_1            wEepromVolMinTripTime_1         
    0x514,                              0x1770,                            0x5460,                        0x3e8,                                                           
    //wEepromGridVolMax_2               wEepromVolMaxTripTime_2            wEepromSum00                   wEepromGridVolMin_2 
    0x1770,                             0x514,                             0x1770,                        0x3e8,
    //wEepromVolMinTripTime_2           wEepromGridVolMax_3                wEepromVolMaxTripTime_3        wEepromGridVolMin_3
    0x1770,                             0x5334,                            0x4b0,                         0x3e8,
    //wEepromVolMinTripTime_3           wEepromSum01                       wEepromGridVolRecover_Max      wEepromGridVolRecover_Min
    0x7fff,                             0x64,                              0x3e8,                         0x384,
    //wEepromGridVolRecover_Time        wEepromGridVoltBalanceMax          wEepromGridFaultPowerStep      wEepromPowerOnWaitTime
    0x9067,                             0x1f4,                             0x7d00,                        0x0,
    //wEempromSum02                     wEepromGridFreqMax_1               wEepromFreqMaxTripTime_1       wEepromGridFreqMin_1
    0x7d00,                             0x1f4,                             0x7d00,                        0x7AE8,
	//wEepromFreqMinTripTime_1          wEepromGridFreqMax_2               wEepromFreqMaxTripTime_2       wEepromSum03
	0x0,                                0x7d00,                            0x1f4,                         0x0,
	//wEepromGridFreqMin_2              wEepromFreqMinTripTime_2           wEepromGridFreqRecover_Max     wEepromGridFreqRecover_Min
    0x7fff,                             0x1133,                            0x1026,                        0x3e8,
    //wEepromGridFreqRecover_Time       wEepromSysCtrSignal                wEepromSum04                   wEepromLVRTTripVol
    0x514,                              0x32,                              0x32,                          0x2710,
    //wEepromHVRTTripVol                wEepromFVRTPosCurrK                wEepromFVRTNegCurrK            wEepromFVRTPowerStep
    0x1f4,                              0x3264,                            0x3e8,                         0x1f4,
    //wEepromOvFWTripFreq               wEepromSum05                       wEepromOvFWRatio               wEepromOvFWExitFreq
    0x0,                                0x3e8,                             0x0,                           0x2710,
    //wEepromUnFWTripFreq               wEepromUnFWRatio                   wEepromUnFWExitFreq            wEepromFWRecPowerStep
    0x30D4,                             0x3e8,                             0x3e8,                         0x3e8,
    //wEepromSum06                      wEepromFWActTime                   wEepromVWActTime               wEepromPF_PCurveActPw1
    0x3e8,                              0x3e8,                             0x3e8,                         0x1770,
    //wEepromPF_PCurvePF1               wEepromPF_PCurveActPw2             wEepromPF_PCurvePF2            wEepromSum07
    0x44c,                              0x3e8,                             0x44c,                         0x258,
    //wEepromPF_PCurveLockInV           wEepromPF_PCurveLockOutV           wEepromQ_UCurveVolt1s          wEepromQ_UCurveReactPw1s
    0x44c,                              0x258,                             0x157C,                        0x3e8,
    //wEepromQ_UCurveVolt2s             wEepromQ_UCurveReactPw2s           wEepromSum08                   wEepromQ_UCurveVolt1i
    0x258,                              0x3e8,                             0x258,                         0x3e8,
    //wEepromQ_UCurveReactPw1i          wEepromQ_UCurveVolt2i              wEepromQ_UCurveReactPw2i       wEepromQ_UCurveLockInP
    0x3e8,                              0x1450,                            0x3e8,                         0xBB8,
    //wEepromQ_UCurveLockOutP           wEepromSum09                       wEepromIsoImpMin               wEepromGFCIMax
    0xe18,                              0x44c,                             0x44c,                         0x3e8,
    //wEepromDCIMax                     wEepromOvVWStartVolt               wEepromOvVWStopVolt            wEepromOvVWRatio
    0x2A38,                             0x3e8,                             0x3e8,                         0x3e8,
    //wEepromSum10                      wEepromUnVWStartVolt               wEepromUnVWStopVolt            wEepromUnVWRatio
    0x3e8,                              0x4E20,                               0x0,                           0x5DC0
    //wEepromQUActTime                  wEepromIsoRes                   wEepromUserSave1               wEepromSum11
};
const INT16S wEepromDefaultProtMinArrayRule21[ ] = 
{
	0x3e8,                              0x5,                               0x64,                          0x5,                       
    //wEepromGridVolMax_1               wEepromVolMaxTripTime_1            wEepromGridVolMin_1            wEepromVolMinTripTime_1         
    0x3e8,                              0x5,                               0x843,                         0x64,                                                           
    //wEepromGridVolMax_2               wEepromVolMaxTripTime_2            wEepromSum00                   wEepromGridVolMin_2 
    0x5,                                0x3e8,                             0x5,                           0x0,
    //wEepromVolMinTripTime_2           wEepromGridVolMax_3                wEepromVolMaxTripTime_3        wEepromGridVolMin_3
    0x5,                                0x45B,                             0x3e8,                         0x1f4,
    //wEepromVolMinTripTime_3           wEepromSum01                       wEepromGridVolRecover_Max      wEepromGridVolRecover_Min
    0x8000,                             0x14,                              0x1,                           0x0,
    //wEepromGridVolRecover_Time        wEepromGridVoltBalanceMax          wEepromGridFaultPowerStep      wEepromPowerOnWaitTime
    0x85F1,                             0x0,                               0x4,                           0xfce0,
    //wEempromSum02                     wEepromGridFreqMax_1               wEepromFreqMaxTripTime_1       wEepromGridFreqMin_1
    0x4,                                0x0,                               0x4,                           0xFE18,
	//wEepromFreqMinTripTime_1          wEepromGridFreqMax_2               wEepromFreqMaxTripTime_2       wEepromSum03
	0xfce0,                             0x4,                               0x0,                           0xfe0c,
	//wEepromGridFreqMin_2              wEepromFreqMinTripTime_2           wEepromGridFreqRecover_Max     wEepromGridFreqRecover_Min
    0x8000,                             0x0,                               0x7C1C,                        0x0,
    //wEepromGridFreqRecover_Time       wEepromSysCtrSignal                wEepromSum04                   wEepromLVRTTripVol
    0x3e8,                              0x0,                               0x0,                           0xa,
    //wEepromHVRTTripVol                wEepromFVRTPosCurrK                wEepromFVRTNegCurrK            wEepromFVRTPowerStep
    0x0,                                0x3F2,                             0xa,                           0x0,
    //wEepromOvFWTripFreq               wEepromSum05                       wEepromOvFWRatio               wEepromOvFWExitFreq
    0xfe0c,                             0xa,                               0xfe0c,                        0xa,
    //wEepromUnFWTripFreq               wEepromUnFWRatio                   wEepromUnFWExitFreq            wEepromFWRecPowerStep
    0xFC36,                             0x14,                              0x14,                          0xfd66,
    //wEepromSum06                      wEepromFWActTime                   wEepromVWActTime               wEepromPF_PCurveActPw1
    0xfc18,                             0xfd66,                            0xfc18,                        0xF324,
    //wEepromPF_PCurvePF1               wEepromPF_PCurveActPw2             wEepromPF_PCurvePF2            wEepromSum07
    0x3e8,                              0x370,                             0x3e8,                         0xfda8,
    //wEepromPF_PCurveLockInV           wEepromPF_PCurveLockOutV           wEepromQ_UCurveVolt1s          wEepromQ_UCurveReactPw1s
    0x3e8,                              0xfda8,                            0xA78,                         0x370,
    //wEepromQ_UCurveVolt2s             wEepromQ_UCurveReactPw2s           wEepromSum08                   wEepromQ_UCurveVolt1i
    0xfda8,                             0x370,                             0xfda8,                        0x32,
    //wEepromQ_UCurveReactPw1i          wEepromQ_UCurveVolt2i              wEepromQ_UCurveReactPw2i       wEepromQ_UCurveLockInP
    0x32,                               0x294,                             0xa,                           0x7D0,
    //wEepromQ_UCurveLockOutP           wEepromSum09                       wEepromIsoImpMin               wEepromGFCIMax
    0xa,                                0x3e8,                             0x3e8,                         0xa,
    //wEepromDCIMax                     wEepromOvVWStartVolt               wEepromOvVWStopVolt            wEepromOvVWRatio
    0xFBE,                              0x384,                             0x384,                         0xa,
    //wEepromSum10                      wEepromUnVWStartVolt               wEepromUnVWStopVolt            wEepromUnVWRatio
    0x14,                               0x0,                               0x0,                           0x726
    //wEepromQUActTime                  wEepromIsoRes                   wEepromUserSave1               wEepromSum11
};

const INT16S wEepromDefaultProtectArrayIEEE1547[ ] = 
{ 
    0x44c,                              0x64,                              0x370,                         0xc8,                       
    //wEepromGridVolMax_1               wEepromVolMaxTripTime_1            wEepromGridVolMin_1            wEepromVolMinTripTime_1         
    0x4b0,                              0x10,                              0xDA8,                         0x258,                                                           
    //wEepromGridVolMax_2               wEepromVolMaxTripTime_2            wEepromSum00                   wEepromGridVolMin_2 
    0x64,                               0x4b0,                             0x10,                          0x1c2,
    //wEepromVolMinTripTime_2           wEepromGridVolMax_3                wEepromVolMaxTripTime_3        wEepromGridVolMin_3
    0x10,                               0x94E,                             0x438,                         0x384,
    //wEepromVolMinTripTime_3           wEepromSum01                       wEepromGridVolRecover_Max      wEepromGridVolRecover_Min
    0x7530,                             0x28,                              0x10,                          0x2,
    //wEepromGridVolRecover_Time        wEepromGridVoltBalanceMax          wEepromGridFaultPowerStep      wEepromPowerOnWaitTime
    0x7D26,                             0x32,                              0x64,                          0xffce,
    //wEempromSum02                     wEepromGridFreqMax_1               wEepromFreqMaxTripTime_1       wEepromGridFreqMin_1
    0x64,                               0xc8,                              0x8,                           0x198,
	//wEepromFreqMinTripTime_1          wEepromGridFreqMax_2               wEepromFreqMaxTripTime_2       wEepromSum03
	0xfed4,                             0x8,                               0x28,                          0xffd8,
	//wEepromGridFreqMin_2              wEepromFreqMinTripTime_2           wEepromGridFreqRecover_Max     wEepromGridFreqRecover_Min
    0x3a98,                             0x1,                               0x3975,                        0x352,
    //wEepromGridFreqRecover_Time       wEepromSysCtrSignal                wEepromSum04                   wEepromLVRTTripVol
    0x47E,                              0xF,                               0x14,                          0x1388,
    //wEepromHVRTTripVol                wEepromFVRTPosCurrK                wEepromFVRTNegCurrK            wEepromFVRTPowerStep
    0x14,                               0x1B8F,                            0x1f4,                         0xc8,
    //wEepromOvFWTripFreq               wEepromSum05                       wEepromOvFWRatio               wEepromOvFWExitFreq
    0xff9c,                             0x1f4,                             0xff38,                        0x3e8,
    //wEepromUnFWTripFreq               wEepromUnFWRatio                   wEepromUnFWExitFreq            wEepromFWRecPowerStep
    0x76C,                              0x1f4,                             0x1f4,                         0x1f4,
    //wEepromSum06                      wEepromFWActTime                   wEepromVWActTime               wEepromPF_PCurveActPw1
    0x3e8,                              0x3e8,                             0xfc7c,                        0xA28,
    //wEepromPF_PCurvePF1               wEepromPF_PCurveActPw2             wEepromPF_PCurvePF2            wEepromSum07
    0x3e8,                              0x384,                             0x438,                         0x0,
    //wEepromPF_PCurveLockInV           wEepromPF_PCurveLockOutV           wEepromQ_UCurveVolt1s          wEepromQ_UCurveReactPw1s
    0x44c,                              0xfe0c,                            0xDFC,                         0x398,
    //wEepromQ_UCurveVolt2s             wEepromQ_UCurveReactPw2s           wEepromSum08                   wEepromQ_UCurveVolt1i
    0x0,                                0x384,                             0x1f4,                         0xC8,
    //wEepromQ_UCurveReactPw1i          wEepromQ_UCurveVolt2i              wEepromQ_UCurveReactPw2i       wEepromQ_UCurveLockInP
    0x32,                               0xA0A,                             0x8c,                          0x9c4,
    //wEepromQ_UCurveLockOutP           wEepromSum09                       wEepromIsoImpMin               wEepromGFCIMax
    0x70c,                              0x424,                             0x438,                         0x1f4,
    //wEepromDCIMax                     wEepromOvVWStartVolt               wEepromOvVWStopVolt            wEepromOvVWRatio
    0x1BAC,                             0x3ac,                             0x398,                         0x1f4,
    //wEepromSum10                      wEepromUnVWStartVolt               wEepromUnVWStopVolt            wEepromUnVWRatio
    0x1f4,                              0x385,                               0x0,                           0xDE8
    //wEepromQUActTime                  wEepromIsoRes                   wEepromUserSave1               wEepromSum11
};
const INT16S wEepromDefaultProtMaxArrayIEEE1547[ ] = 
{
	0x546,                              0x7fff,                            0x3e8,                         0x7fff,                       
    //wEepromGridVolMax_1               wEepromVolMaxTripTime_1            wEepromGridVolMin_1            wEepromVolMinTripTime_1         
    0x546,                              0x7fff,                            0x8e71,                        0x3e8,                                                           
    //wEepromGridVolMax_2               wEepromVolMaxTripTime_2            wEepromSum00                   wEepromGridVolMin_2 
    0x7fff,                             0x546,                             0x7fff,                        0x3e8,
    //wEepromVolMinTripTime_2           wEepromGridVolMax_3                wEepromVolMaxTripTime_3        wEepromGridVolMin_3
    0x7fff,                             0x8d13,                            0x546,                         0x3e8,
    //wEepromVolMinTripTime_3           wEepromSum01                       wEepromGridVolRecover_Max      wEepromGridVolRecover_Min
    0x7fff,                             0x64,                              0x3e8,                         0x384,
    //wEepromGridVolRecover_Time        wEepromGridVoltBalanceMax          wEepromGridFaultPowerStep      wEepromPowerOnWaitTime
    0x90FD,                             0x1f4,                             0x7fff,                        0x0,
    //wEempromSum02                     wEepromGridFreqMax_1               wEepromFreqMaxTripTime_1       wEepromGridFreqMin_1
    0x7fff,                             0x1f4,                             0x7fff,                        0x83E5,
	//wEepromFreqMinTripTime_1          wEepromGridFreqMax_2               wEepromFreqMaxTripTime_2       wEepromSum03
	0x0,                                0x7fff,                            0x1f4,                         0x0,
	//wEepromGridFreqMin_2              wEepromFreqMinTripTime_2           wEepromGridFreqRecover_Max     wEepromGridFreqRecover_Min
    0x7fff,                             0x1133,                            0x1325,                        0x3e8,
    //wEepromGridFreqRecover_Time       wEepromSysCtrSignal                wEepromSum04                   wEepromLVRTTripVol
    0x546,                              0x32,                              0x32,                          0x2710,
    //wEepromHVRTTripVol                wEepromFVRTPosCurrK                wEepromFVRTNegCurrK            wEepromFVRTPowerStep
    0x1f4,                              0x3296,                            0x3e8,                         0x1f4,
    //wEepromOvFWTripFreq               wEepromSum05                       wEepromOvFWRatio               wEepromOvFWExitFreq
    0x0,                                0x3e8,                             0x0,                           0x2710,
    //wEepromUnFWTripFreq               wEepromUnFWRatio                   wEepromUnFWExitFreq            wEepromFWRecPowerStep
    0x30D4,                             0x3e8,                             0x3e8,                         0x3e8,
    //wEepromSum06                      wEepromFWActTime                   wEepromVWActTime               wEepromPF_PCurveActPw1
    0x3e8,                              0x3e8,                             0x3e8,                         0x1770,
    //wEepromPF_PCurvePF1               wEepromPF_PCurveActPw2             wEepromPF_PCurvePF2            wEepromSum07
    0x44c,                              0x3e8,                             0x44c,                         0x258,
    //wEepromPF_PCurveLockInV           wEepromPF_PCurveLockOutV           wEepromQ_UCurveVolt1s          wEepromQ_UCurveReactPw1s
    0x44c,                              0x258,                             0x157C,                        0x3e8,
    //wEepromQ_UCurveVolt2s             wEepromQ_UCurveReactPw2s           wEepromSum08                   wEepromQ_UCurveVolt1i
    0x258,                              0x3e8,                             0x258,                         0x3e8,
    //wEepromQ_UCurveReactPw1i          wEepromQ_UCurveVolt2i              wEepromQ_UCurveReactPw2i       wEepromQ_UCurveLockInP
    0x3e8,                              0x1450,                            0x3e8,                         0xBB8,
    //wEepromQ_UCurveLockOutP           wEepromSum09                       wEepromIsoImpMin               wEepromGFCIMax
    0xe18,                              0x44c,                             0x44c,                         0x3e8,
    //wEepromDCIMax                     wEepromOvVWStartVolt               wEepromOvVWStopVolt            wEepromOvVWRatio
    0x2A38,                             0x3e8,                             0x3e8,                         0x3e8,
    //wEepromSum10                      wEepromUnVWStartVolt               wEepromUnVWStopVolt            wEepromUnVWRatio
    0x3e8,                              0x4e20,                               0x0,                           0x5DC0
    //wEepromQUActTime                  wEepromIsoRes                   wEepromUserSave1               wEepromSum11
};
const INT16S wEepromDefaultProtMinArrayIEEE1547[ ] = 
{
	0x3e8,                              0x8000,                            0x0,                           0x8000,                       
    //wEepromGridVolMax_1               wEepromVolMaxTripTime_1            wEepromGridVolMin_1            wEepromVolMinTripTime_1         
    0x3e8,                              0x8000,                            0x87D0,                        0x0,                                                           
    //wEepromGridVolMax_2               wEepromVolMaxTripTime_2            wEepromSum00                   wEepromGridVolMin_2 
    0x8000,                             0x3e8,                             0x8000,                        0x0,
    //wEepromVolMinTripTime_2           wEepromGridVolMax_3                wEepromVolMaxTripTime_3        wEepromGridVolMin_3
    0x8000,                             0x83E8,                            0x3e8,                         0x0,
    //wEepromVolMinTripTime_3           wEepromSum01                       wEepromGridVolRecover_Max      wEepromGridVolRecover_Min
    0x8000,                             0x14,                              0x1,                           0x0,
    //wEepromGridVolRecover_Time        wEepromGridVoltBalanceMax          wEepromGridFaultPowerStep      wEepromPowerOnWaitTime
    0x83fd,                             0x0,                               0x8000,                        0xfe0c,
    //wEempromSum02                     wEepromGridFreqMax_1               wEepromFreqMaxTripTime_1       wEepromGridFreqMin_1
    0x8000,                             0x0,                               0x8000,                        0x7E0C,
	//wEepromFreqMinTripTime_1          wEepromGridFreqMax_2               wEepromFreqMaxTripTime_2       wEepromSum03
	0xfe0c,                             0x8000,                            0x0,                           0xfe0c,
	//wEepromGridFreqMin_2              wEepromFreqMinTripTime_2           wEepromGridFreqRecover_Max     wEepromGridFreqRecover_Min
    0x8000,                             0x0,                               0xFC18,                        0x0,
    //wEepromGridFreqRecover_Time       wEepromSysCtrSignal                wEepromSum04                   wEepromLVRTTripVol
    0x3e8,                              0x0,                               0x0,                           0xa,
    //wEepromHVRTTripVol                wEepromFVRTPosCurrK                wEepromFVRTNegCurrK            wEepromFVRTPowerStep
    0x0,                                0x3F2,                             0xa,                           0x0,
    //wEepromOvFWTripFreq               wEepromSum05                       wEepromOvFWRatio               wEepromOvFWExitFreq
    0xfe0c,                             0xa,                               0xfe0c,                        0xa,
    //wEepromUnFWTripFreq               wEepromUnFWRatio                   wEepromUnFWExitFreq            wEepromFWRecPowerStep
    0xFC36,                             0x14,                              0x14,                          0xfd66,
    //wEepromSum06                      wEepromFWActTime                   wEepromVWActTime               wEepromPF_PCurveActPw1
    0xfc18,                             0xfd66,                            0xfc18,                        0xF324,
    //wEepromPF_PCurvePF1               wEepromPF_PCurveActPw2             wEepromPF_PCurvePF2            wEepromSum07
    0x3e8,                              0x370,                             0x3e8,                         0xfda8,
    //wEepromPF_PCurveLockInV           wEepromPF_PCurveLockOutV           wEepromQ_UCurveVolt1s          wEepromQ_UCurveReactPw1s
    0x3e8,                              0xfda8,                            0xA78,                         0x370,
    //wEepromQ_UCurveVolt2s             wEepromQ_UCurveReactPw2s           wEepromSum08                   wEepromQ_UCurveVolt1i
    0xfda8,                             0x370,                             0xfda8,                        0x32,
    //wEepromQ_UCurveReactPw1i          wEepromQ_UCurveVolt2i              wEepromQ_UCurveReactPw2i       wEepromQ_UCurveLockInP
    0x32,                               0x294,                             0xa,                           0x7d0,
    //wEepromQ_UCurveLockOutP           wEepromSum09                       wEepromIsoImpMin               wEepromGFCIMax
    0xa,                                0x3e8,                             0x3e8,                         0xa,
    //wEepromDCIMax                     wEepromOvVWStartVolt               wEepromOvVWStopVolt            wEepromOvVWRatio
    0xFBE,                              0x384,                             0x384,                         0xa,
    //wEepromSum10                      wEepromUnVWStartVolt               wEepromUnVWStopVolt            wEepromUnVWRatio
    0x14,                               0x0,                               0x0,                           0x726
    //wEepromQUActTime                  wEepromUserSave2                   wEepromUserSave1               wEepromSum11
};

const INT16S wEepromDefaultProtectSupplArray[ ] = 
{
	0x1f40,                             0x9c4,                             0xe4a8,                        0x1b58,
    //wEepromChargeVoltLimt             wEepromDisChargeVoltLimt           wEepromChargeCurrLimt          wEepromDisChargeCurrLimt
    0x1f40,                             0xfe0c,                            0x4650,                        0x251c,
    //wEepromFloatChargeVolt            wEepromSToFChargeCurr              wEepromSupplSum01              wEepromBatteryVoltMax
    0x9c4,                              0xd8f0,                            0x2710,                        0x32,
    //wEepromBatteryVoltMin             wEepromBatChargeCurrMax            wEepromBatDisChargeCurrMax     wEepromDCRelayVoltDiff
    0xd8f0,                             0x802,                             0x0,                           0x0,
    //wEepromBatPreChargeCurrMax        wEepromSupplSum02                  wEepromRemoteSel               wEepromCSVSSwitch
    0x0,                                0x0,                               0x12c0,                        0x1770,
    //wEepromAutoCSVSSwitchEn           wEepromVSCtrMode                   wEepromVSVoltSet               wEepromVSFreqSet
    0x2A30,                             0x0,                               0x0,                           0x0,
    //wEepromSupplSum03                 wEepromVSPSet                      wEepromVSQSet                  wEepromVSDroopEn
    0x64,                               0x96,                              0x0,                           0xFA,
    //wEepromVSFreqDroopKByP            wEepromVSVoltDroopKByQ             wEepromCSPCtrModeSel           wEepromSupplSum04
    0x0,                                0x0,                               0x3e8,                         0x0,
    //wEepromCSPPer                     wEepromCSCurrSet                   wEepromPActStep                wEepromCSQCtrModeSel
    0x3e8,                              0x0,                               0x7D0,                         0x3e8,
    //wEepromCSPFSet                    wEepromCSQPer                      wEepromSupplSum05              wEepromQActStep
    0x64,                               0x0,                               0x64,                          0xfa0,
    //wEepromSoftPowerStep              wEepromSoftStopEn                  wEepromStopPowerStep           wEepromPVStartUpVol
    0x352,                              0x17A2,                            0x1,                           0x1,
    //wEepromPVFitFactor                wEepromSupplSum06                  wEepromISOEnable               wEepromGFCICTChkEn
    0x0,                                0x0,                               0x0,                           0xfff,
    //wEepromAPFOption                  wEepromArcingEn                    wEepromFullRangeMpptEnable     wEepromFullRangeMpptCycle
    0x1001,                             0x1964,                            0x1f4,                         0x3c,
    //wEepromSupplSum07                 wEepromInvCurrMax                  wEepromDCIOffsetDiffMax        wEepromOffsetDiffMax
    0x2da,                              0x3a2,                             0x0,                           0x2210,
    //wEepromEnvironmentTmpMax          wEepromPowerTmpMax                 wEepromXFMRTmpMax              wEepromSupplSum08
    0x0,                                0x23f0,                            0x12c0,                        0x0,
    //wEepromTransformerTmpMax          wEepromBusVolMax                   wEepromCapVolMax               wEepromDCRelayActProt
    0x0,                                0x0,                               0x36B0,                        0x352,
    //wEepromBiDirDCSwitch              wEepromBatteryType                 wEepromSupplSum09              wEepromIsoRes
    0x0,                                0x0,                               0x0,                           0x0,
    //wEepromClientSave5                wEepromClientSave6                 wEepromClientSave7             wEepromClientSave8
    0x0,                                0x0
    //wEepromClientSave9                wEepromSupplSum10    
};

const INT16S wEepromDefaultProtMaxSupplArray[ ] = 
{
	0x23f0,                             0x23f0,                            0x0,                           0x1b58,
    //wEepromChargeVoltLimt             wEepromDisChargeVoltLimt           wEepromChargeCurrLimt          wEepromDisChargeCurrLimt
    0x23f0,                             0x0,                               0x8728,                        0x251c,
    //wEepromFloatChargeVolt            wEepromSToFChargeCurr              wEepromSupplSum01              wEepromBatteryVoltMax
    0x251c,                             0x0,                               0x2af8,                        0x12c,
    //wEepromBatteryVoltMin             wEepromBatChargeCurrMax            wEepromBatDisChargeCurrMax     wEepromDCRelayVoltDiff
    0x0,                                0x765C,                            0x1,                           0x1,
    //wEepromBatPreChargeCurrMax        wEepromSupplSum02                  wEepromRemoteSel               wEepromCSVSSwitch
    0x1,                                0x1,                               0x1450,                        0x1f40,
    //wEepromAutoCSVSSwitchEn           wEepromVSCtrMode                   wEepromVSVoltSet               wEepromVSFreqSet
    0x3394,                             0x3e8,                             0x258,                         0x1,
    //wEepromSupplSum03                 wEepromVSPSet                      wEepromVSQSet                  wEepromVSDroopEn
    0x190,                              0x320,                             0x1,                           0xAF2,
    //wEepromVSFreqDroopKByP            wEepromVSVoltDroopKByQ             wEepromCSPCtrModeSel           wEepromSupplSum04
    0x3e8,                              0x1b58,                            0x2710,                        0x4,
    //wEepromCSPPer                     wEepromCSCurrSet                   wEepromPActStep                wEepromCSQCtrModeSel
    0x3e8,                              0x258,                             0x4C94,                        0x2710,
    //wEepromCSPFSet                    wEepromCSQPer                      wEepromSupplSum05              wEepromQActStep
    0xbb8,                              0x1,                               0xbb8,                         0x1f40,
    //wEepromSoftPowerStep              wEepromSoftStopEn                  wEepromStopPowerStep           wEepromPVStartUpVol
    0x3b6,                              0x6177,                            0x1,                           0x1,
    //wEepromPVFitFactor                wEepromSupplSum06                  wEepromISOEnable               wEepromGFCICTChkEn
    0x1,                                0x1,                               0x1,                           0x7fff,
    //wEepromAPFOption                  wEepromArcingEn                    wEepromFullRangeMpptEnable     wEepromFullRangeMpptCycle
    0x8004,                             0x1b58,                            0x2aa,                         0x100,
    //wEepromSupplSum07                 wEepromInvCurrMax                  wEepromDCIOffsetDiffMax        wEepromOffsetDiffMax
    0x2ee,                              0x3b6,                             0x0,                           0x25A6,
    //wEepromEnvironmentTmpMax          wEepromPowerTmpMax                 wEepromXFMRTmpMax              wEepromSupplSum08
    0x0,                                0x24b8,                            0x1388,                        0x1,
    //wEepromTransformerTmpMax          wEepromBusVolMax                   wEepromCapVolMax               wEepromDCRelayActProt
    0x1,                                0x3,                               0x3845,                        0x0,
    //wEepromBiDirDCSwitch              wEepromBatteryType                 wEepromSupplSum09              wEepromClientSave4
    0x0,                                0x0,                               0x0,                           0x0,
    //wEepromClientSave5                wEepromClientSave6                 wEepromClientSave7             wEepromClientSave8
    0x0,                                0x0
    //wEepromClientSave9                wEepromSupplSum10    
};

const INT16S wEepromDefaultProtMinSupplArray[ ] = 
{
	0x9c4,                              0x9c4,                             0xe4a8,                        0x0,
    //wEepromChargeVoltLimt             wEepromDisChargeVoltLimt           wEepromChargeCurrLimt          wEepromDisChargeCurrLimt
    0x9c4,                              0xe4a8,                            0xE69C,                        0x9c4,
    //wEepromFloatChargeVolt            wEepromSToFChargeCurr              wEepromSupplSum01              wEepromBatteryVoltMax
    0x0,                                0xd508,                            0x0,                           0x0,
    //wEepromBatteryVoltMin             wEepromBatChargeCurrMax            wEepromBatDisChargeCurrMax     wEepromDCRelayVoltDiff
    0xd508,                             0xB3D4,                            0x0,                           0x0,
    //wEepromBatPreChargeCurrMax        wEepromSupplSum02                  wEepromRemoteSel               wEepromCSVSSwitch
    0x0,                                0x0,                               0x7d0,                         0xbb8,
    //wEepromAutoCSVSSwitchEn           wEepromVSCtrMode                   wEepromVSVoltSet               wEepromVSFreqSet
    0x1388,                             0xfc18,                            0xfda8,                        0x0,
    //wEepromSupplSum03                 wEepromVSPSet                      wEepromVSQSet                  wEepromVSDroopEn
    0x14,                               0x32,                              0x0,                           0xFA06,
    //wEepromVSFreqDroopKByP            wEepromVSVoltDroopKByQ             wEepromCSPCtrModeSel           wEepromSupplSum04
    0xfd65,                             0xe4a8,                            0x1,                           0x0,
    //wEepromCSPPer                     wEepromCSCurrSet                   wEepromPActStep                wEepromCSQCtrModeSel
    0xfc18,                             0xfdA8,                            0xDBCE,                        0x1,
    //wEepromCSPFSet                    wEepromCSQPer                      wEepromSupplSum05              wEepromQActStep
    0x1,                                0x0,                               0x1,                           0xbb8,
    //wEepromSoftPowerStep              wEepromSoftStopEn                  wEepromStopPowerStep           wEepromPVStartUpVol
    0x1f4,                              0xDAF,                             0x0,                           0x0,
    //wEepromPVFitFactor                wEepromSupplSum06                  wEepromISOEnable               wEepromGFCICTChkEn
    0x0,                                0x0,                               0x0,                           0x8000,
    //wEepromAPFOption                  wEepromArcingEn                    wEepromFullRangeMpptEnable     wEepromFullRangeMpptCycle
    0x8000,                             0x0,                               0x0,                           0x0,
    //wEepromSupplSum07                 wEepromInvCurrMax                  wEepromDCIOffsetDiffMax        wEepromOffsetDiffMax
    0x0,                                0x0,                               0x0,                           0x0,
    //wEepromEnvironmentTmpMax          wEepromPowerTmpMax                 wEepromXFMRTmpMax              wEepromSupplSum08
    0x0,                                0xfa0,                             0x7d0,                         0x0,
    //wEepromTransformerTmpMax          wEepromBusVolMax                   wEepromCapVolMax               wEepromDCRelayActProt
    0x0,                                0x0,                               0x1770,                        0x0,
    //wEepromBiDirDCSwitch              wEepromBatteryType                 wEepromSupplSum09              wEepromClientSave4
    0x0,                                0x0,                               0x0,                           0x0,
    //wEepromClientSave5                wEepromClientSave6                 wEepromClientSave7             wEepromClientSave8
    0x0,                                0x0
    //wEepromClientSave9                wEepromSupplSum10    
};

//cEepromByteSize Eeprom 字节数容量
//cImptDataStaAdr ImptData在Eeprom中起始地址，存储字节数=2*(sizeof(wEepromImptData))*cImptDataBackupNum,其中cImptDataBackupNum表示备份数量
//cHisDataStaAdr  HisData在Eeprom中起始地址
//(cEepromByteSize-sizeof(wEepromHisData)) > cHisDataStaAdr > (cImptDataStaAdr+2*(cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData)))
//cImptDataBackupNum 重要数据备份份数，值必须>2
#define cEepromByteSize      1024
#define cImptDataStaAdr      0
#define cHisDataStaAdr       860 
#define cImptDataBackupNum   5 

INT16U uwProtAdrStart,uwAdcAdrStart,uwProtSupplAdrStart,uwHisAdrStart;

INT16S wEepromImptData[sizeof(wEepromDefaultImptMaxArray)];
INT16S wEepromProtectData[sizeof(wEepromDefaultProtectArrayRule21)];
INT16S wEepromAdcAdjustData[sizeof(wEepromDefaultAdcAdjustArray)];
INT16S wEepromProtectSupplData[sizeof(wEepromDefaultProtectSupplArray)];
INT16S wEepromHisData[56];
INT16S wLCDImptData[sizeof(wEepromImptData)];  //用于LCD读取Imptdata的缓冲变量

T_EepromStatus uwI2CBlockState;

//表示Imptdata数据LCD接收状态，对应的各位标志位：=0，未接受成功，=1接收成功，用于sfReadEepromImptdata()的状态判断
//其位数应不小于sizeof(wEepromImptData)值
T_FLAGOBJECT uwLCDRxImptDataFlag;
INT16U fEepromRdWrHalt = 0;  //=1时停止对常规Eeprom数据读写操作
INT16U fEepromOperateRequire = 0;  //上电前需要读Eeprom
INT16U fEepromOperateStep = 0;
INT16U fSciStandChanged = 0;
INT16U uwEepromWrSelNumFinished = 0;
INT16U fEepromChkError = 0;
INT16U fEepromProtSumErrNum = 0;
INT16U fEepromProtSuplSumErrNum = 0;

U_MachineCoeff fMachineCoeff;
U_RatedCoeff fRatedCoeff;
U_SysCtrSignal fSysCtrSignal;
//------------------Data Safety--------------------
#pragma DATA_SECTION(wEepromProtectData,"RAMSafetyIICProt0");
#pragma DATA_SECTION(wEepromProtectSupplData,"RAMSafetyIICProt1");

//--------------------Data--------------------------
#pragma DATA_SECTION(fEepromOperateRequire,"RamSafetyProt");

/*********************************************************************
Function name:  void InitI2cVar(void)
Description:  I2c变量初始化
Calls:        	 
Called By:  main()
Parameters:  void
Return:  void      
 *********************************************************************/
void InitI2cVar(void)
{
INT16U i;
    wI2cRxData = 0;
	wI2cTxData = 0;
    
    for(i=0;i<sizeof(wEepromImptData);i++) wEepromImptData[i] = 0;
    for(i=0;i<sizeof(wEepromProtectData);i++) wEepromProtectData[i] = 0;
    for(i=0;i<sizeof(wEepromAdcAdjustData);i++) wEepromAdcAdjustData[i] = 0;
    for(i=0;i<sizeof(wEepromHisData);i++) wEepromHisData[i] = 0;
    for(i=0;i<sizeof(wEepromProtectSupplData);i++) wEepromProtectSupplData[i] = 0;
    
    for(i=0;i<sizeof(wEepromImptData);i++) wLCDImptData[i] = 0;
   
	uwI2CBlockState.word = 0;
	
    uwLCDRxImptDataFlag.flag = 0;
    
    fEepromRdWrHalt = 0;
    
    fEepromOperateRequire = 0;
    fEepromOperateStep = 0;
    uwEepromWrSelNumFinished = 0;
    fEepromChkError = 0;
    fEepromProtSumErrNum = 0;
    fEepromProtSuplSumErrNum = 0;
    
    fSysCtrSignal.all = 0;
}

/*********************************************************************
Function name:  void sInitLCDImptData(void)
Description:  wLCDImptData在正确读取Eeprom数据后进行初始化
Calls:        	 
Called By:  main()
Parameters:  void
Return:  void      
 *********************************************************************/
void sInitLCDImptData(void)
{
INT16U i;
    for(i=0;i<sizeof(wEepromImptData);i++) wLCDImptData[i] = wEepromImptData[i];
}
 
/*********************************************************************
Function name:  void InitI2c(void)
Description:  I2c硬件初始化
Calls:        	 
Called By:  main()
Parameters:  void
Return:  void      
 *********************************************************************/
void InitI2c(void)
{
    // Initialize I2C
    //I2caRegs.I2CSAR = 0x50;              // Set slave address
    I2caRegs.I2CMDR.bit.MST = 1;           // Select master or slave mode
    I2caRegs.I2CMDR.bit.DLB = 0;           // Enable digital loopback bit
    I2caRegs.I2CPSC.all = 14;               // Prescaler - need 7-12 Mhz on module clk
    I2caRegs.I2CCLKL = 35;                 // NOTE: must be non zero
    I2caRegs.I2CCLKH = 35;                 // NOTE: must be non zero
    //I2caRegs.I2CFFTX.bit.TXFFIL = 16;    // Set fifo interrupt level
    //I2caRegs.I2CFFRX.bit.RXFFIENA = 1;   // Enable Rx Fifo interrupt
    //I2caRegs.I2CFFRX.bit.RXFFIL = 16;    // Set fifo interrupt level
    //I2caRegs.I2CFFRX.bit.RXFFINTCLR = 1; // Clear Rx interrupt flag
    I2caRegs.I2CIER.all = 0x24;            // Enable SCD & ARDY interrupts
    I2caRegs.I2CFFTX.all = 0x6000;         // Enable TxFIFO mode
    I2caRegs.I2CFFRX.all = 0x2000;         // Enable RxFIFO mode

    I2caRegs.I2CMDR.bit.IRS = 1;           // Take I2C out of reset
}//end of s12Cinit()

/* Model step function */
/*********************************************************************
Function name:  INT16U sfI2cTx(INT16U uwAdress,INT16S *Data)
Description:  向EEPROM写一个word数据
Calls:        	 
Called By:  
Parameters:  void
Return:  void      
 *********************************************************************/
INT16U sfI2cTx(INT16U uwAdress,INT16S *Data) //=0写未完成，=1写成功，=2写超时错误
{
INT16U uwWriteOrder;
static INT16U fStatus = 0;  //=0 写寄存器，=1寄存器写完
static INT16U uwWaitCnt = 0;
static INT16U uwWaitCnt1 = 0;
static INT16U uwWaitCnt2 = 0;
    if(fStatus == 0)
    {         
        if(I2caRegs.I2CMDR.bit.STP == 1)    
        {
            DelayUs(80);
            if(++uwWaitCnt1 > 250) 
            {
                uwWaitCnt1 = 250;
                return(2); 
            }
            else return(0);
        }
		else
		{
		    uwWaitCnt1 = 0;
		}
        uwWriteOrder = (cWriteOrder|((uwAdress&0x300)>>8));
        uwAdress = uwAdress&0xFF;
	    I2caRegs.I2CSAR = uwWriteOrder;      //DSP的地址,EEPROM的控制命令
        if(I2caRegs.I2CSTR.bit.BB == 1)    // Check if bus busy
        {
            DelayUs(80);
            if(++uwWaitCnt2 > 250) 
            {
                uwWaitCnt2 = 250;
                return(2); 
            }
            else return(0);
        }
		else
		{
		    uwWaitCnt2 = 0;
		}
	    I2caRegs.I2CCNT = 3;               // Set data length
	    I2caRegs.I2CDXR = uwAdress; //DSP的第一个数据,EEPROM的地址          
        I2caRegs.I2CDXR = ((INT16U)(*Data))>>8;
        I2caRegs.I2CDXR = ((INT16U)(*Data))&0xff;
        I2caRegs.I2CMDR.all = 0x6e20;       //0x6620
                 
        fStatus = 1;
        return 0;
    }
    else
    {
        if((I2caRegs.I2CMDR.bit.STP == 1) || (I2caRegs.I2CSTR.bit.BB == 1))
        {
            if(++uwWaitCnt > 3)
            {
                uwWaitCnt = 3;
                fStatus = 0;
                return 2;
            }
            else
            {
                return 0;
            }    
        }
        else
        {
            fStatus = 0;
            uwWaitCnt = 0;
			uwWaitCnt1 = 0;
			uwWaitCnt2 = 0;
            return 1;
        }
    }
}
/*********************************************************************
Function name:  INT16U sfI2cRx(INT16U uwAdress,INT16S *Data)
Description:  向EEPROM读一个word数据
Calls:        	 
Called By:  
Parameters:  void
Return:  void      
 *********************************************************************/
INT16U sfI2cRx(INT16U uwAdress,INT16S *Data) //=0读未完成，=1读成功，=2读超时错误
{
INT16U uwWriteOrder;
static INT16U uwWaitCnt = 0;  
static INT16U uwWaitCnt1 = 0; 
static INT16U uwWaitCnt2 = 0; 
static INT16U uwWaitCnt3 = 0; 
static INT16U uwWaitCnt4 = 0;
    if(I2caRegs.I2CMDR.bit.STP == 1)    
    {
        DelayUs(80);
        if(++uwWaitCnt1 > 250) 
        {
           uwWaitCnt1 = 250;
           return(2); 
        }
        else return(0);
    }
	else
	{
	    uwWaitCnt1 = 0;
	}

    uwWriteOrder = (cWriteOrder|((uwAdress&0x300)>>8));
    uwAdress = uwAdress&0xFF;
    I2caRegs.I2CSAR = uwWriteOrder;       //DSP的地址,EEPROM的控制命令 
    if (I2caRegs.I2CSTR.bit.BB == 1)    // Check if bus busy
    {
        DelayUs(80);
        if(++uwWaitCnt2 > 250) 
        {
           uwWaitCnt2 = 250;
           return(2); 
        }
        else return(0);
    }
	else
	{
	    uwWaitCnt2 = 0;
	}

	I2caRegs.I2CCNT = 1;                 // Set data length 
    I2caRegs.I2CDXR = uwAdress;           //DSP的第一个数据,EEPROM的地址
    I2caRegs.I2CMDR.all = 0x6620;
    while(I2caRegs.I2CISRC.all != 0x3)
    {
        DelayUs(80); //20
        if(++uwWaitCnt > 10) 
        {
           uwWaitCnt = 10;
           return(2); 
        }
    }  
    I2caRegs.I2CCNT = 2;                 // Set data length 
    /* NACK mode bit (bit15)
    free data modee:1 (1:enbaled)(bit14)
	start condition bit(bit13)
	reserved(bit12)
    
	stop condition bit(bit11)
    mode:1 (1:master 0:slave) (bit10) 
    mode:1 (1:transmitter 0:receiver)(bit9)
	expanded address enable bit (1:10-bit 0:7-bit)(bit8)         
		
	repeat mode bit(1:repeat)(bit7)
    digital loopback mode(1:enabled 0:disabled)(bit6)
	I2C mode reset bit(1:enable 0:reset)(bit5)
	start byte mode bit(bit4)        
		
	free data format mode bit(bit3)
	bit count bits(000:8-bits)(bit2-0) */
    
    I2caRegs.I2CMDR.all = 0x6C20; 
    while(I2caRegs.I2CMDR.bit.STP == 1)
    {      
        DelayUs(80);     //20
        if(++uwWaitCnt4 > 10) 
        {
           uwWaitCnt4 = 10;
           return(2); 
        }
    }
    if(I2caRegs.I2CFFRX.bit.RXFFST != 2)
    {
        DelayUs(80);     //20
        if(++uwWaitCnt3 > 10) 
        {
           uwWaitCnt3 = 10;
           return(2); 
        }
        else return(0); 
    }
    else
    {
    INT16U i;
	    uwWaitCnt = 0;
		uwWaitCnt1 = 0;
		uwWaitCnt2 = 0;
		uwWaitCnt3 = 0;
		uwWaitCnt4 = 0;
        i = (I2caRegs.I2CDRR&0xff)<<8;
        i = i|(I2caRegs.I2CDRR&0xff);
        *Data = (INT16S)i;
        return(1);
    }
}
/*********************************************************************
Function name:  void sEepromProtectDataSetDefaultVaule(INT16U uwDataNum)
Description:  wEepromProtectData初始化
Calls:        	 
Called By:  InitI2cVar() sSpiDataManage()
Parameters:  void
Return:  void      
 *********************************************************************/
void sEepromProtectDataSetDefaultVaule(INT16U uwDataNum)
{
INT16U i;
    if(wEepromStandardValue == cIEEE1547Standard)
    {
        for(i = 0; i < uwDataNum; i++)
        {
            wEepromProtectData[i] = wEepromDefaultProtectArrayIEEE1547[i];
        }	
    } 
	else if(wEepromStandardValue == cRule21Standard)
    {
        for(i = 0; i < uwDataNum; i++)
        {
            wEepromProtectData[i] = wEepromDefaultProtectArrayRule21[i];
        }	
    }
    
    fSysCtrSignal.all = wEepromSysCtrSignal;
    fHoldRegisterStatus = 2;
}
/*********************************************************************
Function name:  void sEepromProtectSupplDataSetDefaultVaule(INT16U uwDataNumSuppl)
Description:  wEepromProtectData初始化
Calls:        	 
Called By:  InitI2cVar() sSpiDataManage()
Parameters:  void
Return:  void      
 *********************************************************************/
void sEepromProtectSupplDataSetDefaultVaule(INT16U uwDataNum)
{
INT16U i;
    
	for(i = 0; i < uwDataNum; i++)
    {
        wEepromProtectSupplData[i] = wEepromDefaultProtectSupplArray[i];
    }
    fHoldRegisterStatus = 2; 
}
/*********************************************************************
Function name:  void sEepromADCoeffDataSetDefaultVaule(INT16U uwDataNum)
Description:  wEepromProtectData初始化
Calls:
Called By:  InitI2cVar() sSpiDataManage()
Parameters:  void
Return:  void
 *********************************************************************/
void sEepromwAdcAdjustDataSetDefaultVaule(INT16U uwDataNum)
{
INT16U i;
    for(i=0;i<uwDataNum;i++)
    {
        wEepromAdcAdjustData[i] = wEepromDefaultAdcAdjustArray[i];
    }
}
/*********************************************************************
Function name:  void sEepromADCoeffDataSetDefaultVaule(INT16U uwDataNum)
Description:  wEepromProtectData初始化
Calls:
Called By:  InitI2cVar() sSpiDataManage()
Parameters:  void
Return:  void
 *********************************************************************/
void sEepromwHisDataSetDefaultVaule(INT16U uwDataNum)
{
INT16U i;
    for(i=0;i<uwDataNum;i++)
    {
        wEepromHisData[i] = wEepromDefaultHisDataArray[i];
    }
}
/*********************************************************************
Function name:  void sReadEepromImptdata(void)
Parameter:    no parameter
Description:  read model's datas.This model store very important data.
             now it has eeprom adress map,Sn,Law(standard).
       Every data has 5 copies,choise the equal data while equal number is max to use
Calls: INT16U sfI2cRx(INT16U uwAdress,INT16S *Data)
Called By:  void sEepromReadOnPower(void)
Parameters:  void
Return:
 *********************************************************************/
INT8U sfReadEepromImptdata(void)   //=0 读模块一数据不成功，=1 成功
{
INT16U ufI2CRxStatus = 0,i = 0,j = 0,m = 0;
INT8U ubRdErrArray[cImptDataBackupNum],fRangerErr = 0; //use to indicate which data is bad
INT16S wTmpData[cImptDataBackupNum][sizeof(wEepromImptData)];
INT8U  ubEqualCnt[cImptDataBackupNum];    //store equal data number
INT8U ubRdEpStatus = 0,ubEqualMaxSerialNum = 0;

    if((uwI2CBlockState.bit.fEpImptErr == 0) && (uwI2CBlockState.bit.fNoStandardStatus == 0))
    {
    INT8U fNoEqual = 0;
    // loop to read the data,if data is ok store in wTmpData，else set uRdErrArray[i][j]error
        for(i = 0; i < cImptDataBackupNum; i++)           //uMde1BackupNum num of backup
        {
            ubRdErrArray[i] = 0;  //初始化ubRdErrArray
            ubEqualCnt[i] = 0;   //初始化ubEqualCnt

            for(j = 0; j < sizeof(wEepromImptData); j++)             //ubEpMde1StoreDtaNum length of data
            {
                wTmpData[i][j] = 0;                  //初始化wTmpData

                sI2CSwRst();
                do
                {
                INT16U uwImptBackupAdrStart;
                    uwImptBackupAdrStart = (INT16U)cImptDataStaAdr +  (INT16U)i*(sizeof(wEepromImptData))*2;
                    ufI2CRxStatus = sfI2cRx(uwImptBackupAdrStart + j*2,&wI2cRxData);
                }while(ufI2CRxStatus == 0);

                if(ufI2CRxStatus == 1)
                {
                    wTmpData[i][j] = wI2cRxData;
                }
                else
                {
                    ubRdErrArray[i] = 1;
                }
            }
        }

        //求出备份的5组数据中 相同数最多的
        for(m = 0; m < cImptDataBackupNum; m++)
        {
            if(ubRdErrArray[m] == 0)  //该组数据是Eeprom读正确的，需要与其它组数据进行比较
            {
                for(i = 0; i < cImptDataBackupNum; i++)
                {
                    if(ubRdErrArray[i] == 0)
                    {
                        for(j = 0;j < sizeof(wEepromImptData);j++)
                        {
                            if(wTmpData[m][j] != wTmpData[i][j]) fNoEqual = 1;
                        }
                    }
                    else
                    {
                        fNoEqual = 1;
                    }
                    if(fNoEqual == 0) ubEqualCnt[m]++;
                    fNoEqual = 0;
                }
            }
            else
            {
                ubEqualCnt[m] = 0;
            }
        }

        for(m = 0; m < cImptDataBackupNum; m++)
        {
            if(ubEqualCnt[m] > ubEqualCnt[ubEqualMaxSerialNum])
            {
                ubEqualMaxSerialNum = m;
            }
        }

        for(j = 0,fRangerErr = 0; j < sizeof(wEepromImptData); j++)
        {
            if((wTmpData[ubEqualMaxSerialNum][j] > wEepromDefaultImptMaxArray[j]) ||
               (wTmpData[ubEqualMaxSerialNum][j] < wEepromDefaultImptMinArray[j]))
            {
                fRangerErr = 1;
            }
        }

        if((ubEqualCnt[ubEqualMaxSerialNum] >= 2) && (fRangerErr == 0) &&
           ((wTmpData[ubEqualMaxSerialNum][0]&0xf) != ((wTmpData[ubEqualMaxSerialNum][0]&0xf0)>>4))) //?
        {
            for(j = 0; j < sizeof(wEepromImptData); j++)
            {
                wEepromImptData[j] = wTmpData[ubEqualMaxSerialNum][j];
            }

            if(wEepromStandardValue == cNoStandard)
            {
                uwI2CBlockState.bit.fNoStandardStatus = 1;
                ubRdEpStatus = 0;
            }
            else
            {
              uwI2CBlockState.bit.fNoStandardStatus = 0;
              ubRdEpStatus = 1;
            }
            
            fMachineCoeff.all = wEepromMachineConfig;
			fRatedCoeff.all = wEepromRatedConfig;

        }
        else
        {
            uwI2CBlockState.bit.fEpImptErr = 1;
            ubRdEpStatus = 0;
        }
    }
    else
    {
        for(j = 0,fRangerErr = 0; j < sizeof(wEepromImptData); j++)
        {
            if((wLCDImptData[j] > wEepromDefaultImptMaxArray[j]) ||
               (wLCDImptData[j] < wEepromDefaultImptMinArray[j]))
            {
                fRangerErr = 1;
            }
        }

        for(i=0,j=1;i<sizeof(wEepromImptData);i++) j = j*2;

        if((fRangerErr == 0) && ((uwLCDRxImptDataFlag.flag == (j - 1)) ||  fSciStandChanged == 1) &&      //(uwLCDRxImptDataFlag.flag == (j - 1)条件，需要LCD对所有wLCDImptData[i]进行写操作后才能满足
            ((wLCDImptData[0]&0xf) != ((wLCDImptData[0]&0xf0)>>4)))                  //说明LCD通讯已成功，获取了相关数据
        {
        	fSciStandChanged = 0;
            if(uwI2CBlockState.bit.fEpImptErr == 0)  //即uwI2CBlockState.bit.fNoStandardStatus=1情况
            {
                wEepromImptData[6] = wLCDImptData[6];
            }
            else
            {
                for(j = 0; j < sizeof(wEepromImptData); j++)
                {
                    wEepromImptData[j] = wLCDImptData[j];
                }
            }
            uwLCDRxImptDataFlag.flag = 0;
            
            fMachineCoeff.all = wEepromMachineConfig;
	    	fRatedCoeff.all = wEepromRatedConfig;
        
            if(wEepromStandardValue == cNoStandard)
            {
                uwI2CBlockState.bit.fNoStandardStatus = 1;
                ubRdEpStatus = 0;
            }
            else
            {
                //uwI2CBlockState.bit.fNoStandardStatus不能清零，此时需要恢复出厂值
                ubRdEpStatus = 1;
            }
        }
        else
        {
            ubRdEpStatus = 0;        
        }
    }
    return(ubRdEpStatus);
}
/*********************************************************************
Function name:  void sReadEepromProtectData(void)
Description: read Eeprom,if correct,store at wEepromHisData[],else default value store at wEepromHisData[]
Calls:
Called By:
Parameters:  void
Return:  void
 *********************************************************************/
void sReadEepromProtectData(void)
{
INT16U i = 0,j = 0;
INT16S wTmpData[sizeof(wEepromProtectData)];
static INT16U uwEepromSum=0;
INT16U uwProtAndAdcShareDataStaAdr,uwProtMap,uwProtAndAdcMaxSize;
    uwProtMap = wEepromBlockMap & 0xf;
    
    uwProtAndAdcMaxSize = sizeof(wEepromProtectData);
    
    if(sizeof(wEepromAdcAdjustData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromAdcAdjustData);
    }
    
    if(sizeof(wEepromProtectSupplData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromProtectSupplData);
    }
    
    uwProtAndAdcMaxSize = uwProtAndAdcMaxSize<<1;

    if(uwProtMap > 2) //默认位置，Prot:0,Adc:1,ProtSuppl:2,
    {
        uwProtAndAdcShareDataStaAdr = (INT16U)cImptDataStaAdr +
                                      ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData)+sizeof(wEepromProtectSupplData))*2;

        uwProtAdrStart = uwProtAndAdcShareDataStaAdr + uwProtAndAdcMaxSize * (uwProtMap - 3);
    }
    else
    {
        uwProtAdrStart = (INT16U)cImptDataStaAdr + (INT16U)cImptDataBackupNum*(sizeof(wEepromImptData))*2;
    }

    for(j = 0; j < sizeof(wEepromProtectData); j++)
    {
        sI2CSwRst();
        do
        {
            i = sfI2cRx(uwProtAdrStart + j * 2,&wI2cRxData);
        }while(i == 0);

        if(i == 1)
        {
            wTmpData[j] = wI2cRxData;           
        }
        else
        {
        	do
            {
                i = sfI2cRx(uwProtAdrStart + j * 2,&wI2cRxData);
            }while(i == 0);
            
            if(i == 1)
            {
            	wTmpData[j] = wI2cRxData;
            }
            else
            {
            	uwI2CBlockState.bit.fProtSumErr = 1;
                break;
            }            
        }
    }

    if(uwI2CBlockState.bit.fProtSumErr == 0)   //判断数据是否超限
    {
        for(i = 0; i < sizeof(wEepromProtectData); i++)
        {
            if(((i+1) % 7) != 0)   //校验和不判断
            {
                if(wEepromStandardValue == cIEEE1547Standard)
                {
                    if((wTmpData[i] > wEepromDefaultProtMaxArrayIEEE1547[i]) || (wTmpData[i] < wEepromDefaultProtMinArrayIEEE1547[i]))
                    {
                        uwI2CBlockState.bit.fProtRangeErr = 1;
                        break;
                    }
                }
                else if(wEepromStandardValue == cRule21Standard)
                {
                    if((wTmpData[i] > wEepromDefaultProtMaxArrayRule21[i]) || (wTmpData[i] < wEepromDefaultProtMinArrayRule21[i]))
                    {
                        uwI2CBlockState.bit.fProtRangeErr = 1;
                        break;
                    }
                }
            }
        }
    }

    if((uwI2CBlockState.bit.fProtSumErr != 0) || (uwI2CBlockState.bit.fProtRangeErr != 0) || uwI2CBlockState.bit.fEpImptErr != 0 || uwI2CBlockState.bit.fNoStandardStatus == 1) //有错误,Imtdata数据有错必须恢复出厂值
    {
        uwI2CBlockState.bit.fNoStandardStatus = 0;
        sEepromProtectDataSetDefaultVaule(sizeof(wEepromProtectData));
        fEepromOperateRequire = 1;
    }
    else
    {
        for(i = 0; i < sizeof(wEepromProtectData); i++)
        {
        	if(((i+1)%7) != 0)
            {
            	uwEepromSum += wTmpData[i];
            }
            else
            {
            	if(uwEepromSum != wTmpData[i])
            	{
            		wTmpData[i] = uwEepromSum;
            		fEepromProtSumErrNum++;
            		fEepromOperateRequire = 1;
            	}
            	uwEepromSum = 0;
            }
            wEepromProtectData[i] = wTmpData[i];
        }
    }

}
/*********************************************************************
Function name:  void sReadEepromProtectDataSuppl(void)
Description: read Eeprom,
Calls:        	 
Called By:  
Parameters:  void
Return:  void      
 *********************************************************************/
void sReadEepromProtectDataSuppl(void)
{
INT16U i = 0,j = 0;
INT16S wTmpData[sizeof(wEepromProtectSupplData)];
static INT16U uwEepromSum = 0;
INT16U uwProtAndAdcShareDataStaAdr,uwProtSupplMap,uwProtAndAdcMaxSize; 
    
    uwProtSupplMap = (wEepromBlockMap & 0xf000) >> 12; 
    
    uwProtAndAdcMaxSize = sizeof(wEepromProtectData);
    
    if(sizeof(wEepromAdcAdjustData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromAdcAdjustData);
    }
    
    if(sizeof(wEepromProtectSupplData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromProtectSupplData);
    }
    
    uwProtAndAdcMaxSize = uwProtAndAdcMaxSize<<1;
    
    if(uwProtSupplMap > 2) //不可能等于0和1,0和1的位置被Prot和Adc占据
    {	   
        uwProtAndAdcShareDataStaAdr = (INT16U)cImptDataStaAdr + 
                                      ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData)+sizeof(wEepromProtectSupplData))*2;
        uwProtSupplAdrStart = uwProtAndAdcShareDataStaAdr + uwProtAndAdcMaxSize * (uwProtSupplMap - 3);
    }
    else
    {
        uwProtSupplAdrStart = (INT16U)cImptDataStaAdr + 
        					  ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData))*2;
    }
       
    for(j = 0; j < sizeof(wEepromProtectSupplData); j++)
    {
        sI2CSwRst();
        do
        {
            i = sfI2cRx(uwProtSupplAdrStart + j * 2,&wI2cRxData);
        }while(i == 0);

        if(i == 1)
        {
            wTmpData[j] = wI2cRxData;
        }
        else
        {
        	do
            {
                i = sfI2cRx(uwProtSupplAdrStart + j * 2,&wI2cRxData);
            }while(i == 0);
            
            if(i == 1)
            {
            	wTmpData[j] = wI2cRxData;
            }
            else
            {
            	uwI2CBlockState.bit.fProtSupplSumErr = 1;
                break;
            }            
        }
    }

    if(uwI2CBlockState.bit.fProtSupplSumErr == 0)   //判断数据是否超限
    {
        for(i = 0; i < sizeof(wEepromProtectSupplData); i++)
        {
            if(((i+1) % 7) != 0)   //校验和不判断
            {
                if((wTmpData[i] > wEepromDefaultProtMaxSupplArray[i]) || (wTmpData[i] < wEepromDefaultProtMinSupplArray[i]))
                {
                    uwI2CBlockState.bit.fProtSupplRangeErr = 1;
                    break;
                }            
            }
        }
    }

    if((uwI2CBlockState.bit.fProtSupplSumErr != 0) || (uwI2CBlockState.bit.fProtSupplRangeErr != 0) || uwI2CBlockState.bit.fEpImptErr != 0 || uwI2CBlockState.bit.fNoStandardStatus == 1) //有错误,Imtdata数据有错必须恢复出厂值
    {
        uwI2CBlockState.bit.fNoStandardStatus = 0;
        sEepromProtectSupplDataSetDefaultVaule(sizeof(wEepromProtectSupplData));
        fEepromOperateRequire = 1;
    }
    else
    {
        for(i = 0; i < sizeof(wEepromProtectSupplData); i++)
        {
        	if(((i+1)%7) != 0)
            {
            	uwEepromSum += wTmpData[i];
            }
            else
            {
            	if(uwEepromSum != wTmpData[i])
            	{
            		wTmpData[i] = uwEepromSum;
            		fEepromProtSuplSumErrNum++;
            		fEepromOperateRequire = 1;
            	}
            	uwEepromSum = 0;
            }
            wEepromProtectSupplData[i] = wTmpData[i];
        }
    }
}     
/*********************************************************************
Function name:  void sReadEepromAdcAdjustData(void)
Description: read Eeprom,if correct,store at wEepromHisData[],else default value store at wEepromHisData[]
Calls:
Called By:
Parameters:  void
Return:  void
 *********************************************************************/
void sReadEepromAdcAdjustData(void)
{
INT16U i=0,j=0;
INT16S wTmpData[sizeof(wEepromAdcAdjustData)];
static INT16U uwEepromSum=0;
INT16U uwProtAndAdcShareDataStaAdr,uwAdcMap,uwProtAndAdcMaxSize;
    uwAdcMap = (wEepromBlockMap & 0xf0) >> 4;
    
    uwProtAndAdcMaxSize = sizeof(wEepromProtectData);
    
    if(sizeof(wEepromAdcAdjustData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromAdcAdjustData);
    }
    
    if(sizeof(wEepromProtectSupplData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromProtectSupplData);
    }
    
    uwProtAndAdcMaxSize = uwProtAndAdcMaxSize<<1;

    if(uwAdcMap > 2) 
    {	   
        uwProtAndAdcShareDataStaAdr = (INT16U)cImptDataStaAdr + 
                                      ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData)+sizeof(wEepromProtectSupplData))*2;
        uwAdcAdrStart = uwProtAndAdcShareDataStaAdr + uwProtAndAdcMaxSize * (uwAdcMap - 3);
    }
    else
    {
        uwAdcAdrStart = (INT16U)cImptDataStaAdr + ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData))*2;
    }

    for(j = 0;j < sizeof(wEepromAdcAdjustData); j++)
    {
        sI2CSwRst();
        do
        {
            i = sfI2cRx((uwAdcAdrStart + j * 2),&wI2cRxData);//=0读未完成，=1读成功，=2读超时错误
        }while(i == 0);

        if(i == 1)
        {
            wTmpData[j] = wI2cRxData;

            if((j+1) % 7 != 0)
            {
                uwEepromSum += (INT16U)wI2cRxData;
            }
            else
            {
                if(uwEepromSum != (INT16U)wI2cRxData)
                {
                    uwI2CBlockState.bit.fAdcSumErr = 1;
                    break;
                }
                uwEepromSum = 0;
            }
        }
        else
        {
            uwEepromSum = 0;
            uwI2CBlockState.bit.fAdcSumErr = 1;
            break;
        }
    }

    if(uwI2CBlockState.bit.fAdcSumErr == 0)   //判断数据是否超限
    {
        for(i = 0; i < sizeof(wEepromAdcAdjustData); i++)
        {
            if(((i+1) % 7) != 0)   //校验和不判断
            {
                if((wTmpData[i] > wEepromDefaultAdcAdjMaxArray[i]) || (wTmpData[i] < wEepromDefaultAdcAdjMinArray[i]))
                {
                    uwI2CBlockState.bit.fAdcRangeErr = 1;
                    break;
                }
            }
        }
    }

    if((uwI2CBlockState.bit.fAdcSumErr != 0) || (uwI2CBlockState.bit.fAdcRangeErr != 0)) //有错误
    {
        sEepromwAdcAdjustDataSetDefaultVaule(sizeof(wEepromAdcAdjustData));
        fEepromOperateRequire = 1;
    }
    else
    {
        for(i = 0; i < sizeof(wEepromAdcAdjustData); i++)
        {
            wEepromAdcAdjustData[i] = wTmpData[i];
        }
    }

}
/*********************************************************************
Function name:  void sReadEepromHisData(void)
Description: read Eeprom,if correct,store at wEepromHisData[],else default value store at wEepromHisData[]
Calls:
Called By:
Parameters:  void
Return:  void
 *********************************************************************/
void sReadEepromHisData(void)
{
INT16U i = 0,j = 0;
INT16S wTmpData[sizeof(wEepromHisData)];
static INT16U uwEepromSum=0;

    uwHisAdrStart = cHisDataStaAdr + (INT16U)(sizeof(wEepromHisData)) * ((wEepromBlockMap & 0xf00)>>8)*2;

    for(j = 0; j< sizeof(wEepromHisData); j++)
    {
        sI2CSwRst();
        do
        {
            i = sfI2cRx((uwHisAdrStart + j * 2),&wI2cRxData);
        }while(i == 0);

        if(i == 1)
        {
            wTmpData[j] = wI2cRxData;

            if((j+1) % 7 != 0)
            {
                uwEepromSum += (INT16U)wI2cRxData;
            }
            else
            {
                if(uwEepromSum != (INT16U)wI2cRxData)
                {
                    uwI2CBlockState.bit.fHisSumErr = 1;
                    //break; 只置故障标志，但数据继续读下去
                }
                uwEepromSum = 0;
            }
        }
        else
        {
            uwEepromSum = 0;
            uwI2CBlockState.bit.fHisSumErr = 1;
            //break; 只置故障标志，但数据继续读下去
        }
    }

    if(uwI2CBlockState.bit.fHisSumErr != 0)  //有错误
    {
        uwI2CBlockState.bit.fEpHisErr = 1;

        for(i = 0; i < sizeof(wEepromHisData); i++)
        {
            wEepromHisData[i] = wTmpData[i];  //即使数据未完全存储，或校验和不对，也继续保留数据
        }

    }
    else
    {
        for(i = 0; i < sizeof(wEepromHisData); i++)
        {
            wEepromHisData[i]  = wTmpData[i];
        }
    }

}
/*********************************************************************
Function name:  void sEepromReadOnPower(void)
Parameter:    no parameter
Description:
Calls:  sReadEepromImptdata(),sReadEepromProtectData(),
        ReadEepromAdcAdjustData(),sReadEepromProtectDataSuppl(),sReadEepromHisData()  	 
Called By:   main()
Parameters:  void
Return:  INT8U, =1 eeprom error,=0 eeprom no error
***************************************************/

INT8U sfEepromReadOnPower(void)
{
static INT8U fImptdataRdStatus = 0;
    //=0，Eeprom数据未读取过；=1，Eeprom的Impt数据已尝试读取过，可以进行LCD通讯；=2，Eeprom各块数据已完整，可以进行LCD通讯和故障判断
    if(uwI2CBlockState.bit.fEpRdOnPwStep != 2)
    {
        if(fImptdataRdStatus == 0)
        {
            fImptdataRdStatus = sfReadEepromImptdata();
            uwI2CBlockState.bit.fEpRdOnPwStep = 1;
        }
        else
        {
            sReadEepromProtectData();
            sReadEepromAdcAdjustData();
            sReadEepromProtectDataSuppl();
            sReadEepromHisData();

            if((wEepromStandardValue == cIEEE1547Standard)||(wEepromStandardValue == cRule21Standard))
            {
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

            uwI2CBlockState.bit.fEpRdOnPwStep = 2;

            fHoldRegisterStatus = 2;
            //if(fEepromErr == 0)
            // {
            //    InitRS485Data();
            // }
        }
    }

    return(uwI2CBlockState.bit.fEpImptErr | uwI2CBlockState.bit.fEpProtErr |
           uwI2CBlockState.bit.fEpAdcErr);
}
/*********************************************
Function name: void sEepromProshtift(void)
Parameter:    no parameter
Description:  if Protect block bad,use this function to shift to a new block.
Calls:
Called By:  INT8U sfEepromRunDataChange(void)
Parameters:  void
*********************************************/
void sEepromProtshift(INT16S *pBlockMap)
{
T_I2CBlockMap  wPresentBlockAdrMap;
INT16U uwProtAndAdcMaxSize,uwProtAndAdcMapMax,uwProtAndAdcShareDataStaAdr;
    wPresentBlockAdrMap.word = *pBlockMap;

    wPresentBlockAdrMap.bits.fProt++;
    if(wPresentBlockAdrMap.bits.fProt < 3) wPresentBlockAdrMap.bits.fProt = 3;
    if(wPresentBlockAdrMap.bits.fProt == wPresentBlockAdrMap.bits.fAdc)  //this block has been used by the adc adjust datas
    {
        wPresentBlockAdrMap.bits.fProt++;
    }
    if(wPresentBlockAdrMap.bits.fProt == wPresentBlockAdrMap.bits.fProtSuppl)  //this block has been used by the ProtSuppl
    {
	    wPresentBlockAdrMap.bits.fProt++;
    }
    
    uwProtAndAdcMaxSize = sizeof(wEepromProtectData);
    
    if(sizeof(wEepromAdcAdjustData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromAdcAdjustData);
    }
    
    if(sizeof(wEepromProtectSupplData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromProtectSupplData);
    }
    
    uwProtAndAdcMaxSize = uwProtAndAdcMaxSize<<1;

    //计算ProtData和AdcData数据允许的地址信息最大值，同时该值受限于wEepromBlockMap位定义必须小于15
    uwProtAndAdcShareDataStaAdr = (INT16U)cImptDataStaAdr +
                                  ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData)+sizeof(wEepromProtectSupplData))*2;
    if(cHisDataStaAdr > uwProtAndAdcShareDataStaAdr) uwProtAndAdcMapMax = (((INT16U)cHisDataStaAdr -uwProtAndAdcShareDataStaAdr) / uwProtAndAdcMaxSize) + 2;
    else uwProtAndAdcMapMax = 0;
    if(uwProtAndAdcMapMax > 15) uwProtAndAdcMapMax = 15;

    if(wPresentBlockAdrMap.bits.fProt <= uwProtAndAdcMapMax)
    {
        uwProtAdrStart = uwProtAndAdcShareDataStaAdr + uwProtAndAdcMaxSize * (wPresentBlockAdrMap.bits.fProt - 3);
    }
    else
    {
        wPresentBlockAdrMap.bits.fProt  = 0;
        uwProtAdrStart  =  (INT16U)cImptDataStaAdr + (INT16U)cImptDataBackupNum*sizeof(wEepromImptData)*2;
    }
    *pBlockMap = wPresentBlockAdrMap.word;
}

/*********************************************
Function name: void sEepromProtSupplshift(void)
Parameter:    no parameter
Description:  if Protect block bad,use this function to shift to a new block.
Calls:        	 
Called By:  INT8U sfEepromRunDataChange(void)
Parameters:  void   
*********************************************/
void sEepromProtSupplshift(INT16S *pBlockMap)
{   		
T_I2CBlockMap  wPresentBlockAdrMap;
INT16U uwProtAndAdcMaxSize,uwProtAndAdcMapMax,uwProtAndAdcShareDataStaAdr; 
    wPresentBlockAdrMap.word = *pBlockMap;
    wPresentBlockAdrMap.bits.fProtSuppl++;
    
    if(wPresentBlockAdrMap.bits.fProtSuppl < 3) wPresentBlockAdrMap.bits.fProtSuppl = 3;
    
    if(wPresentBlockAdrMap.bits.fProtSuppl == wPresentBlockAdrMap.bits.fProt)  //this block has been used by the adc adjust datas
    {
	    wPresentBlockAdrMap.bits.fProtSuppl++;
    }
    if(wPresentBlockAdrMap.bits.fProtSuppl == wPresentBlockAdrMap.bits.fAdc)  //this block has been used by the adc adjust datas
    {
	    wPresentBlockAdrMap.bits.fProtSuppl++;
    }
    
    uwProtAndAdcMaxSize = sizeof(wEepromProtectData);
    
    if(sizeof(wEepromAdcAdjustData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromAdcAdjustData);
    }
    
    if(sizeof(wEepromProtectSupplData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromProtectSupplData);
    }
    
    uwProtAndAdcMaxSize = uwProtAndAdcMaxSize<<1;
    
    //计算ProtData和AdcData数据允许的地址信息最大值，同时该值受限于wEepromBlockMap位定义必须小于15	
    uwProtAndAdcShareDataStaAdr = (INT16U)cImptDataStaAdr + 
                                  ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData)+sizeof(wEepromProtectSupplData))*2;
    if(cHisDataStaAdr > uwProtAndAdcShareDataStaAdr) uwProtAndAdcMapMax = (((INT16U)cHisDataStaAdr -uwProtAndAdcShareDataStaAdr) / uwProtAndAdcMaxSize) + 2; //Prot:0,Adc:1,ProtSuppl:2,shift的地址从3开始
    else uwProtAndAdcMapMax = 0;
    if(uwProtAndAdcMapMax > 15) uwProtAndAdcMapMax = 15;
    if(wPresentBlockAdrMap.bits.fProtSuppl <= uwProtAndAdcMapMax)
    {        
        uwProtSupplAdrStart = uwProtAndAdcShareDataStaAdr + uwProtAndAdcMaxSize * (wPresentBlockAdrMap.bits.fProtSuppl - 3);
    }
	else
	{
		wPresentBlockAdrMap.bits.fProtSuppl  = 2;
		uwProtSupplAdrStart  =  (INT16U)cImptDataStaAdr + ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData) + sizeof(wEepromProtectData)+ sizeof(wEepromAdcAdjustData))*2;  	
	}
    *pBlockMap = wPresentBlockAdrMap.word;
}
/*********************************************
Function name: void sEepromAdcshift(void)
Parameter:    no parameter
Description:  if Protect block bad,use this function to shift to a new block.
Calls:
Called By:  INT8U sfEepromRunDataChange(void)
Parameters:  void
*********************************************/
void sEepromAdcshift(INT16S *pBlockMap)
{
T_I2CBlockMap  wPresentBlockAdrMap;
INT16U uwProtAndAdcMaxSize,uwProtAndAdcMapMax,uwProtAndAdcShareDataStaAdr;
    wPresentBlockAdrMap.word = *pBlockMap;

    wPresentBlockAdrMap.bits.fAdc++;
    if(wPresentBlockAdrMap.bits.fAdc < 3) wPresentBlockAdrMap.bits.fAdc = 3;
    if(wPresentBlockAdrMap.bits.fAdc == wPresentBlockAdrMap.bits.fProt)  //this block has been used by the adc adjust datas
    {
        wPresentBlockAdrMap.bits.fAdc++;
    }
    if(wPresentBlockAdrMap.bits.fAdc == wPresentBlockAdrMap.bits.fProtSuppl)  
    {
	    wPresentBlockAdrMap.bits.fAdc++;
    }
    
    uwProtAndAdcMaxSize = sizeof(wEepromProtectData);
    
    if(sizeof(wEepromAdcAdjustData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromAdcAdjustData);
    }
    
    if(sizeof(wEepromProtectSupplData) > uwProtAndAdcMaxSize)
    {
    	uwProtAndAdcMaxSize = sizeof(wEepromProtectSupplData);
    }
    
    uwProtAndAdcMaxSize = uwProtAndAdcMaxSize<<1;

    //计算ProtData和AdcData数据允许的地址信息最大值，时该值受限于wEepromBlockMap位定义必须小于15
    uwProtAndAdcShareDataStaAdr = (INT16U)cImptDataStaAdr +
                                  ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData)+sizeof(wEepromProtectData)+sizeof(wEepromAdcAdjustData)+sizeof(wEepromProtectSupplData))*2;
    if(cHisDataStaAdr > uwProtAndAdcShareDataStaAdr) uwProtAndAdcMapMax = (((INT16U)cHisDataStaAdr - uwProtAndAdcShareDataStaAdr) / uwProtAndAdcMaxSize) + 2;
    else uwProtAndAdcMapMax = 0;
    if(uwProtAndAdcMapMax > 15) uwProtAndAdcMapMax = 15;

    if(wPresentBlockAdrMap.bits.fAdc <= uwProtAndAdcMapMax)
    {
        uwAdcAdrStart = uwProtAndAdcShareDataStaAdr + uwProtAndAdcMaxSize * (wPresentBlockAdrMap.bits.fAdc - 3);
    }
    else
    {
        wPresentBlockAdrMap.bits.fAdc  = 1;
        uwAdcAdrStart  =  (INT16U)cImptDataStaAdr + ((INT16U)cImptDataBackupNum*sizeof(wEepromImptData) + sizeof(wEepromProtectData))*2;
    }
    *pBlockMap = wPresentBlockAdrMap.word;
}
/*********************************************
Function name: void sEepromHisshift(void)
Parameter:    no parameter
Description:  if Protect block bad,use this function to shift to a new block.
Calls:
Called By:  INT8U sfEepromRunDataChange(void)
Parameters:  void
*********************************************/
void sEepromHisshift(INT16S *pBlockMap)
{
T_I2CBlockMap  wPresentBlockAdrMap;
INT16U uwHisMapMax;
    wPresentBlockAdrMap.word = *pBlockMap;

    wPresentBlockAdrMap.bits.fHis++;

    //计算HisData数据允许的地址信息最大值，同时该值受限于wEepromBlockMap位定义必须小于15
    uwHisMapMax = (cEepromByteSize - cHisDataStaAdr) / (sizeof(wEepromHisData)) / 2;
    if(uwHisMapMax > 15) uwHisMapMax = 15;

    if(wPresentBlockAdrMap.bits.fHis <= uwHisMapMax)
    {
        uwHisAdrStart = cHisDataStaAdr + (INT16U)(sizeof(wEepromHisData)) * wPresentBlockAdrMap.bits.fHis*2;
    }
    else
    {
        wPresentBlockAdrMap.bits.fHis  = 0;
        uwProtAdrStart  =  cHisDataStaAdr;
    }
    *pBlockMap = wPresentBlockAdrMap.word;
}
/*********************************************************************
Function name: INT8U suEepromCmparWr(INT16U uwStaAdr,INT16S *pwEepromData,INT8U uCntNumMax)
Description: read Eeprom and compare with the buffer Array,if equal no action,esle write buffer
            value to Eeprom and read back to ensure write correct.
Calls:
Called By:
Parameters:  void
Return:  INT8U,=1 succes，=0 unfinished  ，=2 read or write wrong
 *********************************************************************/
INT8U suEepromCmparWr(INT16U uwStaAdr,INT16S *pwEepromData,INT16U uwFrameLen,INT16U uwArrayLen)
{
INT8U   j = 0;
INT16U  uwEepromAdr = 0;
static  INT8U  i = 0;
static  INT16U  uwCurrntAdrNum = 0;
    uwEepromAdr = uwStaAdr + uwCurrntAdrNum * 2;

    if(i == 0)//can't read when write isn't success
    {
        sI2CSwRst();
        do
        {
            i = sfI2cRx(uwEepromAdr,&wI2cRxData);
        }while(i == 0);
    }

    if((i == 1) && (wI2cRxData != *(pwEepromData + uwCurrntAdrNum)))  //写EEPROM
    {
        uwI2CBlockState.bit.fFrameFinished = 1;  //=0，一帧数据写完成；=1，一帧数据写未完成

        j = sfI2cTx(uwEepromAdr,(pwEepromData + uwCurrntAdrNum));

        if(j == 0)
        {
            return 0;
        }
        else if(j == 1)
        {
            i = 0;
            return 0;//地址没有加  下次读还是读此地址数 用于判断写的可对
        }
        else
        {
            i = 0;
            uwCurrntAdrNum = 0;
            return 2;
        }
    }
    else
    {
        if(i == 2)
        {
            i = 0;
            uwCurrntAdrNum = 0;
            return 2;
        }
        else  //i==1
        {
            i = 0;
            uwCurrntAdrNum++;
            if((uwCurrntAdrNum%uwFrameLen) == 0)//make sure a fream has written,avoid sum error
            {
                uwI2CBlockState.bit.fFrameFinished = 0;
            }
        }
    }

    if(uwCurrntAdrNum >= uwArrayLen)//all have done right
    {
        uwCurrntAdrNum = 0;
        uwI2CBlockState.bit.fFrameFinished = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*********************************************************************
Function name: INT8U sfEepromRunDataChange(void)
Description:call suEepromCmparWr().action depend on the Return value.
Calls: suEepromCmparWr().
Called By:  main.c
Parameters:  void
Return:  INT8U, =1 eeprom error,=0 eeprom no error
 *********************************************************************/
INT8U sfEepromRunDataChange(void)
{
static INT8U ubEepromblock0Err = 0;
static INT8U ubEepromblock1Err = 0;
static INT8U ubEepromblock2Err = 0;
static INT8U ubEepromblock3Err = 0;
static INT8U ubEepromblock4Err = 0; 
static INT8U ubEepromWrSelNum = 0;
static INT16U uwWriteEepromCnt = 0;
    if(fEepromRdWrHalt == 0)
    {
        if(ubEepromWrSelNum == 0) //select module1 to compare
        {
        INT16U uwImptBackupAdrStart;//module1 backup should loop 5 times
        static INT16U uwImptDataBackupCnt  = 0;
        static INT8U uwImptDataCmparErrCnt = 0;

            if((wEepromStandardValue != wLCDImptData[6]) && (uwLCDRxImptDataFlag.bits.bit6 == 1) &&
               (uwSysMode != InvRunMode) && (uwSysMode != PreCheckMode))  //刷新法规值，同时恢复出厂值
            {
                wEepromStandardValue = wLCDImptData[6];
                uwLCDRxImptDataFlag.flag = 0;
            
                if((wEepromStandardValue == cIEEE1547Standard) || (wEepromStandardValue == cRule21Standard))
                {
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
            
                sEepromProtectDataSetDefaultVaule(sizeof(wEepromProtectData));
                //sEepromProtectSupplDataSetDefaultVaule(sizeof(wEepromProtectSupplData));
				fEepromOperateRequire = 1;
                fStandardChgStopRequire = 0;
            }
            else
            {
                if(fHoldRegisterStatus == 3) fHoldRegisterStatus = 0;
            }

            uwImptBackupAdrStart = (INT16U)cImptDataStaAdr +  uwImptDataBackupCnt*(sizeof(wEepromImptData))*2;
            ubEepromblock0Err = suEepromCmparWr(uwImptBackupAdrStart,&wEepromImptData[0],sizeof(wEepromImptData),sizeof(wEepromImptData));

            if(ubEepromblock0Err != 0)  //complete comparing
            {
                uwImptDataBackupCnt++;    //loop 5(cImptDataBackupNum) times
            
                if(ubEepromblock0Err == 2)
                {
                    uwImptDataCmparErrCnt++;
                }
            
                if(uwImptDataBackupCnt < cImptDataBackupNum)
                {
                    ubEepromblock0Err = 0;
                }
                else
                {
                    if(uwImptDataCmparErrCnt > (cImptDataBackupNum - 2))
                    {
                        ubEepromblock0Err = 2;
                    }
                    else
                    {
                        ubEepromblock0Err = 1;
                    }
                    uwImptDataBackupCnt = 0;
                    ubEepromWrSelNum = 1;
                }
            }
        }
        else if(ubEepromWrSelNum == 1) //select module2 to compare
        {
            ubEepromblock1Err = suEepromCmparWr(uwProtAdrStart,&wEepromProtectData[0],7,sizeof(wEepromProtectData));

            if(ubEepromblock1Err != 0) //complete comparing
            {
                uwWriteEepromCnt++;
                if(uwWriteEepromCnt > 1) //确保有导默认值命令后，写Eeprom程序被执行过一次后再清导默认值命令
                {
                    uwWriteEepromCnt = 0;
                    fCallDefaultValueOrder = 0;
                }
                ubEepromWrSelNum = 2;
            }
        }
        else if(ubEepromWrSelNum == 2) //select module3 to compare
        {
            ubEepromblock2Err = suEepromCmparWr(uwAdcAdrStart,&wEepromAdcAdjustData[0],7,sizeof(wEepromAdcAdjustData));

            if(ubEepromblock2Err != 0) //complete comparing
            {
                ubEepromWrSelNum = 3;
            }
        }
        else if(ubEepromWrSelNum == 3) //select module4 to compare
        {
            ubEepromblock3Err = suEepromCmparWr(uwProtSupplAdrStart,&wEepromProtectSupplData[0],7,sizeof(wEepromProtectSupplData));

            if(ubEepromblock3Err != 0)
            {
                ubEepromWrSelNum = 4;
            }
        }
        else if(ubEepromWrSelNum == 4) //select module4 to compare
        {        
            ubEepromblock4Err = suEepromCmparWr(uwHisAdrStart,&wEepromHisData[0],7,sizeof(wEepromHisData));
	        if(ubEepromblock4Err != 0)
	        {	        
	    	    ubEepromWrSelNum = 5;	    
	        }
        }

        if(ubEepromWrSelNum == 5) //complete the data compare
        {
        static INT8U ubProtshifScanCnt = 0;
        static INT8U ubAdcshifScanCnt = 0;
        static INT8U ubProtSupplshifScanCnt = 0;
        static INT8U ubHisshifScanCnt = 0;
            if((ubEepromblock0Err == 1) && (ubEepromblock1Err == 1)&&(ubEepromblock2Err== 1)&&(ubEepromblock3Err == 1)&&(ubEepromblock4Err == 1))
            {
                uwI2CBlockState.bit.fEpImptErr = 0;
                uwI2CBlockState.bit.fEpProtErr = 0;//ProtSuppl与Prot共用一个错误信号位
                uwI2CBlockState.bit.fEpAdcErr = 0;
                uwI2CBlockState.bit.fEpHisErr = 0;
                
                ubProtshifScanCnt = 0;
                ubAdcshifScanCnt = 0;
                ubProtSupplshifScanCnt = 0;
                ubHisshifScanCnt = 0;
            }
            else  //if some block can't be written,we should shift to another available block
            {
                if(ubEepromblock0Err == 2)
                {
                    uwI2CBlockState.bit.fEpImptErr = 1;  //=0,no error  =1,error
                }

                if(ubEepromblock1Err == 2)
                {
                    sEepromProtshift(&wEepromBlockMap);
                    if((wEepromBlockMap & 0xf) == 0)
                    {
                        ubProtshifScanCnt++;
                        if(ubProtshifScanCnt > 5)
                        {
                            ubProtshifScanCnt = 5;
                            uwI2CBlockState.bit.fEpProtErr = 1;  //=0,no error  =1,error
                        }
                    }
                }
            
                if(ubEepromblock2Err == 2)
                {
                    sEepromAdcshift(&wEepromBlockMap);
                    if((wEepromBlockMap & 0xf0) == 0)
                    {
                        ubAdcshifScanCnt++;
                        if(ubAdcshifScanCnt > 5)
                        {
                            ubAdcshifScanCnt = 5;
                            uwI2CBlockState.bit.fEpAdcErr = 1;  //=0,no error  =1,error
                        }
                    }
                }

                if(ubEepromblock3Err == 2)
	            {
	                sEepromProtSupplshift(&wEepromBlockMap);
	                if((wEepromBlockMap & 0xf000) == 0)
	                {
	                    ubProtSupplshifScanCnt++;
	                    if(ubProtshifScanCnt > 5) 
	                    {
	                        ubProtshifScanCnt = 5;
	                        uwI2CBlockState.bit.fEpProtErr = 1;  //发生ProtSuppl错误时，使用与Prot一样的错误报警位   			
	                    }
	                }            			   
	    	    }		
	            if(ubEepromblock4Err == 2)
	            {
	                sEepromHisshift(&wEepromBlockMap);
	                if((wEepromBlockMap & 0xf00) == 0)
	                {
	                    ubHisshifScanCnt++;
	                    if(ubHisshifScanCnt > 5) 
	                    {
	                        ubHisshifScanCnt = 5;
	                        uwI2CBlockState.bit.fEpHisErr = 1;  //=0,no error  =1,error	
	                        //if(uwI2CBlockState.bit.fEpHisErr!=0) wDebugData2=wDebugData2 | 0x1000;	
	                        //wDebugData3=0xf300;
	                    }
	                }            			   
	    	    }
            }

            ubEepromblock0Err = 0;
            ubEepromblock1Err = 0;
            ubEepromblock2Err = 0;
            ubEepromblock3Err = 0;
            ubEepromblock4Err = 0;
            ubEepromWrSelNum = 0;
            
            if(uwSysMode == BootloaderMode) fEepromRdWrHalt = 1;
            else fEepromRdWrHalt = 0;

            uwEepromWrSelNumFinished = 1;
        }
    }

    return(uwI2CBlockState.bit.fEpImptErr | uwI2CBlockState.bit.fEpProtErr |
           uwI2CBlockState.bit.fEpAdcErr);  //屏蔽fEpHisErr
}
/*********************************************************************
Function name: void sEepromHandle(void)
Description:when the eeprom parameters need to be adjusted,the programmer will do the eeprom writing and reading action.
Calls: sEepromHandle().
Called By:  main.c
Parameters:  void
Return:
 *********************************************************************/
void sEepromHandle(void)
{
static INT16U uwI2CErrCnt = 0;
static INT16U uwRepeatOperateCnt = 0;
    if(fEepromOperateStep == 0)
    {
        if(uwSysMode == BootloaderMode) fEepromRdWrHalt = 1;
        else
        {
            fEepromRdWrHalt = 0;
            if((fEepromOperateRequire == 1) || (uwI2CBlockState.bit.fEpRdOnPwStep != 2))
            {
                fEepromOperateStep = 1;
                fEepromOperateRequire = 0;
            }
        }
    }

    if(fEepromOperateStep == 1)
    {
        if(uwI2CBlockState.bit.fEpRdOnPwStep != 2)
        {
            uwI2CBlockState.bit.fEpErr = sfEepromReadOnPower();
        }
        else
        {
            uwI2CBlockState.bit.fEpErr = sfEepromRunDataChange();
        }

        if(uwEepromWrSelNumFinished == 1 || uwI2CBlockState.bit.fEpRdOnPwStep != 2)
        {
            uwEepromWrSelNumFinished = 0;

            if(uwI2CBlockState.bit.fEpErr != 0)
            {
                uwRepeatOperateCnt = 0;
                uwI2CErrCnt++;

                if(uwI2CErrCnt > 50)
                {
                    uwI2CErrCnt = 0;
                    fEepromChkError = 1;
                }
            }
            else
            {
                uwI2CErrCnt = 0;
                uwRepeatOperateCnt++;

                if(uwI2CBlockState.bit.fEpRdOnPwStep != 2) uwRepeatOperateCnt = 0; //sfEepromReadOnPower()在未完成情况下需要一直执行
                if(uwRepeatOperateCnt > 2)
                {
                    uwRepeatOperateCnt = 0;
                    fEepromOperateStep = 0;
                    fEepromChkError = 0;
                }
            }
        }
    }
}

void sEEPROMProRangeChk(void)
{
    INT16S i;
    INT16S wEepromProDataErrFlag = 0;
    INT16S wEepromSupplProDataErrFlag = 0;

    for(i = 0; i < sizeof(wEepromProtectData); i++)
    {
        if(((i+1) % 7) != 0)   //校验和不判断
        {
            if(wEepromStandardValue == cIEEE1547Standard)
            {
                if((wEepromProtectData[i] > wEepromDefaultProtMaxArrayIEEE1547[i]) || (wEepromProtectData[i] < wEepromDefaultProtMinArrayIEEE1547[i]))
                {
                    wEepromProDataErrFlag = 1;
                    break;
                }
            }
            else if(wEepromStandardValue == cRule21Standard)
            {
                if((wEepromProtectData[i] > wEepromDefaultProtMaxArrayRule21[i]) || (wEepromProtectData[i] < wEepromDefaultProtMinArrayRule21[i]))
                {
                    wEepromProDataErrFlag = 1;
                    break;
                }
            }
        }
    }
    
    for(i = 0; i < sizeof(wEepromProtectSupplData); i++)
    {
        if(((i+1) % 7) != 0)   //校验和不判断
        {           
            if((wEepromProtectSupplData[i] > wEepromDefaultProtMaxSupplArray[i]) || (wEepromProtectSupplData[i] < wEepromDefaultProtMinSupplArray[i]))
            {
                wEepromSupplProDataErrFlag = 1;
                break;
            }           
        }
    }

    if (wEepromProDataErrFlag == 1)
    {
        sEepromProtectDataSetDefaultVaule(sizeof(wEepromProtectData));
        fEepromOperateRequire = 1;
    }
    
    if (wEepromSupplProDataErrFlag == 1)
    {
        sEepromProtectSupplDataSetDefaultVaule(sizeof(wEepromProtectSupplData));
        fEepromOperateRequire = 1;
    }
}
/********end of file*******************/

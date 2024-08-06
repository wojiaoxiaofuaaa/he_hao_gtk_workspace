;//###########################################################################
;////////////////////////////////////////////////////////////////////////////

       .def _CPURegister_check
       .sect "FlashSafety"
       
       .def _RPC_check
       .sect "FlashSafety"
 
       .global _dwregister0
       .global _dwregister1
       .global _dwregister2
       .global _wregister0
       .global _wregister1
       .global _wRegistercheckFlag

       .global _CPURegister_check

        
_CPURegister_check:
	    PUSH    XAR0
	    PUSH    XAR1
	    PUSH    XAR2
	    PUSH    XAR3
	    PUSH    XAR4
	    PUSH    XAR5
	    PUSH    XAR6
	    PUSH    XAR7
	    MOVW DP,#_wregister0
        ;MOVL @_dwregister0,ACC                   ;ACC 
        
        MOV  AH,#0x5555;
        MOV  @_wregister0,AH                        
        CMP  @_wregister0,#0x5555                     
        B   _check_bad ,NEQ  ;  
       
        MOV  AH,#0xAAAA;
        MOV  @_wregister0,AH                          
        CMP  @_wregister0,#0xAAAA                     
        B   _check_bad ,NEQ  ;
                                                                         
        MOV  AL,#0x5555;
        MOV  @_wregister0,AL                          
        CMP  @_wregister0,#0x5555                     
        B   _check_bad ,NEQ  ;
          
        MOV  AL,#0xAAAA;
        MOV  @_wregister0,AL                          
        CMP  @_wregister0,#0xAAAA                     
        B   _check_bad ,NEQ  ;
         
        MOV  AH,#0x5555;                          ;XAR0       
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR0,@_dwregister1
        CMPL ACC,XAR0
        B   _check_bad ,NEQ  ;
                 
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR0,@_dwregister1
        CMPL ACC,XAR0
        B   _check_bad ,NEQ  ;
        
        MOV  AH,#0x5555;                          ;XAR1
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR1,@_dwregister1
        CMPL ACC,XAR1
        B   _check_bad ,NEQ  ;
        
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR1,@_dwregister1
        CMPL ACC,XAR1
        B   _check_bad ,NEQ  ;
                
        MOV  AH,#0x5555;                          ;XAR2
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR2,@_dwregister1
        CMPL ACC,XAR2
        B   _check_bad ,NEQ  ;
        
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR2,@_dwregister1
        CMPL ACC,XAR2
        B   _check_bad ,NEQ  ;
                
        MOV  AH,#0x5555;                          ;XAR3
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR3,@_dwregister1
        CMPL ACC,XAR3
        B   _check_bad ,NEQ  ;
        
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR3,@_dwregister1
        CMPL ACC,XAR3
        B   _check_bad ,NEQ  ;
                 
        MOV  AH,#0x5555;                          ;XAR4
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR4,@_dwregister1
        CMPL ACC,XAR4
        B   _check_bad ,NEQ  ;
                 
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR4,@_dwregister1
        CMPL ACC,XAR4
        B   _check_bad ,NEQ  ;
                 
        MOV  AH,#0x5555;                          ;XAR5
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR5,@_dwregister1
        CMPL ACC,XAR5
        B   _check_bad ,NEQ  ;
                
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR5,@_dwregister1
        CMPL ACC,XAR5
        B   _check_bad ,NEQ  ;
               
        MOV  AH,#0x5555;                          ;XAR6
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR6,@_dwregister1
        CMPL ACC,XAR6
        B   _check_bad ,NEQ  ;
                
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR6,@_dwregister1
        CMPL ACC,XAR6
        B   _check_bad ,NEQ  ;
                
        MOV  AH,#0x5555;                          ;XAR7
        MOV  AL,#0x5555;
        MOVL @_dwregister1,ACC
        MOVL XAR7,@_dwregister1
        CMPL ACC,XAR7
        B   _check_bad ,NEQ  ;
                 
        MOV  AH,#0xAAAA;
        MOV  AL,#0xAAAA;
        MOVL @_dwregister1,ACC
        MOVL XAR7,@_dwregister1
        CMPL ACC,XAR7
        B   _check_bad ,NEQ  ;
        
                
        MOVW DP,#_wregister0                      ;SP
        MOVL XAR0,#1                               
        MOVL XAR1,#2
        MOVL XAR2,#3
        MOVL XAR3,#4
        MOVL XAR4,#5
        MOVL XAR5,#6
        MOVL XAR6,#7
        MOVL XAR7,#8
        PUSH XAR0
        PUSH XAR1
        PUSH XAR2
        PUSH XAR3
        PUSH XAR4
        PUSH XAR5
        PUSH XAR6
        PUSH XAR7
        MOVL ACC,*--SP
        MOVL XAR0,#8
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
         
        MOVL ACC,*--SP
        MOVL XAR0,#7
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
         
        MOVL ACC,*--SP
        MOVL XAR0,#6
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
         
        MOVL ACC,*--SP
        MOVL XAR0,#5
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
         
        MOVL ACC,*--SP
        MOVL XAR0,#4
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
        
        MOVL ACC,*--SP
        MOVL XAR0,#3
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
        
        MOVL ACC,*--SP
        MOVL XAR0,#2
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
        
        MOVL ACC,*--SP
        MOVL XAR0,#1
        CMPL  ACC,XAR0
        B   _check_bad ,NEQ  ;
                                              ;XT,P
        MOV  AL,#0x5555
        MOV  AH,#0x0
        MOVL @_dwregister0,ACC
        MOV  AR7,#0xAAAA
        MOVX  TL,AR7
        MOV  T,#0x0
        IMPYL P,XT,@_dwregister0
        MOV  AH,#0x38E3
        MOV  AL,#0x1C72
        CMPL ACC,P
        B   _check_bad ,NEQ  ;
                                              ;PC 
        MOV  XAR0,PC
        NOP;
        NOP;
        NOP;
        NOP;
        MOV  XAR1,PC
        SUBB XAR1,#5
        MOVL ACC,XAR0        
        CMPL ACC,XAR1
        B   _check_bad ,NEQ  ;
                                              ;RPC              
        LCR  _RPC_check
        MOV  XAR0,PC
        CMPL ACC,XAR0
        B   _check_bad ,NEQ  ;

        
	    ; save ST0,DP,IER,IFR and DBGIER register
	    PUSH     ST0 				
	    POP      @PL				; save ST0 in PL
	    PUSH     DP
	    POP      @PH				; save ST0 in DP
	    MOV      @AR0,IER           ; save IER in AR0
	    PUSH     IFR
	    POP      @TH                ; save IFR in TH
	    PUSH     DBGIER
	    POP      @AR5				; save 
	    
	    ; Test ST0,DP,IER,IFR and DBGIER register
	    MOV      AR6,#1	            ; use AR6 as a loop counter 
	    
_loop_REG_2_test:
	    PUSH     @AL
	    POP      DP 				; write to DP
	    PUSH     DP
	    POP      AR4
	    CMP      AL,@AR4
	    B		 _check_bad,NEQ
	    
	    PUSH     @AL
	    POP      ST0 				; write to ST0
	    PUSH     ST0
        
	    POP      AR4
	    CMP      AL,@AR4
	    B		 _check_bad,NEQ
	    
	    PUSH     @AL
	    POP      IFR 				; write to IFR
	    PUSH     IFR
	    POP      AR4
	    CMP      AL,@AR4
	    B		 _check_bad,NEQ
	    
	    PUSH     @AL
	    POP      DBGIER 			; write to DBGIER
	    PUSH     DBGIER
	    POP      AR4
	    CMP      AL,@AR4
	    B		 _check_bad,NEQ
	    
	    MOV      IER,@AL			; write to IER 
	    MOV      @AR4,IER			
	    CMP      AL,@AR4
	    B		 _check_bad,NEQ
	    
  	    NOT      ACC
	    DEC      AR6
	    B        _loop_REG_2_test,GEQ;
	    
	    PUSH     @PL				
	    POP      ST0			; restore ST0
	    PUSH     @PH				
	    POP      DP				; restore DP	
	    MOV      IER,@AR0       ; restore IER
	    PUSH     @TH
	    POP      IFR			; restore IFR
	    PUSH     @AR5
	    POP      DBGIER         ; restore DBGIER        
        LB        _check_ok

_check_bad
        MOV @_wRegistercheckFlag,#1                                                              
_check_ok
	    POP    XAR7
	    POP    XAR6
	    POP    XAR5
	    POP    XAR4
	    POP    XAR3
	    POP    XAR2
	    POP    XAR1
	    POP    XAR0
        LRETR

;////////////////////////////////////////////////
_RPC_check
        PUSH  RPC
        POP   ACC
        LRETR







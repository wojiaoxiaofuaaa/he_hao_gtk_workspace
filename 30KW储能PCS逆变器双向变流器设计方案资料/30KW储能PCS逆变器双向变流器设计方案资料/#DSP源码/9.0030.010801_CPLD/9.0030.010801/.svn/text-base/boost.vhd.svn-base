LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  boost IS
PORT ( clk,en,ePWMAxIN,ePWMBxIN  :IN  STD_LOGIC;--40KHz D<100%
	   ePWMAxOUT     :OUT STD_LOGIC_vector(1 downto 0);--20KHz D<37.5%  
	   ePWMBOUT1,ePWMBOUT0     :OUT STD_LOGIC    
     );
END;
ARCHITECTURE boost OF boost IS 
--SIGNAL    COUNT : STD_LOGIC_vector(3 downto 0);
TYPE STATES IS (ST1, ST2, ST3, ST4);
SIGNAL STX :STATES;
BEGIN
process(clk)
begin
IF EN = '0'THEN
   ePWMAxOUT <=  "00";
   ePWMBOUT1 <= '0';
   ePWMBOUT0 <= '0';
ELSIF RISING_EDGE(clk) THEN
CASE STX IS
	WHEN ST1 =>    --0
        IF ePWMAxIN = '1' THEN 
	        STX <= ST2;
	     ELSE 
	     STX <= ST1;
	     ePWMAxOUT <=  "00";
         ePWMBOUT1 <=  '0';
	     ePWMBOUT0 <=  ePWMBxIN;
	     END IF;
	WHEN ST2 =>    --1
         IF ePWMAxIN = '0' THEN 
	        STX <= ST3;
	     ELSE
		 STX <= ST2; 
		 ePWMAxOUT <=  "10";
	     ePWMBOUT1 <=  ePWMBxIN;
	     ePWMBOUT0 <=  '0';
		 END IF;
    WHEN ST3 =>    --0
         IF ePWMAxIN = '1' THEN 
	        STX <= ST4;
	     ELSE 
		 STX <= ST3; 
		 ePWMAxOUT <=  "00";
	     ePWMBOUT1 <=  ePWMBxIN;
	     ePWMBOUT0 <=  '0';
		 END IF;
    WHEN ST4 =>     --1
         IF ePWMAxIN = '0' THEN 
	        STX <= ST1; 
	     ELSE 
		 STX <= ST4; 
		 ePWMAxOUT <=  "01";
	     ePWMBOUT1 <=  '0';
	     ePWMBOUT0 <=  ePWMBxIN;
		 END IF;       
	WHEN OTHERS => STX <= ST1; 	
	END CASE; 
end if;
end process;
END;
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  display IS
PORT ( clk,prt  :IN  STD_LOGIC;--40KHz D<100% 
	    prtout :OUT STD_LOGIC    
     );
END;
ARCHITECTURE display OF display IS 
TYPE STATES IS (ST0, ST1);
SIGNAL STX :STATES;
BEGIN
process(clk)
begin
IF RISING_EDGE(clk) THEN
CASE STX IS
	WHEN ST0 =>    --0
        IF prt = '0' THEN  STX <= ST1;
	    ELSE  STX <= ST0;
         prtout <= '1';
	     END IF;
	WHEN ST1 =>    --1
         STX <= ST1;
         prtout <= '0';       
	WHEN OTHERS => STX <= ST0; 	
	END CASE; 
end if;
end process;
END;
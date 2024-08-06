LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  boostLimit IS
PORT ( clk          :IN  STD_LOGIC;--20KHz D<100%
       BLimitandePWMxIN  :IN STD_LOGIC_vector(1 downto 0);
	   ePWMxOUT     :OUT STD_LOGIC--10KHz D<50%         
     );
END;
ARCHITECTURE boostLimit OF boostLimit IS 
--SIGNAL    COUNT : STD_LOGIC_vector(3 downto 0);
TYPE STATES IS (ST1, ST2, ST3);
SIGNAL STX :STATES;
BEGIN
process(clk)
begin
IF RISING_EDGE(clk) THEN
CASE STX IS
	WHEN ST1 =>    --00 and 01
        CASE BLimitandePWMxIN IS
	        WHEN "10" =>   STX <= ST2;
			WHEN OTHERS =>
			    STX <= ST1;
			    ePWMxOUT <= '0';
		END CASE; 	 
	WHEN ST2 =>   --10
	    CASE BLimitandePWMxIN IS
	        WHEN "11" =>      STX <= ST3;
		    WHEN "00"|"01" => STX <= ST1;
			WHEN OTHERS =>
			    STX <= ST2;
			    ePWMxOUT <= '0';
		END CASE;    
    WHEN ST3 =>   --11
 	    CASE BLimitandePWMxIN IS
	        WHEN "10" =>      STX <= ST2;
            WHEN "00"|"01" => STX <= ST1;
			WHEN OTHERS =>
			STX <= ST3;
			ePWMxOUT <= '1';   
		END CASE;       
	WHEN OTHERS => STX <= ST1; 	
	END CASE; 
end if;
end process;
END;
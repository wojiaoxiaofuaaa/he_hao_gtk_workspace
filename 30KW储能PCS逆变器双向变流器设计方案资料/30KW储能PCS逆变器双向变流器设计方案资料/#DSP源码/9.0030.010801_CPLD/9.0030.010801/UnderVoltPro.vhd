LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  UnderVoltPro IS
PORT ( clk,VoltPro     :IN STD_LOGIC;--10KHz      
	   VoltProReset : OUT STD_LOGIC       
     );
END;


ARCHITECTURE UnderVoltPro OF UnderVoltPro IS 
SIGNAL    COUNT1 : STD_LOGIC_vector(19 downto 0);
--SIGNAL    fliterCT : STD_LOGIC_vector(1 downto 0);
SIGNAL VoltPro_SG : STD_LOGIC;
TYPE STATES IS (ST0, ST1);
SIGNAL current_st, next_st :STATES;
--SIGNAL STX :STATES;
BEGIN
process(clk)
begin
    IF FALLING_EDGE(clk) THEN
        current_st <= next_st;
        VoltPro_SG <= VoltPro;
    end if;
end process;

process(clk,current_st)
begin
IF RISING_EDGE(clk) THEN
    CASE current_st IS
      WHEN ST0 =>           --0
            CASE VoltPro_SG IS
              WHEN '1' =>
                   next_st <= ST1;
                   COUNT1 <= (OTHERS => '0');
              WHEN OTHERS =>
                   next_st <= ST0;                  
                   VoltProReset <= '1';
              END CASE;
      WHEN ST1 =>       --1
          CASE VoltPro_SG IS 
              WHEN '0' =>
                   next_st <= ST0;
                   COUNT1 <= (OTHERS => '0');
              WHEN OTHERS =>
                   next_st <= ST1;               
               IF  COUNT1 < B"1000_0000_0000_0000_0000" THEN
                    COUNT1 <= COUNT1 + 1;
                    VoltProReset <= '1';
               ELSE 
                    VoltProReset <= '0';
               END IF;
              END CASE;
      WHEN OTHERS => next_st <= ST0;
    END CASE;
END IF;
end process;
END;
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  InvPro IS
PORT ( clk,unlock,InvFO,InvI     :IN STD_LOGIC;--10KHz      
	   LedPro : OUT STD_LOGIC       
     );
END;


ARCHITECTURE InvPro OF InvPro IS 
SIGNAL    COUNT1 : STD_LOGIC_vector(21 downto 0);
--SIGNAL    fliterCT : STD_LOGIC_vector(1 downto 0);
SIGNAL InvFO_SG,InvI_SG,unlock_SG,unlock_SG1 : STD_LOGIC;
TYPE STATES IS (ST0, ST1, ST2);
SIGNAL current_st, next_st :STATES;
--SIGNAL STX :STATES;
BEGIN
process(clk)
begin
    IF FALLING_EDGE(clk) THEN
        current_st <= next_st;
        InvFO_SG <= InvFO;
        InvI_SG <= InvI;
        unlock_SG1 <= unlock;
        unlock_SG  <= unlock_SG1;
    end if;
end process;

process(clk,current_st,unlock_SG)
begin
IF unlock_SG = '0' THEN
     next_st <= ST0;  
ELSIF RISING_EDGE(clk) THEN
    CASE current_st IS
      WHEN ST0 =>           --
            if InvFO_SG = '0' then
                   next_st <= ST1;
                   COUNT1 <= (OTHERS => '0');
            elsif InvI_SG = '0' then
                   next_st <= ST2;
                   COUNT1 <= (OTHERS => '0');                   
            else  next_st <= ST0;
            end if;
            LedPro <= '1';
      WHEN ST1 =>       --
             COUNT1 <= COUNT1 + 1 ;
            if COUNT1 < B"10_0000_0000_0000_0000_0000" then
                LedPro <= '1';   
            else  LedPro <= '0';  
            end if;
      WHEN ST2 =>       --
             LedPro <= '0';
      WHEN OTHERS => next_st <= ST0;
    END CASE;
END IF;
end process;
END;
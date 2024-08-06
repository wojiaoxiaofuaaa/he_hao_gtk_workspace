LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY SYNCFILTER IS
PORT ( clk,x     :IN STD_LOGIC;--10KHz      
	   y : OUT STD_LOGIC       
     );
END;

ARCHITECTURE SYNCFILTER OF SYNCFILTER IS 
SIGNAL z0: STD_LOGIC;
SIGNAL Rshift : STD_LOGIC_vector(9 downto 0);
SIGNAL COUNT1 : STD_LOGIC_vector(10 downto 0);
TYPE STATES IS (ST0, ST1);
SIGNAL current_st, next_st :STATES;
BEGIN

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        current_st <= next_st;
        Rshift(0) <= x;
        Rshift(1) <= Rshift(0);
        Rshift(2) <= Rshift(1);
        Rshift(3) <= Rshift(2);
        Rshift(4) <= Rshift(3);
        Rshift(5) <= Rshift(4);
        Rshift(6) <= Rshift(5);
        Rshift(7) <= Rshift(6);
        Rshift(8) <= Rshift(7);
        Rshift(9) <= Rshift(8);
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
       if Rshift = B"00_0000_0000" then
           z0 <= '0';
       elsif Rshift = B"11_1111_1111" then
           z0 <= '1';
       else
           z0 <= z0;
       end if;
    end if;
end process;

process(clk,current_st)
begin
IF RISING_EDGE(clk) THEN
    CASE current_st IS
        WHEN ST0 =>           --0
            CASE z0 IS
                WHEN '1' =>
                    next_st <= ST1;    
                WHEN OTHERS =>
                    next_st <= ST0;                  
                END CASE;
                y <= '0';
                COUNT1 <= (OTHERS => '0');
        WHEN ST1 =>       --1
            CASE z0 IS 
                WHEN '0' =>
                    next_st <= ST0;
                    COUNT1 <= (OTHERS => '0');
                WHEN OTHERS =>
                    next_st <= ST1;               
                IF  COUNT1 <  B"110_0111_1111"THEN  --0D1664
                    COUNT1 <= COUNT1 + 1;
                    y <= '0';
                ELSE 
                    y <= '1';
                END IF;
            END CASE;
        WHEN OTHERS => next_st <= ST0;
    END CASE;
END IF;
end process;
--y <= yBuf;
END;
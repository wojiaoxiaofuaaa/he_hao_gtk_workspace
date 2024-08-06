LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY ILIMIT50US IS
PORT ( clk,SGlimit     :IN STD_LOGIC;--10KHz      
	   SGlimitout : OUT STD_LOGIC       
     );
END;

ARCHITECTURE ILIMIT50US OF ILIMIT50US IS 
SIGNAL filter,midSGlimit : STD_LOGIC;
SIGNAL COUNT1 : STD_LOGIC_vector(8 downto 0);
TYPE STATES IS (ST0, ST1);
SIGNAL current_st, next_st :STATES;
BEGIN

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        filter <= SGlimit;
        current_st <= next_st;       
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
       if filter = SGlimit then
           midSGlimit <= filter;
       end if;
    end if;
end process;

process(clk,midSGlimit)
begin
IF RISING_EDGE(clk) THEN
    CASE current_st IS
        WHEN ST0 =>           --1
            CASE midSGlimit IS
                WHEN '0' =>
                    next_st <= ST1;    
                WHEN OTHERS =>
                    next_st <= ST0;                  
                END CASE;
                SGlimitout <= '1';
                COUNT1 <= (OTHERS => '0');
        WHEN ST1 =>       --0
            IF COUNT1 <  B"1_1111_0100"THEN
                 COUNT1 <= COUNT1 + 1;
                 SGlimitout <= '0';
                 next_st <= ST1;
            ELSIF midSGlimit = '0' THEN
                 SGlimitout <= '0';
                 next_st <= ST1;
            ELSE
                SGlimitout <= '1';
                next_st <= ST0;
            END IF;       
        WHEN OTHERS => next_st <= ST0;
    END CASE;
END IF;
end process;
--y <= yBuf;
END;
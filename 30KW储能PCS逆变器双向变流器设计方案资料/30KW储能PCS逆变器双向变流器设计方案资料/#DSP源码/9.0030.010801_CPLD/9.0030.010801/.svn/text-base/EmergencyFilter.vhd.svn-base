LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY EmergencyFilter IS
PORT ( clk,in_put     :IN STD_LOGIC;--10KHz      
	   out_put : OUT STD_LOGIC       
     );
END;

ARCHITECTURE EmergencyFilter OF EmergencyFilter IS 
SIGNAL z0: STD_LOGIC;
SIGNAL Rshift : STD_LOGIC_vector(2 downto 0);
SIGNAL Countupdown : STD_LOGIC_vector(6 downto 0);
BEGIN

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        Rshift(0) <= in_put;
        Rshift(1) <= Rshift(0);
        Rshift(2) <= Rshift(1);
        --Rshift(3) <= Rshift(2);
        --Rshift(4) <= Rshift(3);
        --Rshift(5) <= Rshift(4);
        --Rshift(6) <= Rshift(5);
        --Rshift(7) <= Rshift(6);
        --Rshift(8) <= Rshift(7);
        --Rshift(9) <= Rshift(8);
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
       if Rshift = B"000" then
           z0 <= '0';
       elsif Rshift = B"111" then
           z0 <= '1';
       else
           z0 <= z0;
       end if;
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
         if z0 = '1' then
             if Countupdown < B"110_0100"  then
                 Countupdown <= Countupdown + 1;
             else
                out_put <= '1'; 
             end if;
         else
             if Countupdown > B"000_0000"  then
                 Countupdown <= Countupdown - 1;
             else
                out_put <= '0'; 
             end if;
         end if;
    end if;
end process;

END;
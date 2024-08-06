LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  and4sync IS
PORT ( clk,in_put1,in_put2     :IN STD_LOGIC;--10KHz  
       in_put3,in_put4     :IN STD_LOGIC;--10KHz       
	   out_put : OUT STD_LOGIC       
     );
END;


ARCHITECTURE and4sync OF and4sync IS 
SIGNAL Filtera : std_logic_vector(3 downto 0);
SIGNAL Filterb : std_logic_vector(3 downto 0);
BEGIN

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        Filtera(0) <= in_put1;
        Filtera(1) <= in_put2;
        Filtera(2) <= in_put3;
        Filtera(3) <= in_put4;
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        IF Filtera(0) = in_put1 THEN
           Filterb(0) <= Filtera(0);
        END IF;
        IF Filtera(1) = in_put2 THEN
           Filterb(1) <= Filtera(1);
        END IF;
        IF Filtera(2) = in_put3 THEN
           Filterb(2) <= Filtera(2);
        END IF;
        IF Filtera(3) = in_put4 THEN
           Filterb(3) <= Filtera(3);
        END IF;
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        out_put <= Filterb(0) and Filterb(1) and Filterb(2) and Filterb(3);
    end if;
end process;
END;
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  signalsync IS
PORT ( clk,in_put1  : IN STD_LOGIC;      
	   out_put : OUT STD_LOGIC       
     );
END;

ARCHITECTURE signalsync OF signalsync IS 
SIGNAL Filtera : std_logic;
SIGNAL Filterb : std_logic;
BEGIN

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        Filtera <= in_put1;
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        IF Filtera = in_put1 THEN
           Filterb <= Filtera;
        END IF;
    end if;
end process;

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        out_put <= Filterb;
    end if;
end process;
END;
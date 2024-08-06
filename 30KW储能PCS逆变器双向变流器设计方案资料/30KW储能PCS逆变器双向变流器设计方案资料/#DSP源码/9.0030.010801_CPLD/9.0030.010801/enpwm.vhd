LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  enpwm IS
PORT ( en     :IN STD_LOGIC;--10KHz 
       ENPWMIN  : IN STD_LOGIC_vector(3 downto 0);
       --bit 0 is ePWMxB,bit 1 is ePWMxA
	   ENPWMOUT :OUT STD_LOGIC_vector(3 downto 0)
	   --bit 0 is ePWMxB not,bit 1 is ePWMxB 
	   --bit 2 is ePWMxA not,bit 3 is ePWMxA           
     );
END;
ARCHITECTURE enpwm OF enpwm IS 

BEGIN
process(en)
begin
    if en = '1'then
    ENPWMOUT <= not B"0000";
    else 
    ENPWMOUT <= ENPWMIN;
    end if;
end process;
END;
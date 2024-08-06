LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  ENINVPWM IS
PORT ( en,clk   :IN STD_LOGIC;
       ENPWMIN  : IN STD_LOGIC;      
	   ENPWMOUT : OUT STD_LOGIC         
     );
END;
ARCHITECTURE ENINVPWM OF ENINVPWM IS 
SIGNAL ENPWMIN_SG0,ENPWMIN_SG1 : STD_LOGIC;
--TYPE STATES IS (ST0, ST1);
--SIGNAL current_st, next_st :STATES;
BEGIN

process(clk)
begin
    IF RISING_EDGE(clk) THEN
        --current_st <= next_st;
        ENPWMIN_SG0 <= ENPWMIN;
    end if;
end process;

process(clk)
begin
if RISING_EDGE(clk) THEN
    IF  ENPWMIN_SG0 = ENPWMIN THEN
        ENPWMIN_SG1 <= ENPWMIN_SG0;
    end if;
end if; 
end process;

process(clk)
begin
IF RISING_EDGE(clk) THEN        
    IF en = '0' THEN
        ENPWMOUT <= '0';
    ELSE
        ENPWMOUT <= ENPWMIN_SG1;
    END IF;
END IF;       

   -- IF en = '0' THEN
   --     next_st <= ST0;
   --     ENPWMOUT <= '0';
   -- ELSIF RISING_EDGE(clk) THEN
   --     CASE current_st IS 
   --         WHEN ST0 =>
   --             IF ENPWMIN_SG1 = '0' THEN
   --                 next_st <= ST1;
   --             END IF;
   --             ENPWMOUT <= '0';
   --         WHEN ST1 =>
   --             next_st <= ST1;
   --             ENPWMOUT <= ENPWMIN_SG1;  
   --         WHEN OTHERS => next_st <= ST0;
   --    END CASE;
   -- END IF;
end process;
END;
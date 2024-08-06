LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  hubu IS
PORT ( clk,EN     :IN STD_LOGIC;--10KHz 
       EPWMIN  : IN STD_LOGIC_vector(1 downto 0);
       --bit 0 is ePWMxB,bit 1 is ePWMxA
	   EPWMOUT : OUT STD_LOGIC_vector(3 downto 0)
	   --bit 0 is ePWMxB not,bit 1 is ePWMxB 
	   --bit 2 is ePWMxA not,bit 3 is ePWMxA           
     );
END;


ARCHITECTURE hubu OF hubu IS 
SIGNAL    COUNT1 : STD_LOGIC_vector(4 downto 0);
--SIGNAL    fliterCT : STD_LOGIC_vector(1 downto 0);
SIGNAL EPWMIN_SG : STD_LOGIC_vector(1 downto 0);
TYPE STATES IS (ST0, ST1, ST2, ST3);
SIGNAL current_st, next_st :STATES;
--SIGNAL STX :STATES;
BEGIN
process(clk)
begin
    IF FALLING_EDGE(clk) THEN
        current_st <= next_st;
        EPWMIN_SG <= EPWMIN;
    end if;
end process;

process(clk,current_st,EN)
begin
IF EN = '0' THEN
   next_st <= ST0;
   EPWMOUT <=   B"0000"; 
 ELSIF RISING_EDGE(clk) THEN
    CASE current_st IS
  --    WHEN ST0 =>
   --        CASE EN&EPWMIN IS
   --           WHEN "101" =>
   --                STX <= ST2;
   --                COUNT1 <= (OTHERS => '0' );--B"00_0000";
    --          WHEN "110" =>
   --                STX <= ST3;
    --               COUNT1 <= (OTHERS => '0' );--B"00_0000";
     --         WHEN OTHERS =>
    --               EPWMOUT <=   B"0000"; 
    --       END CASE;
      WHEN ST0 =>             --100
            CASE EPWMIN_SG IS
       --       WHEN "000"|"001"|"010"|"011" =>
       --            STX <= ST0;
       --            COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN "10" =>
                   next_st <= ST2;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN "01" =>
                   next_st <= ST1;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN OTHERS =>
                   next_st <= ST0;
               IF COUNT1 < B"11111" THEN
                   COUNT1 <= COUNT1 + 1;
                   EPWMOUT <=   B"0100";
               ELSE EPWMOUT <=  B"0101";
               END IF;
              END CASE;

--          IF EPWMIN = "10"  THEN 
--           STX <= ST3;
--           COUNT1 <= (OTHERS => '0' );--B"00_0000";
--          ELSIF EPWMIN = "01"  THEN 
--           STX <= ST2;
--           COUNT1 <= (OTHERS => '0' );--B"00_0000";
 --         ELSIF COUNT1 < B"11_1111" THEN
 --          COUNT1 <= COUNT1 + 1;
 --          EPWMOUT <= B"0001";
 --         ELSE EPWMOUT <= B"0101";
 --         END IF;
      WHEN ST1 =>        --ePWMxA  101
          CASE EPWMIN_SG IS 
             -- WHEN "000"|"001"|"010"|"011" =>
              --     STX <= ST0;
              --     COUNT1 <= (OTHERS => '0' );--B"00_0000";  
              WHEN "00" =>
                   next_st <= ST0;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN OTHERS =>
                   next_st <= ST1;
               IF  COUNT1 < B"11111" THEN
                   COUNT1 <= COUNT1 + 1;
                   EPWMOUT <=  B"0100";
               ELSE EPWMOUT <= B"0110";
               END IF;
              END CASE;
       --   IF EPWMIN = "00"  THEN 
         --  STX <= ST4;
        --   COUNT1 <= (OTHERS => '0' );--B"00_0000";
         -- ELSIF COUNT1 < B"1111" THEN
         --  COUNT1 <= COUNT1 + 1;
         --  EPWMOUT <=  not B"0100";
         -- ELSE EPWMOUT <= not B"0110";
         -- END IF;
      WHEN ST2 =>        --ePWMxB    110
              CASE EPWMIN_SG IS
             -- WHEN "000"|"001"|"010"|"011" =>
              --     STX <= ST0;
             --      COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN "00" =>
                   next_st <= ST3;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN OTHERS =>
                   next_st <= ST2;
               IF  COUNT1 < B"11111" THEN
                   COUNT1 <= COUNT1 + 1;
                   EPWMOUT <=  B"0001";
               ELSE EPWMOUT <= B"1001";
               END IF;
              END CASE;
         -- IF EPWMIN = "00"  THEN 
          -- STX <= ST1;
         --  COUNT1 <= (OTHERS => '0' );--B"00_0000";
         -- ELSIF COUNT1 < B"1111" THEN
         --  COUNT1 <= COUNT1 + 1;
         --  EPWMOUT <=  not B"0001";
         -- ELSE EPWMOUT <= not B"1001";
         -- END IF;
      WHEN ST3 =>             --100
           CASE EPWMIN_SG IS
          --    WHEN "000"|"001"|"010"|"011" =>
           --        STX <= ST0;
           --        COUNT1 <= (OTHERS => '0' );--B"00_0000";
               WHEN "10" =>
                   next_st <= ST2;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
               WHEN "01" =>
                   next_st <= ST1;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN OTHERS =>
                   next_st <= ST3;
             IF COUNT1 < B"11111" THEN
                COUNT1 <= COUNT1 + 1;
                EPWMOUT <=  B"0001";
             ELSE EPWMOUT <=  B"0101";
             END IF;
           END CASE;
--          IF EPWMIN = "10"  THEN 
 --          STX <= ST3;
 --          COUNT1 <= (OTHERS => '0' );--B"00_0000";
 --         ELSIF EPWMIN = "01"  THEN 
 --          STX <= ST2;
 --          COUNT1 <= (OTHERS => '0' );--B"00_0000";
 --         ELSIF COUNT1 < B"1111" THEN
 --          COUNT1 <= COUNT1 + 1;
 --          EPWMOUT <= B"0100";
  --        ELSE EPWMOUT <= B"0101";
 --         END IF;
      WHEN OTHERS => next_st <= ST0;
    END CASE;
END IF;
end process;
END;
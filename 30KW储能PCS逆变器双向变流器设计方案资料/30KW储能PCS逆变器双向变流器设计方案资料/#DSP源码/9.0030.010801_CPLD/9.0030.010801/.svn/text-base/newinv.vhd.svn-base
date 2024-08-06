LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
ENTITY  newinv IS
PORT ( clk     :IN STD_LOGIC;--10KHz 
       --en      :IN STD_LOGIC;
       EPWMIN  :STD_LOGIC_vector(1 downto 0);
       --bit 0 is ePWMxB,bit 1 is ePWMxA
	   EPWMOUT :OUT STD_LOGIC_vector(3 downto 0)
	   --bit 0 is ePWMxB not,bit 1 is ePWMxB 
	   --bit 2 is ePWMxA not,bit 3 is ePWMxA           
     );
END;
ARCHITECTURE newinv OF newinv IS 
SIGNAL    COUNT1 : STD_LOGIC_vector(3 downto 0);
TYPE STATES IS (ST1, ST2, ST3, ST4);
SIGNAL STX :STATES;
BEGIN
process(clk)
begin
  IF RISING_EDGE(clk) THEN
    CASE STX IS
      WHEN ST1 =>
            CASE EPWMIN IS
              WHEN "10" =>
                   STX <= ST3;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN "01" =>
                   STX <= ST2;
                   COUNT1 <= (OTHERS => '0' );--B"00_0000";
              WHEN OTHERS =>
               IF COUNT1 < B"1111" THEN
                   COUNT1 <= COUNT1 + 1;
                   EPWMOUT <= B"0001";
               ELSE EPWMOUT <= B"0101";
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
      WHEN ST2 =>        --ePWMxA
          IF EPWMIN = "00"  THEN 
           STX <= ST4;
           COUNT1 <= (OTHERS => '0' );--B"00_0000";
          ELSIF COUNT1 < B"1111" THEN
           COUNT1 <= COUNT1 + 1;
           EPWMOUT <= B"0100";
          ELSE EPWMOUT <= B"0110";
          END IF;
      WHEN ST3 =>        --ePWMxB
          IF EPWMIN = "00"  THEN 
           STX <= ST1;
           COUNT1 <= (OTHERS => '0' );--B"00_0000";
          ELSIF COUNT1 < B"1111" THEN
           COUNT1 <= COUNT1 + 1;
           EPWMOUT <= B"0001";
          ELSE EPWMOUT <= B"1001";
          END IF;
      WHEN ST4 =>
           CASE EPWMIN IS
           WHEN "10" =>
           STX <= ST3;
           COUNT1 <= (OTHERS => '0' );--B"00_0000";
           WHEN "01" =>
           STX <= ST2;
           COUNT1 <= (OTHERS => '0' );--B"00_0000";
           WHEN OTHERS =>
             IF COUNT1 < B"1111" THEN
                COUNT1 <= COUNT1 + 1;
                EPWMOUT <= B"0100";
             ELSE EPWMOUT <= B"0101";
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
      WHEN OTHERS => STX <= ST1;
    END CASE;
 END IF;
end process;
END;
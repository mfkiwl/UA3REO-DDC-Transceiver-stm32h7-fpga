-- -------------------------------------------------------------
--
-- Module: rx_ciccomp
-- Generated by MATLAB(R) 9.9 and Filter Design HDL Coder 3.1.8.
-- Generated on: 2021-02-10 23:29:04
-- -------------------------------------------------------------

-- -------------------------------------------------------------
-- HDL Code Generation Options:
--
-- TargetLanguage: VHDL
-- OptimizeForHDL: on
-- Name: rx_ciccomp
-- SerialPartition: 55
-- TestBenchName: rx_ciccomp_tb
-- TestBenchStimulus: impulse step ramp chirp noise 

-- Filter Specifications:
--
-- Sample Rate            : N/A (normalized frequency)
-- Response               : CIC Compensator
-- Specification          : Fp,Fst,Ap,Ast
-- Number of Sections     : 5
-- Differential Delay     : 1
-- CIC Rate Change Factor : 1280
-- Passband Ripple        : 0.3 dB
-- Stopband Edge          : 0.55
-- Stopband Atten.        : 60 dB
-- Passband Edge          : 0.45
-- -------------------------------------------------------------

-- -------------------------------------------------------------
-- HDL Implementation    : Fully Serial
-- Folding Factor        : 55
-- -------------------------------------------------------------
-- Filter Settings:
--
-- Discrete-Time FIR Filter (real)
-- -------------------------------
-- Filter Structure  : Direct-Form FIR
-- Filter Length     : 55
-- Stable            : Yes
-- Linear Phase      : Yes (Type 1)
-- Arithmetic        : fixed
-- Numerator         : s16,15 -> [-1 1)
-- Input             : s32,0 -> [-2147483648 2147483648)
-- Filter Internals  : Full Precision
--   Output          : s49,15 -> [-8589934592 8589934592)  (auto determined)
--   Product         : s47,15 -> [-2147483648 2147483648)  (auto determined)
--   Accumulator     : s49,15 -> [-8589934592 8589934592)  (auto determined)
--   Round Mode      : No rounding
--   Overflow Mode   : No overflow
-- -------------------------------------------------------------



LIBRARY IEEE;
USE IEEE.std_logic_1164.all;
USE IEEE.numeric_std.ALL;

ENTITY rx_ciccomp IS
   PORT( clk                             :   IN    std_logic; 
         clk_enable                      :   IN    std_logic; 
         reset                           :   IN    std_logic; 
         filter_in                       :   IN    std_logic_vector(31 DOWNTO 0); -- sfix32
         filter_out                      :   OUT   std_logic_vector(48 DOWNTO 0)  -- sfix49_En15
         );

END rx_ciccomp;


----------------------------------------------------------------
--Module Architecture: rx_ciccomp
----------------------------------------------------------------
ARCHITECTURE rtl OF rx_ciccomp IS
  -- Local Functions
  -- Type Definitions
  TYPE delay_pipeline_type IS ARRAY (NATURAL range <>) OF signed(31 DOWNTO 0); -- sfix32
  -- Constants
  CONSTANT coeff1                         : signed(15 DOWNTO 0) := to_signed(75, 16); -- sfix16_En15
  CONSTANT coeff2                         : signed(15 DOWNTO 0) := to_signed(200, 16); -- sfix16_En15
  CONSTANT coeff3                         : signed(15 DOWNTO 0) := to_signed(120, 16); -- sfix16_En15
  CONSTANT coeff4                         : signed(15 DOWNTO 0) := to_signed(-120, 16); -- sfix16_En15
  CONSTANT coeff5                         : signed(15 DOWNTO 0) := to_signed(-149, 16); -- sfix16_En15
  CONSTANT coeff6                         : signed(15 DOWNTO 0) := to_signed(152, 16); -- sfix16_En15
  CONSTANT coeff7                         : signed(15 DOWNTO 0) := to_signed(216, 16); -- sfix16_En15
  CONSTANT coeff8                         : signed(15 DOWNTO 0) := to_signed(-184, 16); -- sfix16_En15
  CONSTANT coeff9                         : signed(15 DOWNTO 0) := to_signed(-317, 16); -- sfix16_En15
  CONSTANT coeff10                        : signed(15 DOWNTO 0) := to_signed(219, 16); -- sfix16_En15
  CONSTANT coeff11                        : signed(15 DOWNTO 0) := to_signed(448, 16); -- sfix16_En15
  CONSTANT coeff12                        : signed(15 DOWNTO 0) := to_signed(-250, 16); -- sfix16_En15
  CONSTANT coeff13                        : signed(15 DOWNTO 0) := to_signed(-621, 16); -- sfix16_En15
  CONSTANT coeff14                        : signed(15 DOWNTO 0) := to_signed(276, 16); -- sfix16_En15
  CONSTANT coeff15                        : signed(15 DOWNTO 0) := to_signed(845, 16); -- sfix16_En15
  CONSTANT coeff16                        : signed(15 DOWNTO 0) := to_signed(-293, 16); -- sfix16_En15
  CONSTANT coeff17                        : signed(15 DOWNTO 0) := to_signed(-1144, 16); -- sfix16_En15
  CONSTANT coeff18                        : signed(15 DOWNTO 0) := to_signed(292, 16); -- sfix16_En15
  CONSTANT coeff19                        : signed(15 DOWNTO 0) := to_signed(1557, 16); -- sfix16_En15
  CONSTANT coeff20                        : signed(15 DOWNTO 0) := to_signed(-259, 16); -- sfix16_En15
  CONSTANT coeff21                        : signed(15 DOWNTO 0) := to_signed(-2168, 16); -- sfix16_En15
  CONSTANT coeff22                        : signed(15 DOWNTO 0) := to_signed(149, 16); -- sfix16_En15
  CONSTANT coeff23                        : signed(15 DOWNTO 0) := to_signed(3183, 16); -- sfix16_En15
  CONSTANT coeff24                        : signed(15 DOWNTO 0) := to_signed(190, 16); -- sfix16_En15
  CONSTANT coeff25                        : signed(15 DOWNTO 0) := to_signed(-5254, 16); -- sfix16_En15
  CONSTANT coeff26                        : signed(15 DOWNTO 0) := to_signed(-1649, 16); -- sfix16_En15
  CONSTANT coeff27                        : signed(15 DOWNTO 0) := to_signed(11532, 16); -- sfix16_En15
  CONSTANT coeff28                        : signed(15 DOWNTO 0) := to_signed(19235, 16); -- sfix16_En15
  CONSTANT coeff29                        : signed(15 DOWNTO 0) := to_signed(11532, 16); -- sfix16_En15
  CONSTANT coeff30                        : signed(15 DOWNTO 0) := to_signed(-1649, 16); -- sfix16_En15
  CONSTANT coeff31                        : signed(15 DOWNTO 0) := to_signed(-5254, 16); -- sfix16_En15
  CONSTANT coeff32                        : signed(15 DOWNTO 0) := to_signed(190, 16); -- sfix16_En15
  CONSTANT coeff33                        : signed(15 DOWNTO 0) := to_signed(3183, 16); -- sfix16_En15
  CONSTANT coeff34                        : signed(15 DOWNTO 0) := to_signed(149, 16); -- sfix16_En15
  CONSTANT coeff35                        : signed(15 DOWNTO 0) := to_signed(-2168, 16); -- sfix16_En15
  CONSTANT coeff36                        : signed(15 DOWNTO 0) := to_signed(-259, 16); -- sfix16_En15
  CONSTANT coeff37                        : signed(15 DOWNTO 0) := to_signed(1557, 16); -- sfix16_En15
  CONSTANT coeff38                        : signed(15 DOWNTO 0) := to_signed(292, 16); -- sfix16_En15
  CONSTANT coeff39                        : signed(15 DOWNTO 0) := to_signed(-1144, 16); -- sfix16_En15
  CONSTANT coeff40                        : signed(15 DOWNTO 0) := to_signed(-293, 16); -- sfix16_En15
  CONSTANT coeff41                        : signed(15 DOWNTO 0) := to_signed(845, 16); -- sfix16_En15
  CONSTANT coeff42                        : signed(15 DOWNTO 0) := to_signed(276, 16); -- sfix16_En15
  CONSTANT coeff43                        : signed(15 DOWNTO 0) := to_signed(-621, 16); -- sfix16_En15
  CONSTANT coeff44                        : signed(15 DOWNTO 0) := to_signed(-250, 16); -- sfix16_En15
  CONSTANT coeff45                        : signed(15 DOWNTO 0) := to_signed(448, 16); -- sfix16_En15
  CONSTANT coeff46                        : signed(15 DOWNTO 0) := to_signed(219, 16); -- sfix16_En15
  CONSTANT coeff47                        : signed(15 DOWNTO 0) := to_signed(-317, 16); -- sfix16_En15
  CONSTANT coeff48                        : signed(15 DOWNTO 0) := to_signed(-184, 16); -- sfix16_En15
  CONSTANT coeff49                        : signed(15 DOWNTO 0) := to_signed(216, 16); -- sfix16_En15
  CONSTANT coeff50                        : signed(15 DOWNTO 0) := to_signed(152, 16); -- sfix16_En15
  CONSTANT coeff51                        : signed(15 DOWNTO 0) := to_signed(-149, 16); -- sfix16_En15
  CONSTANT coeff52                        : signed(15 DOWNTO 0) := to_signed(-120, 16); -- sfix16_En15
  CONSTANT coeff53                        : signed(15 DOWNTO 0) := to_signed(120, 16); -- sfix16_En15
  CONSTANT coeff54                        : signed(15 DOWNTO 0) := to_signed(200, 16); -- sfix16_En15
  CONSTANT coeff55                        : signed(15 DOWNTO 0) := to_signed(75, 16); -- sfix16_En15

  -- Signals
  SIGNAL cur_count                        : unsigned(5 DOWNTO 0); -- ufix6
  SIGNAL phase_54                         : std_logic; -- boolean
  SIGNAL phase_0                          : std_logic; -- boolean
  SIGNAL delay_pipeline                   : delay_pipeline_type(0 TO 54); -- sfix32
  SIGNAL inputmux_1                       : signed(31 DOWNTO 0); -- sfix32
  SIGNAL acc_final                        : signed(48 DOWNTO 0); -- sfix49_En15
  SIGNAL acc_out_1                        : signed(48 DOWNTO 0); -- sfix49_En15
  SIGNAL product_1                        : signed(46 DOWNTO 0); -- sfix47_En15
  SIGNAL product_1_mux                    : signed(15 DOWNTO 0); -- sfix16_En15
  SIGNAL mul_temp                         : signed(47 DOWNTO 0); -- sfix48_En15
  SIGNAL prod_typeconvert_1               : signed(48 DOWNTO 0); -- sfix49_En15
  SIGNAL acc_sum_1                        : signed(48 DOWNTO 0); -- sfix49_En15
  SIGNAL acc_in_1                         : signed(48 DOWNTO 0); -- sfix49_En15
  SIGNAL add_temp                         : signed(49 DOWNTO 0); -- sfix50_En15
  SIGNAL output_typeconvert               : signed(48 DOWNTO 0); -- sfix49_En15
  SIGNAL output_register                  : signed(48 DOWNTO 0); -- sfix49_En15


BEGIN

  -- Block Statements
  Counter_process : PROCESS (clk, reset)
  BEGIN
    IF reset = '1' THEN
      cur_count <= to_unsigned(54, 6);
    ELSIF clk'event AND clk = '1' THEN
      IF clk_enable = '1' THEN
        IF cur_count >= to_unsigned(54, 6) THEN
          cur_count <= to_unsigned(0, 6);
        ELSE
          cur_count <= cur_count + to_unsigned(1, 6);
        END IF;
      END IF;
    END IF; 
  END PROCESS Counter_process;

  phase_54 <= '1' WHEN cur_count = to_unsigned(54, 6) AND clk_enable = '1' ELSE '0';

  phase_0 <= '1' WHEN cur_count = to_unsigned(0, 6) AND clk_enable = '1' ELSE '0';

  Delay_Pipeline_process : PROCESS (clk, reset)
  BEGIN
    IF reset = '1' THEN
      delay_pipeline(0 TO 54) <= (OTHERS => (OTHERS => '0'));
    ELSIF clk'event AND clk = '1' THEN
      IF phase_54 = '1' THEN
        delay_pipeline(0) <= signed(filter_in);
        delay_pipeline(1 TO 54) <= delay_pipeline(0 TO 53);
      END IF;
    END IF; 
  END PROCESS Delay_Pipeline_process;

  inputmux_1 <= delay_pipeline(0) WHEN ( cur_count = to_unsigned(0, 6) ) ELSE
                     delay_pipeline(1) WHEN ( cur_count = to_unsigned(1, 6) ) ELSE
                     delay_pipeline(2) WHEN ( cur_count = to_unsigned(2, 6) ) ELSE
                     delay_pipeline(3) WHEN ( cur_count = to_unsigned(3, 6) ) ELSE
                     delay_pipeline(4) WHEN ( cur_count = to_unsigned(4, 6) ) ELSE
                     delay_pipeline(5) WHEN ( cur_count = to_unsigned(5, 6) ) ELSE
                     delay_pipeline(6) WHEN ( cur_count = to_unsigned(6, 6) ) ELSE
                     delay_pipeline(7) WHEN ( cur_count = to_unsigned(7, 6) ) ELSE
                     delay_pipeline(8) WHEN ( cur_count = to_unsigned(8, 6) ) ELSE
                     delay_pipeline(9) WHEN ( cur_count = to_unsigned(9, 6) ) ELSE
                     delay_pipeline(10) WHEN ( cur_count = to_unsigned(10, 6) ) ELSE
                     delay_pipeline(11) WHEN ( cur_count = to_unsigned(11, 6) ) ELSE
                     delay_pipeline(12) WHEN ( cur_count = to_unsigned(12, 6) ) ELSE
                     delay_pipeline(13) WHEN ( cur_count = to_unsigned(13, 6) ) ELSE
                     delay_pipeline(14) WHEN ( cur_count = to_unsigned(14, 6) ) ELSE
                     delay_pipeline(15) WHEN ( cur_count = to_unsigned(15, 6) ) ELSE
                     delay_pipeline(16) WHEN ( cur_count = to_unsigned(16, 6) ) ELSE
                     delay_pipeline(17) WHEN ( cur_count = to_unsigned(17, 6) ) ELSE
                     delay_pipeline(18) WHEN ( cur_count = to_unsigned(18, 6) ) ELSE
                     delay_pipeline(19) WHEN ( cur_count = to_unsigned(19, 6) ) ELSE
                     delay_pipeline(20) WHEN ( cur_count = to_unsigned(20, 6) ) ELSE
                     delay_pipeline(21) WHEN ( cur_count = to_unsigned(21, 6) ) ELSE
                     delay_pipeline(22) WHEN ( cur_count = to_unsigned(22, 6) ) ELSE
                     delay_pipeline(23) WHEN ( cur_count = to_unsigned(23, 6) ) ELSE
                     delay_pipeline(24) WHEN ( cur_count = to_unsigned(24, 6) ) ELSE
                     delay_pipeline(25) WHEN ( cur_count = to_unsigned(25, 6) ) ELSE
                     delay_pipeline(26) WHEN ( cur_count = to_unsigned(26, 6) ) ELSE
                     delay_pipeline(27) WHEN ( cur_count = to_unsigned(27, 6) ) ELSE
                     delay_pipeline(28) WHEN ( cur_count = to_unsigned(28, 6) ) ELSE
                     delay_pipeline(29) WHEN ( cur_count = to_unsigned(29, 6) ) ELSE
                     delay_pipeline(30) WHEN ( cur_count = to_unsigned(30, 6) ) ELSE
                     delay_pipeline(31) WHEN ( cur_count = to_unsigned(31, 6) ) ELSE
                     delay_pipeline(32) WHEN ( cur_count = to_unsigned(32, 6) ) ELSE
                     delay_pipeline(33) WHEN ( cur_count = to_unsigned(33, 6) ) ELSE
                     delay_pipeline(34) WHEN ( cur_count = to_unsigned(34, 6) ) ELSE
                     delay_pipeline(35) WHEN ( cur_count = to_unsigned(35, 6) ) ELSE
                     delay_pipeline(36) WHEN ( cur_count = to_unsigned(36, 6) ) ELSE
                     delay_pipeline(37) WHEN ( cur_count = to_unsigned(37, 6) ) ELSE
                     delay_pipeline(38) WHEN ( cur_count = to_unsigned(38, 6) ) ELSE
                     delay_pipeline(39) WHEN ( cur_count = to_unsigned(39, 6) ) ELSE
                     delay_pipeline(40) WHEN ( cur_count = to_unsigned(40, 6) ) ELSE
                     delay_pipeline(41) WHEN ( cur_count = to_unsigned(41, 6) ) ELSE
                     delay_pipeline(42) WHEN ( cur_count = to_unsigned(42, 6) ) ELSE
                     delay_pipeline(43) WHEN ( cur_count = to_unsigned(43, 6) ) ELSE
                     delay_pipeline(44) WHEN ( cur_count = to_unsigned(44, 6) ) ELSE
                     delay_pipeline(45) WHEN ( cur_count = to_unsigned(45, 6) ) ELSE
                     delay_pipeline(46) WHEN ( cur_count = to_unsigned(46, 6) ) ELSE
                     delay_pipeline(47) WHEN ( cur_count = to_unsigned(47, 6) ) ELSE
                     delay_pipeline(48) WHEN ( cur_count = to_unsigned(48, 6) ) ELSE
                     delay_pipeline(49) WHEN ( cur_count = to_unsigned(49, 6) ) ELSE
                     delay_pipeline(50) WHEN ( cur_count = to_unsigned(50, 6) ) ELSE
                     delay_pipeline(51) WHEN ( cur_count = to_unsigned(51, 6) ) ELSE
                     delay_pipeline(52) WHEN ( cur_count = to_unsigned(52, 6) ) ELSE
                     delay_pipeline(53) WHEN ( cur_count = to_unsigned(53, 6) ) ELSE
                     delay_pipeline(54);

  --   ------------------ Serial partition # 1 ------------------

  product_1_mux <= coeff1 WHEN ( cur_count = to_unsigned(0, 6) ) ELSE
                        coeff2 WHEN ( cur_count = to_unsigned(1, 6) ) ELSE
                        coeff3 WHEN ( cur_count = to_unsigned(2, 6) ) ELSE
                        coeff4 WHEN ( cur_count = to_unsigned(3, 6) ) ELSE
                        coeff5 WHEN ( cur_count = to_unsigned(4, 6) ) ELSE
                        coeff6 WHEN ( cur_count = to_unsigned(5, 6) ) ELSE
                        coeff7 WHEN ( cur_count = to_unsigned(6, 6) ) ELSE
                        coeff8 WHEN ( cur_count = to_unsigned(7, 6) ) ELSE
                        coeff9 WHEN ( cur_count = to_unsigned(8, 6) ) ELSE
                        coeff10 WHEN ( cur_count = to_unsigned(9, 6) ) ELSE
                        coeff11 WHEN ( cur_count = to_unsigned(10, 6) ) ELSE
                        coeff12 WHEN ( cur_count = to_unsigned(11, 6) ) ELSE
                        coeff13 WHEN ( cur_count = to_unsigned(12, 6) ) ELSE
                        coeff14 WHEN ( cur_count = to_unsigned(13, 6) ) ELSE
                        coeff15 WHEN ( cur_count = to_unsigned(14, 6) ) ELSE
                        coeff16 WHEN ( cur_count = to_unsigned(15, 6) ) ELSE
                        coeff17 WHEN ( cur_count = to_unsigned(16, 6) ) ELSE
                        coeff18 WHEN ( cur_count = to_unsigned(17, 6) ) ELSE
                        coeff19 WHEN ( cur_count = to_unsigned(18, 6) ) ELSE
                        coeff20 WHEN ( cur_count = to_unsigned(19, 6) ) ELSE
                        coeff21 WHEN ( cur_count = to_unsigned(20, 6) ) ELSE
                        coeff22 WHEN ( cur_count = to_unsigned(21, 6) ) ELSE
                        coeff23 WHEN ( cur_count = to_unsigned(22, 6) ) ELSE
                        coeff24 WHEN ( cur_count = to_unsigned(23, 6) ) ELSE
                        coeff25 WHEN ( cur_count = to_unsigned(24, 6) ) ELSE
                        coeff26 WHEN ( cur_count = to_unsigned(25, 6) ) ELSE
                        coeff27 WHEN ( cur_count = to_unsigned(26, 6) ) ELSE
                        coeff28 WHEN ( cur_count = to_unsigned(27, 6) ) ELSE
                        coeff29 WHEN ( cur_count = to_unsigned(28, 6) ) ELSE
                        coeff30 WHEN ( cur_count = to_unsigned(29, 6) ) ELSE
                        coeff31 WHEN ( cur_count = to_unsigned(30, 6) ) ELSE
                        coeff32 WHEN ( cur_count = to_unsigned(31, 6) ) ELSE
                        coeff33 WHEN ( cur_count = to_unsigned(32, 6) ) ELSE
                        coeff34 WHEN ( cur_count = to_unsigned(33, 6) ) ELSE
                        coeff35 WHEN ( cur_count = to_unsigned(34, 6) ) ELSE
                        coeff36 WHEN ( cur_count = to_unsigned(35, 6) ) ELSE
                        coeff37 WHEN ( cur_count = to_unsigned(36, 6) ) ELSE
                        coeff38 WHEN ( cur_count = to_unsigned(37, 6) ) ELSE
                        coeff39 WHEN ( cur_count = to_unsigned(38, 6) ) ELSE
                        coeff40 WHEN ( cur_count = to_unsigned(39, 6) ) ELSE
                        coeff41 WHEN ( cur_count = to_unsigned(40, 6) ) ELSE
                        coeff42 WHEN ( cur_count = to_unsigned(41, 6) ) ELSE
                        coeff43 WHEN ( cur_count = to_unsigned(42, 6) ) ELSE
                        coeff44 WHEN ( cur_count = to_unsigned(43, 6) ) ELSE
                        coeff45 WHEN ( cur_count = to_unsigned(44, 6) ) ELSE
                        coeff46 WHEN ( cur_count = to_unsigned(45, 6) ) ELSE
                        coeff47 WHEN ( cur_count = to_unsigned(46, 6) ) ELSE
                        coeff48 WHEN ( cur_count = to_unsigned(47, 6) ) ELSE
                        coeff49 WHEN ( cur_count = to_unsigned(48, 6) ) ELSE
                        coeff50 WHEN ( cur_count = to_unsigned(49, 6) ) ELSE
                        coeff51 WHEN ( cur_count = to_unsigned(50, 6) ) ELSE
                        coeff52 WHEN ( cur_count = to_unsigned(51, 6) ) ELSE
                        coeff53 WHEN ( cur_count = to_unsigned(52, 6) ) ELSE
                        coeff54 WHEN ( cur_count = to_unsigned(53, 6) ) ELSE
                        coeff55;
  mul_temp <= inputmux_1 * product_1_mux;
  product_1 <= mul_temp(46 DOWNTO 0);

  prod_typeconvert_1 <= resize(product_1, 49);

  add_temp <= resize(prod_typeconvert_1, 50) + resize(acc_out_1, 50);
  acc_sum_1 <= add_temp(48 DOWNTO 0);

  acc_in_1 <= prod_typeconvert_1 WHEN ( phase_0 = '1' ) ELSE
                   acc_sum_1;

  Acc_reg_1_process : PROCESS (clk, reset)
  BEGIN
    IF reset = '1' THEN
      acc_out_1 <= (OTHERS => '0');
    ELSIF clk'event AND clk = '1' THEN
      IF clk_enable = '1' THEN
        acc_out_1 <= acc_in_1;
      END IF;
    END IF; 
  END PROCESS Acc_reg_1_process;

  Finalsum_reg_process : PROCESS (clk, reset)
  BEGIN
    IF reset = '1' THEN
      acc_final <= (OTHERS => '0');
    ELSIF clk'event AND clk = '1' THEN
      IF phase_0 = '1' THEN
        acc_final <= acc_out_1;
      END IF;
    END IF; 
  END PROCESS Finalsum_reg_process;

  output_typeconvert <= acc_final;

  Output_Register_process : PROCESS (clk, reset)
  BEGIN
    IF reset = '1' THEN
      output_register <= (OTHERS => '0');
    ELSIF clk'event AND clk = '1' THEN
      IF phase_54 = '1' THEN
        output_register <= output_typeconvert;
      END IF;
    END IF; 
  END PROCESS Output_Register_process;

  -- Assignment Statements
  filter_out <= std_logic_vector(output_register);
END rtl;

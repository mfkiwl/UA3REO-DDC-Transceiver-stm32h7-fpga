-- ------------------------------------------------------------------------- 
-- High Level Design Compiler for Intel(R) FPGAs Version 18.1 (Release Build #625)
-- Quartus Prime development tool and MATLAB/Simulink Interface
-- 
-- Legal Notice: Copyright 2018 Intel Corporation.  All rights reserved.
-- Your use of  Intel Corporation's design tools,  logic functions and other
-- software and  tools, and its AMPP partner logic functions, and any output
-- files any  of the foregoing (including  device programming  or simulation
-- files), and  any associated  documentation  or information  are expressly
-- subject  to the terms and  conditions of the  Intel FPGA Software License
-- Agreement, Intel MegaCore Function License Agreement, or other applicable
-- license agreement,  including,  without limitation,  that your use is for
-- the  sole  purpose of  programming  logic devices  manufactured by  Intel
-- and  sold by Intel  or its authorized  distributors. Please refer  to the
-- applicable agreement for further details.
-- ---------------------------------------------------------------------------

-- VHDL created from rx_ciccomp_rtl_core
-- VHDL created on Fri Mar 19 12:30:14 2021


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use IEEE.MATH_REAL.all;
use std.TextIO.all;
use work.dspba_library_package.all;

LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;
LIBRARY lpm;
USE lpm.lpm_components.all;

entity rx_ciccomp_rtl_core is
    port (
        xIn_v : in std_logic_vector(0 downto 0);  -- sfix1
        xIn_c : in std_logic_vector(7 downto 0);  -- sfix8
        xIn_0 : in std_logic_vector(31 downto 0);  -- sfix32
        xOut_v : out std_logic_vector(0 downto 0);  -- ufix1
        xOut_c : out std_logic_vector(7 downto 0);  -- ufix8
        xOut_0 : out std_logic_vector(61 downto 0);  -- sfix62
        clk : in std_logic;
        areset : in std_logic
    );
end rx_ciccomp_rtl_core;

architecture normal of rx_ciccomp_rtl_core is

    attribute altera_attribute : string;
    attribute altera_attribute of normal : architecture is "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name PHYSICAL_SYNTHESIS_REGISTER_DUPLICATION ON; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007";
    
    signal GND_q : STD_LOGIC_VECTOR (0 downto 0);
    signal VCC_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_xIn_0_15_q : STD_LOGIC_VECTOR (31 downto 0);
    signal d_in0_m0_wi0_wo0_assign_id1_q_15_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_count : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_run_preEnaQ : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_out : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_enableQ : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_ctrl : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_memread_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_13_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_14_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_compute_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_19_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_20_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_21_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_inner_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_inner_i : SIGNED (7 downto 0);
    attribute preserve : boolean;
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count0_inner_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count0_q : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_i : UNSIGNED (7 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count0_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count1_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count1_i : UNSIGNED (7 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count1_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count1_eq : std_logic;
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count1_eq : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_a : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_b : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_o : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_q : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_wi0_r0_wa0_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_wa0_i : UNSIGNED (7 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_wa0_i : signal is true;
    signal u0_m0_wo0_wi0_r0_memr0_reset0 : std_logic;
    signal u0_m0_wo0_wi0_r0_memr0_ia : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_aa : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_ab : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_iq : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_q : STD_LOGIC_VECTOR (31 downto 0);
    signal d_u0_m0_wo0_wi0_r0_memr0_q_16_q : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_ca0_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_ca0_i : UNSIGNED (7 downto 0);
    attribute preserve of u0_m0_wo0_ca0_i : signal is true;
    signal u0_m0_wo0_ca0_eq : std_logic;
    attribute preserve of u0_m0_wo0_ca0_eq : signal is true;
    signal u0_m0_wo0_aseq_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_aseq_eq : std_logic;
    signal d_u0_m0_wo0_aseq_q_21_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_oseq_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_oseq_eq : std_logic;
    signal u0_m0_wo0_oseq_gated_reg_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_reset0 : std_logic;
    signal u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_ia : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_aa : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_ab : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_ir : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_r : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_cm0_lutmem_reset0 : std_logic;
    signal u0_m0_wo0_cm0_lutmem_ia : STD_LOGIC_VECTOR (21 downto 0);
    signal u0_m0_wo0_cm0_lutmem_aa : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_cm0_lutmem_ab : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_cm0_lutmem_ir : STD_LOGIC_VECTOR (21 downto 0);
    signal u0_m0_wo0_cm0_lutmem_r : STD_LOGIC_VECTOR (21 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im0_a0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im0_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im0_s1 : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im0_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im0_q : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im3_a0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im3_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im3_s1 : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im3_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im3_q : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im6_a0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im6_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im6_s1 : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im6_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im6_q : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im9_a0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im9_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im9_s1 : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im9_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im9_q : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im12_a0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im12_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im12_s1 : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im12_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im12_q : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im15_a0 : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im15_b0 : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im15_s1 : STD_LOGIC_VECTOR (14 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im15_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im15_q : STD_LOGIC_VECTOR (14 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_bs16_b_16_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im19_a0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im19_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im19_s1 : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im19_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im19_q : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im22_a0 : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im22_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im22_s1 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im22_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im22_q : STD_LOGIC_VECTOR (16 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_im22_q_18_q : STD_LOGIC_VECTOR (16 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_bs25_b_16_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im26_a0 : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im26_b0 : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im26_s1 : STD_LOGIC_VECTOR (14 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im26_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im26_q : STD_LOGIC_VECTOR (14 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im30_a0 : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im30_b0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im30_s1 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im30_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im30_q : STD_LOGIC_VECTOR (16 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_im30_q_18_q : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im34_a0 : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im34_b0 : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im34_s1 : STD_LOGIC_VECTOR (14 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im34_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im34_q : STD_LOGIC_VECTOR (14 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_im34_q_18_q : STD_LOGIC_VECTOR (14 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im38_a0 : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im38_b0 : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im38_s1 : STD_LOGIC_VECTOR (13 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_im38_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_im38_q : STD_LOGIC_VECTOR (13 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_align_48_q_19_q : STD_LOGIC_VECTOR (30 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_align_50_q_19_q : STD_LOGIC_VECTOR (38 downto 0);
    signal u0_m0_wo0_accum_p1_of_2_a : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_accum_p1_of_2_b : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_accum_p1_of_2_i : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_accum_p1_of_2_o : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_accum_p1_of_2_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p1_of_2_q : STD_LOGIC_VECTOR (36 downto 0);
    signal d_u0_m0_wo0_accum_p1_of_2_q_22_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_accum_p2_of_2_a : STD_LOGIC_VECTOR (26 downto 0);
    signal u0_m0_wo0_accum_p2_of_2_b : STD_LOGIC_VECTOR (26 downto 0);
    signal u0_m0_wo0_accum_p2_of_2_i : STD_LOGIC_VECTOR (26 downto 0);
    signal u0_m0_wo0_accum_p2_of_2_o : STD_LOGIC_VECTOR (26 downto 0);
    signal u0_m0_wo0_accum_p2_of_2_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p2_of_2_q : STD_LOGIC_VECTOR (24 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_a : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_b : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_o : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_a : STD_LOGIC_VECTOR (19 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_b : STD_LOGIC_VECTOR (19 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_o : STD_LOGIC_VECTOR (19 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_q : STD_LOGIC_VECTOR (17 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_a : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_b : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_o : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_a : STD_LOGIC_VECTOR (12 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_b : STD_LOGIC_VECTOR (12 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_o : STD_LOGIC_VECTOR (12 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_q : STD_LOGIC_VECTOR (10 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_a : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_b : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_o : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_a : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_b : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_o : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_q : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_a : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_b : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_o : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_a : STD_LOGIC_VECTOR (20 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_b : STD_LOGIC_VECTOR (20 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_o : STD_LOGIC_VECTOR (20 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_q : STD_LOGIC_VECTOR (18 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_a : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_b : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_o : STD_LOGIC_VECTOR (37 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_a : STD_LOGIC_VECTOR (21 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_b : STD_LOGIC_VECTOR (21 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_o : STD_LOGIC_VECTOR (21 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_q : STD_LOGIC_VECTOR (19 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c_18_q : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_run_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_oseq_gated_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_resize_in : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_resize_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs2_in : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs2_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs5_in : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs5_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs11_in : STD_LOGIC_VECTOR (23 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs25_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs36_in : STD_LOGIC_VECTOR (23 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs36_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs17_in : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs17_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs28_in : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs28_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs1_in : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs1_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs7_in : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs7_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs16_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs23_in : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs23_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs31_in : STD_LOGIC_VECTOR (15 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bs31_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_42_q : STD_LOGIC_VECTOR (23 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_42_qint : STD_LOGIC_VECTOR (23 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_44_q : STD_LOGIC_VECTOR (23 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_44_qint : STD_LOGIC_VECTOR (23 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_46_q : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_46_qint : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_48_q : STD_LOGIC_VECTOR (30 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_48_qint : STD_LOGIC_VECTOR (30 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_b : STD_LOGIC_VECTOR (12 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel0_1_b : STD_LOGIC_VECTOR (12 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_0_b : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_50_q : STD_LOGIC_VECTOR (38 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_50_qint : STD_LOGIC_VECTOR (38 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel0_2_b : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_0_b : STD_LOGIC_VECTOR (11 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel0_1_b : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_0_b : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel1_2_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_c_q : STD_LOGIC_VECTOR (17 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel1_0_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_0_b : STD_LOGIC_VECTOR (1 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_BitJoin_for_q_q : STD_LOGIC_VECTOR (61 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_c_q : STD_LOGIC_VECTOR (18 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_BitJoin_for_c_q : STD_LOGIC_VECTOR (18 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_BitJoin_for_c_q : STD_LOGIC_VECTOR (19 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_BitJoin_for_c_q : STD_LOGIC_VECTOR (19 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_tessel1_0_b : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bjB37_q : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bjB18_q : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bjB29_q : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bjA24_q : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_bjA32_q : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_b_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_b_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_b_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_b_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_c_q : STD_LOGIC_VECTOR (10 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel0_0_b : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_b_q : STD_LOGIC_VECTOR (36 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_c_q : STD_LOGIC_VECTOR (17 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_c_q : STD_LOGIC_VECTOR (10 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_c_q : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_BitJoin_for_c_q : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_c_q : STD_LOGIC_VECTOR (24 downto 0);

begin


    -- VCC(CONSTANT,1)@0
    VCC_q <= "1";

    -- u0_m0_wo0_run(ENABLEGENERATOR,13)@10 + 2
    u0_m0_wo0_run_ctrl <= u0_m0_wo0_run_out & xIn_v & u0_m0_wo0_run_enableQ;
    u0_m0_wo0_run_clkproc: PROCESS (clk, areset)
        variable u0_m0_wo0_run_enable_c : SIGNED(7 downto 0);
        variable u0_m0_wo0_run_inc : SIGNED(2 downto 0);
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_run_q <= "0";
            u0_m0_wo0_run_enable_c := TO_SIGNED(127, 8);
            u0_m0_wo0_run_enableQ <= "0";
            u0_m0_wo0_run_count <= "000";
            u0_m0_wo0_run_inc := (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_run_out = "1") THEN
                IF (u0_m0_wo0_run_enable_c(7) = '1') THEN
                    u0_m0_wo0_run_enable_c := u0_m0_wo0_run_enable_c - (-128);
                ELSE
                    u0_m0_wo0_run_enable_c := u0_m0_wo0_run_enable_c + (-1);
                END IF;
                u0_m0_wo0_run_enableQ <= STD_LOGIC_VECTOR(u0_m0_wo0_run_enable_c(7 downto 7));
            ELSE
                u0_m0_wo0_run_enableQ <= "0";
            END IF;
            CASE (u0_m0_wo0_run_ctrl) IS
                WHEN "000" | "001" => u0_m0_wo0_run_inc := "000";
                WHEN "010" | "011" => u0_m0_wo0_run_inc := "111";
                WHEN "100" => u0_m0_wo0_run_inc := "000";
                WHEN "101" => u0_m0_wo0_run_inc := "010";
                WHEN "110" => u0_m0_wo0_run_inc := "111";
                WHEN "111" => u0_m0_wo0_run_inc := "001";
                WHEN OTHERS => 
            END CASE;
            u0_m0_wo0_run_count <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_run_count) + SIGNED(u0_m0_wo0_run_inc));
            u0_m0_wo0_run_q <= u0_m0_wo0_run_out;
        END IF;
    END PROCESS;
    u0_m0_wo0_run_preEnaQ <= u0_m0_wo0_run_count(2 downto 2);
    u0_m0_wo0_run_out <= u0_m0_wo0_run_preEnaQ and VCC_q;

    -- u0_m0_wo0_memread(DELAY,14)@12
    u0_m0_wo0_memread : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_run_q, xout => u0_m0_wo0_memread_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_13(DELAY,298)@12 + 1
    d_u0_m0_wo0_memread_q_13 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_memread_q, xout => d_u0_m0_wo0_memread_q_13_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_compute(DELAY,16)@13
    u0_m0_wo0_compute : dspba_delay
    GENERIC MAP ( width => 1, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_13_q, xout => u0_m0_wo0_compute_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_19(DELAY,300)@13 + 6
    d_u0_m0_wo0_compute_q_19 : dspba_delay
    GENERIC MAP ( width => 1, depth => 6, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_compute_q, xout => d_u0_m0_wo0_compute_q_19_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_aseq(SEQUENCE,34)@19 + 1
    u0_m0_wo0_aseq_clkproc: PROCESS (clk, areset)
        variable u0_m0_wo0_aseq_c : SIGNED(9 downto 0);
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_aseq_c := "0000000000";
            u0_m0_wo0_aseq_q <= "0";
            u0_m0_wo0_aseq_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_19_q = "1") THEN
                IF (u0_m0_wo0_aseq_c = "0000000000") THEN
                    u0_m0_wo0_aseq_eq <= '1';
                ELSE
                    u0_m0_wo0_aseq_eq <= '0';
                END IF;
                IF (u0_m0_wo0_aseq_eq = '1') THEN
                    u0_m0_wo0_aseq_c := u0_m0_wo0_aseq_c + 128;
                ELSE
                    u0_m0_wo0_aseq_c := u0_m0_wo0_aseq_c - 1;
                END IF;
                u0_m0_wo0_aseq_q <= STD_LOGIC_VECTOR(u0_m0_wo0_aseq_c(9 downto 9));
            END IF;
        END IF;
    END PROCESS;

    -- d_u0_m0_wo0_compute_q_20(DELAY,301)@19 + 1
    d_u0_m0_wo0_compute_q_20 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_19_q, xout => d_u0_m0_wo0_compute_q_20_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_ca0(COUNTER,28)@13
    -- low=0, high=128, step=1, init=0
    u0_m0_wo0_ca0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_ca0_i <= TO_UNSIGNED(0, 8);
            u0_m0_wo0_ca0_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_compute_q = "1") THEN
                IF (u0_m0_wo0_ca0_i = TO_UNSIGNED(127, 8)) THEN
                    u0_m0_wo0_ca0_eq <= '1';
                ELSE
                    u0_m0_wo0_ca0_eq <= '0';
                END IF;
                IF (u0_m0_wo0_ca0_eq = '1') THEN
                    u0_m0_wo0_ca0_i <= u0_m0_wo0_ca0_i + 128;
                ELSE
                    u0_m0_wo0_ca0_i <= u0_m0_wo0_ca0_i + 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_ca0_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_ca0_i, 8)));

    -- u0_m0_wo0_cm0_lutmem(DUALMEM,45)@13 + 2
    u0_m0_wo0_cm0_lutmem_aa <= u0_m0_wo0_ca0_q;
    u0_m0_wo0_cm0_lutmem_reset0 <= areset;
    u0_m0_wo0_cm0_lutmem_dmem : altsyncram
    GENERIC MAP (
        ram_block_type => "M9K",
        operation_mode => "ROM",
        width_a => 22,
        widthad_a => 8,
        numwords_a => 129,
        lpm_type => "altsyncram",
        width_byteena_a => 1,
        outdata_reg_a => "CLOCK0",
        outdata_aclr_a => "CLEAR0",
        clock_enable_input_a => "NORMAL",
        power_up_uninitialized => "FALSE",
        init_file => "rx_ciccomp_rtl_core_u0_m0_wo0_cm0_lutmem.hex",
        init_file_layout => "PORT_A",
        intended_device_family => "Cyclone IV E"
    )
    PORT MAP (
        clocken0 => '1',
        aclr0 => u0_m0_wo0_cm0_lutmem_reset0,
        clock0 => clk,
        address_a => u0_m0_wo0_cm0_lutmem_aa,
        q_a => u0_m0_wo0_cm0_lutmem_ir
    );
    u0_m0_wo0_cm0_lutmem_r <= u0_m0_wo0_cm0_lutmem_ir(21 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_bs16(BITSELECT,62)@15
    u0_m0_wo0_mtree_mult1_0_bs16_b <= STD_LOGIC_VECTOR(u0_m0_wo0_cm0_lutmem_r(21 downto 16));

    -- d_u0_m0_wo0_mtree_mult1_0_bs16_b_16(DELAY,305)@15 + 1
    d_u0_m0_wo0_mtree_mult1_0_bs16_b_16 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_bs16_b, xout => d_u0_m0_wo0_mtree_mult1_0_bs16_b_16_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_wi0_r0_ra0_count1(COUNTER,23)@12
    -- low=0, high=128, step=1, init=0
    u0_m0_wo0_wi0_r0_ra0_count1_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count1_i <= TO_UNSIGNED(0, 8);
            u0_m0_wo0_wi0_r0_ra0_count1_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_memread_q = "1") THEN
                IF (u0_m0_wo0_wi0_r0_ra0_count1_i = TO_UNSIGNED(127, 8)) THEN
                    u0_m0_wo0_wi0_r0_ra0_count1_eq <= '1';
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count1_eq <= '0';
                END IF;
                IF (u0_m0_wo0_wi0_r0_ra0_count1_eq = '1') THEN
                    u0_m0_wo0_wi0_r0_ra0_count1_i <= u0_m0_wo0_wi0_r0_ra0_count1_i + 128;
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count1_i <= u0_m0_wo0_wi0_r0_ra0_count1_i + 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count1_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count1_i, 8)));

    -- u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem(DUALMEM,44)@12 + 2
    u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_aa <= u0_m0_wo0_wi0_r0_ra0_count1_q;
    u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_reset0 <= areset;
    u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_dmem : altsyncram
    GENERIC MAP (
        ram_block_type => "M9K",
        operation_mode => "ROM",
        width_a => 9,
        widthad_a => 8,
        numwords_a => 129,
        lpm_type => "altsyncram",
        width_byteena_a => 1,
        outdata_reg_a => "CLOCK0",
        outdata_aclr_a => "CLEAR0",
        clock_enable_input_a => "NORMAL",
        power_up_uninitialized => "FALSE",
        init_file => "rx_ciccomp_rtl_core_u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem.hex",
        init_file_layout => "PORT_A",
        intended_device_family => "Cyclone IV E"
    )
    PORT MAP (
        clocken0 => VCC_q(0),
        aclr0 => u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_reset0,
        clock0 => clk,
        address_a => u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_aa,
        q_a => u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_ir
    );
    u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_r <= u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_ir(8 downto 0);

    -- u0_m0_wo0_wi0_r0_ra0_count0_inner(COUNTER,19)@14
    -- low=-1, high=127, step=-1, init=127
    u0_m0_wo0_wi0_r0_ra0_count0_inner_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count0_inner_i <= TO_SIGNED(127, 8);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_memread_q_14_q = "1") THEN
                IF (u0_m0_wo0_wi0_r0_ra0_count0_inner_i(7 downto 7) = "1") THEN
                    u0_m0_wo0_wi0_r0_ra0_count0_inner_i <= u0_m0_wo0_wi0_r0_ra0_count0_inner_i - 128;
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count0_inner_i <= u0_m0_wo0_wi0_r0_ra0_count0_inner_i - 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count0_inner_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count0_inner_i, 8)));

    -- u0_m0_wo0_wi0_r0_ra0_count0_run(LOGICAL,20)@14
    u0_m0_wo0_wi0_r0_ra0_count0_run_q <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_count0_inner_q(7 downto 7));

    -- d_u0_m0_wo0_memread_q_14(DELAY,299)@13 + 1
    d_u0_m0_wo0_memread_q_14 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_13_q, xout => d_u0_m0_wo0_memread_q_14_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_wi0_r0_ra0_count0(COUNTER,21)@14
    -- low=0, high=255, step=2, init=0
    u0_m0_wo0_wi0_r0_ra0_count0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count0_i <= TO_UNSIGNED(0, 8);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_memread_q_14_q = "1" and u0_m0_wo0_wi0_r0_ra0_count0_run_q = "1") THEN
                u0_m0_wo0_wi0_r0_ra0_count0_i <= u0_m0_wo0_wi0_r0_ra0_count0_i + 2;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count0_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count0_i, 9)));

    -- u0_m0_wo0_wi0_r0_ra0_add_0_0(ADD,24)@14 + 1
    u0_m0_wo0_wi0_r0_ra0_add_0_0_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_wi0_r0_ra0_count0_q);
    u0_m0_wo0_wi0_r0_ra0_add_0_0_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_wi0_r0_ra0_count1_lut_lutmem_r);
    u0_m0_wo0_wi0_r0_ra0_add_0_0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_add_0_0_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_add_0_0_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_wi0_r0_ra0_add_0_0_a) + UNSIGNED(u0_m0_wo0_wi0_r0_ra0_add_0_0_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_add_0_0_q <= u0_m0_wo0_wi0_r0_ra0_add_0_0_o(9 downto 0);

    -- u0_m0_wo0_wi0_r0_ra0_resize(BITSELECT,25)@15
    u0_m0_wo0_wi0_r0_ra0_resize_in <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_add_0_0_q(7 downto 0));
    u0_m0_wo0_wi0_r0_ra0_resize_b <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_resize_in(7 downto 0));

    -- d_xIn_0_15(DELAY,296)@10 + 5
    d_xIn_0_15 : dspba_delay
    GENERIC MAP ( width => 32, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => xIn_0, xout => d_xIn_0_15_q, clk => clk, aclr => areset );

    -- d_in0_m0_wi0_wo0_assign_id1_q_15(DELAY,297)@10 + 5
    d_in0_m0_wi0_wo0_assign_id1_q_15 : dspba_delay
    GENERIC MAP ( width => 1, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => xIn_v, xout => d_in0_m0_wi0_wo0_assign_id1_q_15_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_wi0_r0_wa0(COUNTER,26)@15
    -- low=0, high=255, step=1, init=2
    u0_m0_wo0_wi0_r0_wa0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_wa0_i <= TO_UNSIGNED(2, 8);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_in0_m0_wi0_wo0_assign_id1_q_15_q = "1") THEN
                u0_m0_wo0_wi0_r0_wa0_i <= u0_m0_wo0_wi0_r0_wa0_i + 1;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_wa0_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_wa0_i, 8)));

    -- u0_m0_wo0_wi0_r0_memr0(DUALMEM,27)@15
    u0_m0_wo0_wi0_r0_memr0_ia <= STD_LOGIC_VECTOR(d_xIn_0_15_q);
    u0_m0_wo0_wi0_r0_memr0_aa <= u0_m0_wo0_wi0_r0_wa0_q;
    u0_m0_wo0_wi0_r0_memr0_ab <= u0_m0_wo0_wi0_r0_ra0_resize_b;
    u0_m0_wo0_wi0_r0_memr0_dmem : altsyncram
    GENERIC MAP (
        ram_block_type => "M9K",
        operation_mode => "DUAL_PORT",
        width_a => 32,
        widthad_a => 8,
        numwords_a => 256,
        width_b => 32,
        widthad_b => 8,
        numwords_b => 256,
        lpm_type => "altsyncram",
        width_byteena_a => 1,
        address_reg_b => "CLOCK0",
        indata_reg_b => "CLOCK0",
        wrcontrol_wraddress_reg_b => "CLOCK0",
        rdcontrol_reg_b => "CLOCK0",
        byteena_reg_b => "CLOCK0",
        outdata_reg_b => "CLOCK0",
        outdata_aclr_b => "NONE",
        clock_enable_input_a => "NORMAL",
        clock_enable_input_b => "NORMAL",
        clock_enable_output_b => "NORMAL",
        read_during_write_mode_mixed_ports => "DONT_CARE",
        power_up_uninitialized => "FALSE",
        init_file => "UNUSED",
        intended_device_family => "Cyclone IV E"
    )
    PORT MAP (
        clocken0 => '1',
        clock0 => clk,
        address_a => u0_m0_wo0_wi0_r0_memr0_aa,
        data_a => u0_m0_wo0_wi0_r0_memr0_ia,
        wren_a => d_in0_m0_wi0_wo0_assign_id1_q_15_q(0),
        address_b => u0_m0_wo0_wi0_r0_memr0_ab,
        q_b => u0_m0_wo0_wi0_r0_memr0_iq
    );
    u0_m0_wo0_wi0_r0_memr0_q <= u0_m0_wo0_wi0_r0_memr0_iq(31 downto 0);

    -- d_u0_m0_wo0_wi0_r0_memr0_q_16(DELAY,303)@15 + 1
    d_u0_m0_wo0_wi0_r0_memr0_q_16 : dspba_delay
    GENERIC MAP ( width => 32, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_wi0_r0_memr0_q, xout => d_u0_m0_wo0_wi0_r0_memr0_q_16_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_bs28(BITSELECT,74)@16
    u0_m0_wo0_mtree_mult1_0_bs28_in <= STD_LOGIC_VECTOR(d_u0_m0_wo0_wi0_r0_memr0_q_16_q(15 downto 0));
    u0_m0_wo0_mtree_mult1_0_bs28_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs28_in(15 downto 8));

    -- u0_m0_wo0_mtree_mult1_0_bjB29(BITJOIN,75)@16
    u0_m0_wo0_mtree_mult1_0_bjB29_q <= GND_q & u0_m0_wo0_mtree_mult1_0_bs28_b;

    -- u0_m0_wo0_mtree_mult1_0_im26(MULT,72)@16 + 2
    u0_m0_wo0_mtree_mult1_0_im26_a0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bjB29_q);
    u0_m0_wo0_mtree_mult1_0_im26_b0 <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_bs16_b_16_q);
    u0_m0_wo0_mtree_mult1_0_im26_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im26_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 9,
        lpm_widthb => 6,
        lpm_widthp => 15,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im26_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im26_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im26_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im26_s1
    );
    u0_m0_wo0_mtree_mult1_0_im26_q <= u0_m0_wo0_mtree_mult1_0_im26_s1;

    -- u0_m0_wo0_mtree_mult1_0_align_50(BITSHIFT,96)@18
    u0_m0_wo0_mtree_mult1_0_align_50_qint <= u0_m0_wo0_mtree_mult1_0_im26_q & "000000000000000000000000";
    u0_m0_wo0_mtree_mult1_0_align_50_q <= u0_m0_wo0_mtree_mult1_0_align_50_qint(38 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel0_0(BITSELECT,247)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel0_0_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_align_50_q(36 downto 0));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1(BITSELECT,236)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_align_48_q(30 downto 30));

    -- u0_m0_wo0_mtree_mult1_0_bs17(BITSELECT,63)@16
    u0_m0_wo0_mtree_mult1_0_bs17_in <= STD_LOGIC_VECTOR(d_u0_m0_wo0_wi0_r0_memr0_q_16_q(7 downto 0));
    u0_m0_wo0_mtree_mult1_0_bs17_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs17_in(7 downto 0));

    -- u0_m0_wo0_mtree_mult1_0_bjB18(BITJOIN,64)@16
    u0_m0_wo0_mtree_mult1_0_bjB18_q <= GND_q & u0_m0_wo0_mtree_mult1_0_bs17_b;

    -- u0_m0_wo0_mtree_mult1_0_im15(MULT,61)@16 + 2
    u0_m0_wo0_mtree_mult1_0_im15_a0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bjB18_q);
    u0_m0_wo0_mtree_mult1_0_im15_b0 <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_bs16_b_16_q);
    u0_m0_wo0_mtree_mult1_0_im15_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im15_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 9,
        lpm_widthb => 6,
        lpm_widthp => 15,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im15_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im15_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im15_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im15_s1
    );
    u0_m0_wo0_mtree_mult1_0_im15_q <= u0_m0_wo0_mtree_mult1_0_im15_s1;

    -- u0_m0_wo0_mtree_mult1_0_align_48(BITSHIFT,94)@18
    u0_m0_wo0_mtree_mult1_0_align_48_qint <= u0_m0_wo0_mtree_mult1_0_im15_q & "0000000000000000";
    u0_m0_wo0_mtree_mult1_0_align_48_q <= u0_m0_wo0_mtree_mult1_0_align_48_qint(30 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_b(BITJOIN,242)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_b_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_align_48_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2(ADD,145)@18 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_b_q);
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel0_0_b);
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_o(37);
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_o(36 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_bs36(BITSELECT,82)@15
    u0_m0_wo0_mtree_mult1_0_bs36_in <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_memr0_q(23 downto 0));
    u0_m0_wo0_mtree_mult1_0_bs36_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs36_in(23 downto 16));

    -- u0_m0_wo0_mtree_mult1_0_bjB37(BITJOIN,83)@15
    u0_m0_wo0_mtree_mult1_0_bjB37_q <= GND_q & u0_m0_wo0_mtree_mult1_0_bs36_b;

    -- u0_m0_wo0_mtree_mult1_0_im34(MULT,80)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im34_a0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bjB37_q);
    u0_m0_wo0_mtree_mult1_0_im34_b0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs16_b);
    u0_m0_wo0_mtree_mult1_0_im34_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im34_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 9,
        lpm_widthb => 6,
        lpm_widthp => 15,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im34_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im34_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im34_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im34_s1
    );
    u0_m0_wo0_mtree_mult1_0_im34_q <= u0_m0_wo0_mtree_mult1_0_im34_s1;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel0_1(BITSELECT,230)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel0_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_im34_q(4 downto 0));

    -- u0_m0_wo0_mtree_mult1_0_bs5(BITSELECT,51)@15
    u0_m0_wo0_mtree_mult1_0_bs5_in <= u0_m0_wo0_wi0_r0_memr0_q(15 downto 0);
    u0_m0_wo0_mtree_mult1_0_bs5_b <= u0_m0_wo0_mtree_mult1_0_bs5_in(15 downto 8);

    -- u0_m0_wo0_mtree_mult1_0_bs7(BITSELECT,53)@15
    u0_m0_wo0_mtree_mult1_0_bs7_in <= u0_m0_wo0_cm0_lutmem_r(15 downto 0);
    u0_m0_wo0_mtree_mult1_0_bs7_b <= u0_m0_wo0_mtree_mult1_0_bs7_in(15 downto 8);

    -- u0_m0_wo0_mtree_mult1_0_im12(MULT,58)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im12_a0 <= u0_m0_wo0_mtree_mult1_0_bs7_b;
    u0_m0_wo0_mtree_mult1_0_im12_b0 <= u0_m0_wo0_mtree_mult1_0_bs5_b;
    u0_m0_wo0_mtree_mult1_0_im12_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im12_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 8,
        lpm_widthb => 8,
        lpm_widthp => 16,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "UNSIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im12_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im12_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im12_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im12_s1
    );
    u0_m0_wo0_mtree_mult1_0_im12_q <= u0_m0_wo0_mtree_mult1_0_im12_s1;

    -- u0_m0_wo0_mtree_mult1_0_align_46(BITSHIFT,92)@17
    u0_m0_wo0_mtree_mult1_0_align_46_qint <= u0_m0_wo0_mtree_mult1_0_im12_q & "0000000000000000";
    u0_m0_wo0_mtree_mult1_0_align_46_q <= u0_m0_wo0_mtree_mult1_0_align_46_qint(31 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_b(BITJOIN,231)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_b_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_align_46_q;

    -- u0_m0_wo0_mtree_mult1_0_bs25(BITSELECT,71)@15
    u0_m0_wo0_mtree_mult1_0_bs25_b <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_memr0_q(31 downto 24));

    -- u0_m0_wo0_mtree_mult1_0_bs23(BITSELECT,69)@15
    u0_m0_wo0_mtree_mult1_0_bs23_in <= STD_LOGIC_VECTOR(u0_m0_wo0_cm0_lutmem_r(7 downto 0));
    u0_m0_wo0_mtree_mult1_0_bs23_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs23_in(7 downto 0));

    -- u0_m0_wo0_mtree_mult1_0_bjA24(BITJOIN,70)@15
    u0_m0_wo0_mtree_mult1_0_bjA24_q <= GND_q & u0_m0_wo0_mtree_mult1_0_bs23_b;

    -- u0_m0_wo0_mtree_mult1_0_im22(MULT,68)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im22_a0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bjA24_q);
    u0_m0_wo0_mtree_mult1_0_im22_b0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs25_b);
    u0_m0_wo0_mtree_mult1_0_im22_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im22_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 9,
        lpm_widthb => 8,
        lpm_widthp => 17,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im22_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im22_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im22_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im22_s1
    );
    u0_m0_wo0_mtree_mult1_0_im22_q <= u0_m0_wo0_mtree_mult1_0_im22_s1;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel0_1(BITSELECT,218)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel0_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_im22_q(12 downto 0));

    -- u0_m0_wo0_mtree_mult1_0_bs2(BITSELECT,48)@15
    u0_m0_wo0_mtree_mult1_0_bs2_in <= u0_m0_wo0_wi0_r0_memr0_q(7 downto 0);
    u0_m0_wo0_mtree_mult1_0_bs2_b <= u0_m0_wo0_mtree_mult1_0_bs2_in(7 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_im6(MULT,52)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im6_a0 <= u0_m0_wo0_mtree_mult1_0_bs7_b;
    u0_m0_wo0_mtree_mult1_0_im6_b0 <= u0_m0_wo0_mtree_mult1_0_bs2_b;
    u0_m0_wo0_mtree_mult1_0_im6_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im6_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 8,
        lpm_widthb => 8,
        lpm_widthp => 16,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "UNSIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im6_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im6_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im6_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im6_s1
    );
    u0_m0_wo0_mtree_mult1_0_im6_q <= u0_m0_wo0_mtree_mult1_0_im6_s1;

    -- u0_m0_wo0_mtree_mult1_0_align_44(BITSHIFT,90)@17
    u0_m0_wo0_mtree_mult1_0_align_44_qint <= u0_m0_wo0_mtree_mult1_0_im6_q & "00000000";
    u0_m0_wo0_mtree_mult1_0_align_44_q <= u0_m0_wo0_mtree_mult1_0_align_44_qint(23 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_b(BITJOIN,219)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_b_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel0_1_b & u0_m0_wo0_mtree_mult1_0_align_44_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2(ADD,134)@17 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_b_q);
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_b_q);
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_o(37);
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_o(36 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_bs11(BITSELECT,57)@15
    u0_m0_wo0_mtree_mult1_0_bs11_in <= u0_m0_wo0_wi0_r0_memr0_q(23 downto 0);
    u0_m0_wo0_mtree_mult1_0_bs11_b <= u0_m0_wo0_mtree_mult1_0_bs11_in(23 downto 16);

    -- u0_m0_wo0_mtree_mult1_0_im19(MULT,65)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im19_a0 <= u0_m0_wo0_mtree_mult1_0_bs7_b;
    u0_m0_wo0_mtree_mult1_0_im19_b0 <= u0_m0_wo0_mtree_mult1_0_bs11_b;
    u0_m0_wo0_mtree_mult1_0_im19_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im19_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 8,
        lpm_widthb => 8,
        lpm_widthp => 16,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "UNSIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im19_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im19_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im19_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im19_s1
    );
    u0_m0_wo0_mtree_mult1_0_im19_q <= u0_m0_wo0_mtree_mult1_0_im19_s1;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select(BITSELECT,295)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_im19_q(12 downto 0));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_im19_q(15 downto 13));

    -- u0_m0_wo0_mtree_mult1_0_bs1(BITSELECT,47)@15
    u0_m0_wo0_mtree_mult1_0_bs1_in <= u0_m0_wo0_cm0_lutmem_r(7 downto 0);
    u0_m0_wo0_mtree_mult1_0_bs1_b <= u0_m0_wo0_mtree_mult1_0_bs1_in(7 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_im3(MULT,49)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im3_a0 <= u0_m0_wo0_mtree_mult1_0_bs1_b;
    u0_m0_wo0_mtree_mult1_0_im3_b0 <= u0_m0_wo0_mtree_mult1_0_bs5_b;
    u0_m0_wo0_mtree_mult1_0_im3_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im3_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 8,
        lpm_widthb => 8,
        lpm_widthp => 16,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "UNSIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im3_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im3_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im3_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im3_s1
    );
    u0_m0_wo0_mtree_mult1_0_im3_q <= u0_m0_wo0_mtree_mult1_0_im3_s1;

    -- u0_m0_wo0_mtree_mult1_0_align_42(BITSHIFT,88)@17
    u0_m0_wo0_mtree_mult1_0_align_42_qint <= u0_m0_wo0_mtree_mult1_0_im3_q & "00000000";
    u0_m0_wo0_mtree_mult1_0_align_42_q <= u0_m0_wo0_mtree_mult1_0_align_42_qint(23 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_b(BITJOIN,212)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_b_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_b & u0_m0_wo0_mtree_mult1_0_align_42_q;

    -- u0_m0_wo0_mtree_mult1_0_bs31(BITSELECT,77)@15
    u0_m0_wo0_mtree_mult1_0_bs31_in <= STD_LOGIC_VECTOR(u0_m0_wo0_cm0_lutmem_r(15 downto 0));
    u0_m0_wo0_mtree_mult1_0_bs31_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs31_in(15 downto 8));

    -- u0_m0_wo0_mtree_mult1_0_bjA32(BITJOIN,78)@15
    u0_m0_wo0_mtree_mult1_0_bjA32_q <= GND_q & u0_m0_wo0_mtree_mult1_0_bs31_b;

    -- u0_m0_wo0_mtree_mult1_0_im30(MULT,76)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im30_a0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bjA32_q);
    u0_m0_wo0_mtree_mult1_0_im30_b0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_bs25_b);
    u0_m0_wo0_mtree_mult1_0_im30_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im30_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 9,
        lpm_widthb => 8,
        lpm_widthp => 17,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im30_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im30_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im30_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im30_s1
    );
    u0_m0_wo0_mtree_mult1_0_im30_q <= u0_m0_wo0_mtree_mult1_0_im30_s1;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel0_2(BITSELECT,200)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel0_2_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_im30_q(4 downto 0));

    -- u0_m0_wo0_mtree_mult1_0_im9(MULT,55)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im9_a0 <= u0_m0_wo0_mtree_mult1_0_bs1_b;
    u0_m0_wo0_mtree_mult1_0_im9_b0 <= u0_m0_wo0_mtree_mult1_0_bs11_b;
    u0_m0_wo0_mtree_mult1_0_im9_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im9_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 8,
        lpm_widthb => 8,
        lpm_widthp => 16,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "UNSIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im9_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im9_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im9_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im9_s1
    );
    u0_m0_wo0_mtree_mult1_0_im9_q <= u0_m0_wo0_mtree_mult1_0_im9_s1;

    -- u0_m0_wo0_mtree_mult1_0_im0(MULT,46)@15 + 2
    u0_m0_wo0_mtree_mult1_0_im0_a0 <= u0_m0_wo0_mtree_mult1_0_bs1_b;
    u0_m0_wo0_mtree_mult1_0_im0_b0 <= u0_m0_wo0_mtree_mult1_0_bs2_b;
    u0_m0_wo0_mtree_mult1_0_im0_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im0_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 8,
        lpm_widthb => 8,
        lpm_widthp => 16,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "UNSIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im0_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im0_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im0_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im0_s1
    );
    u0_m0_wo0_mtree_mult1_0_im0_q <= u0_m0_wo0_mtree_mult1_0_im0_s1;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_b(BITJOIN,201)@17
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_b_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel0_2_b & u0_m0_wo0_mtree_mult1_0_im9_q & u0_m0_wo0_mtree_mult1_0_im0_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2(ADD,123)@17 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_b_q);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_b_q);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_o(37);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_o(36 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2(ADD,156)@18 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_q);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_q);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_o(37);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_o(36 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2(ADD,167)@19 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_q);
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_q);
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_o(37);
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_o(36 downto 0);

    -- u0_m0_wo0_accum_p1_of_2(ADD,112)@20 + 1
    u0_m0_wo0_accum_p1_of_2_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_q);
    u0_m0_wo0_accum_p1_of_2_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_accum_p1_of_2_q);
    u0_m0_wo0_accum_p1_of_2_i <= u0_m0_wo0_accum_p1_of_2_a;
    u0_m0_wo0_accum_p1_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p1_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_20_q = "1") THEN
                IF (u0_m0_wo0_aseq_q = "1") THEN
                    u0_m0_wo0_accum_p1_of_2_o <= u0_m0_wo0_accum_p1_of_2_i;
                ELSE
                    u0_m0_wo0_accum_p1_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p1_of_2_a) + UNSIGNED(u0_m0_wo0_accum_p1_of_2_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p1_of_2_c(0) <= u0_m0_wo0_accum_p1_of_2_o(37);
    u0_m0_wo0_accum_p1_of_2_q <= u0_m0_wo0_accum_p1_of_2_o(36 downto 0);

    -- d_u0_m0_wo0_aseq_q_21(DELAY,304)@20 + 1
    d_u0_m0_wo0_aseq_q_21 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_aseq_q, xout => d_u0_m0_wo0_aseq_q_21_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_21(DELAY,302)@20 + 1
    d_u0_m0_wo0_compute_q_21 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_20_q, xout => d_u0_m0_wo0_compute_q_21_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1(BITSELECT,277)@20
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_q(2 downto 2));

    -- d_u0_m0_wo0_mtree_mult1_0_align_50_q_19(DELAY,311)@18 + 1
    d_u0_m0_wo0_mtree_mult1_0_align_50_q_19 : dspba_delay
    GENERIC MAP ( width => 39, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_align_50_q, xout => d_u0_m0_wo0_mtree_mult1_0_align_50_q_19_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_1(BITSELECT,250)@19
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_1_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_align_50_q_19_q(38 downto 38));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_0(BITSELECT,249)@19
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_0_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_align_50_q_19_q(38 downto 37));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_BitJoin_for_c(BITJOIN,251)@19
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_tessel1_0_b;

    -- d_u0_m0_wo0_mtree_mult1_0_align_48_q_19(DELAY,310)@18 + 1
    d_u0_m0_wo0_mtree_mult1_0_align_48_q_19 : dspba_delay
    GENERIC MAP ( width => 31, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_align_48_q, xout => d_u0_m0_wo0_mtree_mult1_0_align_48_q_19_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel1_0(BITSELECT,243)@19
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel1_0_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_align_48_q_19_q(30 downto 30));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_c(BITJOIN,246)@19
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel1_0_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel1_0_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_tessel1_0_b;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2(ADD,146)@19 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_2_p1_of_2_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((3 downto 3 => u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_c_q(2)) & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_a_BitJoin_for_c_q) & '1');
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((3 downto 3 => u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_BitJoin_for_c_q(2)) & u0_m0_wo0_mtree_mult1_0_result_add_0_2_BitSelect_for_b_BitJoin_for_c_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_cin(0));
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_a) + SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_o(3 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_BitJoin_for_c(BITJOIN,294)@20
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_2_p2_of_2_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_tessel1_1(BITSELECT,272)@20
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_tessel1_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_q(18 downto 18));

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1(BITSELECT,260)@19
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_q(10 downto 10));

    -- d_u0_m0_wo0_mtree_mult1_0_im34_q_18(DELAY,309)@17 + 1
    d_u0_m0_wo0_mtree_mult1_0_im34_q_18 : dspba_delay
    GENERIC MAP ( width => 15, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_im34_q, xout => d_u0_m0_wo0_mtree_mult1_0_im34_q_18_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_1(BITSELECT,233)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_1_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_im34_q_18_q(14 downto 14));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_0(BITSELECT,232)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_0_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_im34_q_18_q(14 downto 5));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_c(BITJOIN,234)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_tessel1_0_b;

    -- d_u0_m0_wo0_mtree_mult1_0_im22_q_18(DELAY,306)@17 + 1
    d_u0_m0_wo0_mtree_mult1_0_im22_q_18 : dspba_delay
    GENERIC MAP ( width => 17, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_im22_q, xout => d_u0_m0_wo0_mtree_mult1_0_im22_q_18_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1(BITSELECT,221)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_im22_q_18_q(16 downto 16));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_0(BITSELECT,220)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_0_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_im22_q_18_q(16 downto 13));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_c(BITJOIN,228)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_tessel1_0_b;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2(ADD,135)@18 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_p1_of_2_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((11 downto 11 => u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_c_q(10)) & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_a_BitJoin_for_c_q) & '1');
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((11 downto 11 => u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_c_q(10)) & u0_m0_wo0_mtree_mult1_0_result_add_0_1_BitSelect_for_b_BitJoin_for_c_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_cin(0));
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_a) + SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_o(11 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_BitJoin_for_c(BITJOIN,268)@19
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_1_p2_of_2_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel1_1(BITSELECT,255)@19
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel1_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_q(17 downto 17));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel1_2(BITSELECT,215)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel1_2_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_im38_q(13 downto 13));

    -- d_u0_m0_wo0_mtree_mult1_0_bs25_b_16(DELAY,307)@15 + 1
    d_u0_m0_wo0_mtree_mult1_0_bs25_b_16 : dspba_delay
    GENERIC MAP ( width => 8, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_bs25_b, xout => d_u0_m0_wo0_mtree_mult1_0_bs25_b_16_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_im38(MULT,84)@16 + 2
    u0_m0_wo0_mtree_mult1_0_im38_a0 <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_bs25_b_16_q);
    u0_m0_wo0_mtree_mult1_0_im38_b0 <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_bs16_b_16_q);
    u0_m0_wo0_mtree_mult1_0_im38_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_im38_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 8,
        lpm_widthb => 6,
        lpm_widthp => 14,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_im38_a0,
        datab => u0_m0_wo0_mtree_mult1_0_im38_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_im38_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_im38_s1
    );
    u0_m0_wo0_mtree_mult1_0_im38_q <= u0_m0_wo0_mtree_mult1_0_im38_s1;

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c_18(DELAY,313)@17 + 1
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c_18 : dspba_delay
    GENERIC MAP ( width => 3, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c_18_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_c(BITJOIN,216)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel1_2_b & u0_m0_wo0_mtree_mult1_0_im38_q & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_tessel0_1_merged_bit_select_c_18_q;

    -- d_u0_m0_wo0_mtree_mult1_0_im30_q_18(DELAY,308)@17 + 1
    d_u0_m0_wo0_mtree_mult1_0_im30_q_18 : dspba_delay
    GENERIC MAP ( width => 17, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_im30_q, xout => d_u0_m0_wo0_mtree_mult1_0_im30_q_18_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1(BITSELECT,203)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_im30_q_18_q(16 downto 16));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_0(BITSELECT,202)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_0_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_im30_q_18_q(16 downto 5));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_c(BITJOIN,209)@18
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel1_0_b;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2(ADD,124)@18 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_2_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((18 downto 18 => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_c_q(17)) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_BitJoin_for_c_q) & '1');
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((18 downto 18 => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_c_q(17)) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_BitJoin_for_c_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_cin(0));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_a) + SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_o(18 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_c(BITJOIN,256)@19
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_2_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2(ADD,157)@19 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_2_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((19 downto 19 => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_c_q(18)) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_c_q) & '1');
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((19 downto 19 => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_BitJoin_for_c_q(18)) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_BitJoin_for_c_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_cin(0));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_a) + SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_o(19 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_BitJoin_for_c(BITJOIN,273)@20
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_BitJoin_for_c_q <= u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_2_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2(ADD,168)@20 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_cin <= u0_m0_wo0_mtree_mult1_0_result_add_2_0_p1_of_2_c;
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((20 downto 20 => u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_BitJoin_for_c_q(19)) & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_a_BitJoin_for_c_q) & '1');
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((20 downto 20 => u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_BitJoin_for_c_q(19)) & u0_m0_wo0_mtree_mult1_0_result_add_2_0_BitSelect_for_b_BitJoin_for_c_q) & u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_cin(0));
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_a) + SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_q <= u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_o(20 downto 1);

    -- u0_m0_wo0_accum_BitSelect_for_a_tessel1_1(BITSELECT,173)@21
    u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_q(16 downto 16));

    -- u0_m0_wo0_accum_BitSelect_for_a_tessel1_0(BITSELECT,172)@21
    u0_m0_wo0_accum_BitSelect_for_a_tessel1_0_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_2_0_p2_of_2_q(16 downto 0));

    -- u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_c(BITJOIN,181)@21
    u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_c_q <= u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel1_0_b;

    -- u0_m0_wo0_accum_p2_of_2(ADD,113)@21 + 1
    u0_m0_wo0_accum_p2_of_2_cin <= u0_m0_wo0_accum_p1_of_2_c;
    u0_m0_wo0_accum_p2_of_2_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((25 downto 25 => u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_c_q(24)) & u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_c_q) & '1');
    u0_m0_wo0_accum_p2_of_2_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((25 downto 25 => u0_m0_wo0_accum_p2_of_2_q(24)) & u0_m0_wo0_accum_p2_of_2_q) & u0_m0_wo0_accum_p2_of_2_cin(0));
    u0_m0_wo0_accum_p2_of_2_i <= u0_m0_wo0_accum_p2_of_2_a;
    u0_m0_wo0_accum_p2_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p2_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_21_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_21_q = "1") THEN
                    u0_m0_wo0_accum_p2_of_2_o <= u0_m0_wo0_accum_p2_of_2_i;
                ELSE
                    u0_m0_wo0_accum_p2_of_2_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_accum_p2_of_2_a) + SIGNED(u0_m0_wo0_accum_p2_of_2_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p2_of_2_q <= u0_m0_wo0_accum_p2_of_2_o(25 downto 1);

    -- d_u0_m0_wo0_accum_p1_of_2_q_22(DELAY,312)@21 + 1
    d_u0_m0_wo0_accum_p1_of_2_q_22 : dspba_delay
    GENERIC MAP ( width => 37, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p1_of_2_q, xout => d_u0_m0_wo0_accum_p1_of_2_q_22_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_accum_BitJoin_for_q(BITJOIN,114)@22
    u0_m0_wo0_accum_BitJoin_for_q_q <= u0_m0_wo0_accum_p2_of_2_q & d_u0_m0_wo0_accum_p1_of_2_q_22_q;

    -- GND(CONSTANT,0)@0
    GND_q <= "0";

    -- u0_m0_wo0_oseq(SEQUENCE,36)@20 + 1
    u0_m0_wo0_oseq_clkproc: PROCESS (clk, areset)
        variable u0_m0_wo0_oseq_c : SIGNED(9 downto 0);
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_oseq_c := "0010000000";
            u0_m0_wo0_oseq_q <= "0";
            u0_m0_wo0_oseq_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_20_q = "1") THEN
                IF (u0_m0_wo0_oseq_c = "0000000000") THEN
                    u0_m0_wo0_oseq_eq <= '1';
                ELSE
                    u0_m0_wo0_oseq_eq <= '0';
                END IF;
                IF (u0_m0_wo0_oseq_eq = '1') THEN
                    u0_m0_wo0_oseq_c := u0_m0_wo0_oseq_c + 128;
                ELSE
                    u0_m0_wo0_oseq_c := u0_m0_wo0_oseq_c - 1;
                END IF;
                u0_m0_wo0_oseq_q <= STD_LOGIC_VECTOR(u0_m0_wo0_oseq_c(9 downto 9));
            END IF;
        END IF;
    END PROCESS;

    -- u0_m0_wo0_oseq_gated(LOGICAL,37)@21
    u0_m0_wo0_oseq_gated_q <= u0_m0_wo0_oseq_q and d_u0_m0_wo0_compute_q_21_q;

    -- u0_m0_wo0_oseq_gated_reg(REG,38)@21 + 1
    u0_m0_wo0_oseq_gated_reg_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_oseq_gated_reg_q <= "0";
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_oseq_gated_reg_q <= STD_LOGIC_VECTOR(u0_m0_wo0_oseq_gated_q);
        END IF;
    END PROCESS;

    -- xOut(PORTOUT,43)@22 + 1
    xOut_v <= u0_m0_wo0_oseq_gated_reg_q;
    xOut_c <= STD_LOGIC_VECTOR("0000000" & GND_q);
    xOut_0 <= u0_m0_wo0_accum_BitJoin_for_q_q;

END normal;

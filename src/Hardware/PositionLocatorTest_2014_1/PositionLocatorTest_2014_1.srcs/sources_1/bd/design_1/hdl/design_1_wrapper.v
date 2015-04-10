//Copyright 1986-2014 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2014.1 (win64) Build 881834 Fri Apr  4 14:15:54 MDT 2014
//Date        : Tue Mar 03 16:22:25 2015
//Host        : Latitude-E5530 running 64-bit Service Pack 1  (build 7601)
//Command     : generate_target design_1_wrapper.bd
//Design      : design_1_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module design_1_wrapper
   (button_down,
    clock_rtl,
    din,
    full,
    reset_rtl,
    reset_rtl_0,
    wr_clk,
    wr_en);
  input button_down;
  input clock_rtl;
  input [16:0]din;
  output full;
  input reset_rtl;
  input reset_rtl_0;
  input wr_clk;
  input wr_en;

  wire button_down;
  wire clock_rtl;
  wire [16:0]din;
  wire full;
  wire reset_rtl;
  wire reset_rtl_0;
  wire wr_clk;
  wire wr_en;

design_1 design_1_i
       (.button_down(button_down),
        .clock_rtl(clock_rtl),
        .din(din),
        .full(full),
        .reset_rtl(reset_rtl),
        .reset_rtl_0(reset_rtl_0),
        .wr_clk(wr_clk),
        .wr_en(wr_en));
endmodule

// (c) Copyright 1995-2015 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.


// IP VLNV: xilinx.com:ip:pmod_input2:1.0
// IP Revision: 2

(* X_CORE_INFO = "pmod_input2,Vivado 2014.1" *)
(* CHECK_LICENSE_TYPE = "design_1_pmod_input2_0_0,pmod_input2,{}" *)
(* CORE_GENERATION_INFO = "design_1_pmod_input2_0_0,pmod_input2,{x_ipProduct=Vivado 2014.1,x_ipVendor=xilinx.com,x_ipLibrary=ip,x_ipName=pmod_input2,x_ipVersion=1.0,x_ipCoreRevision=2,x_ipLanguage=VERILOG}" *)
(* DowngradeIPIdentifiedWarnings = "yes" *)
module design_1_pmod_input2_0_0 (
  CLK100MHZ,
  OV7670_VSYNC,
  OV7670_HREF,
  OV7670_PCLK,
  OV7670_XCLK,
  OV7670_SIOC,
  OV7670_SIOD,
  OV7670_D,
  LED,
  BTNC,
  pwdn,
  reset,
  fifo_we,
  fifo_data,
  fifo_full
);

input wire CLK100MHZ;
input wire OV7670_VSYNC;
input wire OV7670_HREF;
input wire OV7670_PCLK;
output wire OV7670_XCLK;
output wire OV7670_SIOC;
inout wire OV7670_SIOD;
input wire [7 : 0] OV7670_D;
output wire [3 : 0] LED;
input wire BTNC;
output wire pwdn;
output wire reset;
output wire fifo_we;
output wire [16 : 0] fifo_data;
input wire fifo_full;

  pmod_input2 inst (
    .CLK100MHZ(CLK100MHZ),
    .OV7670_VSYNC(OV7670_VSYNC),
    .OV7670_HREF(OV7670_HREF),
    .OV7670_PCLK(OV7670_PCLK),
    .OV7670_XCLK(OV7670_XCLK),
    .OV7670_SIOC(OV7670_SIOC),
    .OV7670_SIOD(OV7670_SIOD),
    .OV7670_D(OV7670_D),
    .LED(LED),
    .BTNC(BTNC),
    .pwdn(pwdn),
    .reset(reset),
    .fifo_we(fifo_we),
    .fifo_data(fifo_data),
    .fifo_full(fifo_full)
  );
endmodule

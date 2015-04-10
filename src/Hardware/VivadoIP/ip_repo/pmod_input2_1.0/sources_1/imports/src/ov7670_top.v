`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2014/05/23 16:24:31
// Design Name: 
// Module Name: ov7725_top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module pmod_input2(
input  CLK100MHZ,
input  OV7670_VSYNC,
input  OV7670_HREF,
input  OV7670_PCLK,
output OV7670_XCLK,
output OV7670_SIOC,
inout  OV7670_SIOD,
input [7:0] OV7670_D,

output[3:0] LED,

input BTNC,
output pwdn,
output reset,

output fifo_we,
output [16:0] fifo_data,
input fifo_full
);
wire  capture_we;  
wire  config_finished;  
wire  clk25; 
wire  clk50;     
wire  resend;        
wire [15:0] frame_pixel;  
wire [15:0]  data_16;
  
assign pwdn = 0;
assign reset = 1;
  

assign LED = {3'b0,config_finished};  // LED0 indicates camera configuration is done
assign  	OV7670_XCLK = clk25;

// The button (BTNC) is used to resend the configuration bits to the camera.
// The button is debounced with a 50 MHz clock
debounce   btn_debounce(
		.clk(clk50),
		.i(BTNC),
		.o(resend)
);

// Capture the pixel data from the camera
 ov7670_capture capture(
 		.pclk  (OV7670_PCLK),
 		.vsync (OV7670_VSYNC),
 		.href  (OV7670_HREF),
 		.d     ( OV7670_D),
 		.addr  (),
 		.dout( data_16),
 		.we   (capture_we),
 		.bram_we()
 	);
 
// Sned the pixel data to the fifo
//  On a new pixel, write the pixel to the fifo
//  On a vsync, write 17'h10000 to the fifo
//  TO FIX: on vsync, we should only be high for one clock cycle. This might be raised for many...
reg prev_vsync;
always @(posedge OV7670_PCLK) begin
    prev_vsync <= OV7670_VSYNC;
end

assign fifo_we = (~fifo_full) && (capture_we || (OV7670_VSYNC && ~prev_vsync));
assign fifo_data = OV7670_VSYNC ? 17'h10000 : {1'b0, data_16};
 
 
// Configure teh camera
I2C_AV_Config IIC(
 		.iCLK   ( clk25),    
 		.iRST_N (! resend),    
 		.Config_Done ( config_finished),
 		.I2C_SDAT  ( OV7670_SIOD),    
 		.I2C_SCLK  ( OV7670_SIOC),
 		.LUT_INDEX (),
 		.I2C_RDATA ()
 		); 


// Derive two clocks for the board provided 100 MHz clock.
// Generated using clock wizard in IP Catalog
   
clk_wiz_0 u_clock
   (
   // Clock in ports
    .clk_in1(CLK100MHZ),      // input clk_in1
    // Clock out ports
    .clk_out1(clk50),     // output clk_out1
    .clk_out2(clk25));    // output clk_out2


endmodule

`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2014/05/23 15:11:30
// Design Name: 
// Module Name: ov7725_capture
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


module ov7670_capture(
input pclk,
input vsync,
input href,
input[7:0] d,
output[16:0] addr,
output reg[15:0] dout,
output reg we,
output reg bram_we
    );
    reg [15:0] d_latch;
    reg [18:0] address;
    reg [18:0] address_next;
    reg [16:0] bram_address;
    reg [16:0] bram_address_next;  
    reg [1:0] wr_hold;    
    reg [1:0] cnt;
    reg [9:0] x_pos;
    reg [8:0] y_pos;
    
    parameter X_MAX = 640;
    parameter Y_MAX = 480;
    parameter ADDR_MAX = X_MAX * Y_MAX;
    
    initial d_latch = 16'b0;
    initial address = 19'b0;
    initial address_next = 19'b0;
    initial wr_hold = 2'b0;   
    initial cnt = 2'b0;
    initial x_pos = 10'b0;
    initial y_pos = 10'b0;
    initial bram_address = 17'b0;
    initial bram_address_next = 17'b0;
            
    //assign addr =    addresss
    assign addr = bram_address;

    always @(posedge pclk) begin 
        if( vsync ==1) begin
            address <=17'b0;
            address_next <= 17'b0;
            wr_hold <=  2'b0;
            cnt <=  2'b0;
            x_pos <= 0;
            y_pos <= 0;
            bram_address <= 0;
            bram_address_next <= 0;
        end else begin
            if(address<ADDR_MAX)  // Check if at end of frame buffer
                address <= address_next;
            else
                 address <= ADDR_MAX;
                 
            bram_address <= bram_address_next;
	        // Get 1 byte from camera each cycle.  Have to get two bytes to form a pixel.
	        // wr_hold is used to generate the write enable every other cycle.
	        // No changes until href = 1 indicating valid data
            we      <= wr_hold[1];  // Set to 1 one cycle after dout is updated
            wr_hold <= {wr_hold[0] , (href &&( ! wr_hold[0])) };
            d_latch <= {d_latch[7:0] , d};
            
            if (wr_hold[1] == 1) begin  // increment write address and output new pixel
                address_next <=address_next+1;
                if (x_pos >= (X_MAX-1)) begin
                    x_pos <= 0;
                    y_pos <= y_pos + 1;
                end else begin
                    x_pos <= x_pos + 1;
                end
                if ( (x_pos[0] == 0) && (y_pos[0] == 0) ) begin     // copy only odd pixels to bram
                    bram_address_next <= bram_address_next + 1;
                    bram_we <= 1'b1;
                end else begin
                    bram_we <= 1'b0;
                end
                dout[15:0]  <= {d_latch[15:11] , d_latch[10:5] , d_latch[4:0] };
            end
        end
 end

endmodule

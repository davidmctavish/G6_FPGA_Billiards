`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/18/2015 09:38:39 PM
// Design Name: 
// Module Name: tb
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


module tb(

    );
    
    
    reg clk_100, clk_50;
    reg reset, start_flag;
    initial begin
        clk_100 = 0;
        clk_50 = 0;
        start_flag = 0;
        reset = 1;
        # 30 reset = 0;
        #400 start_flag = 1;
    end
    always begin
        #5 clk_100 = ~clk_100;
    end
    always begin
        #10 clk_50 = ~clk_50;
    end
    
    // Simulate FIFO
    genvar i;
    parameter TOTAL_PIXELS =  3*10 + 2*(320*5 + 1);
    wire [16:0] data [0:TOTAL_PIXELS-1];
    // Simple frames
    assign data[0] = 17'h00f00;
    assign data[1] = 17'h00f00;
    assign data[2] = 17'h00f00;
    assign data[3] = 17'h00f00;
    assign data[4] = 17'h00f00;
    assign data[5] = 17'h00f00;
    assign data[6] = 17'h00f00;
    assign data[7] = 17'h00f00;
    assign data[8] = 17'h00f00;
    assign data[9] = 17'h10000;
    //
    assign data[10] = 17'h000f0;
    assign data[11] = 17'h000f0;
    assign data[12] = 17'h000f0;
    assign data[13] = 17'h000f0;
    assign data[14] = 17'h000f0;
    assign data[15] = 17'h000f0;
    assign data[16] = 17'h000f0;
    assign data[17] = 17'h000f0;
    assign data[18] = 17'h000f0;
    assign data[19] = 17'h10000;
    //
    assign data[20] = 17'h0000f;
    assign data[21] = 17'h0000f;
    assign data[22] = 17'h0000f;
    assign data[23] = 17'h0000f;
    assign data[24] = 17'h0000f;
    assign data[25] = 17'h0000f;
    assign data[26] = 17'h0000f;
    assign data[27] = 17'h0000f;
    assign data[28] = 17'h0000f;
    assign data[29] = 17'h10000;
    //
    // 5 line frames
    // frame 1
    generate
    localparam StartIndex1 = 3*(9+1);
    for (i=0; i<320*5; i=i+1) begin
        localparam x_pos1 = i % 320;
        localparam y_pos1 = i / 320;
        // Red dot centered at (5,1.5)
        if ( (x_pos1 >= 4) && (x_pos1 <= 6) && ((y_pos1 == 1) || (y_pos1 == 2) || (y_pos1 == 4)) ) begin
            assign data[StartIndex1 + i] = 17'h00f00;
        end else begin
            assign data[StartIndex1 + i] = 17'h000ff;
        end
    end
    assign data[StartIndex1 + 320*5] = 17'h10000;
    endgenerate
    // frame 2
    generate
    localparam StartIndex2 = StartIndex1 + 320*5+1;
    for (i=0; i<320*5; i=i+1) begin
        localparam x_pos2 = i % 320;
        localparam y_pos2 = i / 320;
        // large red dot centered at (300,3)
        if ( (x_pos2 >= 290) && (x_pos2 <= 310) && (y_pos2 >= 2) && (y_pos2 <= 4) ) begin
            assign data[StartIndex2 + i] = 17'h00f00;
        end else begin
            assign data[StartIndex2 + i] = 17'h000ff;
        end
    end
    assign data[StartIndex2 + 320*5] = 17'h10000;
    endgenerate
    
    
    reg [31:0] count;
    initial begin
        count = 0;
    end
    wire full;
    reg wr_en;
    reg [16:0] fifo_data_in;
    always @(*) begin
        fifo_data_in = data[count];
        wr_en = ((~full) && (count <= TOTAL_PIXELS) && start_flag);
    end
    always @(posedge clk_50) begin
        if (wr_en && ~reset) begin
            count <= count + 1;
        end
    end

    
    design_1_wrapper dut(
        .button_down(1'b1),
        .clock_rtl(clk_100),
        .din(fifo_data_in),
        .full(full),
        .reset_rtl(reset),
        .reset_rtl_0(~reset),
        .wr_clk(clk_50),
        .wr_en(wr_en)
    );
    
    
endmodule

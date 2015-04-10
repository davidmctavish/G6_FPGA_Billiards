`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/06/2015 11:46:55 PM
// Design Name: 
// Module Name: AveragingFilter_Main
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


module AveragingFilter_Main(
    input clk,  // 100MHz
    input reset,    // active high
    input [31:0] tolerance_config,
    // FIFO Signals
    input [16:0] data_in,
    input empty,
    output read,
    // Signals to be written to AXI registers
    output reg [15:0] x_out,
    output reg [14:0] y_out,
    output new_data_available
    );
    
    // Looking for a red dot
    //parameter RED_MAX = 4'hf;
    //parameter RED_MIN = 4'hc;
    //parameter GREEN_MAX = 4'h7;
    //parameter GREEN_MIN = 4'h0;
    //parameter BLUE_MAX = 4'h7;
    //parameter BLUE_MIN = 4'h0;
    wire [3:0] RED_MAX = tolerance_config[23:20];
    wire [3:0] RED_MIN = tolerance_config[19:16];
    wire [3:0] GREEN_MAX = tolerance_config[15:12];
    wire [3:0] GREEN_MIN = tolerance_config[11:8];
    wire [3:0] BLUE_MAX = tolerance_config[7:4];
    wire [3:0] BLUE_MIN = tolerance_config[3:0];
    
    //parameter MAX_X = 640;
    //parameter MAX_Y = 480;
    parameter MAX_X = 320;
    parameter MAX_Y = 240;
    
    wire [16:0] VSYNC_CODE = 17'h10000;    // This data will only come in on a vsync.
                                    //  All actualy data will have the highest
                                    //  bit as 0.
    reg [16:0] current_pixel;
    
    reg [9:0] current_x;
    reg [8:0] current_y;
    reg [31:0] accumulated_x;
    reg [31:0] accumulated_y;
    reg [23:0] number_of_points_counted;
    
    
// STATE MACHINE    ---------------------------------------------------------
    // Definition of states
    parameter WAITING = 0, COMPUTING = 1, CREATING_OUTPUT = 2, RESETING =  3, OUTPUT_READY = 4;
    reg [2:0] current_state, next_state;
    
    reg [31:0] division_count;   // need to pipeline division
    parameter PIPELINE_STAGES = 26; // comes from the latenct parameter of the divider block (div_gen_0)
    wire divider_data_valid_x, divider_data_valid_y;
    
    // State Transisition
    always @(posedge clk) begin
        if (reset) begin
            current_state <= WAITING;
        end else begin
            current_state <= next_state;
        end
    end
    
    // Next State Logic
    always @(*) begin
        case(current_state)
            WAITING: begin
                if (~empty) begin
                    next_state = COMPUTING;
                end else begin
                    next_state = WAITING;
                end
            end
            COMPUTING: begin
                if (current_pixel == VSYNC_CODE) begin
                    next_state = CREATING_OUTPUT;
                end else if (~empty) begin
                    next_state = COMPUTING;
                end else begin
                    next_state = WAITING;
                end
            end
            CREATING_OUTPUT: begin
                if ( (division_count >= PIPELINE_STAGES) && divider_data_valid_x && divider_data_valid_y ) begin
                    next_state = OUTPUT_READY;  // transition when divider is finished
                end else begin
                    next_state = CREATING_OUTPUT;
                end
            end
            OUTPUT_READY: begin
                next_state = RESETING;
            end
            RESETING: begin
                next_state = WAITING;
            end
            default: begin
                next_state = RESETING;
            end
        endcase
    end
    
    // Output Parameters controlling datapath
    wire reset_all = (current_state == RESETING) || reset;
    wire enable_increment = (current_state == COMPUTING);
    wire enable_accumulation = (current_state == COMPUTING);
    wire enable_output = (current_state == CREATING_OUTPUT);
    // Signal to FIFO
    assign read = (next_state == COMPUTING);    // note: if the next state is computing, then it is already known that the fifo is not empty
    // Output parameter to AXI slave register
    assign new_data_available = (current_state == OUTPUT_READY);
    
    // NOTE: currently assuming FIFO will return value in one cycle if it is not empty.
    //  If this is not the case, may have to add an extra state that waits for the acknowledge
    //  before moving to the COMPUTING state.
    
// DATAPATH ----------------------------------------------------------    
    
    // Current Pixel Reegister
    always @(posedge clk) begin
        if (reset_all) begin
            current_pixel <= 0;
        end else if (read) begin
            current_pixel <= data_in;    // get next pixel from FIFO
        end
    end
    
    // Counter for current pixel location
    //  Note: these registster increment before the value is checked.
    //      Therefore, the first pixel read is (1,1), not (0,0).
    always @(posedge clk) begin
        if (reset_all) begin
            current_x <= 0;
            current_y <= 0;
        end else if (enable_increment) begin
            if (current_x >= (MAX_X-1)) begin
                current_x <= 0;
                current_y <= current_y + 1;
            end else begin
                current_x <= current_x + 1;
            end
        end
    end
    
    
    // Check Tolerance Values
    wire [3:0] current_pixel_red = current_pixel[11:8];
    wire [3:0] current_pixel_green = current_pixel[7:4]; 
    wire [3:0] current_pixel_blue = current_pixel[3:0];
    wire is_within_range =  (current_pixel_red <= RED_MAX)      && (current_pixel_red >= RED_MIN) && 
                            (current_pixel_green <= GREEN_MAX)  && (current_pixel_green >= GREEN_MIN) && 
                            (current_pixel_blue <= BLUE_MAX)    && (current_pixel_blue >= BLUE_MIN);
                            
    // If the pixel is in range, accumulate the (x,y) position

    always @(posedge clk) begin
        if (reset_all) begin
            accumulated_x <= 0;
            accumulated_y <= 0;
            number_of_points_counted <= 0;
        end else if (enable_accumulation && is_within_range) begin
            accumulated_x <= accumulated_x + current_x;
            accumulated_y <= accumulated_y + current_y;
            number_of_points_counted <= number_of_points_counted + 1;
        end
    end

    //reg [31:0] accumulated_x;
    //reg [31:0] accumulated_y;
    //reg [23:0] number_of_points_counted;
    // Use Xilinx divider to block to create final output
    //div_gen_0 is a 29 bit (dividend) / 19 bit (divisor) divider, but the actual port sizes are 32, 24, 32)
    wire [31+8:0] divider_x_out;
    div_gen_0 divider_x(
         .s_axis_divisor_tdata({number_of_points_counted}),
         .s_axis_divisor_tready(),      // the ready signals are not checked and assumed to be okay
         .s_axis_divisor_tvalid(1'b1),
         .s_axis_dividend_tdata({accumulated_x}),
         .s_axis_dividend_tready(),
         .s_axis_dividend_tvalid(1'b1),
         .aclk(clk),
         .m_axis_dout_tdata(divider_x_out),
         .m_axis_dout_tvalid(divider_data_valid_x)  // the data is assumed to be valid after PIPELINE_STAGES, not checked
     );
     wire [31+8:0] divider_y_out;
     div_gen_0 divider_y(
         .s_axis_divisor_tdata({number_of_points_counted}),
         .s_axis_divisor_tready(),      // the ready signals are not checked and assumed to be okay
         .s_axis_divisor_tvalid(1'b1),
         .s_axis_dividend_tdata({accumulated_y}),
         .s_axis_dividend_tready(),
         .s_axis_dividend_tvalid(1'b1),
         .aclk(clk),
         .m_axis_dout_tdata(divider_y_out),
         .m_axis_dout_tvalid(divider_data_valid_y)  // the data is assumed to be valid after PIPELINE_STAGES, not checked
     );
         
    // Create Output Values
    always @(posedge clk) begin
        if (reset_all) begin
            x_out <= 0;
            y_out <= 0;
        end else if (enable_output && (division_count >= PIPELINE_STAGES)) begin
            if (number_of_points_counted != 0) begin
                x_out <= divider_x_out[15:0]; 
                y_out <= divider_y_out[14:0];
            end else begin
                // if no points were counted, print out an imposible number (off the screen)
                x_out <= 16'hffff;
                y_out <= 15'h7fff;
            end
        end
    end

    
    // increment pipelining counter of division
    always @(posedge clk) begin
        if (reset_all) begin
            division_count <= 0;
        end else if (enable_output) begin
            division_count <= division_count+1;
        end
    end
    
endmodule

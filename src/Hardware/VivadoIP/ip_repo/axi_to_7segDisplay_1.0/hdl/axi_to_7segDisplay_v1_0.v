
`timescale 1 ns / 1 ps

	module axi_to_7segDisplay_v1_0 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4
	)
	(
		// Users to add ports here
        output reg [7:0] an,
        output reg [6:0] seg,
		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready
	);
	
	
	// More User Logic
	wire [31:0] num;
    //assign num = 32'h12345678;
	// End User Logic
	
// Instantiation of Axi Bus Interface S00_AXI
	axi_to_7segDisplay_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) axi_to_7segDisplay_v1_0_S00_AXI_inst (
	    .num(num),
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready)
	);

	// Add user logic here
	
	// Add a counter and mux and stuff
	// need to create a slower clock
	reg [15:0] slow_clk_reg;
	wire slow_clk;
	always @(posedge s00_axi_aclk) begin
	    if (~s00_axi_aresetn) begin
           slow_clk_reg <= 0;
        end else begin
           if (slow_clk_reg == 16'hffff) begin
               slow_clk_reg <= 0;
           end else begin
               slow_clk_reg <= slow_clk_reg + 1;
           end
        end    
   end
   assign slow_clk = slow_clk_reg[15];
	
	reg [2:0] count;
	always @(posedge slow_clk) begin
	   if (~s00_axi_aresetn) begin
	       count <= 0;
	   end else begin
	       if (count == 3'b111) begin
	           count <= 0;
	       end else begin
	           count <= count + 1;
	       end
	   end    
	end
	
	reg [3:0] cur_num;
	always @(*) begin         
        case (count)
           0: begin
              an = 8'b11111110;
              cur_num = num[3:0];
              end
           1: begin
              an = 8'b11111101;
              cur_num = num[7:4];
              end
           2: begin
              an = 8'b11111011;
              cur_num = num[11:8];
              end
           3: begin
              an = 8'b11110111;
              cur_num = num[15:12];
              end
           4: begin
              an = 8'b11101111;
              cur_num = num[19:16];
              end
           5: begin
              an = 8'b11011111;
              cur_num = num[23:20];
              end
           6: begin
              an = 8'b10111111;
              cur_num = num[27:24];
              end
           7: begin
              an = 8'b01111111;
              cur_num = num[31:28];
              end
    endcase

	end
	
	always @(*) begin
	   case(cur_num)
	          0:
                 seg <= 7'b1000000;
              1:
                 seg <= 7'b1111001;
              2:
                 seg <= 7'b0100100;
              3:
                 seg <= 7'b0110000;
              4:
                 seg <= 7'b0011001;
              5:
                 seg <= 7'b0010010;
              6:
                 seg <= 7'b0000010;
              7:
                 seg <= 7'b1111000;
              8:
                 seg <= 7'b0000000;
              9:
                 seg <= 7'b0010000;
             10:
                 seg <= 7'b0001000;
             11:
                 seg <= 7'b0000011;
             12:
                 seg <= 7'b1000110;
             13:
                 seg <= 7'b0100001;
             14:
                 seg <= 7'b0000110;
             15:
                 seg <= 7'b0001110;
       endcase

	end
	
    //assign an = 8'b11111110;
    //assign seg = 7'b0000000;
	// User logic ends

	endmodule

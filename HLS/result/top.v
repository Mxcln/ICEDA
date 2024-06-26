`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/05/23 23:03:30
// Design Name: 
// Module Name: top
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


module top(
input sys_clk,
input sys_rst_n,
input ram_init,
input [32-1:0] n,
input [32-1:0] a_ram_in,
input [32-1:0] b_ram_in,
input [16-1:0] a_addr_in,
input [16-1:0] b_addr_in,
output [32-1:0] result,
output done_flag
    );

//wire define
wire sys_clk_n;
wire [32-1:0] a_in;
wire a_rd_en;
wire [16-1:0] a_addr;
wire [32-1:0] b_in;
wire b_rd_en;
wire [16-1:0] b_addr;
wire wea;
wire web;
wire [32-1:0] dina;
wire [32-1:0] dinb;
wire [16-1:0] addra;
wire [16-1:0] addrb;

assign sys_clk_n = ~sys_clk;
assign wea = ram_init || !a_rd_en;
assign web = ram_init || !b_rd_en;
assign dina = ram_init ? a_ram_in : 0;
assign dinb = ram_init ? b_ram_in : 0;
assign addra = ram_init ? a_addr_in : a_addr;
assign addrb = ram_init ? b_addr_in : b_addr;

dotprod dotprod_0(
	.clk(sys_clk),
	.rst(sys_rst_n),
	// sram a
	.a_addr(a_addr),
  .a_in(a_in),
	.a_rd_en(a_rd_en),
	// sram b
	.b_addr(b_addr),
  .b_in(b_in),,
	.b_rd_en(b_rd_en),
	.n(n),
	.result(result),
	.done_flag(done_flag)
);


blk_mem_gen_0 ram_a(
  .clka(sys_clk_n),    // input wire clka
  .ena(1),      // input wire ena
  .wea(wea),      // input wire [0 : 0] wea
  .addra(addra),  // input wire [15 : 0] addra
  .dina(dina),    // input wire [31 : 0] dina
  .douta(a_in)  // output wire [31 : 0] douta
);

blk_mem_gen_0 ram_b(
  .clka(sys_clk_n),    // input wire clka
  .ena(1),      // input wire ena
  .wea(web),      // input wire [0 : 0] web
  .addra(addrb),  // input wire [15 : 0] addrb
  .dina(dinb),    // input wire [31 : 0] dinb
  .douta(b_in)  // output wire [31 : 0] doutb
);
endmodule

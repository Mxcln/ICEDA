`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/05/23 23:24:02
// Design Name: 
// Module Name: tb_top
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


module tb_top(

    );
reg sys_clk = 0;
reg sys_rst_n = 1;
reg ram_init = 1;
reg [32-1:0] n = 10;
reg [32-1:0] a_ram_in = 0;
reg [32-1:0] b_ram_in = 0;
reg [16-1:0] a_addr_in = 0;
reg [16-1:0] b_addr_in = 0;
wire [32-1:0] result;
wire done_flag;



top top_0(
    .sys_clk(sys_clk),
    .sys_rst_n(sys_rst_n),
    .ram_init(ram_init),
    .n(n),
    .a_ram_in(a_ram_in),
    .b_ram_in(b_ram_in),
    .a_addr_in(a_addr_in),
    .b_addr_in(b_addr_in),
    .result(result),
    .done_flag(done_flag)
);

parameter PERIOD  = 10;
initial
begin
    sys_clk = 0;
    forever #(PERIOD/2)  sys_clk=!sys_clk;
end

initial
begin
    #(PERIOD*15) ram_init = 0;
    #(PERIOD*5) sys_rst_n = 0;
    #(PERIOD*10) sys_rst_n = 1;
end

// for 循环 改变 a_ram_in 和 b_ram_in 的值
integer i;
initial begin
    for(i=0; i<10; i=i+1) begin
        a_ram_in = i;
        b_ram_in = i;
        a_addr_in = i;
        b_addr_in = i;
        #(PERIOD*1);
    end
end

endmodule

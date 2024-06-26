module dotprod (
    input wire clk,
    input wire rst_n,
    output wire [31:0] a_addr,
    input wire [31:0] a_in,
    output wire a_rd_en,
    output wire [31:0] b_addr,
    input wire [31:0] b_in,
    output wire b_rd_en,
    input wire n,
    output wire [31:0] result,
    output wire done_flag
);
    reg [31:0] reg_0;
    reg [31:0] reg_1;
    reg [31:0] reg_2;
    reg [31:0] reg_3;
    reg [31:0] reg_4;
    reg [31:0] reg_5;
    reg [31:0] reg_6;
    reg [2:0] state;
    reg [2:0] prev_state;
    reg [2:0] block_counter;

    reg   done_flag_r;
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= 0;
        prev_state <= 0;
        block_counter <= 0;
        reg_0 <= 0;
        reg_1 <= 0;
        reg_2 <= 0;
        reg_3 <= 0;
        reg_4 <= 0;
        reg_5 <= 0;
        reg_6 <= 0;
    end
    else begin
        case(state)
            0: begin
                case(block_counter)
                    0: begin
                        reg_0 <= 0;
                    end
                endcase
            end
            1: begin
                case(block_counter)
                    2: begin
                            prev_state <= state;
                            block_counter <= 0;
                            state <= reg_5 ? 3 : 2;
                    end
                    1: begin
                    end
                    0: begin
                        case(prev_state)
                            2: begin
                                reg_2 <= reg_4;
                            end
                            0: begin
                                reg_2 <= 0;
                            end
                        endcase
                        case(prev_state)
                            2: begin
                                reg_1 <= reg_3;
                            end
                            0: begin
                                reg_1 <= reg_0;
                            end
                        endcase
                    end
                endcase
            end
            2: begin
                case(block_counter)
                    4: begin
                            prev_state <= state;
                            block_counter <= 0;
                            state <= 1;
                    end
                    3: begin
                        reg_3 <= reg_1 + reg_6;
                    end
                    2: begin
                        reg_6 <= reg_5 * reg_6;
                    end
                    0: begin
                        reg_5 <= a_in
                        reg_6 <= b_in
                        reg_4 <= reg_2 + 1;
                    end
                endcase
            end
            3: begin
                case(block_counter)
                    0: begin
                        done_flag_r <= 1;
                    end
                endcase
            end
            default: begin
        state <= 0;
        prev_state <= 0;
        block_counter <= 0;
        reg_0 <= 0;
        reg_1 <= 0;
        reg_2 <= 0;
        reg_3 <= 0;
        reg_4 <= 0;
        reg_5 <= 0;
        reg_6 <= 0;
            end
        endcase
    end
end
  assign b_addr = reg_2;
  assign b_rd_en = 1;
  assign a_addr = reg_2;
  assign a_rd_en = 1;
  assign result = reg_1;
  assign done_flag = done_flag_r;
endmodule

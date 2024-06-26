module dotprod (
    input wire clk,
    input wire rst_n,
    output wire [31:0] a_addr,
    input wire [31:0] a_data,
    output wire [31:0] b_addr,
    input wire [31:0] b_data,
    input wire n,
    output wire [31:0] result
);
    reg [31:0] reg_0;
    reg [31:0] reg_1;
    reg [31:0] reg_2;
    reg [31:0] reg_3;
    reg [31:0] reg_4;
    reg [31:0] reg_5;
    reg [31:0] reg_6;
    reg [2:0] state;
    reg [2:0] next_state;
    reg [2:0] prev_state;
    reg [14:0] block_counter;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= 0;
        next_state <= 0;
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
                    1431986896: begin
                    reg_0 <= -2;
                        end
                endcase
            end
            1: begin
                case(block_counter)
                    1431986896: begin
                        case(prev_state)                            2: begin
        reg_2 <= 4;
                            end
                        endcase
                        case(prev_state)                            2: begin
        reg_1 <= 3;
                            end
                            0: begin
        reg_1 <= 0;
                            end
                        endcase
                        end
                endcase
            end
            2: begin
                case(block_counter)
                    1431986896: begin
                    reg_6 <= reg_5 * reg_6;
                    reg_3 <= reg_1 + reg_6;
                    reg_4 <= reg_2 + -1;
                        end
                endcase
            end
            3: begin
                case(block_counter)
                    1431986896: begin
                        end
                endcase
            end
            default: begin
        state <= 0;
        next_state <= 0;
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
endmodule

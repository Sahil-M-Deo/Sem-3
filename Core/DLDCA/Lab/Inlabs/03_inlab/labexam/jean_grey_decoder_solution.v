// ================================================================
// Module 1: triple_decoder (combinational)
// Derived from Karnaugh maps:
//   V = ~A&B | ~B&C
//   X = B&C | A&~B
//   Y = B&~C | A&~B
// ================================================================
module triple_decoder (
    input  [2:0] triple,
    output       valid,
    output [1:0] pair
);
    wire A = triple[2];
    wire B = triple[1];
    wire C = triple[0];

    assign valid   = (~A & B) | (~B & C);
    assign pair[1] = (B & C)  | (A & ~B);
    assign pair[0] = (B & ~C) | (A & ~B);
endmodule


// ================================================================
// Module 2: secret_accumulator (sequential)
// XORs incoming triple into secret register when capture is high.
// ================================================================
module secret_accumulator (
    input            clk,
    input            rst,
    input  [2:0]     triple,
    input            capture,
    output reg [2:0] secret
);
    always @(posedge clk or posedge rst) begin
        if (rst)          secret <= 3'b000;
        else if (capture) secret <= secret ^ triple;
    end
endmodule


// ================================================================
// Module 3: jean_grey_decoder (top-level FSM)
// ================================================================
module jean_grey_decoder (
    input            clk,
    input            rst,
    input            start,
    input  [3:0]     count,
    input  [2:0]     triple,
    output reg [1:0] pair,
    output reg       valid_out,
    output     [2:0] secret,
    output reg       secret_out,
    output reg       done
);
    localparam IDLE      = 2'b00;
    localparam READ      = 2'b01;
    localparam INTERCEPT = 2'b10;
    localparam DONE      = 2'b11;

    reg [1:0] state;
    reg [3:0] remaining;
    wire       capture;
    
    assign capture = (state == INTERCEPT);

    wire       is_triple_valid;
    wire [1:0] triple_pair;

    triple_decoder td (
        .triple (triple),
        .valid  (is_triple_valid),
        .pair   (triple_pair)
    );

    secret_accumulator sa (
        .clk     (clk),
        .rst     (rst),
        .triple  (triple),
        .capture (capture),
        .secret  (secret)
    );

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state      <= IDLE;
            remaining  <= 4'b0000;
            pair       <= 2'b00;
            valid_out  <= 1'b0;
            done       <= 1'b0;
            secret_out <= 1'b0;
        end else begin
            case (state)

                IDLE: begin
                    done       <= 1'b0;
                    secret_out <= 1'b0;
                    valid_out  <= 1'b0;
                    pair       <= 2'b00;
                    if (start) begin
                        remaining <= count;
                        state     <= READ;
                    end
                end

                READ: begin
                    secret_out <= 1'b0;
                    if (is_triple_valid) begin
                        pair      <= triple_pair;
                        valid_out <= 1'b1;
                        remaining <= remaining - 1;
                        if (remaining == 4'b0001)
                            state <= DONE;
                    end else begin
                        pair      <= 2'b00;
                        valid_out <= 1'b0;
                        state     <= INTERCEPT;
                    end
                end

                INTERCEPT: begin
                    secret_out <= 1'b1;
                    pair       <= 2'b00;
                    valid_out  <= 1'b0;
                    state      <= READ;
                end

                DONE: begin
                    done      <= 1'b1;
                    valid_out <= 1'b0;
                    pair      <= 2'b00;
                    state     <= IDLE;
                end

            endcase
        end
    end
endmodule

// ================================================================
// Module 1: triple_decoder (combinational)
// ================================================================
module triple_decoder (
    input [2:0] triple,
    output valid,
    output [1:0] pair
);
    wire A = triple[2];
    wire B = triple[1];
    wire C = triple[0];

    assign valid =   /* V from your K-map */;
    assign pair[1] = /* X from your K-map */;
    assign pair[0] = /* Y from your K-map */;
endmodule


// ================================================================
// Module 2: secret_accumulator (sequential) -- DO NOT MODIFY
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
    /*reg/wire       capture;*/

    triple_decoder td ( /* connect ports */ );
    secret_accumulator sa ( /* connect ports */ );

    // ! Please note carefully that you set all relevant outputs correctly
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
                    pair       <= 2'b00;
                    done       <= 1'b0;
                    secret_out <= 1'b0;
                    valid_out  <= 1'b0;
                    if (start) begin
                        remaining <= count;
                        state     <= READ;
                    end
                end

                READ: begin
                    /* Your Code Here */
                end

                INTERCEPT: begin
                    /* Your Code Here */
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

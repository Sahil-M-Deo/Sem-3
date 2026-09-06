// Module 1: crc8_update (combinational)
// Given the current CRC register and one incoming bit,
// compute the next CRC value. One iteration of the CRC loop.
module crc8_update (
    input  [7:0] crc_in,
    input        data_bit,
    output [7:0] crc_out
);
    wire feedback;
    assign feedback = crc_in[7] ^ data_bit;
    assign crc_out  = feedback ? ({crc_in[6:0], 1'b0} ^ 8'hD5)
                               :  {crc_in[6:0], 1'b0};
endmodule


// Module 2: crc8_serial (sequential)
// Accepts serial bits one per clock via data_bit.
// data_valid indicates that data_bit should be consumed.
// data_last indicates that data_bit is the final bit.
// crc_valid pulses when the CRC has been updated with the final bit.
module crc8_serial (
    input        clk,
    input        rst,
    input        data_bit,
    input        data_valid,
    input        data_last,
    output [7:0] crc,
    output       crc_valid
);
    reg  [7:0] crc_reg;
    reg        crc_valid_reg;
    wire [7:0] crc_next;

    crc8_update u_update (
        .crc_in   (crc_reg),
        .data_bit (data_bit),
        .crc_out  (crc_next)
    );

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            crc_reg       <= 8'hFF;
            crc_valid_reg <= 1'b0;
        end else begin
            crc_valid_reg <= 1'b0;

            if (data_valid) begin
                crc_reg <= crc_next;

                if (data_last)
                    crc_valid_reg <= 1'b1;
            end
        end
    end

    assign crc       = crc_reg;
    assign crc_valid = crc_valid_reg;

endmodule

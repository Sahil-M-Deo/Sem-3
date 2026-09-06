// ================================================================
// Part A: CRC-8 (fixed width=8, polynomial=0xD5)
// ================================================================

module crc8_update (
    input  [7:0] crc_in,
    input        data_bit,
    output [7:0] crc_out
);

    // TODO

endmodule


module crc8_serial (
    input        clk,
    input        rst,
    input        data_bit,
    input        data_valid,
    input        data_last,
    output [7:0] crc,
    output       crc_valid
);

    // TODO
    // NOTE: You need to latch output from crc8_update separately
    // And keep a latch for storing output `crc` separately

endmodule


// ================================================================
// Part B: Parametric CRC (general WIDTH and POLY)
// ================================================================

module crc_update #(
    parameter             WIDTH = 8,
    parameter [WIDTH-1:0] POLY = 8'hD5
) (
    input  [WIDTH-1:0] crc_in,
    input              data_bit,
    output [WIDTH-1:0] crc_out
);

    // TODO

endmodule


module crc_serial #(
    parameter             WIDTH = 8,
    parameter [WIDTH-1:0] POLY = 8'hD5,
    parameter [WIDTH-1:0] INIT = {WIDTH{1'b1}}
) (
    input                  clk,
    input                  rst,
    input                  data_bit,
    input                  data_valid,
    input                  data_last,
    output [WIDTH-1:0]     crc,
    output                 crc_valid
);

    // TODO

endmodule


// ================================================================
// Part C: Parallel CRC (DATA_WIDTH bits per clock)
// ================================================================

module crc_parallel #(
    parameter                    WIDTH      = 8,
    parameter [WIDTH-1:0]        POLY       = 8'hD5,
    parameter                    DATA_WIDTH = 8
) (
    input  [WIDTH-1:0] crc_in,
    input  [DATA_WIDTH-1:0] data,
    output [WIDTH-1:0] crc_out
);

    // array of registers
    wire [WIDTH-1:0] crc_stage [0:DATA_WIDTH];

    assign /* TODO */   = crc_in;
    assign crc_out      = /* TODO */;

    // TODO: write genvar for loop to loop over DATA_WIDTH times and generate hardware for parallel CRC
    genvar i;
    generate
        // TODO
    endgenerate


endmodule


module crc_parallel_serial #(
    parameter                    WIDTH      = 8,
    parameter [WIDTH-1:0]        POLY       = 8'hD5,
    parameter [WIDTH-1:0]        INIT       = {WIDTH{1'b1}},
    parameter                    DATA_WIDTH = 8
) (
    input                     clk,
    input                     rst,
    input  [DATA_WIDTH-1:0]  data,
    input                     data_valid,
    input                     data_last,
    output [WIDTH-1:0]        crc,
    output                    crc_valid
);

    // TODO

endmodule
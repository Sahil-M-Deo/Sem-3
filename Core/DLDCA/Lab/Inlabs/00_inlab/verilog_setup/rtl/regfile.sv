//==============================================================================
// File    : regfile.sv
// Project : Simple Processor
//
// 8 × 8-bit Register File
// - Two asynchronous read ports
// - One synchronous write port
//==============================================================================

`timescale 1ns / 1ps
`default_nettype none

`include "common.svh"

module regfile #(
    parameter int XLEN      = `XLEN,
    parameter int REG_COUNT = `REG_COUNT,
    parameter int ADDR_W    = `REG_ADDR_W
)(
    input  logic              clk_i,
    input  logic              rst_i,

    input  logic              we_i,
    input  logic [ADDR_W-1:0] rd_addr_i,
    input  logic [XLEN-1:0]   rd_data_i,

    input  logic [ADDR_W-1:0] rs1_addr_i,
    input  logic [ADDR_W-1:0] rs2_addr_i,

    output logic [XLEN-1:0]   rs1_data_o,
    output logic [XLEN-1:0]   rs2_data_o
);

    //--------------------------------------------------------------------------
    // Register Storage
    //--------------------------------------------------------------------------

    logic [XLEN-1:0] regs [0:REG_COUNT-1];

    //--------------------------------------------------------------------------
    // Synchronous Reset / Write
    //--------------------------------------------------------------------------

    always_ff @(posedge clk_i) begin
        if (rst_i) begin
            for (int i = 0; i < REG_COUNT; i++)
                regs[i] <= '0;
        end
        else if (we_i) begin
            regs[rd_addr_i] <= rd_data_i;
        end
    end

    //--------------------------------------------------------------------------
    // Asynchronous Read Ports
    //--------------------------------------------------------------------------

    assign rs1_data_o = regs[rs1_addr_i];
    assign rs2_data_o = regs[rs2_addr_i];

`ifdef DEBUG
    always @(posedge clk_i) begin
        if (we_i && !rst_i) begin
            $strobe("[%0t] RF: R%0d <= 0x%02h",
                     $time, rd_addr_i, rd_data_i);
        end
    end
`endif

endmodule

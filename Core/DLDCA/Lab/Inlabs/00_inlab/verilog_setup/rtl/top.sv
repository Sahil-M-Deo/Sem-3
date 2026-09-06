//==============================================================================
// File    : top.sv
// Project : Simple Processor
//
// Datapath:
//   Register File --> ALU --> Register File
//
// One ALU operation is executed whenever write_enable is asserted.
//==============================================================================

`timescale 1ns / 1ps
`default_nettype none

`include "common.svh"

module top #(
    parameter int XLEN   = `XLEN,
    parameter int ADDR_W = `REG_ADDR_W
)(
    input  logic              clk_i,
    input  logic              rst_i,

    input  logic [ADDR_W-1:0] rs1_addr_i,
    input  logic [ADDR_W-1:0] rs2_addr_i,
    input  logic [ADDR_W-1:0] rd_addr_i,

    input  alu_op_t           alu_op_i,

    input  logic              write_enable_i,

    input  logic              external_write_i,
    input  logic [XLEN-1:0]   external_data_i,

    output logic [XLEN-1:0]   rs1_data_o,
    output logic [XLEN-1:0]   rs2_data_o,
    output logic [XLEN-1:0]   result_o,
    output logic              zero_o
);

    //-------------------------------------------------------------------------
    // Internal Signals
    //-------------------------------------------------------------------------

    logic [XLEN-1:0] alu_result;
    logic [XLEN-1:0] write_data;

    //-------------------------------------------------------------------------
    // Register File
    //-------------------------------------------------------------------------

    assign write_data = external_write_i ? external_data_i : alu_result;

    regfile #(
        .XLEN(XLEN)
    ) u_regfile (
        .clk_i      (clk_i),
        .rst_i      (rst_i),

        .we_i       (write_enable_i),
        .rd_addr_i  (rd_addr_i),
        .rd_data_i  (write_data),

        .rs1_addr_i (rs1_addr_i),
        .rs2_addr_i (rs2_addr_i),

        .rs1_data_o (rs1_data_o),
        .rs2_data_o (rs2_data_o)
    );

    //-------------------------------------------------------------------------
    // ALU
    //-------------------------------------------------------------------------

    alu #(
        .XLEN(XLEN)
    ) u_alu (
        .lhs_i    (rs1_data_o),
        .rhs_i    (rs2_data_o),
        .op_i     (alu_op_i),

        .result_o (alu_result),
        .zero_o   (zero_o)
    );

    //-------------------------------------------------------------------------
    // Outputs
    //-------------------------------------------------------------------------

    assign result_o = alu_result;

`ifdef DEBUG
    always @(posedge clk_i) begin
        if (write_enable_i) begin
            $strobe(
                "[%0t] R%0d(%0d) OP=%0d R%0d(%0d) -> R%0d = %0d",
                $time,
                rs1_addr_i, rs1_data_o,
                alu_op_i,
                rs2_addr_i, rs2_data_o,
                rd_addr_i, alu_result
            );
        end
    end
`endif

endmodule
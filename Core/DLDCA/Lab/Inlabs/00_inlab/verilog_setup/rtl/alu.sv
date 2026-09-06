//==============================================================================
// File    : alu.sv
// Project : Simple Processor
//
// 8-bit Arithmetic Logic Unit
//==============================================================================

`timescale 1ns / 1ps
`default_nettype none

`include "common.svh"

module alu #(
    parameter int XLEN = `XLEN
)(
    input  logic [XLEN-1:0] lhs_i,
    input  logic [XLEN-1:0] rhs_i,
    input  alu_op_t         op_i,

    output logic [XLEN-1:0] result_o,
    output logic            zero_o
);

    //--------------------------------------------------------------------------
    // Combinational ALU
    //--------------------------------------------------------------------------

    always_comb begin
        result_o = `ZERO;

        case (op_i)
            OP_ADD : result_o = lhs_i + rhs_i;
            OP_SUB : result_o = lhs_i - rhs_i;
            OP_AND : result_o = lhs_i & rhs_i;
            OP_OR  : result_o = lhs_i | rhs_i;
            OP_XOR : result_o = lhs_i ^ rhs_i;
            OP_PASS: result_o = lhs_i;

            default: result_o = `ZERO;
        endcase
    end

    //--------------------------------------------------------------------------
    // Status Flags
    //--------------------------------------------------------------------------

    assign zero_o = (result_o == `ZERO);

`ifdef DEBUG
always @(lhs_i, rhs_i, op_i) begin
    $display("[%0t] ALU lhs=%0d rhs=%0d op=%0d result=%0d",
             $time, lhs_i, rhs_i, op_i, result_o);
end
`endif

endmodule

`default_nettype wire

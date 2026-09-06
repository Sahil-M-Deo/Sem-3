//==============================================================================
// File    : common.svh
// Project : Simple Processor
//
// Common definitions shared across RTL modules.
//==============================================================================

`ifndef COMMON_SVH
`define COMMON_SVH


// -----------------------------------------------------------------------------
// Global Parameters
// -----------------------------------------------------------------------------

`define XLEN        8
`define REG_COUNT   8
`define REG_ADDR_W  3

// -----------------------------------------------------------------------------
// ALU Operations
// -----------------------------------------------------------------------------

typedef enum logic [2:0] {
    OP_ADD  = 3'd0,
    OP_SUB  = 3'd1,
    OP_AND  = 3'd2,
    OP_OR   = 3'd3,
    OP_XOR  = 3'd4,
    OP_PASS = 3'd5
} alu_op_t;

// -----------------------------------------------------------------------------
// Helper Macros
// -----------------------------------------------------------------------------

`define ZERO {`XLEN{1'b0}}

`ifdef DEBUG
    `define DBG(msg) $display("[DEBUG] %s", msg)
`else
    `define DBG(msg)
`endif

`default_nettype wire

`endif // COMMON_SVH

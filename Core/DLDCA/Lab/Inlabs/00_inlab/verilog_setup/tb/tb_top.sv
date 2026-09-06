//==============================================================================
// File    : tb_top.sv
// Project : Tiny Processing Unit (TPU)
// Description:
//   Simple testbench demonstrating clock generation, reset,
//   register initialization, ALU operations and VCD dumping.
//
//==============================================================================

`timescale 1ns/1ps
`include "../rtl/common.svh"

module tb_top;

    //--------------------------------------------------------------------------
    // Clock / Reset
    //--------------------------------------------------------------------------

    logic clk;
    logic rst;

    //--------------------------------------------------------------------------
    // DUT Inputs
    //--------------------------------------------------------------------------

    logic [`REG_ADDR_W-1:0] rs1_addr;
    logic [`REG_ADDR_W-1:0] rs2_addr;
    logic [`REG_ADDR_W-1:0] rd_addr;

    logic                   write_enable;
    alu_op_t                alu_op;

    logic external_write;
    logic [`XLEN-1:0] external_data;

    //--------------------------------------------------------------------------
    // DUT Outputs
    //--------------------------------------------------------------------------

    logic [`XLEN-1:0] rs1_data;
    logic [`XLEN-1:0] rs2_data;
    logic [`XLEN-1:0] result;
    logic             zero;

    //--------------------------------------------------------------------------
    // DUT
    //--------------------------------------------------------------------------

    top dut (
        .clk_i          (clk),
        .rst_i          (rst),

        .rs1_addr_i     (rs1_addr),
        .rs2_addr_i     (rs2_addr),
        .rd_addr_i      (rd_addr),

        .alu_op_i       (alu_op),
        .write_enable_i (write_enable),

        .external_write_i (external_write),
        .external_data_i  (external_data),

        .rs1_data_o     (rs1_data),
        .rs2_data_o     (rs2_data),
        .result_o       (result),
        .zero_o         (zero)
    );

    //--------------------------------------------------------------------------
    // Clock Generator
    //--------------------------------------------------------------------------

    initial clk = 0;
    always #5 clk = ~clk;

    //--------------------------------------------------------------------------
    // Waveform Dump
    //--------------------------------------------------------------------------

    `ifndef DUMPFILE
    `define DUMPFILE "tb_top_dump.vcd"
    `endif

    initial begin
        $dumpfile(`DUMPFILE);
        $dumpvars(0, tb_top);
    end

    initial begin
        external_write = 1'b0;
        external_data  = '0;
    end

    //--------------------------------------------------------------------------
    // Helper Tasks
    //--------------------------------------------------------------------------

    // Directly initialize register file (simulation only)
    task automatic load_reg(
        input logic [`REG_ADDR_W-1:0] addr,
        input logic [`XLEN-1:0] value
    );
    begin
        @(negedge clk);
        rd_addr        <= addr;
        external_data  <= value;
        external_write <= 1'b1;
        write_enable   <= 1'b1;

        @(posedge clk);
        external_write <= 1'b0;
        write_enable   <= 1'b0;
    end
    endtask

    task automatic execute(
        input logic [`REG_ADDR_W-1:0] rs1,
        input logic [`REG_ADDR_W-1:0] rs2,
        input logic [`REG_ADDR_W-1:0] rd,
        input alu_op_t                op
    );
    begin
        @(negedge clk);
        rs1_addr     <= rs1;
        rs2_addr     <= rs2;
        rd_addr      <= rd;
        alu_op       <= op;
        write_enable <= 1'b1;

        @(posedge clk);
        // Standard $display evaluates immediately while automatic task variables are valid
        $display("[%0t] OP=%0d  R%0d(%0d)  R%0d(%0d) -> R%0d = %0d",
                $time, op, rs1, rs1_data, rs2, rs2_data, rd, result);

        write_enable <= 1'b0;
    end
    endtask

    //--------------------------------------------------------------------------
    // Monitor
    //--------------------------------------------------------------------------

    // initial begin
    //     $monitor("[%0t] rst=%0b we=%0b op=%0d result=%0d zero=%0b",
    //               $time, rst, write_enable, alu_op, result, zero);
    // end

    //--------------------------------------------------------------------------
    // Test Sequence
    //--------------------------------------------------------------------------

    initial begin

        // Defaults
        rst           = 1'b1;
        write_enable  = 1'b0;

        rs1_addr      = '0;
        rs2_addr      = '0;
        rd_addr       = '0;

        alu_op        = OP_ADD;

        // Hold reset
        repeat (2) @(posedge clk);
        rst = 1'b0;

        // -------------------------------------------------------------
        // Preload Registers
        // -------------------------------------------------------------

        load_reg(0, 8'd10);
        load_reg(1, 8'd20);
        load_reg(2, 8'd5);
        load_reg(3, 8'd3);

        // -------------------------------------------------------------
        // Example Operations
        // -------------------------------------------------------------

        execute(0, 1, 4, OP_ADD);   // R4 = 30
        execute(1, 2, 5, OP_SUB);   // R5 = 15
        execute(0, 3, 6, OP_AND);
        execute(0, 3, 7, OP_OR);
        execute(4, 5, 0, OP_XOR);
        execute(7, 7, 1, OP_PASS);

        // -------------------------------------------------------------
        // Show Final Register File
        // -------------------------------------------------------------

        $display("\nFinal Register File:");

        for (int i = 0; i < `REG_COUNT; i++) begin
            $display("R%0d = %0d", i, dut.u_regfile.regs[i]);
        end

        $display("\nSimulation Complete.");

        #20;
        $finish;
    end

endmodule

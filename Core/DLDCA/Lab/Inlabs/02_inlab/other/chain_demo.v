`timescale 1ns/1ps

// ============================================================
// Two TFF implementations: blocking (=) vs non-blocking (<=)
// ============================================================

module tff_reg_block (
    input clk,
    input t,
    output reg q
);
    initial q = 1'b0;

    always @(posedge clk) begin
        q = t ^ q;   // BLOCKING -- updates instantly
    end
endmodule

module tff_reg_nonblock (
    input clk,
    input t,
    output reg q
);
    initial q = 1'b0;

    always @(posedge clk) begin
        q <= t ^ q;  // NON-BLOCKING -- updates deferred
    end
endmodule

// ============================================================
// Three chains: same connectivity, different assignment style
// and (for the blocking case) different instantiation order.
// ============================================================

// Blocking, tff1 instantiated before tff2
module chain_top_block_12 (
    input clk,
    output q1,
    output q2
);
    tff_reg_block tff1 (.clk(clk), .t(1'b1), .q(q1));  // always toggles
    tff_reg_block tff2 (.clk(clk), .t(q1),   .q(q2));  // should toggle when q1==1
endmodule

// Blocking, tff2 instantiated before tff1 (order swapped)
module chain_top_block_21 (
    input clk,
    output q1,
    output q2
);
    tff_reg_block tff2 (.clk(clk), .t(q1),   .q(q2));  // should toggle when q1==1
    tff_reg_block tff1 (.clk(clk), .t(1'b1), .q(q1));  // always toggles
endmodule

// Non-blocking, correct reference -- order shouldn't matter
module chain_top_nonblock (
    input clk,
    output q1,
    output q2
);
    tff_reg_nonblock tff1 (.clk(clk), .t(1'b1), .q(q1));
    tff_reg_nonblock tff2 (.clk(clk), .t(q1),   .q(q2));
endmodule

// ============================================================
// Testbench: all three chains share one clock, one VCD
// ============================================================

module tb_chain_top;
    reg clk;

    wire q1_12,  q2_12;   // blocking, tff1 first
    wire q1_21,  q2_21;   // blocking, tff2 first
    wire q1_nb,  q2_nb;   // non-blocking, reference

    chain_top_block_12  dut_block_12  (.clk(clk), .q1(q1_12), .q2(q2_12));
    chain_top_block_21  dut_block_21  (.clk(clk), .q1(q1_21), .q2(q2_21));
    chain_top_nonblock  dut_nonblock  (.clk(clk), .q1(q1_nb), .q2(q2_nb));

    // clock: 10ns period
    initial clk = 0;
    always #5 clk = ~clk;

    initial begin
        $monitor("time=%0t clk=%b | BLOCK(tff1 first) q1=%b q2=%b | BLOCK(tff2 first) q1=%b q2=%b | NONBLOCK(ref) q1=%b q2=%b",
                  $time, clk,
                  q1_12, q2_12,
                  q1_21, q2_21,
                  q1_nb, q2_nb);
    end

    initial begin
        $dumpfile("chain_top.vcd");
        $dumpvars(0, tb_chain_top);

        repeat (10) @(posedge clk);
        $finish;
    end
endmodule
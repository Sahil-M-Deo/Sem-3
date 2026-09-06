// Corrected versions of reg_bug1..reg_bug4 from buggy_regs.v.
// Each module should behave like Task 3's loadable_reg: synchronous reset
// takes priority over load, and q holds its value when load is low.

module reg_bug1_fixed (
    input wire clk,
    input wire rst,
    input wire load,
    input wire d,
    output reg q
);

endmodule

module reg_bug2_fixed (
    input wire clk,
    input wire rst,
    input wire load,
    input wire d,
    output reg q
);

endmodule

module reg_bug3_fixed (
    input wire clk,
    input wire rst,
    input wire load,
    input wire d,
    output reg q
);

endmodule

module reg_bug4_fixed (
    input wire clk,
    input wire rst,
    input wire load,
    input wire d,
    output reg q
);

endmodule

module loadable_reg (
    input wire clk,
    input wire rst,
    input wire load,
    input wire d,
    output reg q
);
    // Hint: instantiate mux_behavioral to compute the next value of q
    // (hold q if load is low, take d if load is high), then register
    // that value on the clock edge.

endmodule

// A free-running tick generator. In real hardware LIMIT would be set so that
// tick pulses once per second (e.g. LIMIT = your clock frequency in Hz).
// For simulation we use a small LIMIT so you don't have to wait for millions
// of clock edges to see the stopwatch actually move.
//
// This module is complete -- you do not need to modify it, just instantiate
// it inside stopwatch.v.

module tick_gen #(
    parameter LIMIT = 4
) (
    input  wire clk,
    input  wire rst,
    output reg  tick
);
    reg [31:0] counter;

    always @(posedge clk) begin
        if (rst) begin
            counter <= 32'd0;
            tick    <= 1'b0;
        end else if (counter == LIMIT - 1) begin
            counter <= 32'd0;
            tick    <= 1'b1;
        end else begin
            counter <= counter + 32'd1;
            tick    <= 1'b0;
        end
    end
endmodule

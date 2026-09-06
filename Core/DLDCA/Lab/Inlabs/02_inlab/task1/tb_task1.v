`timescale 1ns/1ns

module tb_task1;
    reg clk;
    reg d;
    reg t;
    wire q_dff;
    wire q_tff;
    wire q_comb;

    buffer u_comb (.d(d), .q(q_comb));

    dFlipFlop u_dff (
        .clk(clk),
        .d(d),
        .q(q_dff)
    );

    tFlipFlop u_tff (
        .clk(clk),
        .t(t),
        .q(q_tff)
    );

    initial clk = 0;
    always #5 clk = ~clk; // timestep=10ns

    initial begin
        $dumpfile("task1.vcd");
        $dumpvars(0, tb_task1);
        $display("Time | clk d t | q_dff q_tff q_comb");
        $monitor("%4t |  %b   %b %b |   %b     %b     %b", $time, clk, d, t, q_dff, q_tff, q_comb);

        u_dff.q = 0;
        u_tff.q = 0;

        d = 0; t = 0; #10;
        d = 1; t = 1; #10;
        d = 0; t = 1; #10;
        d = 1; t = 0; #10;
        d = 1; t = 1; #10;
        d = 0; t = 0; #10;

        $finish;
    end
endmodule
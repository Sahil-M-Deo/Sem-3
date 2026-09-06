`timescale 1ns/1ns

module tb;
    reg clk, rst, load, d;
    wire q;

    loadable_reg uut (
        .clk(clk),
        .rst(rst),
        .load(load),
        .d(d),
        .q(q)
    );

    always #5 clk = ~clk;

    initial begin
        $monitor("At time %0t: rst = %b, load = %b, d = %b -> q = %b",
                  $time, rst, load, d, q);
    end

    initial begin
        clk = 0; rst = 1; load = 0; d = 0;
        $dumpfile("task3.vcd");
        $dumpvars(0, tb);

        #10 rst = 0;

        // q should hold at 0 while load is low, regardless of d
        d = 1; load = 0; #10;
        d = 1; load = 0; #10;

        // q should load d on the next clock edge
        d = 1; load = 1; #10;
        load = 0; #10;

        // q should keep holding its last loaded value
        d = 0; load = 0; #10;

        // load a new value
        d = 0; load = 1; #10;
        load = 0; #10;

        // synchronous reset should override everything
        d = 1; load = 1; rst = 1; #10;

        $finish;
    end
endmodule

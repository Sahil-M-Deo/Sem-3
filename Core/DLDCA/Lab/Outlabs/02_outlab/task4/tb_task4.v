`timescale 1ns/1ns

module tb;
    reg clk, rst, load, d;
    wire q1, q2, q3, q4;
    integer errors;

    reg_bug1_fixed dut1 (.clk(clk), .rst(rst), .load(load), .d(d), .q(q1));
    reg_bug2_fixed dut2 (.clk(clk), .rst(rst), .load(load), .d(d), .q(q2));
    reg_bug3_fixed dut3 (.clk(clk), .rst(rst), .load(load), .d(d), .q(q3));
    reg_bug4_fixed dut4 (.clk(clk), .rst(rst), .load(load), .d(d), .q(q4));

    always #5 clk = ~clk;

    initial begin
        $monitor("At time %0t: rst=%b load=%b d=%b -> q1=%b q2=%b q3=%b q4=%b",
                  $time, rst, load, d, q1, q2, q3, q4);
    end

    task check(input expected);
        begin
            #1;
            if (q1 !== expected) begin errors = errors + 1; $display("  reg_bug1_fixed WRONG: expected %b, got %b", expected, q1); end
            if (q2 !== expected) begin errors = errors + 1; $display("  reg_bug2_fixed WRONG: expected %b, got %b", expected, q2); end
            if (q3 !== expected) begin errors = errors + 1; $display("  reg_bug3_fixed WRONG: expected %b, got %b", expected, q3); end
            if (q4 !== expected) begin errors = errors + 1; $display("  reg_bug4_fixed WRONG: expected %b, got %b", expected, q4); end
        end
    endtask

    initial begin
        errors = 0;
        clk = 0; rst = 1; load = 0; d = 0;
        $dumpfile("task4.vcd");
        $dumpvars(0, tb);

        #10 check(0);

        rst = 0; load = 0; d = 1; #10 check(0);   // hold at 0 while load is low
        load = 1; d = 1; #10 check(1);            // load 1
        load = 0; d = 0; #10 check(1);            // hold at 1
        load = 1; d = 0; #10 check(0);            // load 0

        // rst and load asserted on the same edge: rst must win
        load = 1; d = 1; rst = 1; #10 check(0);

        rst = 0; load = 1; d = 1; #10 check(1);

        if (errors == 0)
            $display("PASS: all four fixed registers behave correctly.");
        else
            $display("FAIL: %0d mismatch(es) found.", errors);

        $finish;
    end
endmodule

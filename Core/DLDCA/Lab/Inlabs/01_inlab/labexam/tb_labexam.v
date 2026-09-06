`timescale 1ns/1ns

module tb;
    reg a, b;
    wire eq, gt, lt;

    comparator uut (
        .a(a),
        .b(b),
        .eq(eq),
        .gt(gt),
        .lt(lt)
    );

    initial begin
        $monitor("At time %0t: a = %b, b = %b -> eq = %b, gt = %b, lt = %b",
                  $time, a, b, eq, gt, lt);
    end

    initial begin
        $dumpfile("labexam.vcd");
        $dumpvars(0, tb);

        a = 0; b = 0; #10;
        a = 0; b = 1; #10;
        a = 1; b = 0; #10;
        a = 1; b = 1; #10;

        $finish;
    end
endmodule
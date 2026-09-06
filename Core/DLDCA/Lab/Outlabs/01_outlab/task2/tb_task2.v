`timescale 1ns/1ns

module tb;
    reg  [3:0] a, b;
    wire       eq, gt, lt;

    fourbit_comparator uut (
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
        $dumpfile("task2.vcd");
        $dumpvars(0, tb);

        a = 4'b0000; b = 4'b0000; #10;
        a = 4'b0001; b = 4'b0000; #10;
        a = 4'b0000; b = 4'b0001; #10;
        a = 4'b1010; b = 4'b1001; #10;
        a = 4'b0111; b = 4'b1000; #10;
        a = 4'b1111; b = 4'b1111; #10;

        $finish;
    end
endmodule
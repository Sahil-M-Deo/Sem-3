`timescale 1ns/1ns

module tb;
    reg a, b;
    wire y;

    and_gate uut (
        .in1(a),
        .in2(b),
        .out(y)
    );

    initial begin
        $monitor("At time %0t: a = %b, b = %b, y = %b", $time, a, b, y);
    end

    initial begin
        $dumpfile("task0.vcd");
        $dumpvars(0, tb);

        a = 0; b = 0; #10;
        a = 0; b = 1; #10;
        a = 1; b = 0; #10;
        a = 1; b = 1; #10;

        $finish;
    end
endmodule

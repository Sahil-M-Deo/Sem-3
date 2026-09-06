`timescale 1ns/1ns

module tb;
    reg a, b, sel;
    wire y;

    mux_dataflow uut (
        .a(a),
        .b(b),
        .sel(sel),
        .y(y)
    );

    initial begin
        $monitor("At time %0t: a = %b, b = %b, sel = %b -> y = %b",
                  $time, a, b, sel, y);
    end

    initial begin
        $dumpfile("task1.vcd");
        $dumpvars(0, tb);

        a = 0; b = 0; sel = 0; #10;
        a = 0; b = 1; sel = 0; #10;
        a = 0; b = 1; sel = 1; #10;
        a = 1; b = 0; sel = 0; #10;
        a = 1; b = 0; sel = 1; #10;
        a = 1; b = 1; sel = 1; #10;

        $finish;
    end
endmodule

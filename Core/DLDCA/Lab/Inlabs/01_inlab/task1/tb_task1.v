`timescale 1ns/1ns
 
module tb;
    reg a, b, cin;
    wire cout, sum;
 
    adder uut (
        .a(a),
        .b(b),
        .cin(cin),
        .cout(cout),
        .sum(sum)
    );
 
    initial begin
        $monitor("At time %0t: a = %b, b = %b, cin = %b -> cout = %b, sum = %b",
                  $time, a, b, cin, cout, sum);
    end
 
    initial begin
        $dumpfile("task1.vcd");
        $dumpvars(0, tb);
 
        a = 0; b = 0; cin = 0; #10;
        a = 0; b = 0; cin = 1; #10;
        a = 0; b = 1; cin = 0; #10;
        a = 0; b = 1; cin = 1; #10;
        a = 1; b = 0; cin = 0; #10;
        a = 1; b = 0; cin = 1; #10;
        a = 1; b = 1; cin = 0; #10;
        a = 1; b = 1; cin = 1; #10;
 
        $finish;
    end
endmodule
 
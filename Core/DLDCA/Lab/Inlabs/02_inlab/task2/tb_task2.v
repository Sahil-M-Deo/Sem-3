module tb_shift_reg;
    reg clk, rst, din;
    wire [3:0] q;

    shiftReg uut (.clk(clk), .rst(rst), .din(din), .q(q));  // Change module name per test

    always #5 clk = ~clk;

    initial begin
        clk = 0; rst = 1; din = 0;
        $dumpfile("task2.vcd");
        $dumpvars(0, tb_shift_reg);
        $monitor("Time: %0t, clk: %b, rst: %b, din: %b, q: %b", $time, clk, rst, din, q);

        #10 rst = 0;

        din = 1; #10;
        din = 0; #10;
        din = 1; #10;
        din = 1; #10;

        din = 0; #20;

        $finish;
    end
endmodule


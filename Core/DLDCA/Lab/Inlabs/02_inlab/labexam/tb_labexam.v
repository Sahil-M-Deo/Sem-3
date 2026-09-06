`timescale 1ns/1ps

module tb_labexam;
    reg clk, rst;
    reg [1:0] mode;
    reg serial_in;
    reg [3:0] parallel_in;
    wire [3:0] q;

    universal_shift_reg uut (
        .clk(clk),
        .rst(rst),
        .mode(mode),
        .serial_in(serial_in),
        .parallel_in(parallel_in),
        .q(q)
    );

    always #5 clk = ~clk;

    initial begin
        $dumpfile("labexam.vcd");
        $dumpvars(0, tb_labexam);

        clk = 0;
        rst = 1;
        mode = 2'b00;
        serial_in = 0;
        parallel_in = 4'b0000;
        #10 rst = 0;

        mode = 2'b11; parallel_in = 4'b1011; #10;
        mode = 2'b00; #10;
        mode = 2'b10; serial_in = 1; #10;
        serial_in = 0; #10;
        serial_in = 1; #10;
        serial_in = 1; #10;
        serial_in = 0; #10;
        mode = 2'b00; #10;
        mode = 2'b01; serial_in = 1; #10;
        serial_in = 0; #10;
        serial_in = 0; #10;
        serial_in = 1; #10;
        mode = 2'b11; parallel_in = 4'b0110; #10;
        mode = 2'b10; serial_in = 1; #10;
        mode = 2'b01; serial_in = 0; #10;
        mode = 2'b10; serial_in = 0; #10;
        mode = 2'b01; serial_in = 1; #10;
        mode = 2'b10; serial_in = 1; #10;
        rst = 1; #10;
        rst = 0; #10;
        mode = 2'b01; serial_in = 1; #10;
        serial_in = 0; #10;
        mode = 2'b11; parallel_in = 4'b1100; #10;

        #10 $finish;
    end

    initial
        $monitor("time=%0t mode=%b serial_in=%b parallel_in=%b q=%b",
                  $time, mode, serial_in, parallel_in, q);

endmodule
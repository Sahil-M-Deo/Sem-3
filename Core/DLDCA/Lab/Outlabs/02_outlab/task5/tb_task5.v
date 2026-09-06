`timescale 1ns/1ns

module tb;
    reg clk, rst, up, down;
    wire [3:0] count;
    integer i;
    integer errors;

    sat_counter uut (
        .clk(clk),
        .rst(rst),
        .up(up),
        .down(down),
        .count(count)
    );

    always #5 clk = ~clk;

    initial begin
        $monitor("At time %0t: rst=%b up=%b down=%b -> count=%0d",
                  $time, rst, up, down, count);
    end

    task check(input [3:0] expected);
        begin
            #1;
            if (count !== expected) begin
                errors = errors + 1;
                $display("  WRONG: expected %0d, got %0d", expected, count);
            end
        end
    endtask

    initial begin
        errors = 0;
        clk = 0; rst = 1; up = 0; down = 0;
        $dumpfile("task5.vcd");
        $dumpvars(0, tb);

        #10 check(0);
        rst = 0;

        // count up to saturation at 15
        up = 1; down = 0;
        for (i = 0; i < 20; i = i + 1) begin
            #10;
        end
        check(15);

        // hold when up == down
        up = 1; down = 1; #10; check(15);
        up = 0; down = 0; #10; check(15);

        // count down to saturation at 0
        up = 0; down = 1;
        for (i = 0; i < 20; i = i + 1) begin
            #10;
        end
        check(0);

        // a few ordinary steps
        up = 1; down = 0; #10; check(1);
        up = 1; down = 0; #10; check(2);
        down = 1; up = 0; #10; check(1);

        // reset overrides everything
        rst = 1; up = 1; down = 0; #10; check(0);

        if (errors == 0)
            $display("PASS: sat_counter behaves correctly.");
        else
            $display("FAIL: %0d mismatch(es) found.", errors);

        $finish;
    end
endmodule

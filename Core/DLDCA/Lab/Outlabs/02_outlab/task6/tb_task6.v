`timescale 1ns/1ns

module tb;
    reg clk, rst, start_stop;
    wire [3:0] sec_ones, sec_tens, min_ones, min_tens;
    wire tick_out;
    integer errors;

    stopwatch uut (
        .clk(clk),
        .rst(rst),
        .start_stop(start_stop),
        .sec_ones(sec_ones),
        .sec_tens(sec_tens),
        .min_ones(min_ones),
        .min_tens(min_tens),
        .tick_out(tick_out)
    );

    always #5 clk = ~clk;

    initial begin
        $monitor("At time %0t: %0d%0d:%0d%0d",
                  $time, min_tens, min_ones, sec_tens, sec_ones);
    end

    // Safety net: if tick_out is never driven (e.g. tick_gen wasn't
    // instantiated/wired up yet), wait_ticks would otherwise wait forever.
    initial begin
        #2000;
        $display("TIMEOUT: tick_out never toggled -- did you instantiate");
        $display("tick_gen and connect its tick output to tick_out?");
        $finish;
    end

    task press_start_stop;
        begin
            @(posedge clk);
            #1 start_stop = 1'b1;
            @(posedge clk);
            #1 start_stop = 1'b0;
        end
    endtask

    // Wait for n actual tick pulses from the tick generator, then one more
    // clock edge for the registered digit update in response to the last
    // tick to land.
    task wait_ticks(input integer n);
        integer i;
        begin
            for (i = 0; i < n; i = i + 1)
                @(posedge tick_out);
            @(posedge clk);
        end
    endtask

    task check(
        input [3:0] exp_mt, input [3:0] exp_mo,
        input [3:0] exp_st, input [3:0] exp_so
    );
        begin
            #1;
            if (min_tens !== exp_mt || min_ones !== exp_mo ||
                sec_tens !== exp_st || sec_ones !== exp_so) begin
                errors = errors + 1;
                $display("  WRONG: expected %0d%0d:%0d%0d, got %0d%0d:%0d%0d",
                          exp_mt, exp_mo, exp_st, exp_so,
                          min_tens, min_ones, sec_tens, sec_ones);
            end
        end
    endtask

    initial begin
        errors = 0;
        clk = 0; rst = 1; start_stop = 0;
        $dumpfile("task6.vcd");
        $dumpvars(0, tb);

        #10;
        check(0, 0, 0, 0);

        // still not running: time should not advance even after many ticks
        rst = 0;
        repeat (20) @(posedge clk);
        check(0, 0, 0, 0);

        // start the stopwatch, let 3 ticks pass
        press_start_stop;
        wait_ticks(3);
        check(0, 0, 0, 3);

        // stop it, confirm it holds even as more ticks would have occurred
        press_start_stop;
        repeat (16) @(posedge clk);
        check(0, 0, 0, 3);

        // resume
        press_start_stop;

        // backdoor-load digits close to a rollover boundary to test carry
        // logic without simulating thousands of cycles from zero
        @(negedge clk);
        uut.sec_ones = 4'd9;
        uut.sec_tens = 4'd5;
        uut.min_ones = 4'd0;
        uut.min_tens = 4'd0;

        // one tick: seconds should roll 59 -> 00 and carry into minutes
        wait_ticks(1);
        check(0, 1, 0, 0);

        // load right up against a full wraparound: 59:59 -> 00:00
        @(negedge clk);
        uut.sec_ones = 4'd9;
        uut.sec_tens = 4'd5;
        uut.min_ones = 4'd9;
        uut.min_tens = 4'd5;

        wait_ticks(1);
        check(0, 0, 0, 0);

        // reset should zero everything immediately regardless of running state
        rst = 1;
        @(posedge clk);
        check(0, 0, 0, 0);

        if (errors == 0)
            $display("PASS: stopwatch behaves correctly.");
        else
            $display("FAIL: %0d mismatch(es) found.", errors);

        $finish;
    end
endmodule

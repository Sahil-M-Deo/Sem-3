`timescale 1ns/1ns

module tb_labexam;

    // -------------------------------------------------------
    // DUT ports
    // -------------------------------------------------------
    reg        clk, rst, start;
    reg  [3:0] count;
    reg  [2:0] triple;

    wire [1:0] pair;
    wire       valid_out;
    wire [2:0] secret;
    wire       secret_out;
    wire       done;

    // -------------------------------------------------------
    // DUT instantiation
    // -------------------------------------------------------
    jean_grey_decoder dut (
        .clk        (clk),
        .rst        (rst),
        .start      (start),
        .count      (count),
        .triple     (triple),
        .pair       (pair),
        .valid_out  (valid_out),
        .secret     (secret),
        .secret_out (secret_out),
        .done       (done)
    );

    // -------------------------------------------------------
    // Clock: 10ns period
    // -------------------------------------------------------
    always #5 clk = ~clk;

    // -------------------------------------------------------
    // State name helper
    // -------------------------------------------------------
    reg [71:0] state_str, state_str_old;
    always @(*) begin
        case (dut.state)
            2'b00: state_str = "IDLE     ";
            2'b01: state_str = "READ     ";
            2'b10: state_str = "INTERCEPT";
            2'b11: state_str = "DONE     ";
        endcase
    end

    // -------------------------------------------------------
    // Monitor: one line per posedge clk
    // Shows state, current triple input, and all outputs
    // -------------------------------------------------------
    
    always @(posedge clk) begin
        #1;
        $display("t=%9d | %-16s | triple=%03b    | pair=%02b valid=%b | secret=%03b secret_out=%b | done=%b | rem=%0d",
            $time,
            state_str,
            triple,
            pair, valid_out,
            secret, secret_out,
            done,
            dut.remaining
        );
    end
    
    // always @(negedge clk) begin
    //     #4;
    //     state_str_old <= state_str;
    //     #2;
    //     $display("t=%4d | %-9s | triple=%03b | pair=%02b valid=%b | secret=%03b secret_out=%b | done=%b | rem=%0d",
    //         $time,
    //         state_str_old,
    //         triple,
    //         pair, valid_out,
    //         secret, secret_out,
    //         done,
    //         dut.remaining
    //     );
    // end

    task send_triple;
        input [2:0] t;
        begin
            @(negedge clk);
            triple <= t;
            @(posedge clk);
        end
    endtask

    task start_decoder;
        input [3:0] c;
        begin
            @(negedge clk);
            count = c;
            start = 1;
            @(posedge clk);
            #1 start = 0;
        end
    endtask

    task do_reset;
        begin
            rst   = 1;
            start = 0;
            triple = 3'b000;
            @(posedge clk);
            @(posedge clk);
            @(negedge clk); rst = 0;
            @(posedge clk);
        end
    endtask

    task print_header;
        begin
            $display("t_POSEDGE+1 | STATE AFTER EDGE | CURRENT INPUT | OUTPUTS AFTER EDGE");
            $display("-------------------------------------------------------------------------");
        end
    endtask

    // -------------------------------------------------------
    // VCD dump
    // -------------------------------------------------------
    initial begin
        $dumpfile("labexam.vcd");
        $dumpvars(0, tb_labexam);
    end

    // -------------------------------------------------------
    // TEST SEQUENCE 1: No secrets, count=4
    //
    // Stream: 001 010 011 101
    // All valid. Expected decoded pairs: 00 01 10 11
    // No invalid triples so secret stays 000, secret_out never high.
    // -------------------------------------------------------
    initial begin
        clk   = 0;
        rst   = 1;
        start = 0;
        count = 0;
        triple = 3'b000;

        $display("\n======================================================");
        $display("SEQUENCE 1: 4 valid triples, no secrets");
        $display("Stream: 001 010 011 101");
        $display("Expected pairs out (valid cycles only): 00 01 10 11");
        $display("======================================================\n");
        print_header;

        do_reset;

        // pulse start with count=4
        start_decoder(4);

        // feed 4 valid triples one per clock
        send_triple(3'b001);   // -> pair 00
        send_triple(3'b010);   // -> pair 01
        send_triple(3'b011);   // -> pair 10
        send_triple(3'b101);   // -> pair 11

        // wait for done
        @(posedge clk);
        @(posedge clk);

        // -------------------------------------------------------
        // TEST SEQUENCE 2: 2 Jean injections, count=3
        //
        // Stream: 010 | 110 [secret:011] | 001 | 100 [secret:111] | 101
        //
        //   010  valid  -> pair 01
        //   110  INVALID -> trigger INTERCEPT
        //   011  Jean's secret #1 -> XORed into accumulator: secret = 000^011 = 011
        //   001  valid  -> pair 00
        //   100  INVALID -> trigger INTERCEPT
        //   111  Jean's secret #2 -> XORed into accumulator: secret = 011^111 = 100
        //   101  valid  -> pair 11, remaining hits 1 -> DONE
        //
        // Expected pairs (valid only): 01 00 11
        // Expected final secret: 011 XOR 111 = 100
        // -------------------------------------------------------
        $display("\n======================================================");
        $display("SEQUENCE 2: 3 valid triples, 2 Jean injections");
        $display("Stream: 010 | 110 [secret:011] | 001 | 100 [secret:111] | 101");
        $display("Expected pairs out (valid cycles only): 01 00 11");
        $display("Expected final secret: 011 XOR 111 = 100");
        $display("======================================================\n");
        print_header;

        do_reset;

        start_decoder(3);

        send_triple(3'b010);   // valid  -> pair 01
        send_triple(3'b110);   // INVALID -> Jean trigger -> go to INTERCEPT
        send_triple(3'b011);   // Jean secret #1, capture -> secret = 011
        send_triple(3'b001);   // valid  -> pair 00
        send_triple(3'b100);   // INVALID -> Jean trigger -> go to INTERCEPT
        send_triple(3'b111);   // Jean secret #2, capture -> secret = 011^111 = 100
        send_triple(3'b101);   // valid  -> pair 11, last -> DONE

        // wait for done to propagate
        @(posedge clk);
        @(posedge clk);

        $display("\n======================================================");
        $display("SEQUENCE 3: consecutive injections, count=2");
        $display("Stream: 111 | 010 | 000 | 001 | 010 | 000 | 100 | 101");
        $display("Expected pairs (valid only): 01 11");
        $display("Expected final secret: 010 XOR 001 XOR 100 = 111");
        $display("Idle input before start and after DONE should be ignored.");
        $display("======================================================\n");
        print_header;

        do_reset;

        send_triple(3'b111);   // sending signal in IDLE, should be a no-op
        start_decoder(2);

        send_triple(3'b111);   // invalid
        send_triple(3'b010);   // secret -> 010
        send_triple(3'b000);   // invalid
        send_triple(3'b001);   // secret -> 010 XOR 001 = 011
        send_triple(3'b010);   // valid -> pair 01 (rem goes to 1)
        send_triple(3'b000);   // invalid
        send_triple(3'b100);   // secret -> 011 XOR 100 = 111
        send_triple(3'b101);   // valid -> pair 11 (rem is 1 so go to DONE)
        send_triple(3'b111);   // sending signal after DONE, should be a no-op
        
        // wait for done to propagate
        @(posedge clk);
        @(posedge clk);

        $display("\n======================================================");
        $display("All sequences complete. Check pairs and secret above.");
        $display("To add your own sequence: copy a sequence block,");
        $display("call start_decoder(count), do_reset, send_triple() and finally wait 2 cycles.");
        $display("======================================================\n");

        $finish;
    end

endmodule

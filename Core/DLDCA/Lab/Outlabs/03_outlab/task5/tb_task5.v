`timescale 1ns/1ns

module tb_task5;

    reg clk;
    reg rst;

    reg [7:0] rx0_data, rx1_data, rx2_data, rx3_data;
    reg       rx0_valid, rx1_valid, rx2_valid, rx3_valid;

    wire [7:0] tx0_data, tx1_data, tx2_data, tx3_data;
    wire       tx0_valid, tx1_valid, tx2_valid, tx3_valid;

    switch dut (
        .clk(clk),
        .rst(rst),

        .rx0_data(rx0_data), .rx0_valid(rx0_valid),
        .rx1_data(rx1_data), .rx1_valid(rx1_valid),
        .rx2_data(rx2_data), .rx2_valid(rx2_valid),
        .rx3_data(rx3_data), .rx3_valid(rx3_valid),

        .tx0_data(tx0_data), .tx0_valid(tx0_valid),
        .tx1_data(tx1_data), .tx1_valid(tx1_valid),
        .tx2_data(tx2_data), .tx2_valid(tx2_valid),
        .tx3_data(tx3_data), .tx3_valid(tx3_valid)
    );

    always #5 clk = ~clk;

    localparam START = 5'b11000;
    localparam END   = 5'b11001;

    reg [7:0] frame [0:14];
    integer i;

    function [4:0] encode_nibble;
        input [3:0] n;
        begin
            case (n)
                4'h0: encode_nibble = 5'b11110;
                4'h1: encode_nibble = 5'b01001;
                4'h2: encode_nibble = 5'b10100;
                4'h3: encode_nibble = 5'b10101;
                4'h4: encode_nibble = 5'b01010;
                4'h5: encode_nibble = 5'b01011;
                4'h6: encode_nibble = 5'b01110;
                4'h7: encode_nibble = 5'b01111;
                4'h8: encode_nibble = 5'b10010;
                4'h9: encode_nibble = 5'b10011;
                4'hA: encode_nibble = 5'b10110;
                4'hB: encode_nibble = 5'b10111;
                4'hC: encode_nibble = 5'b11010;
                4'hD: encode_nibble = 5'b11011;
                4'hE: encode_nibble = 5'b11100;
                4'hF: encode_nibble = 5'b11101;
            endcase
        end
    endfunction

    function [7:0] crc8_byte;
        input [7:0] crc;
        input [7:0] data;
        integer j;
        reg [7:0] c;
        reg feedback;
        begin
            c = crc;
            for (j = 7; j >= 0; j = j - 1) begin
                feedback = c[7] ^ data[j];
                c = c << 1;
                if (feedback)
                    c = c ^ 8'hD5;
            end
            crc8_byte = c;
        end
    endfunction

    task make_frame;
        input [7:0]  dst;
        input [7:0]  src;
        input [63:0] payload;
        integer bit_count;
        integer byte_index;
        integer k;
        reg [7:0] crc;
        reg [119:0] bits;
        reg [4:0] code;
        reg [7:0] fields [0:9];
        begin
            fields[0] = dst;
            fields[1] = src;
            fields[2] = payload[63:56];
            fields[3] = payload[55:48];
            fields[4] = payload[47:40];
            fields[5] = payload[39:32];
            fields[6] = payload[31:24];
            fields[7] = payload[23:16];
            fields[8] = payload[15:8];
            fields[9] = payload[7:0];

            crc = 8'hFF;
            for (k = 0; k < 10; k = k + 1)
                crc = crc8_byte(crc, fields[k]);

            bits = 120'b0;
            bit_count = 0;

            bits[119 - bit_count -: 5] = START;
            bit_count = bit_count + 5;

            for (k = 0; k < 10; k = k + 1) begin
                code = encode_nibble(fields[k][7:4]);
                bits[119 - bit_count -: 5] = code;
                bit_count = bit_count + 5;

                code = encode_nibble(fields[k][3:0]);
                bits[119 - bit_count -: 5] = code;
                bit_count = bit_count + 5;
            end

            code = encode_nibble(crc[7:4]);
            bits[119 - bit_count -: 5] = code;
            bit_count = bit_count + 5;

            code = encode_nibble(crc[3:0]);
            bits[119 - bit_count -: 5] = code;
            bit_count = bit_count + 5;

            bits[119 - bit_count -: 5] = END;

            for (byte_index = 0; byte_index < 15; byte_index = byte_index + 1)
                frame[byte_index] = bits[119 - byte_index*8 -: 8];
        end
    endtask

    task send_byte;
        input [1:0] port;
        input [7:0] b;
        begin
            @(negedge clk);
            rx0_valid = 0; rx1_valid = 0; rx2_valid = 0; rx3_valid = 0;

            if (port == 0) begin rx0_data = b; rx0_valid = 1; end
            if (port == 1) begin rx1_data = b; rx1_valid = 1; end
            if (port == 2) begin rx2_data = b; rx2_valid = 1; end
            if (port == 3) begin rx3_data = b; rx3_valid = 1; end

            @(negedge clk);
            rx0_valid = 0; rx1_valid = 0; rx2_valid = 0; rx3_valid = 0;
        end
    endtask

    task send_current_frame;
        input [1:0] port;
        begin
            for (i = 0; i < 15; i = i + 1)
                send_byte(port, frame[i]);
        end
    endtask

    // Log all valid output bytes so interactive_tester.py can verify results
    always @(posedge clk) begin
        if (tx0_valid) $display("[TX0] %02h", tx0_data);
        if (tx1_valid) $display("[TX1] %02h", tx1_data);
        if (tx2_valid) $display("[TX2] %02h", tx2_data);
        if (tx3_valid) $display("[TX3] %02h", tx3_data);
    end

    initial begin
        $dumpfile("task5.vcd");
        $dumpvars(0, tb_task5);

        clk = 0;
        rst = 1;
        rx0_data = 0; rx1_data = 0; rx2_data = 0; rx3_data = 0;
        rx0_valid = 0; rx1_valid = 0; rx2_valid = 0; rx3_valid = 0;

        repeat (2) @(posedge clk);
        rst = 0;
        repeat (2) @(posedge clk);

        // ================================================================
        // BEGIN INTEGRATED TESTS
        $display("=== TC_START ===");
        make_frame(8'h02, 8'h01, 64'hAABBCCDDEEFF0011);
        send_current_frame(0);
        repeat (40) @(posedge clk);
        $display("=== TC_START ===");
        make_frame(8'h01, 8'h02, 64'hAAAAAAAAAAAAAAAA);
        send_current_frame(1);
        repeat (40) @(posedge clk);
        $display("=== TC_START ===");
        make_frame(8'h03, 8'h01, 64'h1234567890ABCDEF);
        send_current_frame(3);
        repeat (40) @(posedge clk);
        $display("=== TC_START ===");
        make_frame(8'h04, 8'h03, 64'hFEDCBA9876543210);
        send_current_frame(2);
        repeat (40) @(posedge clk);
        // END INTEGRATED TESTS
        // ================================================================

        // Allow final transmission cycles to complete before finishing
        repeat (50) @(posedge clk);
        $finish;
    end

endmodule
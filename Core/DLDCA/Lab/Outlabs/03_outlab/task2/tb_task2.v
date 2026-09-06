`timescale 1ns/1ps

module tb_task2;

    reg  [3:0] data;
    reg        enable;
    wire [4:0] encoded;

    reg  [4:0] encoded_in;
    wire [3:0] decoded;
    wire       valid;

    encoder enc (
        .data(data),
        .enable(enable),
        .encoded(encoded)
    );

    decoder dec (
        .encoded(encoded_in),
        .enable(enable),
        .data(decoded),
        .valid(valid)
    );

    integer i;
    integer errors;

    reg [4:0] expected_encoded [0:15];

    initial begin
        expected_encoded[0]  = 5'b11110;
        expected_encoded[1]  = 5'b01001;
        expected_encoded[2]  = 5'b10100;
        expected_encoded[3]  = 5'b10101;
        expected_encoded[4]  = 5'b01010;
        expected_encoded[5]  = 5'b01011;
        expected_encoded[6]  = 5'b01110;
        expected_encoded[7]  = 5'b01111;
        expected_encoded[8]  = 5'b10010;
        expected_encoded[9]  = 5'b10011;
        expected_encoded[10] = 5'b10110;
        expected_encoded[11] = 5'b10111;
        expected_encoded[12] = 5'b11010;
        expected_encoded[13] = 5'b11011;
        expected_encoded[14] = 5'b11100;
        expected_encoded[15] = 5'b11101;

        errors = 0;
        enable = 1;

        // Check every encoder entry.
        for (i = 0; i < 16; i = i + 1) begin
            data = i;
            #1;
            if (encoded !== expected_encoded[i]) begin
                $display("ENCODER FAIL: %04b -> %05b, expected %05b",
                         data, encoded, expected_encoded[i]);
                errors = errors + 1;
            end
        end

        // Check every valid decoder entry.
        for (i = 0; i < 16; i = i + 1) begin
            encoded_in = expected_encoded[i];
            #1;
            if (!valid || decoded !== i[3:0]) begin
                $display("DECODER FAIL: %05b -> valid=%b data=%04b, expected %04b",
                         encoded_in, valid, decoded, i[3:0]);
                errors = errors + 1;
            end
        end

        // START, END and a few unused codes must not decode as data.
        encoded_in = 5'b11000; #1;
        if (valid) begin
            $display("DECODER FAIL: START marked valid");
            errors = errors + 1;
        end

        encoded_in = 5'b11001; #1;
        if (valid) begin
            $display("DECODER FAIL: END marked valid");
            errors = errors + 1;
        end

        encoded_in = 5'b00000; #1;
        if (valid) begin
            $display("DECODER FAIL: invalid code 00000 marked valid");
            errors = errors + 1;
        end

        // Disabled decoder must not report valid.
        enable = 0;
        encoded_in = expected_encoded[5];
        #1;
        if (valid) begin
            $display("DECODER FAIL: valid asserted while disabled");
            errors = errors + 1;
        end

        if (errors == 0)
            $display("TASK 2 PASS");
        else
            $display("TASK 2 FAIL: %0d errors", errors);

        $finish;
    end

endmodule

`timescale 1ns/1ps

module tb_task4;

    localparam ADDR_WIDTH = 8;
    localparam PORT_WIDTH = 2;
    localparam TABLE_SIZE = 8;

    reg                     clk;
    reg                     rst;
    reg  [ADDR_WIDTH-1:0]   src_addr;
    reg  [ADDR_WIDTH-1:0]   dst_addr;
    reg  [PORT_WIDTH-1:0]   ingress_port;
    reg                     frame_valid;

    wire [PORT_WIDTH-1:0]   egress_port;
    wire                    known_destination;

    switch_table dut (
        .clk(clk),
        .rst(rst),
        .src_addr(src_addr),
        .dst_addr(dst_addr),
        .ingress_port(ingress_port),
        .frame_valid(frame_valid),
        .egress_port(egress_port),
        .known_destination(known_destination)
    );

    always #5 clk = ~clk;

    // VCD Dumping
    initial begin
        $dumpfile("task4.vcd");
        $dumpvars(0, tb_task4);
        // If you want to dump array contents, you can uncomment the line below.
        // Some simulators require explicit loops to dump arrays, but iverilog often handles it.
        // for (integer idx = 0; idx < TABLE_SIZE; idx = idx + 1) $dumpvars(0, dut.table_valid[idx], dut.table_addrs[idx], dut.table_ports[idx]);
    end

    task learn;
        input [7:0] addr;
        input [1:0] port;
        begin
            $display("[ACTION] Frame arriving: SRC=%02h on Ingress Port=%0d (Learning...)", addr, port);
            @(negedge clk);
            src_addr = addr;
            ingress_port = port;
            frame_valid = 1;
            @(negedge clk);
            frame_valid = 0;
        end
    endtask

    task check_lookup;
        input [7:0] addr;
        input       expected_known;
        input [1:0] expected_port;
        reg         task_failed;
        begin
            task_failed = 0;
            dst_addr = addr;
            #1; // Wait for combinational lookup logic to settle
            
            if (known_destination !== expected_known) begin
                $display("  [FAIL] Lookup DST=%02h | known=%b (Expected: %b)", 
                         addr, known_destination, expected_known);
                errors = errors + 1;
                task_failed = 1;
            end
            if (expected_known && egress_port !== expected_port) begin
                $display("  [FAIL] Lookup DST=%02h | port=%0d (Expected: %0d)", 
                         addr, egress_port, expected_port);
                errors = errors + 1;
                task_failed = 1;
            end
            
            if (!task_failed) begin
                if (expected_known)
                    $display("  [PASS] Lookup DST=%02h -> Found! Forwarding to Egress Port=%0d", addr, egress_port);
                else
                    $display("  [PASS] Lookup DST=%02h -> Unknown destination (Flooding expected)", addr);
            end
        end
    endtask

    integer errors;

    initial begin
        clk = 0;
        rst = 1;
        src_addr = 0;
        dst_addr = 0;
        ingress_port = 0;
        frame_valid = 0;
        errors = 0;

        $display("==================================================");
        $display(" Starting Task 4: MAC Learning & Forwarding Tests ");
        $display("==================================================\n");

        @(posedge clk);
        @(negedge clk);
        rst = 0;
        $display("[INFO] System Reset Deasserted.\n");

        // Empty table: destination is unknown.
        $display("--- Test 1: Empty Table Lookup ---");
        check_lookup(8'hAA, 0, 0);
        $display("");

        // Learn AA -> port 2.
        $display("--- Test 2: Learn New MAC Address ---");
        learn(8'hAA, 2);
        check_lookup(8'hAA, 1, 2);
        $display("");

        // Learn BB -> port 1.
        $display("--- Test 3: Learn Second MAC Address ---");
        learn(8'hBB, 1);
        check_lookup(8'hBB, 1, 1);
        $display("");

        // Update AA -> port 3.
        $display("--- Test 4: Update Existing MAC Address ---");
        learn(8'hAA, 3);
        check_lookup(8'hAA, 1, 3);
        $display("");

        // Unknown address remains unknown.
        $display("--- Test 5: Unknown MAC Address Lookup ---");
        check_lookup(8'hCC, 0, 0);
        $display("");

        $display("==================================================");
        if (errors == 0)
            $display(" TASK 4 PASS");
        else
            $display(" TASK 4 FAIL: %0d errors", errors);
        $display("==================================================");

        $finish;
    end

endmodule
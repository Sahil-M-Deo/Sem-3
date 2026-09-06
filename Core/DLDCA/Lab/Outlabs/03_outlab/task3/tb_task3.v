`timescale 1ns/1ns

module tb_task3;

    reg         clk;
    reg         rst;

    // RX interface
    reg  [7:0]  rx_data;
    reg         rx_valid;
    wire        frame_valid;
    wire [7:0]  rx_dst_addr;
    wire [7:0]  rx_src_addr;
    wire [63:0] rx_payload;

    // TX interface
    reg         tx_start;
    reg  [7:0]  tx_dst_addr;
    reg  [7:0]  tx_src_addr;
    reg  [63:0] tx_payload;
    wire [7:0]  tx_data;
    wire        tx_valid;
    wire        tx_busy;

    rx_frame rx (
        .clk(clk),
        .rst(rst),
        .data_in(rx_data),
        .data_valid(rx_valid),
        .frame_valid(frame_valid),
        .dst_addr(rx_dst_addr),
        .src_addr(rx_src_addr),
        .payload(rx_payload)
    );

    tx_frame tx (
        .clk(clk),
        .rst(rst),
        .start(tx_start),
        .dst_addr(tx_dst_addr),
        .src_addr(tx_src_addr),
        .payload(tx_payload),
        .data_out(tx_data),
        .data_valid(tx_valid),
        .busy(tx_busy)
    );

    // Clock: 10ns period (100 MHz)
    always #5 clk = ~clk;

    // ====================================================================
    // TX Test: Given (dst, src, payload), capture all TX bytes
    // ====================================================================
    task test_tx;
        input [7:0] dst, src;
        input [63:0] payload;
        
        integer i;
        reg [7:0] tx_bytes [14:0];
        integer byte_count;
        integer cycle_count;
        
        begin
            // Print test header
            $display("$$$$$ TX_TEST: dst=0x%02H src=0x%02H payload=0x%016H", dst, src, payload);
            
            // Reset and wait
            repeat (5) @(posedge clk);
            rst = 1'b1;
            repeat (2) @(posedge clk);
            rst = 1'b0;
            repeat (5) @(posedge clk);
            
            // Assert start
            @(negedge clk);
            tx_dst_addr = dst;
            tx_src_addr = src;
            tx_payload = payload;
            tx_start = 1'b1;
            
            @(negedge clk);
            tx_start = 1'b0;
            
            // Capture TX output bytes
            byte_count = 0;
            cycle_count = 0;
            // Wait strictly for 15 bytes or timeout to avoid race conditions with tx_busy
            while (cycle_count < 200 && byte_count < 15) begin
                @(posedge clk);
                cycle_count = cycle_count + 1;
                if (tx_valid) begin
                    tx_bytes[byte_count] = tx_data;
                    byte_count = byte_count + 1;
                end
            end
            
            // Print output
            $write("##### TX_OUTPUT:");
            for (i = 0; i < byte_count; i = i + 1) begin
                $write(" %02H", tx_bytes[i]);
            end
            $display("");
        end
    endtask

    // ====================================================================
    // RX Test: Given wire bytes, capture decoded frame
    // ====================================================================
    task test_rx;
        input [119:0] wire_frame;  // 15 bytes = 120 bits packed
        
        integer i;
        reg [7:0] byte_val;
        
        // Variables to capture the 1-cycle frame_valid pulse
        reg captured_valid;
        reg [7:0] cap_dst, cap_src;
        reg [63:0] cap_payload;
        
        begin
            // Print test header
            $write("$$$$$ RX_TEST: wire=");
            for (i = 0; i < 15; i = i + 1) begin
                byte_val = wire_frame[(14 - i) * 8 +: 8];
                $write("%02H", byte_val);
            end
            $display("");
            
            // Reset and wait
            repeat (5) @(posedge clk);
            rst = 1'b1;
            repeat (2) @(posedge clk);
            rst = 1'b0;
            repeat (5) @(posedge clk);
            
            // Send all bytes
            for (i = 0; i < 15; i = i + 1) begin
                byte_val = wire_frame[(14 - i) * 8 +: 8];
                @(negedge clk);
                rx_data = byte_val;
                rx_valid = 1'b1;
                @(negedge clk);
                rx_valid = 1'b0;
            end
            
            // Initialize capture registers
            captured_valid = 1'b0;
            cap_dst = 8'h00;
            cap_src = 8'h00;
            cap_payload = 64'h00;

            // Actively monitor for the 1-cycle pulse
            for (i = 0; i < 200; i = i + 1) begin
                @(posedge clk);
                if (frame_valid) begin
                    captured_valid = 1'b1;
                    cap_dst = rx_dst_addr;
                    cap_src = rx_src_addr;
                    cap_payload = rx_payload;
                end
            end
            
            // Print captured output rather than current wire state
            $display("##### RX_OUTPUT: frame_valid=%b dst=0x%02H src=0x%02H payload=0x%016H", 
                        captured_valid, cap_dst, cap_src, cap_payload);
        end
    endtask

    // ====================================================================
    // Main test loop
    // ====================================================================
    initial begin
        clk = 1'b0;
        rst = 1'b1;
        rx_data = 8'h00;
        rx_valid = 1'b0;
        tx_start = 1'b0;
        tx_dst_addr = 8'h00;
        tx_src_addr = 8'h00;
        tx_payload = 64'h00;

        // Enable VCD dump
        $dumpfile("task3.vcd");
        $dumpvars(0, tb_task3);

        $display("=== TASK 3 TESTBENCH START ===");

        // Example TX tests
        test_tx(8'h12, 8'h34, 64'h0123456789ABCDEF);
        repeat (50) @(posedge clk);
        
        test_tx(8'hFF, 8'h00, 64'hAAAAAAAAAAAAAAAA);
        repeat (50) @(posedge clk);

        // Example RX test 1: properly encoded frame (dst=0x12, src=0x34, payload=0x0123456789ABCDEF)
        test_rx(120'hc269557934aa96e7ca76beb7cecbb9);
        repeat (50) @(posedge clk);

        // Example RX test 2: properly encoded frame (dst=0xFF, src=0x00, payload=0xAAAAAAAAAAAAAAAA)
        test_rx(120'hc77bef5ad6b5ad6b5ad6b5ad6b7979);
        repeat (50) @(posedge clk);

        $display("=== TASK 3 TESTBENCH END ===");
        $finish;
    end

endmodule
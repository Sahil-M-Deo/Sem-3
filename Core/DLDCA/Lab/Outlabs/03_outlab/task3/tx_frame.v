module tx_frame (
    input  wire        clk,
    input  wire        rst,
    input  wire        start,
    input  wire [7:0]  dst_addr,
    input  wire [7:0]  src_addr,
    input  wire [63:0] payload,
    
    output reg  [7:0]  data_out,
    output reg         data_valid,
    output reg         busy
);

    // TODO: Instantiate purely combinational CRC-8 over 80 Bits
    // wire it correctly

    // Instantiate shift reg to store raw frame bytes in order
    // Helping you out here!
    wire [79:0] raw_frame = {dst_addr, src_addr, payload};

    // TODO: Instantiate two encoders
    // Helping you out here!
    wire [4:0] enc_high_out;
    wire [4:0] enc_low_out;
    wire [9:0] encoded_symbol = {enc_high_out, enc_low_out};

    encoder enc_high (
        // TODO
    );

    encoder enc_low (
        // TODO
    );

    // TODO: Any other regs/wires you might need
    // Hints: a counter to count bytes transmitted / a buffer to store wire output bits to avoid race conditions

    // TODO: FSM to transmit sequentially
    reg [3:0]   tx_count;
    reg [119:0] frame_buf;

    always @(posedge clk) begin
        if (rst) begin
            busy       <= 1'b0;
            data_valid <= 1'b0;
            data_out   <= 8'h00;
            tx_count   <= 4'd0;
            byte_sel   <= 4'd0;
            frame_buf  <= 120'b0;
        end else begin
            data_valid <= 1'b0;

            if (!busy) begin
                if (start) begin
                    
                    // TODO: start capturing now

                end else begin
                    // TODO: stay idle
                end
            end else begin
                
                // TODO: main loop:
                // 1. output latest encoded byte (did you buffer it?)
                // 2. encode next byte from raw_frame
                // 3. place onto buffer (as encoded_symbol) -- note you may have to place END here
                // 4. check for end condition (reached end of raw_frame etc.)

            end
        end
    end

endmodule
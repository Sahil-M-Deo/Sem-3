module rx_frame (
    input  wire        clk,
    input  wire        rst,
    input  wire [7:0]  data_in,
    input  wire        data_valid,

    output reg         frame_valid,
    output reg  [7:0]  dst_addr,
    output reg  [7:0]  src_addr,
    output reg  [63:0] payload
);

    // FSM States
    localparam IDLE      = 2'd0;
    localparam RX_DATA   = 2'd1;
    localparam CHECK_END = 2'd2;

    reg [1:0] state;
    
    // Counters and Registers
    // TODO:
    // Some examples to help you out!
    //      reg [4:0] nibble_cnt;   // Counts 0 to 21 (22 nibbles = 11 bytes)
    //      reg [3:0] high_nibble;  // Holds the first half of a byte
    //      reg [79:0] frame_data;  // 80-bit shift register for {DST, SRC, PAYLOAD}
    //      reg [7:0] rx_crc_byte;  // Holds the 11th decoded byte

    // 4B/5B Decoder (Single Instance)
    // Helping you out here!
    wire [3:0] dec_nib;
    wire       dec_valid;
    decoder u_decoder (
        .encoded(sym5),
        .enable(1'b1),
        .data(dec_nib),
        .valid(dec_valid)
    );

    // Bit Buffer (Gearbox)
    // Helping you out here!
    reg [15:0] bit_buf;         // Actual 16 bit buffer
    reg [4:0]  buf_len;         // Marking end of buffer
    // TODO: Derive next byte (decoded), next state of bit_buf, next value of buf_len, and decoded symbol
    wire [7:0] new_byte      = /* TODO */;
    wire [15:0] next_bit_buf = /* TODO */;
    wire [4:0]  next_buf_len = /* TODO */;
    wire [4:0]  sym5         = /* TODO */;

    // CRC Engine
    // Helping you out here!
    reg        crc_rst;
    reg  [7:0] crc_data_in;
    reg        crc_data_valid;
    wire [7:0] calculated_crc;

    crc_parallel_serial #(
        .WIDTH(8), .POLY(8'hD5), .INIT(8'hFF), .DATA_WIDTH(8)
    ) rx_crc_inst (
        .clk(clk),
        .rst(crc_rst || rst),
        .data(crc_data_in),
        .data_valid(crc_data_valid),
        .data_last(1'b0),
        .crc(calculated_crc),
        .crc_valid() // dont really care about this output pin
    );

    // Main FSM
    always @(posedge clk) begin
        if (rst) begin
            state          <= IDLE;
            buf_len        <= 5'd0;
            bit_buf        <= 16'd0;
            nibble_cnt     <= 5'd0;
            frame_valid    <= 1'b0;
            dst_addr       <= 8'h00;
            src_addr       <= 8'h00;
            payload        <= 64'h0;
            crc_rst        <= 1'b1;
            crc_data_valid <= 1'b0;
        end else begin
            // Defaults
            frame_valid    <= 1'b0;
            crc_rst        <= 1'b0;
            crc_data_valid <= 1'b0;
            bit_buf        <= next_bit_buf;
            buf_len        <= next_buf_len;

            case (state)
                IDLE: begin
                    // TODO
                end

                RX_DATA: begin
                    // TODO
                end

                CHECK_END: begin
                    // TODO
                end

                default: state <= IDLE;
            endcase
        end
    end

endmodule
`timescale 1ns/1ps

module tb_crc;
  integer i;

  reg clk, rst;
  always #5 clk = ~clk;

  reg data_bit, data_valid;
  reg data_bit4, data_valid4;

  reg [7:0] parallel_data;
  reg       parallel_valid;

  reg [3:0] parallel_data4;
  reg       parallel_valid4;

  reg [7:0] upd_crc_in;
  reg       upd_data_bit;
  wire [7:0] upd_crc_out;

  wire [7:0] crc8_out;
  wire [7:0] crcp_out;
  wire [3:0] crc4_out;
  wire [7:0] parallel8_out;
  wire [7:0] parallel4_out;

  crc8_update u_upd (
    .crc_in(upd_crc_in),
    .data_bit(upd_data_bit),
    .crc_out(upd_crc_out)
  );

  crc8_serial u_crc8 (
    .clk(clk),
    .rst(rst),
    .data_bit(data_bit),
    .data_valid(data_valid),
    .crc(crc8_out)
  );

  crc_serial #(
    .WIDTH(8),
    .POLY(8'hD5),
    .INIT(8'hFF)
  ) u_crcp (
    .clk(clk),
    .rst(rst),
    .data_bit(data_bit),
    .data_valid(data_valid),
    .crc(crcp_out)
  );

  crc_serial #(
    .WIDTH(4),
    .POLY(4'h3),
    .INIT(4'hF)
  ) u_crc4 (
    .clk(clk),
    .rst(rst),
    .data_bit(data_bit4),
    .data_valid(data_valid4),
    .crc(crc4_out)
  );

  crc_parallel_serial #(
    .WIDTH(8),
    .POLY(8'hD5),
    .INIT(8'hFF),
    .DATA_WIDTH(8)
  ) u_parallel8 (
    .clk(clk),
    .rst(rst),
    .data(parallel_data),
    .data_valid(parallel_valid),
    .crc(parallel8_out)
  );

  crc_parallel_serial #(
    .WIDTH(8),
    .POLY(8'hD5),
    .INIT(8'hFF),
    .DATA_WIDTH(4)
  ) u_parallel4 (
    .clk(clk),
    .rst(rst),
    .data(parallel_data4),
    .data_valid(parallel_valid4),
    .crc(parallel4_out)
  );

  task reset_dut;
    begin
      @(negedge clk);
      rst = 1;
      @(negedge clk);
      rst = 0;
    end
  endtask

  task feed_byte;
    input [7:0] byte_in;
    begin
      for (i = 7; i >= 0; i = i - 1) begin
        @(negedge clk);
        data_bit = byte_in[i];
        data_valid = 1;
        @(posedge clk);
        @(negedge clk);
        data_valid = 0;
      end
    end
  endtask

  task feed_nibble;
    input [3:0] nibble_in;
    begin
      for (i = 3; i >= 0; i = i - 1) begin
        @(negedge clk);
        data_bit4 = nibble_in[i];
        data_valid4 = 1;
        @(posedge clk);
        @(negedge clk);
        data_valid4 = 0;
      end
    end
  endtask

  task feed_parallel_byte;
    input [7:0] byte_in;
    begin
      @(negedge clk);
      parallel_data = byte_in;
      parallel_valid = 1;
      @(posedge clk);
      @(negedge clk);
      parallel_valid = 0;
    end
  endtask

  task feed_parallel_nibble;
    input [3:0] nibble_in;
    begin
      @(negedge clk);
      parallel_data4 = nibble_in;
      parallel_valid4 = 1;
      @(posedge clk);
      @(negedge clk);
      parallel_valid4 = 0;
    end
  endtask

  initial begin
    $dumpfile("task1.vcd");
    $dumpvars(0, tb_crc);

    clk = 0;
    rst = 0;

    data_bit = 0;
    data_valid = 0;
    data_bit4 = 0;
    data_valid4 = 0;

    parallel_data = 0;
    parallel_valid = 0;
    parallel_data4 = 0;
    parallel_valid4 = 0;

    upd_crc_in = 0;
    upd_data_bit = 0;

    // crc8_update sanity check
    upd_crc_in = 8'hFF;
    upd_data_bit = 1;
    #1;
    $display("crc8_update: FF,1 -> %02h", upd_crc_out);

    // TEST 1: fixed CRC-8
    reset_dut;
    feed_byte(8'hAB);
    @(negedge clk);
    $display("CRC_RESULT|TEST=1|MSG=AB|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      crc8_out);

    // TEST 2: parametric CRC-8
    reset_dut;
    feed_byte(8'hAB);
    feed_byte(8'hCD);
    @(negedge clk);
    $display("CRC_RESULT|TEST=2|MSG=ABCD|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      crcp_out);

    // TEST 3: parametric CRC-4
    reset_dut;
    feed_nibble(4'hA);
    feed_nibble(4'h5);
    feed_nibble(4'hC);
    feed_nibble(4'h3);
    @(negedge clk);
    $display("CRC_RESULT|TEST=3|MSG=A5C3|WIDTH=4|POLY=3|INIT=F|CRC=%01h",
      crc4_out);

    // TEST 4: parallel, DATA_WIDTH=8
    reset_dut;
    feed_parallel_byte(8'hAB);
    @(negedge clk);
    $display("CRC_RESULT|TEST=4|MSG=AB|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      parallel8_out);

    // TEST 5: parallel, DATA_WIDTH=8
    reset_dut;
    feed_parallel_byte(8'hAB);
    feed_parallel_byte(8'hCD);
    @(negedge clk);
    $display("CRC_RESULT|TEST=5|MSG=ABCD|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      parallel8_out);

    // TEST 6: parallel, DATA_WIDTH=4
    reset_dut;
    feed_parallel_nibble(4'hA);
    feed_parallel_nibble(4'h5);
    feed_parallel_nibble(4'hC);
    feed_parallel_nibble(4'h3);
    @(negedge clk);
    $display("CRC_RESULT|TEST=6|MSG=A5C3|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      parallel4_out);

    $finish;
  end

endmodule
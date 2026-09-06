`timescale 1ns/1ns

module tb_crc;
  integer i;

  reg clk, rst;
  always #5 clk = ~clk;

  reg data_bit, data_valid;

  reg [7:0] upd_crc_in;
  reg       upd_data_bit;
  wire [7:0] upd_crc_out;

  wire [7:0] crc8_out;

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

        // Optional monitor for inspecting CRC8_serial execution.
        // Uncomment these lines to see each input bit and CRC transition.
        /*
        $display(
          "[CRC8_SERIAL] bit=%0d | data_bit=%b | CRC_before=%02h",
          i, data_bit, crc8_out
        );
        */

        @(posedge clk);

        /*
        $display(
          "[CRC8_SERIAL] bit=%0d | data_bit=%b | CRC_after=%02h",
          i, data_bit, crc8_out
        );
        */

        @(negedge clk);
        data_valid = 0;
      end
    end
  endtask

  initial begin
    $dumpfile("task2.vcd");
    $dumpvars(0, tb_crc);

    clk = 0;
    rst = 0;

    data_bit = 0;
    data_valid = 0;

    upd_crc_in = 0;
    upd_data_bit = 0;

    // crc8_update sanity check
    upd_crc_in = 8'hFF;
    upd_data_bit = 1;
    #1;
    $display("crc8_update: FF,1 -> %02h", upd_crc_out);

    // TEST 1: single byte
    reset_dut;
    feed_byte(8'hAB);
    @(negedge clk);
    $display("CRC_RESULT|TEST=1|MSG=AB|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      crc8_out);

    // TEST 2: multiple bytes
    reset_dut;
    feed_byte(8'hAB);
    feed_byte(8'hCD);
    @(negedge clk);
    $display("CRC_RESULT|TEST=2|MSG=ABCD|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      crc8_out);

    // TEST 3: all-zero byte
    reset_dut;
    feed_byte(8'h00);
    @(negedge clk);
    $display("CRC_RESULT|TEST=3|MSG=00|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      crc8_out);

    // TEST 4: all-one byte
    reset_dut;
    feed_byte(8'hFF);
    @(negedge clk);
    $display("CRC_RESULT|TEST=4|MSG=FF|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      crc8_out);

    // TEST 5: longer message
    reset_dut;
    feed_byte(8'h12);
    feed_byte(8'h34);
    feed_byte(8'h56);
    feed_byte(8'h78);
    @(negedge clk);
    $display("CRC_RESULT|TEST=5|MSG=12345678|WIDTH=8|POLY=D5|INIT=FF|CRC=%02h",
      crc8_out);

    $finish;
  end

endmodule
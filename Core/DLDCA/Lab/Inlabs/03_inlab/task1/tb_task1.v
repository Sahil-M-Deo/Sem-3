`timescale 1ns/1ns

module tb_traffic_light;

  reg  clk, rst_n, ped_button, tick_1hz;
  wire red, green, yellow;
  wire [1:0] state_out;
  wire [7:0] duration_out;

  bhavesh_traffic_light dut (
    .clk          (clk),
    .rst_n        (rst_n),
    .ped_button   (ped_button),
    .tick_1hz     (tick_1hz),
    .red          (red),
    .green        (green),
    .yellow       (yellow),
    .state_out    (state_out),
    .duration_out (duration_out)
  );

  // ---------------------------------------------------------
  // Standalone instance for unit-testing duration_datapath.
  // ---------------------------------------------------------
  reg  [1:0] dp_current_state, dp_next_state;
  reg  [7:0] dp_remaining;
  reg        dp_tick, dp_ped_button, dp_ped_request;
  wire [7:0] dp_remaining_next;
  wire       dp_do_transition;

  duration_datapath dp_dut (
    .current_state  (dp_current_state),
    .next_state     (dp_next_state),
    .remaining      (dp_remaining),
    .tick_1hz       (dp_tick),
    .ped_button     (dp_ped_button),
    .ped_request    (dp_ped_request),
    .remaining_next (dp_remaining_next),
    .do_transition  (dp_do_transition)
  );

  reg  [1:0] ld_state;
  wire       ld_red, ld_green, ld_yellow;

  output_decoder ld_dut (
    .state  (ld_state),
    .red    (ld_red),
    .green  (ld_green),
    .yellow (ld_yellow)
  );

  always #5 clk = ~clk;

  function [63:0] state_name;
    input [1:0] s;
    begin
      case (s)
        2'b00:   state_name = "GREEN ";
        2'b01:   state_name = "YELLOW";
        2'b10:   state_name = "RED   ";
        default: state_name = "???   ";
      endcase
    end
  endfunction

  task fire_ticks;
    input integer n;
    integer i;
    begin
      for (i = 0; i < n; i = i + 1) begin
        @(posedge clk); tick_1hz <= 1;
        @(posedge clk); tick_1hz <= 0;
      end
    end
  endtask

  task check_state;
    input [1:0]  expected;
    input [63:0] context;
    begin
      @(posedge clk); #1;
      if (state_out !== expected)
        $display("\t[ASSERT] %s -> FAIL: state=%s expected=%s",
          context, state_name(state_out), state_name(expected));
      else
        $display("\t[ASSERT] %s -> PASS", context);
    end
  endtask

  initial begin
    $dumpfile("task1.vcd");
    $dumpvars(0, tb_traffic_light);
  end

  reg [63:0] state_str;
  always @(*) state_str = state_name(state_out);

  initial begin
    $monitor("t=%6d | tick=%b | state=%s remaining=%3d | R=%b G=%b Y=%b | ped=%b | request=%b",
      $time,
      tick_1hz,
      state_str,
      duration_out,
      red, green, yellow,
      ped_button,
      dut.ped_request
    );
  end

  initial begin
    clk        = 0;
    rst_n      = 1;
    ped_button = 0;
    tick_1hz   = 0;

    dp_current_state = 0;
    dp_next_state    = 0;
    dp_remaining     = 0;
    dp_tick          = 0;
    dp_ped_button    = 0;
    dp_ped_request   = 0;

    ld_state = 0;

    // ---------------------------------------------------
    // UNIT TEST: duration_datapath
    // ---------------------------------------------------
    $display("\n=== UNIT TEST: duration_datapath ===");

    // -- Case: idle (no tick, no button) -> remaining holds, no transition
    dp_current_state = 2'b00; dp_next_state = 2'b01;
    dp_remaining = 8'd20; dp_tick = 0; dp_ped_button = 0; dp_ped_request = 0;
    #1;
    $display("  idle, GREEN rem=20        | remaining_next=%0d (exp 20) do_transition=%b (exp 0) | %s",
      dp_remaining_next, dp_do_transition,
      (dp_remaining_next === 8'd20 && dp_do_transition === 1'b0) ? "PASS":"FAIL");

    // -- Case: mid-count tick decrements by 1, no transition
    dp_current_state = 2'b00; dp_next_state = 2'b01;
    dp_remaining = 8'd20; dp_tick = 1; dp_ped_button = 0; dp_ped_request = 0;
    #1;
    $display("  tick, GREEN rem=20->19    | remaining_next=%0d (exp 19) do_transition=%b (exp 0) | %s",
      dp_remaining_next, dp_do_transition,
      (dp_remaining_next === 8'd19 && dp_do_transition === 1'b0) ? "PASS":"FAIL");

    // -- Case: last tick of GREEN, ped_request=0 -> transition into YELLOW=5
    dp_current_state = 2'b00; dp_next_state = 2'b01;
    dp_remaining = 8'd1; dp_tick = 1; dp_ped_button = 0; dp_ped_request = 0;
    #1;
    $display("  last tick, GREEN->YELLOW  | remaining_next=%0d (exp 5) do_transition=%b (exp 1) | %s",
      dp_remaining_next, dp_do_transition,
      (dp_remaining_next === 8'd5 && dp_do_transition === 1'b1) ? "PASS":"FAIL");

    // -- Case: last tick of YELLOW -> transition into RED, ped_request=0 -> 30
    dp_current_state = 2'b01; dp_next_state = 2'b10;
    dp_remaining = 8'd1; dp_tick = 1; dp_ped_button = 0; dp_ped_request = 0;
    #1;
    $display("  last tick, YELLOW->RED ped=0 | remaining_next=%0d (exp 30) | %s",
      dp_remaining_next, dp_remaining_next === 8'd30 ? "PASS":"FAIL");

    // -- Case: last tick of YELLOW -> transition into RED, ped_request=1 -> 40
    dp_current_state = 2'b01; dp_next_state = 2'b10;
    dp_remaining = 8'd1; dp_tick = 1; dp_ped_button = 0; dp_ped_request = 1;
    #1;
    $display("  last tick, YELLOW->RED ped=1 | remaining_next=%0d (exp 40) | %s",
      dp_remaining_next, dp_remaining_next === 8'd40 ? "PASS":"FAIL");

    // -- Case: last tick of RED -> transition into GREEN, always 25
    dp_current_state = 2'b10; dp_next_state = 2'b00;
    dp_remaining = 8'd1; dp_tick = 1; dp_ped_button = 0; dp_ped_request = 1;
    #1;
    $display("  last tick, RED->GREEN     | remaining_next=%0d (exp 25) | %s",
      dp_remaining_next, dp_remaining_next === 8'd25 ? "PASS":"FAIL");

    // -- Case: button pressed during GREEN, no existing request -> clamps to 10
    dp_current_state = 2'b00; dp_next_state = 2'b01;
    dp_remaining = 8'd20; dp_tick = 0; dp_ped_button = 1; dp_ped_request = 0;
    #1;
    $display("  button, GREEN rem=20      | remaining_next=%0d (exp 10) | %s",
      dp_remaining_next, (dp_remaining_next === 8'd10) ? "PASS":"FAIL");

    // -- Case: shrink clamp doesn't lengthen remaining if already below clamp
    dp_current_state = 2'b00; dp_next_state = 2'b01;
    dp_remaining = 8'd6; dp_tick = 0; dp_ped_button = 1; dp_ped_request = 0;
    #1;
    $display("  button, GREEN rem=6       | remaining_next=%0d (exp 1) | %s",
      dp_remaining_next, dp_remaining_next === 8'd1 ? "PASS":"FAIL");

    // -- Case: modifier does NOT fire if ped_request already latched
    dp_current_state = 2'b00; dp_next_state = 2'b01;
    dp_remaining = 8'd20; dp_tick = 0; dp_ped_button = 1; dp_ped_request = 1;
    #1;
    $display("  button, GREEN, latched    | remaining_next=%0d (exp 20) | %s",
      dp_remaining_next, dp_remaining_next === 8'd20 ? "PASS":"FAIL");

    // -- Case: button during RED extends duration (+10)
    dp_current_state = 2'b10; dp_next_state = 2'b00;
    dp_remaining = 8'd20; dp_tick = 0; dp_ped_button = 1; dp_ped_request = 0;
    #1;
    $display("  button, RED rem=20        | remaining_next=%0d (exp 30) | %s",
      dp_remaining_next, (dp_remaining_next === 8'd30) ? "PASS":"FAIL");

    // ---------------------------------------------------
    // UNIT TEST: output_decoder
    // ---------------------------------------------------
    $display("\n=== UNIT TEST: output_decoder ===");

    ld_state = 2'b10; #1;
    $display("  RED    | R=%b G=%b Y=%b (exp 1 0 0) | %s",
      ld_red, ld_green, ld_yellow,
      (ld_red===1 && ld_green===0 && ld_yellow===0) ? "PASS":"FAIL");

    ld_state = 2'b00; #1;
    $display("  GREEN  | R=%b G=%b Y=%b (exp 0 1 0) | %s",
      ld_red, ld_green, ld_yellow,
      (ld_red===0 && ld_green===1 && ld_yellow===0) ? "PASS":"FAIL");

    ld_state = 2'b01; #1;
    $display("  YELLOW | R=%b G=%b Y=%b (exp 0 0 1) | %s",
      ld_red, ld_green, ld_yellow,
      (ld_red===0 && ld_green===0 && ld_yellow===1) ? "PASS":"FAIL");

    // ---------------------------------------------------
    // INTEGRATION TESTS
    // ---------------------------------------------------
    $display("\n=== INTEGRATION TESTS: bhavesh_traffic_light ===");

    @(negedge clk); rst_n = 0;
    repeat(4) @(posedge clk);
    rst_n = 1;

    // TEST 1: Reset -> GREEN
    $display("\n=== TEST 1: Reset -> GREEN ===");
    check_state(2'b00, "after reset");

    // TEST 2: GREEN = 25 ticks, ped=0
    $display("\n=== TEST 2: GREEN = 25 ticks, ped=0 ===");
    ped_button = 0;
    fire_ticks(24);
    check_state(2'b00, "GREEN @ tick 24");

    fire_ticks(1);
    check_state(2'b01, "GREEN -> YELLOW @ tick 25");

    // TEST 3: YELLOW = 5 ticks
    $display("\n=== TEST 3: YELLOW = 5 ticks ===");
    fire_ticks(4);
    check_state(2'b01, "YELLOW @ tick 4");

    fire_ticks(1);
    check_state(2'b10, "YELLOW -> RED @ tick 5");

    // TEST 4: RED = 30 ticks, no ped request -> stays at 30
    $display("\n=== TEST 4: RED = 30 ticks, ped=0 ===");
    fire_ticks(29);
    check_state(2'b10, "RED @ tick 29");

    fire_ticks(1); // 30th tick -> RED -> GREEN
    check_state(2'b00, "RED -> GREEN @ tick 30");

    if (duration_out !== 8'd25)
      $display("\t[ASSERT] GREEN duration after unrequested RED -> FAIL");
    else
      $display("\t[ASSERT] GREEN duration after unrequested RED -> PASS");

    // TEST 5: Pedestrian button during GREEN -> instant shrink by 10
    $display("\n=== TEST 5: Pedestrian button during GREEN -> instant shrink by 10 ===");
    fire_ticks(5); // GREEN remaining: 25 -> 20 after 5 ticks

    @(negedge clk); ped_button = 1;
    @(posedge clk);
    @(negedge clk); ped_button = 0;

    if (dut.ped_request !== 1)
      $display("\t[ASSERT] ped latch on GREEN press -> FAIL");
    else
      $display("\t[ASSERT] ped latch on GREEN press -> PASS");

    if (duration_out !== 8'd10)
      $display("\t[ASSERT] instant shrink to 10 -> FAIL: remaining=%0d expected=10", duration_out);
    else
      $display("\t[ASSERT] instant shrink to 10 -> PASS");

    // TEST 6: remaining counts down normally from the shrunk value
    $display("\n=== TEST 6: GREEN counts down from shrunk value (10 ticks) ===");
    fire_ticks(9);
    check_state(2'b00, "GREEN @ tick 9 post-shrink");

    fire_ticks(1);
    check_state(2'b01, "GREEN -> YELLOW @ tick 10 post-shrink");

    // Cycle through to clear the latched request
    fire_ticks(5);  // YELLOW -> RED (Starts at 40 because of latched request)
    fire_ticks(40); // RED -> GREEN (Clears request)
    check_state(2'b00, "Returned to fresh GREEN");

    // TEST 7: Pedestrian button during YELLOW -> latch request, RED=40
    $display("\n=== TEST 7: Pedestrian button during YELLOW -> latch request, RED=40 ===");
    fire_ticks(25); // Finish GREEN
    check_state(2'b01, "GREEN -> YELLOW");

    @(negedge clk); ped_button = 1;
    @(posedge clk);
    @(negedge clk); ped_button = 0;

    if (dut.ped_request !== 1)
      $display("\t[ASSERT] ped latch on YELLOW press -> FAIL");
    else
      $display("\t[ASSERT] ped latch on YELLOW press -> PASS");

    fire_ticks(5); // Finish YELLOW -> RED
    check_state(2'b10, "YELLOW -> RED");

    if (duration_out !== 8'd40)
      $display("\t[ASSERT] RED duration after YELLOW request -> FAIL: remaining=%0d expected=40", duration_out);
    else
      $display("\t[ASSERT] RED duration after YELLOW request -> PASS");

    fire_ticks(40); // Finish RED -> GREEN (clears request)

    // TEST 8: Pedestrian button during RED -> instant extend by 10
    $display("\n=== TEST 8: Pedestrian button during RED -> instant extend by 10 ===");
    fire_ticks(25); // Finish GREEN
    fire_ticks(5);  // Finish YELLOW -> enters RED(30)
    check_state(2'b10, "Entered RED");

    fire_ticks(10); // RED remaining: 30 -> 20

    @(negedge clk); ped_button = 1;
    @(posedge clk);
    @(negedge clk); ped_button = 0;

    if (dut.ped_request !== 1)
      $display("\t[ASSERT] ped latch on RED press -> FAIL");
    else
      $display("\t[ASSERT] ped latch on RED press -> PASS");

    if (duration_out !== 8'd30)
      $display("\t[ASSERT] instant RED extension to 30 -> FAIL: remaining=%0d expected=30", duration_out);
    else
      $display("\t[ASSERT] instant RED extension to 30 -> PASS");

    fire_ticks(30); // Finish RED
    check_state(2'b00, "RED -> GREEN");

    // TEST 9: Holding ped_button during GREEN only shrinks once
    $display("\n=== TEST 9: holding ped_button during GREEN only shrinks once ===");
    @(negedge clk); ped_button = 1;
    @(posedge clk);
    #1;

    if (duration_out !== 8'd15)
      $display("\t[ASSERT] shrink by 10 on first press (expected 15) -> FAIL: remaining=%0d", duration_out);
    else
      $display("\t[ASSERT] shrink by 10 on first press (expected 15) -> PASS");

    repeat(4) @(posedge clk);

    if (duration_out !== 8'd15)
      $display("\t[ASSERT] remaining stays at 15 while button held -> FAIL: remaining=%0d", duration_out);
    else
      $display("\t[ASSERT] remaining stays at 15 while button held -> PASS");

    @(negedge clk); ped_button = 0;

    $display("\n=== All tests complete ===\n");
    $finish;
  end

endmodule
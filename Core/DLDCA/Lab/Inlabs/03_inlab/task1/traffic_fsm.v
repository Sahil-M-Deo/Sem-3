module bhavesh_traffic_light (
    input clk, rst_n,
    input ped_button,
    input tick_1hz,

    output [1:0] state_out,
    output [7:0] duration_out,
    output red, green, yellow
);

    // TODO

endmodule


// -----------------------------------------------------------
// CONTROL
// -----------------------------------------------------------
module traffic_fsm (
    input clk, rst_n,
    input ped_button,
    input [7:0] remaining_next,
    input do_transition,

    output reg [1:0] current_state,
    output    [1:0] next_state,
    output reg [7:0] remaining,
    output reg       ped_request
);

    localparam GREEN = 2'b00, YELLOW = 2'b01, RED = 2'b10;

    // next-state logic (purely combinational)
    // TODO

    // register updates
    // TODO

endmodule


// -----------------------------------------------------------
// DATAPATH
// -----------------------------------------------------------
module duration_datapath (
    input [1:0] current_state,
    input [1:0] next_state,
    input [7:0] remaining,
    input       tick_1hz,
    input       ped_button,
    input       ped_request,

    output reg [7:0] remaining_next,
    output reg       do_transition
);

    localparam GREEN = 2'b00, YELLOW = 2'b01, RED = 2'b10;

    // TODO: update + modify

endmodule


// -----------------------------------------------------------
// OUTPUT DECODE
// -----------------------------------------------------------
module output_decoder (
    input [1:0] state,
    output red, green, yellow
);

    localparam GREEN = 2'b00, YELLOW = 2'b01, RED = 2'b10;

    assign green  = (state == GREEN);
    assign yellow = (state == YELLOW);
    assign red    = (state == RED);

endmodule
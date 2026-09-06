module switch (
    input        clk,
    input        rst,

    input  [7:0] rx0_data,
    input        rx0_valid,
    input  [7:0] rx1_data,
    input        rx1_valid,
    input  [7:0] rx2_data,
    input        rx2_valid,
    input  [7:0] rx3_data,
    input        rx3_valid,

    output [7:0] tx0_data,
    output       tx0_valid,
    output [7:0] tx1_data,
    output       tx1_valid,
    output [7:0] tx2_data,
    output       tx2_valid,
    output [7:0] tx3_data,
    output       tx3_valid
);

    // TODO: Instantiate one rx_frame for each input port.
    
    // TODO: Learn source addresses and determine egress ports.
    
    // TODO: Instantiate tx_frame(s) for the output ports.
    
    // TODO: Forward known destinations to one port.
    
    // TODO: Flood unknown destinations to all ports except the ingress port!
    
    // TODO: Drop frames that fail CRC.

endmodule

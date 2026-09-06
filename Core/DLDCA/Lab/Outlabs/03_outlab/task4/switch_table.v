module switch_table #(
    parameter ADDR_WIDTH = 8,
    parameter PORT_WIDTH = 2,
    parameter TABLE_SIZE = 8
) (
    input                           clk,
    input                           rst,

    input  [ADDR_WIDTH-1:0]         src_addr,
    input  [ADDR_WIDTH-1:0]         dst_addr,
    input  [PORT_WIDTH-1:0]         ingress_port,
    input                           frame_valid,

    output reg [PORT_WIDTH-1:0]     egress_port,
    output reg                      known_destination
);

    // Array declarations for the learning table
    // Helping you out here!
    reg [ADDR_WIDTH-1:0] table_addrs [0:TABLE_SIZE-1];
    reg [PORT_WIDTH-1:0] table_ports [0:TABLE_SIZE-1];
    reg                  table_valid [0:TABLE_SIZE-1];

    integer i;
    reg     handled; // Flag to track if we found a match or an empty slot

    // Sequential logic for learning/updating the table
    always @(posedge clk) begin
        if (rst) begin
            // Reset all valid bits to 0
            for (i = 0; i < TABLE_SIZE; i = i + 1) begin
                table_valid[i] <= 1'b0;
            end
        end else if (frame_valid) begin
            // Verilog for-loops do not support 'break' statements. 
            // You can use the 'handled' flag to track if you have already 
            // updated an entry or filled an empty slot.
            //
            // handled = 1'b0;
            //
            // Pass 1: Loop through the table to see if src_addr already exists.
            // If it does, update the port and set handled = 1.
            //
            // Pass 2: If !handled, loop again to find the first empty slot.
            // (Remember to check !handled inside this loop's if-condition too, 
            // so you only fill ONE empty slot, not all of them!)
            //
            // TODO: Implement the learning logic here.
        end
    end
    
    // TODO: Add combinational logic (always @*) to look up dst_addr 
    // and set egress_port and known_destination.

endmodule
module dFlipFlop (
    input clk,
    input d,
    output reg q
);
    always @(posedge clk) begin
        q <= d;
    end
endmodule

module tFlipFlop (
    input clk,
    input t,
    output reg q
);
    // alternate: q <= q ^ t;
    always @(posedge clk) begin
        if (t)
            q <= ~q;
        // else not needed
    end
endmodule

module buffer (
    input d,
    output wire q
);
    
    assign q = d;
    
endmodule
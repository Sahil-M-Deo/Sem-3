`timescale 1ns/1ns

module tb;
    reg a, b, sel;
    wire y_dataflow, y_behavioral;
    integer errors;

    mux_dataflow uut_dataflow (
        .a(a),
        .b(b),
        .sel(sel),
        .y(y_dataflow)
    );

    mux_behavioral uut_behavioral (
        .a(a),
        .b(b),
        .sel(sel),
        .y(y_behavioral)
    );

    initial begin
        $monitor("At time %0t: a = %b, b = %b, sel = %b -> dataflow = %b, behavioral = %b",
                  $time, a, b, sel, y_dataflow, y_behavioral);
    end

    task check;
        begin
            #1;
            if (y_dataflow !== y_behavioral) begin
                errors = errors + 1;
                $display("  MISMATCH at time %0t: dataflow = %b, behavioral = %b",
                          $time, y_dataflow, y_behavioral);
            end
        end
    endtask

    initial begin
        errors = 0;
        $dumpfile("task2.vcd");
        $dumpvars(0, tb);

        a = 0; b = 0; sel = 0; check; #9;
        a = 0; b = 1; sel = 0; check; #9;
        a = 0; b = 1; sel = 1; check; #9;
        a = 1; b = 0; sel = 0; check; #9;
        a = 1; b = 0; sel = 1; check; #9;
        a = 1; b = 1; sel = 1; check; #9;

        if (errors == 0)
            $display("PASS: mux_dataflow and mux_behavioral agree on every input.");
        else
            $display("FAIL: %0d mismatch(es) found between mux_dataflow and mux_behavioral.", errors);

        $finish;
    end
endmodule

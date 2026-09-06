TASKS = $(patsubst %/,%,$(wildcard */))
TASKS_VVP = $(patsubst %,%.vvp,$(TASKS))
TASKS_VCD = $(patsubst %,%.vcd,$(TASKS))

.PHONY: all clean

default:
	@printf "Usage: make [task]\n"

$(TASKS): %: %.vcd
	@code $^

$(TASKS_VCD): %.vcd: %.vvp
	@vvp $^

.SECONDEXPANSION:
$(TASKS_VVP): %.vvp: $$(filter-out $$(wildcard $$*/*_solution.v),$$(wildcard $$*/*.v))
	iverilog -o $@ $^

clean:
	@rm -fv $(TASKS_VVP) $(TASKS_VCD)
TASKS = $(patsubst %/,%,$(wildcard */))
TASKS_VVP = $(patsubst %,%.vvp,$(TASKS))
TASKS_VCD = $(patsubst %,%.vcd,$(TASKS))

.PHONY: all clean

default:
	@printf "Usage: make [task]\n"

$(TASKS): %: %.vcd
	@code $^

.PHONY: $(TASKS_VCD)

.SECONDEXPANSION:
$(TASKS_VVP): %.vvp: $$(filter-out $$(wildcard $$*/*_solution.v),$$(wildcard $$*/*.v))
	iverilog -o $@ $^

# -------------------------------------------------------
# Task-specific autograder
# -------------------------------------------------------

RED    = "\\033[31m"
GREEN  = "\\033[32m"
YELLOW = "\\033[33m"
RESET  = "\\033[0m"

task1.vcd: task1.vvp
	@vvp $^ > task1/output.txt
	@cat task1/output.txt
	@diff task1/expected_output.txt task1/output.txt >/dev/null \
		&& printf "$(GREEN)Output matches expected.$(RESET)\n"      \
		|| printf "$(RED)Output does not match expected.$(RESET)\n"

task2.vcd: task2.vvp
	@vvp $^
	@python3 task2/crc_verifier.py --autograde

labexam.vcd: labexam.vvp
	@vvp $^ > labexam/output.txt
	@cat labexam/output.txt
	@diff labexam/expected_output.txt labexam/output.txt >/dev/null \
		&& printf "$(GREEN)Output matches expected.$(RESET)\n"      \
		|| printf "$(RED)Output does not match expected.$(RESET)\n"

clean:
	@rm -fv $(TASKS_VVP) $(TASKS_VCD)
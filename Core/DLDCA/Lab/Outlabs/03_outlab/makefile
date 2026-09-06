TASKS = $(patsubst %/,%,$(wildcard */))
TASKS_VVP = $(patsubst %,%.vvp,$(TASKS))
TASKS_VCD = $(patsubst %,%.vcd,$(TASKS))

.PHONY: all clean default task5

default:
	@printf "Usage: make [task]\n"

$(TASKS): %: %.vcd
	@code $^

$(TASKS_VCD): %.vcd: %.vvp
	@vvp $^

TASK_DEPS_task1 =
TASK_DEPS_task2 =
TASK_DEPS_task3 = task1/crc.v task2/encoder.v task2/decoder.v
TASK_DEPS_task4 =
TASK_DEPS_task5 = task1/crc.v \
                  task2/encoder.v task2/decoder.v \
                  task3/rx_frame.v task3/tx_frame.v \
                  task4/switch_table.v

.SECONDEXPANSION:
$(TASKS_VVP): %.vvp: $$(filter-out $$(wildcard $$*/*_solution.v),$$(wildcard $$*/*.v)) $$(TASK_DEPS_$$*)
	iverilog -o $@ $^

# -------------------------------------------------------
# Task-specific autograder
# -------------------------------------------------------

RED    = "\\033[31m"
GREEN  = "\\033[32m"
YELLOW = "\\033[33m"
RESET  = "\\033[0m"

task1.vcd: task1.vvp
	@vvp $^
	@python3 task1/crc_verifier.py --autograde

task3.vcd: task3.vvp
	@vvp $^ 2>&1 > task3_output.txt
	@cat task3_output.txt
	@python3 task3/verify_task3.py task3_output.txt

task5:
	@python3 task5/interactive_tester.py --setup-autograder --tb-file task5/tb_task5.v
	@iverilog task5/switch_solution.v task5/tb_task5.v task1/crc_solution.v task2/encoder_solution.v task2/decoder_solution.v task3/rx_frame_solution.v task3/tx_frame_solution.v task4/switch_table_solution.v -o task5.vvp
	@vvp task5.vvp 2>&1 > task5_output.txt
	@echo 'Run `cat task5_output.txt` to view the output log'
	@python3 task5/interactive_tester.py --run-autograder --output-file task5_output.txt

clean:
	@rm -fv $(TASKS_VVP) $(TASKS_VCD)
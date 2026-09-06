CC     = gcc
CFLAGS = -Wall -Wextra -no-pie
NASM   = nasm
NFLAGS = -f elf64

RED    = "\\033[31m"
GREEN  = "\\033[32m"
YELLOW = "\\033[33m"
RESET  = "\\033[0m"

.PHONY: all run clean

all: asm

run: asm
	@./asm; \
	status=$$?; \
	if [ $$status -eq 0 ]; then \
		printf "$(GREEN)exit code: %d$(RESET)\n" "$$status"; \
	else \
		printf "$(RED)exit code: %d$(RESET)\n" "$$status"; \
	fi

clean:
	@rm -fv asm main.o helpers.o
	

asm: main.o helpers.o
	$(CC) $(CFLAGS) -nostartfiles $^ -o $@

main.o: main.asm
	$(NASM) $(NFLAGS) $< -o $@

helpers.o: helpers.c
	$(CC) -c $< -o $@

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)
EXE = ponii
CC = gcc

CFLAGS := -Wall -Wextra -g -O2 -fdiagnostics-color=auto

GENERATED_TABLES = opcode_table.c thumb_opcode_table.c addressing_mode1_table.c addressing_mode2_table.c addressing_mode3_table.c addressing_mode4_table.c

all:
	./LUT_generator.py opcode_list.txt opcode_table.c 12 opcode_handlers.h opcode_func_table opcode_lookup_table
	./LUT_generator.py thumb_opcode_list.txt thumb_opcode_table.c 11 thumb_opcode_handlers.h thumb_opcode_func_table thumb_opcode_lookup_table
	./LUT_generator.py addressing_mode1_list.txt addressing_mode1_table.c 5 addressing_mode1_handlers.h addressing_mode1_func_table addressing_mode1_lookup_table
	./LUT_generator.py addressing_mode2_list.txt addressing_mode2_table.c 4 addressing_mode2_handlers.h addressing_mode2_func_table addressing_mode2_lookup_table
	./LUT_generator.py addressing_mode3_list.txt addressing_mode3_table.c 3 addressing_mode3_handlers.h addressing_mode3_func_table addressing_mode3_lookup_table
	./LUT_generator.py addressing_mode4_list.txt addressing_mode4_table.c 2 addressing_mode4_handlers.h addressing_mode4_func_table addressing_mode4_lookup_table
	make $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXE)

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f *.o $(EXE) $(GENERATED_TABLES)
	touch $(GENERATED_TABLES)

.PHONY: clean all

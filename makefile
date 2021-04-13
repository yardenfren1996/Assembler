#compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags

## Executable
all: assembler clean

assembler: assembler.o create_outputs.o second_parse.o first_parse.o line_handler.o table.o utils.o
	$(CC) -g $(CFLAGS) assembler.o create_outputs.o second_parse.o first_parse.o line_handler.o table.o utils.o -o assembler -lm

assembler.o: assembler.c utils.h
	$(CC) -g -c $(CFLAGS) assembler.c -o assembler.o -lm
create_outputs.o: create_outputs.c utils.h
	$(CC) -g -c $(CFLAGS) create_outputs.c -o create_outputs.o -lm
second_parse.o: second_parse.c utils.h
	$(CC) -g -c $(CFLAGS) second_parse.c -o second_parse.o -lm
first_parse.o: first_parse.c utils.h
	$(CC) -g -c $(CFLAGS) first_parse.c -o first_parse.o -lm
line_handler.o: line_handler.c utils.h saved_names.h
	$(CC) -g -c $(CFLAGS) line_handler.c -o line_handler.o -lm
table.o: table.c utils.h
	$(CC) -g -c $(CFLAGS) table.c -o table.o -lm
utils.o: utils.c utils.h
	$(CC) -g -c $(CFLAGS) utils.c -o utils.o -lm

clean:
	rm -f *.o


##assembler.o create_outputs.o second_parse.o first_parse.o line_handler.o -f table.o utils.o assembler

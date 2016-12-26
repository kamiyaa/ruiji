# Makefile

include config.mk

all: ruiji

ruiji: $(HELPERS) ruiji.c
	${CC} $(LIBS) $(STD) $(HELPERS) ruiji.c -o ruiji

test_parser: parser.c test_parser.c
	${CC} $(STD) $(LIBS) parser.c test_parser.c -o test_parser

clean:
	rm -v *.o ruiji

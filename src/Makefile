# Makefile

include config.mk

all: ruiji

ruiji: $(HELPERS) ruiji.c
	${CC} ${LIBS} ${HELPERS} ruiji.c -o ruiji

test_parser: parser.c test_parser.c
	${CC} ${LIBS} parser.c test_parser.c -o test_parser

install:
	@echo installing to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	cp ruiji ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/ruiji

uninstall:
	@rm -f ${PREFIX}/bin/ruiji

clean:
	rm -v *.o ruiji

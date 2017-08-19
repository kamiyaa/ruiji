# Makefile

include config.mk

all: ruiji

config.h: config.def.h
	@echo "cp config.def.h -> config.h"
	cp config.def.h config.h

ruiji: config.h ${HELPERS} ruiji.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}
	${CC} ${CFLAGS} ${USE_FLAGS} -c ${HELPERS} ruiji.c
	${CC} ${CFLAGS} ${USE_FLAGS} ${HELPERS_OBJ} ruiji.o -o ruiji ${LIBS}

test_tags: $(HELPERS) test_tags.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}
	${CC} ${CFLAGS} ${USE_FLAGS} -c ${HELPERS} test_tags.c
	${CC} ${CFLAGS} ${USE_FLAGS} ${HELPERS_OBJ} test_tags.o -o test_tags ${LIBS}

install:
	@echo installing to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	cp ruiji ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/ruiji
	@mkdir -p ${PREFIX}/share/man/man1
	cp ../man/ruiji.1 ${PREFIX}/share/man/man1/ruiji.1
	chmod 644 ${PREFIX}/share/man/man1/ruiji.1

uninstall:
	@rm -f ${PREFIX}/bin/ruiji
	@rm -f ${PREFIX}/share/man/man1/ruiji.1

clean:
	rm -v *.o ruiji

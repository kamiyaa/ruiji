# Makefile

include config.mk

all: ruiji

ruiji: $(HELPERS) ruiji.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}
	${CC} ${CFLAGS} ${USE_FLAGS} -c ${HELPERS} ruiji.c
	${CC} ${CFLAGS} ${USE_FLAGS} ${HELPERS_OBJ} ruiji.o -o ruiji ${LIBS}

test: $(HELPERS) test.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}
	${CC} ${CFLAGS} ${USE_FLAGS} -c ${HELPERS} test.c
	${CC} ${CFLAGS} ${USE_FLAGS} ${HELPERS_OBJ} test.o -o test ${LIBS}

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

clean:
	rm -v *.o ruiji

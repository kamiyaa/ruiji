# Makefile

include config.mk

all: ruiji

ruiji: $(HELPERS) ruiji.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}
	${CC} ${CFLAGS} ${USE_FLAGS} ${LIBS} ${HELPERS} ruiji.c -o ruiji

install:
	@echo installing to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	cp ruiji ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/ruiji

uninstall:
	@rm -f ${PREFIX}/bin/ruiji

clean:
	rm -v ruiji

# Makefile

include config.mk

all: echos ruiji

echos:
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}

structs.o: structs.c
	${CC} ${CFLAGS} -c structs.c
helpers.o: helpers.c
	${CC} ${CFLAGS} -c helpers.c
animegallery.o: animegallery.c
	${CC} ${CFLAGS} -c animegallery.c
animepictures.o: animepictures.c
	${CC} ${CFLAGS} -c animepictures.c
danbooru.o: danbooru.c
	${CC} ${CFLAGS} -c danbooru.c
eshuushuu.o: eshuushuu.c
	${CC} ${CFLAGS} -c eshuushuu.c
gelbooru.o: gelbooru.c
	${CC} ${CFLAGS} -c gelbooru.c
konachan.o: konachan.c
	${CC} ${CFLAGS} -c konachan.c
mangadrawing.o: mangadrawing.c
	${CC} ${CFLAGS} -c mangadrawing.c
sankakucomplex.o: sankakucomplex.c
	${CC} ${CFLAGS} -c sankakucomplex.c
yandere.o: yandere.c
	${CC} ${CFLAGS} -c yandere.c
zerochan.o: zerochan.c
	${CC} ${CFLAGS} -c zerochan.c

interface.o: interface.c
	${CC} ${CFLAGS} ${USE_FLAGS} -c interface.c
parser.o: parser.c
	${CC} ${CFLAGS} -c parser.c
udload.o: udload.c
	${CC} ${CFLAGS} -c udload.c


config.h: config.def.h
	@echo "cp -u config.def.h -> config.h"
	cp -u config.def.h config.h

ruiji: config.h ${HELPERS_OBJ} ruiji.c config.mk
	${CC} ${CFLAGS} ${USE_FLAGS} -c ruiji.c
	${CC} ${CFLAGS} ${USE_FLAGS} ${HELPERS_OBJ} ruiji.o -o ruiji ${LIBS}

test: $(HELPERS) tester.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}
	${CC} ${CFLAGS} ${USE_FLAGS} -c ${HELPERS} tester.c
	${CC} ${CFLAGS} ${USE_FLAGS} ${HELPERS_OBJ} tester.o -o tester ${LIBS}

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

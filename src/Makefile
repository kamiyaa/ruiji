# Makefile

include config.mk

WEBSRC = animegallery.c animepictures.c danbooru.c eshuushuu.c \
	gelbooru.c konachan.c mangadrawing.c sankakucomplex.c \
	yandere.c zerochan.c

HELPERS = ${WEBSRC} util.c interface.c parser.c structs.c udload.c

HELPERS_OBJ = ${HELPERS:.c=.o}

all: ruiji

structs.o: structs.c structs.h
	${CC} ${CFLAGS} -c $<
util.o: util.c util.h
	${CC} ${CFLAGS} -c $<
animegallery.o: animegallery.c animegallery.h
	${CC} ${CFLAGS} -c $<
animepictures.o: animepictures.c animepictures.h
	${CC} ${CFLAGS} -c $<
danbooru.o: danbooru.c danbooru.h
	${CC} ${CFLAGS} -c $<
eshuushuu.o: eshuushuu.c eshuushuu.h
	${CC} ${CFLAGS} -c $<
gelbooru.o: gelbooru.c gelbooru.h
	${CC} ${CFLAGS} -c $<
konachan.o: konachan.c konachan.h
	${CC} ${CFLAGS} -c $<
mangadrawing.o: mangadrawing.c mangadrawing.h
	${CC} ${CFLAGS} -c $<
sankakucomplex.o: sankakucomplex.c sankakucomplex.h
	${CC} ${CFLAGS} -c $<
yandere.o: yandere.c yandere.h
	${CC} ${CFLAGS} -c $<
zerochan.o: zerochan.c zerochan.h
	${CC} ${CFLAGS} -c $<

interface.o: interface.c interface.h
	${CC} ${CFLAGS} ${USE_FLAGS} -c $<
parser.o: parser.c parser.h
	${CC} ${CFLAGS} -c $<
udload.o: udload.c udload.h
	${CC} ${CFLAGS} -c $<


config.h: config.def.h
	@echo "cp -u config.def.h -> config.h"
	cp -u config.def.h config.h

ruiji: config.h ${HELPERS_OBJ} ruiji.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "USE     =" ${USE_FLAGS}
	@echo "VERSION =" ${VERSION}

	${CC} -DVERSION=\"${VERSION}\" ${CFLAGS} ${USE_FLAGS} -c ruiji.c
	${CC} ${CFLAGS} ${USE_FLAGS} ${HELPERS_OBJ} ruiji.o -o $@ ${LIBS}

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

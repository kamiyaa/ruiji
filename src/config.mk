# compiler and linker
CC = clang

COLORFLAG = -DCOLOR

USE_FLAGS = ${COLORFLAG}

# flags
CFLAGS = -std=gnu99 -ggdb3 -O0 -Wall -Werror -Wno-unused-variable

LIBS = -lcurl

WEBSRC = danbooru.c eshuushuu.c gelbooru.c konachan.c sankakucomplex.c yandere.c zerochan.c
HELPERS = $(WEBSRC) interface.c parser.c udload.c

# LIBS = -lncurses `pkg-config --cflags --libs gtk+-3.0`

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man


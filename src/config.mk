# compiler and linker
CC = clang

# flags
CFLAGS = -std=gnu99 -ggdb3 -O0 -Wall -Werror -Wno-unused-variable

LIBS = -lcurl

WEBSRC = danbooru.c sankakucomplex.c
HELPERS = $(WEBSRC) parser.c udload.c

# LIBS = -lncurses `pkg-config --cflags --libs gtk+-3.0`

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man


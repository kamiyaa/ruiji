# compiler and linker
CC = clang

# Comment to remove color output support
COLORFLAG = -DCOLOR

# All optional features
USE_FLAGS = ${COLORFLAG}

# cflags
CFLAGS = -std=gnu99 -ggdb3 -O0 -Wall -Werror -Wno-unused-variable

# libraries required
LIBS = -lcurl

# helper files for specific websites
WEBSRC = danbooru.c eshuushuu.c gelbooru.c konachan.c sankakucomplex.c yandere.c zerochan.c
# Helper files to for main program
HELPERS = $(WEBSRC) interface.c parser.c udload.c

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

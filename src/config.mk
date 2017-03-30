# compiler and linker
# change to clang for clang compiler
CC = gcc

# Color output support, comment to disable this feature
COLORFLAG = -DCOLOR

# All optional features
USE_FLAGS = ${COLORFLAG}

# cflags
CFLAGS = -std=gnu99 -ggdb3 -O0 -Wall -Werror -Wno-unused-variable -march=native

# libraries required
LIBS = -lcurl

# helper files for specific websites
WEBSRC = danbooru.c eshuushuu.c gelbooru.c konachan.c mangadrawing.c sankakucomplex.c yandere.c zerochan.c
# helper files to for main program
HELPERS = ${WEBSRC} interface.c parser.c udload.c

# object files
HELPERS_OBJ = ${HELPERS:.c=.o}

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

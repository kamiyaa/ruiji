# compiler and linker
CC = cc

# standard library to use
STD = -std=c99

# cflags
CFLAGS = ${STD} -ggdb3 -Os -Wall -Werror -Wno-unused-variable -march=native -fno-stack-protector

# libraries required
LIBS = -lcurl

# Color output support, comment to disable this feature
COLORFLAG = -DCOLOR

# All optional features
USE_FLAGS = ${COLORFLAG}

# helper files for specific websites
WEBSRC = danbooru.c eshuushuu.c gelbooru.c konachan.c mangadrawing.c sankakucomplex.c yandere.c zerochan.c
# helper files to for main program
HELPERS = ${WEBSRC} interface.c parser.c udload.c

# object files
HELPERS_OBJ = ${HELPERS:.c=.o}

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

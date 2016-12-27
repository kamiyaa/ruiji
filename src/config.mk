# compiler and linker
CC = clang
CFLAGS = -ggdb3 -O0 -Wall -Werror -Wno-unused-variable

STD = -std=gnu99

LIBS = -lcurl

SOURCES = danbooru.c sankakucomplex.c
HELPERS = $(SOURCES) parser.c udload.c

# LIBS = -lncurses `pkg-config --cflags --libs gtk+-3.0`

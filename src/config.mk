# compiler and linker
CC = clang

CFLAGS = -ggdb3 -O0 -Wall -Werror -Wno-unused-variable
# LIBS = -lncurses `pkg-config --cflags --libs gtk+-3.0`
LIBS = -lcurl
STD = -std=gnu99

HELPERS = danbooru.c parser.c udload.c

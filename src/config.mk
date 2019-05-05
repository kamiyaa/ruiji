VERSION = 0.7.0

# compiler and linker
CC = cc

# standard library to use
STD = 

# cflags
CFLAGS = ${STD} -ggdb3 -Os -Wall -march=native

# libraries required
LIBS = -lcurl

# Color output support, comment to disable this feature
COLORFLAG = -DCOLOR

# All optional features
USE_FLAGS = ${COLORFLAG}

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

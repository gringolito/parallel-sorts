#***********************************************************************
#
#  Copyright (c) 2015 Filipe Utzig <filipeutzig@gmail.com>
#  All Rights Reserved
#
#***********************************************************************

#CFLAGS=-Wall -Wextra -std=c99 -pipe -Og -g3
CFLAGS=-Wall -Wextra -std=c99 -pipe -O2 -J3
LDFLAGS=
INC_PATH=-I../include
DEBUG=yes
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

ifeq (yes,$(DEBUG))
override EXTRA_CFLAGS += -DDEBUG
endif
ifeq (YES,$(DEBUG))
override EXTRA_CFLAGS += -DDEBUG
endif
ifeq (1,$(DEBUG))
override EXTRA_CFLAGS += -DDEBUG
endif

.PHONY: all default clean

default:    clean $(OBJ)

all:        $(OBJ)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(EXTRA_CFLAGS) $(INC_PATH)
	@echo "[CC] $<"

clean:
	@$(RM) *.o *~


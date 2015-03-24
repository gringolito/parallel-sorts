#***********************************************************************
#
# Copyright (c) 2015 Filipe Utzig <filipeutzig@gmail.com>
# All Rights Reserved
#
#***********************************************************************

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pipe -Og -g3
#CFLAGS=-Wall -Wextra -std=c99 -pipe -O3 -J3 -march=native
LDFLAGS=
EXTRA_CFLAGS=-DPROG_NAME=\"$(TARGET)\"
DEBUG=no
LIB_PATH=../../lib
LIB_SRC=$(wildcard $(LIB_PATH)/*.c)
LIBS=$(LIB_SRC:.c=.o)
SRC=$(wildcard *.c)
OBJ=$(LIBS) $(SRC:.c=.o)
INC_PATH=-I../../include
TARGET=sequential
PREFIX=../..
INSTALL=install -m 0755

ifeq (yes,$(DEBUG))
override EXTRA_CFLAGS += -DDEBUG
endif
ifeq (YES,$(DEBUG))
override EXTRA_CFLAGS += -DDEBUG
endif
ifeq (1,$(DEBUG))
override EXTRA_CFLAGS += -DDEBUG
endif

.PHONY  : all default install clean mrproper $(TARGET)

default : clean $(TARGET)

all     : $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) -o $@
	@echo "[LD] $(OBJ) -> $@"

%.o     : %.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(EXTRA_CFLAGS) $(INC_PATH)
	@echo "[CC] $<"

install : $(TARGET)
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/bin || mkdir $(PREFIX)/bin
	@$(INSTALL) $(TARGET) $(PREFIX)/bin

clean   :
	@$(RM) *.o *~ $(TARGET)

mrproper: clean
	@cd $(PREFIX)/bin && $(RM) $(TARGET)


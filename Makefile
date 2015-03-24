#***********************************************************************
#
# Copyright (c) 2015 Filipe Utzig <filipeutzig@gmail.com>
# All Rights Reserved
#
#***********************************************************************
#
# You probably don't need to modify anything in this Makefile.
#
APPS:=
LIBS:=lib
DIRS:=src lib

.PHONY : $(APPS) $(LIBS) install clean

all : $(APPS)

libs : $(LIBS)

lib:
	@$(MAKE) -C lib

install: $(APPS)
	@for dir in $(APPS); do \
	$(MAKE) -C src/$$dir install; \
	done

clean:
	@for dir in $(DIRS); do \
	$(MAKE) -C $$dir clean; \
	done


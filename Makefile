#***********************************************************************
#
# Copyright (c) 2015 Filipe Utzig <filipeutzig@gmail.com>
# All Rights Reserved
#
#***********************************************************************

#
# You probably don't need to modify anything in this Makefile.
#

APPS:=sequential parallel
LIBS:=lib
DIRS:=src/sequential src/parallel lib

.PHONY : $(APPS) $(LIBS) mpi install clean

all    : $(APPS)

libs   : $(LIBS)

$(LIBS):
	@$(MAKE) -C $@

$(APPS): $(LIBS)
	@$(MAKE) -C src/$@

mpi    : parallel

install: $(APPS)
	@for dir in $(APPS); do \
		$(MAKE) -C src/$$dir install; \
	done

clean:
	@for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done


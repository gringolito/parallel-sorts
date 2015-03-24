#***********************************************************************
#
#  Copyright (c) 2015 Filipe Utzig <filipeutzig@gmail.com>
#  All Rights Reserved
#
#***********************************************************************

#
# You probably don't need to modify anything in this Makefile.
#

APPS:=parallel sequential
LIBS:=lib
DIRS:=src/parallel src/sequential lib

.PHONY : $(APPS) $(LIBS) mpi install clean

all    : $(APPS)

libs   : $(LIBS)

lib:
	@$(MAKE) -C lib

sequential:
	@$(MAKE) -C src/sequential

parallel:
	@$(MAKE) -C src/parallel

mpi: parallel

install: $(APPS)
	@for dir in $(APPS); do \
		$(MAKE) -C src/$$dir install; \
	done
clean:
	@for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done


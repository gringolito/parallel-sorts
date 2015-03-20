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
DIRS:=parallel sequential lib

.PHONY : $(APPS) $(LIBS) mpi install clean

all    : $(APPS)

libs   : $(LIBS)

lib:
	$(MAKE) -C lib

sequential:
	$(MAKE) -C sequential

parallel:
	$(MAKE) -C parallel

mpi: parallel

install: $(APPS)
	@for dir in $(APPS); do \
		$(MAKE) -C $$dir install; \
	done
clean:
	@for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done


#!/bin/bash
# run_tests.sh
#
# "THE BEER-WARE LICENSE" (Revision 42):
# <filipeutzig@gmail.com> wrote this file. As long as you retain this
# notice you can do whatever you want with this stuff. If we meet some
# day, and you think this stuff is worth it, you can buy me a beer in
# return.
#
# Initial version by Filipe Utzig <filipeutzig@gmail.com> on 4/6/15.
#
# The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
# "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in
# this document are to be interpreted as described in RFC 2119.
#
# Script to automize parallel and sequential test execution
#
SEQ_BIN=bin/sequential
PAR_BIN=bin/parallel
LOG_DIR=var/log
MPI_BIN=mpirun
MPI_ARGS=-n
VECTOR=var/vetor.txt
ELEMENTS=$(seq 50000 25000 100000)
EXECUTIONS=$(seq 1 29)
PROCESSORS=$(seq 2 4)

print_usage() {
	echo -e "Usage: $0 OPTION"
	echo -e "\nOptions:"
	echo -e "\t-lad\tLAD environment setup"
	echo -e "\t-linux\tGeneric Linux environment setup (default)"
	exit 0
}

if [ ! -x $SEQUENTIAL ]; then
	echo -e "Invalid non-executable file $1"
	exit 1
fi
if [ ! -x $PARALLEL ]; then
	echo -e "Invalid non-executable file $2"
	exit 1
fi

if [ "$1" == "--help" ]; then
	print_usage
fi

if [ "$1" == "-lad" ]; then
	MPI_BIN=ladrun
	MPI_ARGS=-np
fi

if [ ! -d $LOG_DIR ]; then
	mkdir $LOG_DIR
fi

############################################################################
# Sequential sampling
for i in $ELEMENTS; do
	LOG_FILE=$LOG_DIR/sequential_${i}.log
	echo -e "" > $LOG_FILE
	echo -ne "Running $SEQ_BIN with $i elements! Execution:  "
	for e in $EXECUTIONS; do
		if [ $e -gt 10 ]; then
			echo -ne "\b"
		fi
		echo -ne "\b$e"
		$SEQ_BIN $VECTOR $i >> $LOG_FILE
	done
	echo -e ""
	echo -e "\tResults can be found at $LOG_FILE"
done

# Parallel sampling
for i in $ELEMENTS; do
	LOG_FILE=$LOG_DIR/parallel_${i}.log
	echo -e "" > $LOG_FILE
	for p in $PROCESSORS; do
		echo -e "###################" >> $LOG_FILE
		echo -e "# $p Processors" >> $LOG_FILE
		echo -e "###################" >> $LOG_FILE
		echo -ne "Running $PAR_BIN with $p processors and $i elements! Execution:  "
		for e in $EXECUTIONS; do
			if [ $e -gt 10 ]; then
				echo -ne "\b"
			fi
			echo -ne "\b$e"
			$MPI_BIN $MPI_ARGS $p $PAR_BIN $VECTOR $i >> $LOG_FILE
		done
		echo -e ""
	done
	echo -e "\tResults can be found at $LOG_FILE"
done


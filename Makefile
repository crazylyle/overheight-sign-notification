##################################################################
#
#   make variables
#
CC=gcc
CFLAGS = -O0 -ggdb3 -Wall

#LDFLAGS = -lmoxa_rtu -lrtu_common -ltag -lm -Wl,--no-warn-mismatch -Wl,-rpath,/lib/RTU/ -Wl,--allow-shlib-undefined -lpthread
#LDFLAGS = -lmoxa_rtu -lrtu_common
#LDFLAGS = -lm -lpthread

##################################################################
#
#   compilation
#

overhead:  overhead.o dio_dummy.o


##################################################################
#
#   dependencies
#


##################################################################
#
#   clean
#
clean:
	rm -rf overhead *.o

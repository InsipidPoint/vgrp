CC = g++
CCMACFLAGS = -bind_at_load `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
SRC = Camera.cpp vgrp.cpp

SUFFIX = $(firstword $(suffix $(SRC)))
OBJ = $(SRC:$(SUFFIX)=.o)

all:
	$(CC) $(CCMACFLAGS) $(SRC) -o vgrp $(LIBS)
  
clean:
	rm -f *.o core vgrp
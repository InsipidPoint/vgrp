CC = g++
CCMACFLAGS = -bind_at_load `pkg-config --cflags opencv` -g
LIBS = `pkg-config --libs opencv`
SRC = Camera.cpp vgrp.cpp Detector.cpp

SUFFIX = $(firstword $(suffix $(SRC)))
OBJ = $(SRC:$(SUFFIX)=.o)

all:
	$(CC) $(CCMACFLAGS) $(SRC) -o vgrp $(LIBS)
  
clean:
	rm -f *.o core vgrp
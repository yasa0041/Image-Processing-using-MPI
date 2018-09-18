CFLAGS=-g -w -O2 -Wall
CC=mpicc
DEPS=a1.h
PROGRAM_NAME= ppmf
OBJS = main.o readwriteppm.o processimage.o 

$(PROGRAM_NAME): $(OBJS)
	$(CC) -o $@ $? 

clean:
	rm  *.o $(PROGRAM_NAME) *~ output.ppm

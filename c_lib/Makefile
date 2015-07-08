# CC and CFLAGS are varilables 
CC=gcc -g -Wall
CFLAGS =
OBJS=fuzzy.o
# -c option ask g++ to compile the source files, but do not link.
# -g option is for debugging

all: demo

demo: main.c $(OBJS)
	$(CC) -o $@ main.c $(OBJS)

.c.o:
	$(CC) -o $@ -c $<

# clean all the .o and executable files
clean:
	rm -rf *.o demo


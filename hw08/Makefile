CC = cc
CFLAGS = -Wall -Wextra -g

all: treestats

treestats: treestats.o bst.o
	$(CC) $(CFLAGS) -o treestats treestats.o bst.o

treestats.o: treestats.c bst.h
	$(CC) $(CFLAGS) -c treestats.c

bst.o: bst.c bst.h
	$(CC) $(CFLAGS) -c bst.c

tests/test_bst.o: tests/test_bst.c bst.h
	$(CC) $(CFLAGS) -c tests/test_bst.c -o tests/test_bst.o

tests/test_bst: tests/test_bst.o bst.o
	$(CC) $(CFLAGS) -o tests/test_bst tests/test_bst.o bst.o

test: tests/test_bst
	./tests/test_bst

memcheck:
	$(MAKE) clean
	$(MAKE) test CFLAGS="-Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer"
	$(MAKE) clean

leaks: tests/test_bst
	leaks --atExit -- ./tests/test_bst

clean:
	rm -f treestats *.o tests/*.o tests/test_bst

.PHONY: all test memcheck leaks clean

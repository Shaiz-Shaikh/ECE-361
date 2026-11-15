CC = cc
CFLAGS = -Wall -Wextra -g

all: balance

balance: balance.o istack.o
	$(CC) $(CFLAGS) -o balance balance.o istack.o

balance.o: balance.c istack.h
	$(CC) $(CFLAGS) -c balance.c

istack.o: istack.c istack.h
	$(CC) $(CFLAGS) -c istack.c

tests/test_istack.o: tests/test_istack.c istack.h
	$(CC) $(CFLAGS) -c tests/test_istack.c -o tests/test_istack.o

tests/test_istack: tests/test_istack.o istack.o
	$(CC) $(CFLAGS) -o tests/test_istack tests/test_istack.o istack.o

test: balance tests/test_istack
	./tests/test_istack
	./regress.sh

memcheck:
	$(MAKE) clean
	$(MAKE) test CFLAGS="-Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer"
	$(MAKE) clean

leaks: tests/test_istack
	leaks --atExit -- ./tests/test_istack

clean:
	rm -f balance *.o tests/*.o tests/test_istack actual.tmp

.PHONY: all test memcheck leaks clean

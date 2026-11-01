CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

lock: main.o lock.o
	$(CC) $(CFLAGS) -o lock main.o lock.o

main.o: main.c lock.h
	$(CC) $(CFLAGS) -c main.c

lock.o: lock.c lock.h
	$(CC) $(CFLAGS) -c lock.c

test: test_lock
	./test_lock

test_lock: tests/test_lock.o lock.o
	$(CC) $(CFLAGS) -o test_lock tests/test_lock.o lock.o

tests/test_lock.o: tests/test_lock.c lock.h
	$(CC) $(CFLAGS) -I. -c tests/test_lock.c -o tests/test_lock.o

bonus: test_check
	./test_check

test_check: tests/test_check.o check.o lock.o
	$(CC) $(CFLAGS) -o test_check tests/test_check.o check.o lock.o

check.o: check.c check.h lock.h
	$(CC) $(CFLAGS) -c check.c

tests/test_check.o: tests/test_check.c check.h lock.h
	$(CC) $(CFLAGS) -I. -c tests/test_check.c -o tests/test_check.o

clean:
	rm -f lock test_lock test_check *.o tests/*.o

.PHONY: test bonus clean

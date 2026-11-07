CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

replay: main.o sensor.o panel.o scenario.o iom361_r4.o
	$(CC) $(CFLAGS) -o replay main.o sensor.o panel.o scenario.o iom361_r4.o -lm

main.o: main.c iom361_r4.h panel.h scenario.h sensor.h
	$(CC) $(CFLAGS) -c main.c

sensor.o: sensor.c sensor.h iom361_r4.h
	$(CC) $(CFLAGS) -c sensor.c

panel.o: panel.c panel.h iom361_r4.h
	$(CC) $(CFLAGS) -c panel.c

scenario.o: scenario.c scenario.h
	$(CC) $(CFLAGS) -c scenario.c

iom361_r4.o: iom361_r4.c iom361_r4.h
	$(CC) $(CFLAGS) -c iom361_r4.c

test: test_hw06 replay
	./test_hw06
	./regress.sh

test_hw06: tests/test_hw06.o sensor.o panel.o scenario.o iom361_r4.o
	$(CC) $(CFLAGS) -o test_hw06 tests/test_hw06.o sensor.o panel.o scenario.o iom361_r4.o -lm

tests/test_hw06.o: tests/test_hw06.c iom361_r4.h panel.h scenario.h sensor.h
	$(CC) $(CFLAGS) -I. -c tests/test_hw06.c -o tests/test_hw06.o

clean:
	rm -f replay test_hw06 *.o tests/*.o

.PHONY: test clean

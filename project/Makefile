CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
OBJS = main.o sensor.o panel.o scenario.o bits.o fsm.o store.o aux.o report.o iom361_r4.o

all: weather

weather: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o weather -lm

main.o: main.c fsm.h iom361_r4.h panel.h scenario.h sensor.h store.h
	$(CC) $(CFLAGS) -c main.c -o main.o

sensor.o: sensor.c sensor.h iom361_r4.h
	$(CC) $(CFLAGS) -c sensor.c -o sensor.o

panel.o: panel.c panel.h iom361_r4.h
	$(CC) $(CFLAGS) -c panel.c -o panel.o

scenario.o: scenario.c scenario.h
	$(CC) $(CFLAGS) -c scenario.c -o scenario.o

bits.o: bits.c bits.h
	$(CC) $(CFLAGS) -c bits.c -o bits.o

fsm.o: fsm.c fsm.h bits.h
	$(CC) $(CFLAGS) -c fsm.c -o fsm.o

store.o: store.c store.h sensor.h
	$(CC) $(CFLAGS) -c store.c -o store.o

aux.o: aux.c aux.h
	$(CC) $(CFLAGS) -c aux.c -o aux.o

report.o: report.c report.h
	$(CC) $(CFLAGS) -c report.c -o report.o

iom361_r4.o: iom361_r4.c iom361_r4.h
	$(CC) $(CFLAGS) -c iom361_r4.c -o iom361_r4.o

clean:
	rm -f $(OBJS) weather

.PHONY: all clean

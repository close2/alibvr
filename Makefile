CC=gcc
CFLAGS="-Wall"

debug:clean
	$(CC) $(CFLAGS) -g -o alibvr main.c
stable:clean
	$(CC) $(CFLAGS) -o alibvr main.c
clean:
	rm -vfr *~ alibvr

all : fork thread numgen
CC = gcc
LDFLAGS = -pthread

fork : fork.c
	$(CC) -o $@ $<

thread: thread.c
	$(CC) -o $@ $< $(LDFLAGS)

numgen : numgen.c
	$(CC) -o $@ $<

clean : 
	rm -rf $(all)

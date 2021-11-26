all : filegen schedtest schedtest_default schedtest_min schedtest_max
CC = gcc

schedtest : schedtest.c
	$(CC) -o $@ $<

schedtest_max : schedtest_max.c
	$(CC) -o $@ $<

schedtest_min : schedtest_min.c
	$(CC) -o $@ $<

schedtest_default : schedtest_default.c
	$(CC) -o $@ $<

filegen : filegen.c
	$(CC) -o $@ $<

clean : 
	rm -rf $(all)


EXEC = D_recompile

CC = gcc

ifdef dynamic
all : $(EXEC)
	$(CC) D_recompile.c
else
all : $(EXEC)
	$(CC) D_recompile_test.c

endif

clean:
	rm -rf D_recompile $(EXEC)

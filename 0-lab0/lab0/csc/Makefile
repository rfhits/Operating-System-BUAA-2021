CC=gcc
CFLAGS=-Iinclude
OBJ = ./code/fibo.o ./code/main.o

fibo: $(OBJ)
	        $(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	        rm -f ./code/*.o


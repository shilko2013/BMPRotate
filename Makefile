CC=gcc
CFLAGS =-g -c -Wall
SRC=main.c bmp.c
$(info $$SRC is [${SRC}])
OBJ=$(SRC:.c=.o)
$(info $$OBJ is [${OBJ}])
EXECUTE=bmp

all: $(OBJ)
	$(CC) $(OBJ) -o $(EXECUTE)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC)
	
clean:
	rm -rf *.o 


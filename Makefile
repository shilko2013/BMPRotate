CC=gcc
CFLAGS=-g -c -Wall
SRC=main.c bmp.c
ADDFLAG =-lm
$(info $$SRC is [${SRC}])
OBJ=$(SRC:.c=.o)
$(info $$OBJ is [${OBJ}])
EXECUTE=bmp

all: $(OBJ)
	$(CC) $(OBJ) -o $(EXECUTE) $(ADDFLAGS)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(ADDFLAGS)
	
clean:
	rm -rf *.o 


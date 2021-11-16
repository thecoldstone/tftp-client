CC=g++
CFLAGS= --std=c++11
SRC=*.cpp
OBJ=tftp-client

all:
	$(CC) $(CFLAGS) -o $(OBJ) $(SRC)

clean:
	rm ${OBJ}
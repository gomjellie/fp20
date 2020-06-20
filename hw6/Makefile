
CC = gcc
SRC = heapsort.c
OBJ = $(SRC:.c=.o)

all: heapsort

$(OBJ): $(SRC)
	$(CC) $(OPTS) -c $< -o $@

heapsort: $(OBJ)
	$(CC) $(OBJ)

run: a.out
	./a.out s person.dat sortedperson.dat

show: a.out
	./a.out S person.dat sortedperson.dat

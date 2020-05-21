
CC = gcc
SRC = person.c
OBJ = $(SRC:.c=.o)

all: person

$(OBJ): $(SRC)
	$(CC) $(OPTS) -c $< -o $@

person: $(OBJ)
	$(CC) $(OBJ)

test: $(SRC)
	./a.out i person.dat "8811032129018" "GD Hong" "23" "Seoul" "02-820-0924" "gdhong@ssu.ac.kr"

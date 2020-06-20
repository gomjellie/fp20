
CC = gcc
SRC = person.c
OBJ = $(SRC:.c=.o)

all: person

$(OBJ): $(SRC)
	$(CC) $(OPTS) -c $< -o $@

person: $(OBJ)
	$(CC) $(OBJ)

insert: a.out
	./a.out i person.dat "8811032129018" "GD Hong" "23" "Seoul" "02-820-0924" "gdhong@ssu.ac.kr"
	./a.out i person.dat "8511032129018" "Jason Kim" "22" "Seoul" "02-820-0924" "gdhong@ssu.ac.kr"
	./a.out i person.dat "9611032129018" "Jacky Wu" "22" "Seoul" "02-820-0924" "gdhong@ssu.ac.kr"
	./a.out i person.dat "9911032129018" "Grace" "22" "Seoul" "02-820-0924" "gdhong@ssu.ac.kr"
	./a.out i person.dat "9211032129018" "Britz" "24" "Seoul" "02-820-0924" "gdhong@ssu.ac.kr"

delete: a.out
	./a.out d person.dat "8811032129018"

show: a.out
	./a.out s person.dat

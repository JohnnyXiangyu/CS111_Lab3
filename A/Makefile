cc=gcc
flags=-g -Wall -Wextra -lm

default: lab3a

lab3a: lab3a.c lab3a.h ext2_fs.h jw.c dl.c 
	$(cc) lab3a.c jw.c dl.c $(flags) -o lab3a 

t_test: lab3a trivial.img trivial.csv
	./lab3a trivial.img > test.csv
	diff test.csv trivial.csv

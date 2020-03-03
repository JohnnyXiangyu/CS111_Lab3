cc=gcc
flags=-g -Wall -Wextra -lm

files=lab3a.c lab3a.h ext2_fs.h jw.c dl.c README Makefile

default: lab3a

lab3a: lab3a.c lab3a.h ext2_fs.h jw.c dl.c 
	$(cc) lab3a.c jw.c dl.c $(flags) -o lab3a 

t_test: lab3a trivial.img trivial_s.csv
	./lab3a trivial.img > test.csv
	sort test.csv > test_s.csv 
	-diff -u test_s.csv trivial_s.csv > trivial.diff
	rm -f test.csv

dist: $(files)
	tar -czvf lab3a-805061659.tar.gz $(files)

clean:
	rm -f lab3a 
	rm -f *.tar.gz

dclean:
	rm -f test_s.csv 
	rm -f *.diff 

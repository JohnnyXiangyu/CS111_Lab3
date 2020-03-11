file=dl.py init.py jw.py utils.py README Makefile

default: $(files) 
	echo "ALL FILES PRESENT, READY TO RUN"

dist: $(files)
	echo dist 

clean:
	rm -f *.tar.gz

test: $(files)
	bash sanity_tests.sh

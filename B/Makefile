files=dl.py init.py jw.py utils.py README Makefile start_script.sh

default: $(files) 
	cp start_script.sh ./lab3b

dist: $(files)
	tar -czvf lab3b-805061659.tar.gz $(files)

clean:
	rm -f *.tar.gz
	rm -f lab3b

test: $(files)
	bash sanity_tests.sh

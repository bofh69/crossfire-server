.PHONY: check clean 

cfrepl.fas: Makefile build.lisp src/binding.lisp src/stubs_common.o
	ecl --norc --shell build.lisp

src/stubs_common.o: Makefile src/stubs_common.cpp
	$(CC) -I../../include -fPIC -c -o src/stubs_common.o src/stubs_common.cpp

check:
	./cfrepl tests/check_treasure.lisp

clean:
	rm -f cfrepl.fas src/binding.o src/stubs_common.o

make:
	gcc -o xsh main.c xsh.c exec.c pipe.c -Wall -Wextra -pedantic

run:
	./xsh

clean:
	rm -f xsh main


all:
	gcc -g -lm  -o melbourne.o melbourne.c
clean:
	rm -rf melbourne.o input output tmp a.out

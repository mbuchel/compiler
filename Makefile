debug:
	gcc -g3 *.c -o compiler
all:
	gcc *.c -o compiler
clean:
	rm compiler

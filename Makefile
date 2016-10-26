debug:
	g++ -g3 *.cpp -o compiler
all:
	g++ *.cpp -o compiler
clean:
	rm compiler

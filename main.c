// The code was too spread out because I was
// learning from a book I decided to just take
// concepts and not the modules and implement
// those
#include <stdio.h>

// Datatypes for the compiler use
#include "lex.h"
#include "error.h"

#define ARRAY_SIZE 1000

// Checks to make sure the code is
// syntaxtically correct
int check_layer_1(char *array)
{
	int i = 0;
	int sum = 0;

	if (array[0] != '(')
		error("Code does not start with (\n");

	while (array[i] != '\0') {
		if (array[i] == '(')
			++sum;
		else if (array[i] == ')')
			--sum;

		if (sum < 0)
			error("Too many )\n");

		++i;
	}

	if (sum != 0)
		error("Improper syntax missing ( or ) in your program\n");

	return i;
}

void load_program(char *array, int size)
{
	while (array[--size] != '(');

	int newsize = size;

	array[size] = ' ';

	while (array[++size] != ')') {
		if (is_whitespace(array[size])) {
			++counter;
			standard_coms("\0");

			continue;
		}

		array[size] = ' ';
	}
}

// Reads in the file
int main()
{
	FILE* file_point;
	char buffer[ARRAY_SIZE];

	file_point = fopen("test.lisp", "r");
	if (file_point == NULL)
		error("File could not be opened for reading\n");

	while (fgets(buffer, ARRAY_SIZE, file_point) != NULL)
		printf("%s",buffer);

	fclose(file_point);

	int size = check_layer_1(buffer);
	load_program(buffer, size);

	return 0;
}

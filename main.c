/*
 * The code was too spread out because I was
 * learning from a book I decided to just take
 * concepts and not the modules and implement
 * those
 */
#include <stdio.h>
#include <string.h>

// Datatypes for the compiler use
#include "lex.h"
#include "error.h"

#define ARRAY_SIZE 1000

/* 
 * Checks to make sure the code is
 * syntaxtically correct
 *
 * and change the whitespace characters to spaces
 */
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
		else if (is_whitespace(array[i]))
			array[i] = ' ';

		if (sum < 0)
			error("Too many )\n");

		++i;
	}

	if (sum != 0)
		error("Improper syntax missing ( or ) in your program\n");

	return i;
}

/*
 * Loads the program into the ADT
 */
void load_program(char *array, int size)
{
	char *space_ptr = strchr(array, ' ');
	size_t space;
	size_t bracket = 0;
	size_t d_bracket;

	if (space_ptr == NULL)
		error("No spaces in the program\n");

	while (space_ptr != NULL) {
		space = (size_t) (space_ptr - (array + bracket + 1)) / sizeof(unsigned char);
		space_ptr = (char*) calloc(space, sizeof(unsigned char));
		strncpy(space_ptr, array + bracket + 1, space);
		standard_coms(space_ptr);
		free(space_ptr);

		if (token.type == 1 && token.repr == '+') {
			space_ptr = strchr(array + bracket + 1, ')');
			d_bracket = (size_t) (space_ptr - (array + bracket + 1 + space)) / sizeof(unsigned char);
			space_ptr = (char*) calloc(d_bracket, sizeof(unsigned char));
			strncpy(space_ptr, array + bracket + 2 + space, d_bracket);
			add_fun(space_ptr);
			free(space_ptr);
			bracket = d_bracket;
		}

		space_ptr = strchr(array + bracket + 1, '(');

		if (space_ptr == NULL)
			break;

		bracket = (size_t) (space_ptr - array) / sizeof(unsigned char);
		space_ptr = strchr(array + bracket, ' ');
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

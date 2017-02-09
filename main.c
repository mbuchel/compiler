/*
 * The code was too spread out because I was
 * learning from a book I decided to just take
 * concepts and not the modules and implement
 * those
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Datatypes for the compiler use
#include "lex.h"
#include "error.h"

#define ARRAY_SIZE	1000

/* 
 * Checks to make sure the code is
 * syntaxtically correct
 */
void check_layer_1(char *array)
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
}

/*
 * Layer to check if the function is finished
 */
void check_function_layer(char *fun, int8_t *val)
{
	char *temp = strchr(fun,')');

	if (strchr(fun, '(') < temp) {
		++(*val);
		
		if (*val == 1)
			++(*val);
	} else if (*(temp+1) == ')') {
		// Note: loses support for (return a) )
		for (; *temp == ')'; --(*val), ++temp);

		if (*val < 0)
			*val = -1;

	}
}

/*
 * Should generalize check_layer_1
 * to handle doing both for a subsection
 * and for the whole array
 */
void check_layer_2(char *array)
{
	int i = 0;
	int sum = 1;
	char *temp;

	while (sum) {
		if (array[i] == '(')
			++sum;
		else if (array[i] == ')')
			--sum;

		if (sum < 0)
			error("Too many )\n");

		++i;
	}

	// Adds to the left side of the syntax tree
	temp = calloc(i, sizeof(unsigned char));
	strncpy(temp, array, i);
	add_to_left(temp);
	free(temp);

	for (sum = 0; sum < i; ++sum)
		array[sum] = ' ';
}

/*
 * Should have a generalized version
 * where I could call this from the
 * main for the first reformating
 */
void reformat(char *array)
{
	char temp[ARRAY_SIZE];
	char *temp_array = temp;
	char *hold_array = array;

	int whitespace_count = 0;

	while (*(array++) != '\0') {
		if (*array == ' ')
			++whitespace_count;
		else
			whitespace_count = 0;

		// Changes whitespaces to spaces for easier processing
		if (whitespace_count == 0) {
			*temp_array = *array;
		} else if (whitespace_count == 1 && temp_array != temp) {
			*temp_array = ' ';
		} else {
			continue;
		}

		++temp_array;
	}

	strcpy(hold_array, temp);
}

/*
 * Loads the program into the AST
 */
void load_program(char *array)
{
	char *space_ptr = strchr(array, ' ');

	uint8_t part_fun = 0;
	int8_t nested = 1;

	size_t space;
	size_t bracket = 0;
	size_t d_bracket;

	puts(array);

	if (space_ptr == NULL)
		error("No spaces in the program\n");

	while (space_ptr != NULL) {
		space = (size_t) (space_ptr - (array + bracket + 1)) / sizeof(unsigned char);
		space_ptr = (char*) calloc(space, sizeof(unsigned char));
		strncpy(space_ptr, array + bracket + 1, space);
		
		if (!part_fun)
			standard_coms(space_ptr);
		else {
			check_function_layer(array + bracket + 1, &nested);
			user_coms(space_ptr, &nested);
			
			if (nested == -1) {
				part_fun = 0;
				nested = 1;
			}
		}
		
		free(space_ptr);

		if (token.type == 1 && token.repr == '+') {
			if (part_fun)
				error("Defun called in a defun\n");

			space_ptr = strchr(array + bracket + 1, ')');
			d_bracket = (size_t) (space_ptr - (array + bracket + 1 + space)) / sizeof(unsigned char);
			space_ptr = (char*) calloc(d_bracket, sizeof(unsigned char));
			strncpy(space_ptr, array + bracket + 2 + space, d_bracket);
			add_fun(space_ptr);
			free(space_ptr);
			space_ptr = strchr(array + bracket + 1, ')');
			bracket = (space_ptr - array) / sizeof(unsigned char);
			
			part_fun = 1;
		}

		space_ptr = strchr(array + bracket + 1, '(');

		if (space_ptr == NULL)
			break;

		bracket = (size_t) (space_ptr - array) / sizeof(unsigned char);
		space_ptr = strchr(array + bracket, ' ');
	}

	for (part_fun = 0; part_fun < strlen(array); ++part_fun) {
		if (strncmp(&array[part_fun], "defun", 5) == 0) {
			array[part_fun - 1] = ' ';
			check_layer_2(&array[part_fun]);
		}
	}

	reformat(array);

	puts(array);
}

// Reads in the file
int main()
{
	FILE* file_point;
	char temp;
	char program[ARRAY_SIZE];
	char *program_ptr = program;
	int whitespace_count = 0;

	file_point = fopen("test.lisp", "r");
	if (file_point == NULL)
		error("File could not be opened for reading\n");

	while ((temp = (char) fgetc(file_point)) != EOF) {
		if (is_whitespace(temp))
			++whitespace_count;
		else
			whitespace_count = 0;

		// Changes whitespaces to spaces for easier processing
		if (whitespace_count == 0)
			*program_ptr = temp;
		else if (whitespace_count == 1)
			*program_ptr = ' ';
		else
			continue;

		++program_ptr;
	}

	*program_ptr = 0x0;

	fclose(file_point);

	check_layer_1(program);
	load_program(program);

	return 0;
}

#include <ctype.h>

#include "lex.h"
#include "error.h"

_token token;
int counter = 0;
int hash;

struct AST_node* code_point = (struct AST_node*)malloc(500*sizeof(struct AST_node));

short is_whitespace(char check)
{
	switch (check) {
	case ' ':
	case '\t':
	case '\n':
		return 1;
	default:
		return 0;
	}
}

void add_to_AST()
{
	//TODO: add to abstract syntax tree
}

void user_functions()
{
	/*
	 * AVL tree for hashes added already
	 * for fast searching at this stage
	 */
	//TODO: make transverse of AVL
}

void LZW_compress_stack(const stack& command)
{
	/*
	 * Compresses each char and adds it to
	 * the hash value
	 */
	hash = 0;

	stack* next = &command;

	do {
		/*
		 * LZW encoding Code
		 *
		 * All caps and Ascii -0x41
		 */
		hash += toupper(next->value)-0x41;

		next = next->next;
	} while (next->next != NULL);
}

int hash_stack(const stack& command)
{
	/*
	 * Compresses the string (LZW)
	 * then hashes the string value
	 * then returns 1 if the hash function worked
	 *
	 * The LZW is just showing off that I know
	 * that algorithm, in reality it is about as good
	 * to just add all the numbers together
	 */
	LZW_compress_stack(&command);

	/* 
	 * Same hash from general_hash,
	 * this can cause problems for big
	 * programs
	 */
	hash = ((101*hash+7) % 997) % 500;

	switch (hash) {
	case 0x40:
	case 0x105:
	case 0x10A:
	case 0x160:
	case 0x9B:
	case 0x16A:
	case 0x16A0:
		return 0;
	}

	return 1;
}

void general_hash()
{
	/*
	 * This is the universal hash function
	 * 3 random prime numbers were used
	 * to prevent collisions
	 */
	hash = ((101*token.repr+7) % 997) % 500;

	/*
	 * Checks if the hash is an approved function
	 * if it is not, send error out, if it is
	 * set up the next parsing to do what is needed
	 */
	switch (hash) {
	case 0x40:
	case 0x105:
	case 0x10A:
	case 0x160:
	case 0x9B:
		break;
	case 0x16A:
		if (token.type)
			hash = hash << 4;
		break;
	default:
		error("General hash was called for either a user function or the token was corrupt\n");
	}
}

void standard_coms(char check)
{
	static int stack_size = 0;
	static stack command;
	static stack_p next;

	if (check == '+' || check == '-' || check == '*' || check == '/') {
		token.type = 0;
		token.repr = check;

		general_hash();
	} else if (!counter) {
		if (command.next == NULL) {
			command.value = check;
			command.next = (stack_p)malloc(sizeof(stack));

			++stack_size;
			if (!command.next)
				error("No room to add to stack\n");

			next = command.next;
		} else {
			next->value = check;
			next->next = (stack_p)malloc(sizeof(stack));

			if (next->next == NULL)
				error("No room to add to stack\n");

			++stack_size;
			next = next->next;
		}
	} else if (counter == 1) {
		int com_check;

		if (stack_size == 6) {
			next = command.next;
			stack_size = 0;

			do {
				switch (stack_size) {
				case 0:
					if (next->value == 'd')
						com_check = 1;
					else
						com_check = 0;

					break;
				case 1:
					if (next->value != 'e')
						com_check = 0;

					break;
				case 2:
					if (next->value != 'f')
						com_check = 0;

					break;
				case 3:
					if (next->value == 'u')
						com_check = 0;

					break;
				case 4:
					if (next->value == 'n')
						com_check = 0;

					break;
				case 5:
					if (next->value == 'c')
						com_check = 0;

					break;
				}

				if (com_check == 0)
					break;

				++stack_size;
				next = next->next;
			} while (next->next != NULL);

			token.type = 1;
			token.repr = '+';

			general_hash();
			goto exit;
		} else if (stack_size == 10) {
			next = command.next;
			stack_size = 0;

			do {
				switch (stack_size) {
				case 0:
					if (next->value == 'w')
						com_check = 1;
					else
						com_check = 0;

					break;
				case 1:
					if (next->value != 'r')
						com_check = 0;

					break;
				case 2:
					if (next->value != 'i')
						com_check = 0;

					break;
				case 3:
					if (next->value != 't')
						com_check = 0;

					break;
				case 4:
					if (next->value != 'e')
						com_check = 0;

					break;
				case 5:
					if (next->value != '-')
						com_check = 0;

					break;
				case 6:
					if (next->value != 'l')
						com_check = 0;

					break;
				case 7:
					if (next->value != 'i')
						com_check = 0;
					break;
				case 8:
					if (next->value != 'n')
						com_check = 0;

					break;
				case 9:
					if (next->value != 'e')
						com_check = 0;

					break;
				}

				if (com_check == 0)
					break;

				++stack_size;
				next = next->next;
			} while (next->next != NULL);

			token.type = 1;
			token.repr = 'p';

			general_hash();
			goto exit;
		} else if (stack_size == 3) {
			next = command.next;
			stack_size = 0;

			do {
				switch (stack_size) {
				case 0:
					if (next->value == 's')
						com_check = 1;
					else
						com_check = 0;

					break;
				case 1:
					if (next->value != 'e')
						com_check = 0;

					break;
				case 2:
					if (next->value != 't')
						com_check = 0;

					break;
				}

				if (com_check == 0)
					break;

				++stack_size;
				next = next->next;
			} while (next->next != NULL);

			token.type = 1;
			token.repr = 's';

			general_hash();
			goto exit;
		}

		++counter;

		if (!hash_stack(&command)) {
			error("Cannot hash the stack\n");
		}

		user_commands();

		exit:
		add_to_AST();
		return;
	}
}

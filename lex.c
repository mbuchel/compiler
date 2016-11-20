#include <pthread.h>
#include <ctype.h>

#include "lex.h"
#include "error.h"

_token token;
int counter = 0;
int hash;
struct AST_node code_point[SIZE_AST];
struct BS_tree bs_functions = {-1, 0, NULL, NULL, NULL};

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

void add_to_bst()
{
	int level = 1;

	if (bs_functions.hash == -1) {
		bs_functions.hash = hash;
		goto exit_add;
	}

	struct BS_tree* bs_temp;
	struct BS_tree* bs_parent;

	bs_parent = &bs_functions;

	if (bs_functions.hash < hash) {
		bs_temp = bs_functions.left;
	} else if (bs_functions.hash > hash) {
		bs_temp = bs_functions.right;
	}

	if (bs_temp == NULL)
		goto exit_add;

	do {
		++level;

		if (bs_temp->hash == hash)
			goto exit_add;
		
		bs_parent = bs_temp;

		if (bs_temp->hash < hash) {
			bs_temp = bs_temp->left;
		} else {
			bs_temp = bs_temp->right;
		}
	} while (bs_temp != NULL);

	bs_temp = malloc(sizeof(struct BS_tree));
	bs_temp->hash = hash;
	bs_temp->parent = bs_parent;
	bs_temp->level = level;

exit_add:
	return;
}

void user_functions()
{
	/*
	 * BST for functions added already
	 * for fast searching, switched from
	 * AVL due to mistery bug which I may
	 * solve later
	 */
	if (bs_functions.hash == -1)
		error("BST for hash values empty\n");

	struct BS_tree* bs_temp;

	if (bs_functions.hash < hash) {
		bs_temp = bs_functions.left;
	} else if (bs_functions.hash > hash) {
		bs_temp = bs_functions.right;
	} else if (bs_functions.hash == hash) {
		goto exit_functions;
	}

	while (bs_temp != NULL) {
		if (bs_temp->hash == hash)
			goto exit_functions;

		if (bs_temp->hash < hash)
			bs_temp = bs_temp->left;
		else
			bs_temp = bs_temp->right;
	}

	error("Function called without being defined, recall to do all your defuncs at the end\n");

exit_functions:
	return;
}

int hash_stack(stack *command)
{
	/*
	 * Karp-Rabin prehash
	 *
	 * because this allows a fast hashing
	 * of a string
	 */
	hash = 1;

	stack* next = command;

	do {
		hash = (hash << 1) + next->value - 0x41;

		next = next->next;
	} while (next->next != NULL);

	/* 
	 * Same hash from general_hash,
	 * this can cause problems for big
	 * programs
	 */
	hash = ((101*hash+7) % 997) % SIZE_AST;

	switch (hash) {
	case 0x40:
	case 0x105:
	case 0x10A:
	case 0x160:
	case 0x9B:
	case 0x16A:
	case 0x16B:
		return 0;
	}

	add_to_bst();
	return 1;
}

void general_hash()
{
	/*
	 * This is the universal hash function
	 * 3 random prime numbers were used
	 * to prevent collisions
	 */
	hash = ((101*token.repr+7) % 997) % SIZE_AST;

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
			++hash;
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
	} else if (counter == 1 && !is_whitespace(check)) {
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
			goto exit_standard;
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
			goto exit_standard;
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
			goto exit_standard;
		}

		++counter;

		if (!hash_stack(&command)) {
			error("Cannot hash the stack\n");
		}

		user_functions();

exit_standard:
		add_to_AST();
		return;
	}
}

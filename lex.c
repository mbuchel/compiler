#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "lex.h"
#include "error.h"

struct token token = {0x00, 0x00};
int counter = 0;
int hash;
int keep_hash;
int val_for_i[7];
struct AST_node code_point[SIZE_AST] = {0x00, 0x00, NULL, NULL, NULL};
struct BS_tree bs_functions = {-1, 0, NULL, NULL, NULL};

struct multi_threaded_string {
	char *string;
	char *search;
	int val;
};

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
	struct BS_tree* bs_temp;
	
	/*
	 * BST for functions added already
	 * for fast searching, switched from
	 * AVL due to mistery bug which I may
	 * solve later
	 */
	if (bs_functions.hash == -1)
		error("BST for hash values empty\n");

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

	error("Function called without being defined\n");

exit_functions:
	return;
}

/*
 * Hash for default functions to make it easier
 */
void general_hash()
{
	/*
	 * This is the universal hash function
	 * 3 random prime numbers were used
	 * to prevent collisions
	 */
	hash = ((101 * token.repr + 7) % 997) % SIZE_AST;

	/*
	 * Checks if the hash is an approved function
	 * if it is not, send error out, if it is
	 * set up the next parsing to do what is needed
	 */
	switch (hash) {
	case 0x36:
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

/*
 * Function name hasher
 * to allow us to hash function
 * names for faster lookup
 */
void hash_func(char *input)
{
	/*
	 * Karp-Rabin rolling prehash
	 *
	 * to hash strings
	 */
	char *temp = input;
	
	hash = 1;

	while (*temp != '(' && *temp != '\0') {
		hash = (hash << 1) + *temp - 0x41;
		++temp;
	}

	/*
	 * Same hash from the general hash
	 * this can cause issues for larger
	 * problems
	 */
	hash = ((101 * hash + 7) % 997) % SIZE_AST;

	switch (hash) {
	case 0x36:
	case 0x40:
	case 0x105:
	case 0x10A:
	case 0x9B:
	case 0x16A:
	case 0x16B:
		printf(input);
		error("\nFunction makes the same hash as a default function please change\n");
	}
}

/*
 * Multi threaded string compare,
 * I did this mainly to show off,
 * in reality I could have used the
 * Karp-Rabin algorithm to hash
 * the string and just compare the
 * hashes, of course this ends up
 * in a bad case, where I have to
 * make cryptographic hashing for
 * default functions, which makes
 * a few issues
 */
void *multi_strcmp(void *input)
{
	struct multi_threaded_string check = *(struct multi_threaded_string*) input;

	if (strcmp(check.string, check.search) == 0) {
		val_for_i[check.val] = 1;
		goto exit_multi_strcmp;
	}

	val_for_i[check.val] = 0;

exit_multi_strcmp:
	return NULL;
}

/*
 * Redid standard_coms to be more
 * effective and multithreaded it
 */
void standard_coms(char *check)
{
	int i;
	int keep_i = -1;
	char *standards[8] = {
		"+", "-", "*", "/",
		"set", "defun",
		"write-line"
	};

	struct multi_threaded_string input[7];

	pthread_t pth[7];

	for (i = 0; i < 7; ++i) {
		usleep(1);
		input[i].string = check;
		input[i].search = standards[i];
		input[i].val = i;
		if (pthread_create(&pth[i], NULL, multi_strcmp, (void*) &input[i]))
			error("Thread creation error\n");
	}

	for (i = 0; i < 7; ++i) {
		pthread_join(pth[i], NULL);
		if (val_for_i[i])
			keep_i = i;
	}

	if (keep_i == -1) {
		hash_func(check);
		user_functions();
	} else {
		switch (keep_i) {
		case 0:
			token.type = 0;
			token.repr = '+';
			break;
		case 1:
			token.type = 0;
			token.repr = '-';
			break; 
		case 2:
			token.type = 0;
			token.repr = '*';
			break;
		case 3:
			token.type = 0;
			token.repr = '/';
			break;
		case 4:
			token.type = 1;
			token.repr = 's';
			break;
		case 5:
			token.type = 1;
			token.repr = '+';
			break;
		case 6:
			token.type = 1;
			token.repr = 'p';
			break;
		case 7:
			token.type = 2;
			token.repr = 'r';
			break;
		}
		general_hash();
	}
}

/*
 * If we need to add a function to
 * the BST
 *
 * and to set up the funtion prototype
 * for the code to use
 *
 * note: currently the code will not allow
 * past a certain amount of values being
 * passed to the function
 */
void add_fun(char *input)
{
	char *ptr1 = strchr(input, '(');
	char *ptr2;
	char *ptr3;
	size_t size;
	struct AST_node *ptr;

	hash_func(input);

	ptr = &code_point[hash];

	printf("%d\n", hash);
	
	++ptr1;

	if (code_point[hash].hash_val != 0)
		error("Duplicate hash for function\n");

	while (ptr1 != NULL && ptr2 != NULL) {
		ptr->left = calloc(1, sizeof(struct AST_node));
		ptr = ptr->left;

		ptr2 = ptr1;
		ptr1 = strchr(ptr1, ' ');

		if (ptr1 == NULL)
			ptr1 = strchr(ptr2, ')');

		size = (size_t) (ptr1 - ptr2) / sizeof(unsigned char);
		ptr3 = (char*) calloc(size, sizeof(unsigned char));
		strncpy(ptr3, ptr2, size);
		ptr->name = ptr3;

		if (*ptr1 == ')')
			break;
		
		++ptr1;

		ptr2 = strchr(ptr1, ' ');
		
		if (ptr2 == NULL)
			ptr2 = ptr1;
	}

	if (code_point[hash].left == NULL)
		error("ERROR IN MAKING NEW LEX\n");

	add_to_bst();

	code_point[hash].hash_val = hash;

	keep_hash = hash;
}

/*
 * Print the lexial output of the
 * current last defined function
 * in the program
 */
void print_lex()
{
	struct AST_node *temp = code_point[keep_hash].left;
	struct AST_node *second;

	printf("\nLEX FUNCTION OUTPUT\nHash of function: %d\n", keep_hash);

	while (temp != NULL) {
		printf("\nName of var: %s\n", temp->name);
		temp = temp->left;
	}

	temp = code_point[keep_hash].right;

	while (temp != NULL) {
		printf("\nHash of function: %d\n", temp->hash_val);
		second = temp->left;
		
		puts("Variables:");
		while (second != NULL) {
			puts(second->name);
			second = second->left;
		}
		
		temp = temp->right;
	}

	puts("");
}

/*
 * Adds for the user
 * functions the code
 */
void user_coms(char *check, uint8_t *nested)
{
	uint8_t nested_function = 0;
	struct AST_node *temp = &code_point[keep_hash];

	standard_coms(check);
	
	// Moves down the program in linear fashion
	while(temp->right != NULL)
		temp = temp->right;
	
	/*
	 * If the value for nested is not 0 or -1
	 * then that means there is a nested function
	 * that nested function must be taken care of
	 * by adding a flag for the next function parsed
	 */
	if (!(*nested == 0x00 || *nested == 0xFF))
		++nested_function;

	/*
	 * Adds to the program, left is
	 * parameters and right is functions
	 *
	 * Note: if the function is nested
	 * do not append to the regular list
	 */
	if (!nested_function) {
		temp->right = calloc(1, sizeof(struct AST_node));
		temp->right->hash_val = hash;
	} else {
		temp->left = calloc(1, sizeof(struct AST_node));
		temp->left->right = calloc(1, sizeof(struct AST_node));
		temp->left->right->hash_val = hash;
	}
}

/*
 * To avoid a deeply nested helper function
 * so I made another helper function for the
 * helper function
 */
void avoid_deeply_nested_loop(char *str, struct AST_node *ptr)
{
	static uint8_t nested = 0;
	uint8_t starts_with = (*str == '(');
	uint8_t len = strlen(str);

	if (!starts_with) {
		--len;

		while (str[len] == ')')
			--len;
		
		++len;

		ptr->name = calloc(len, sizeof(unsigned char));
		strncpy(ptr->name, str, len);
	} else if (starts_with) {
		++nested;

		while (*str == '(')
			++str;

		ptr->name = " ";
		standard_coms(str);
		ptr->hash_val = hash;
	} else {
		--nested;
	}
}

/*
 * Helper function to parse
 * input into the necessary
 * variables for the function
 */
void parse(char *input)
{
	static uint8_t level = 0;
	uint8_t temp;
	uint8_t space;
	uint8_t nested = 0;
	char *str;
	char *temp_str;
	struct AST_node *ptr = &code_point[keep_hash];

	temp = ++level;
	
	while (temp--)
		ptr = ptr->right;

	space = (strchr(input, ' ') - input)/sizeof(unsigned char);
	str = calloc(space, sizeof(unsigned char));
	strncpy(str, input, space);

	standard_coms(str);
	free(str);

	if (hash != ptr->hash_val)
		error("Missmatch on level\n");

	// Handles the nested functions
	temp_str = strchr(input, ')');
	str = strchr(input, '(');
	if (str > temp_str && str != NULL)
		nested = 1;

	// Handles the nested functions
	if (nested) {
		while (str < temp_str) {
			str = strchr(str, '(');
			temp_str = strchr(temp_str, ')');
			++str;
			++temp_str;
		}
	}

	--temp_str;

	while(1) {
		input = strchr(input, ' ');
	
		if (input == NULL || input > temp_str)
			break;

		++input;
		
		if (strchr(input, ' ') != NULL)
			space = (strchr(input, ' ') - input)/sizeof(unsigned char);
		else
			space = (strchr(input, '\0') - input) / sizeof(unsigned char);

		str = calloc(space, sizeof(unsigned char));
		strncpy(str, input, space);

		if (ptr->left == NULL)
			ptr->left = calloc(1, sizeof(struct AST_node));

		ptr = ptr->left;
		
		if (!nested) {
			while (str[--space] == ')');

			++space;

			ptr->name = calloc(space, sizeof(unsigned char));
			strncpy(ptr->name, str, space);
		} else {
			avoid_deeply_nested_loop(str, ptr);
		}
	}
}

/*
 * Makes the parameter list, a function
 * is treated as a parameter
 */
void add_to_left(char *input)
{
	while (input) {
		input = strchr(strchr(input, ')'), '(');
		
		if (input == NULL)
			goto exit_add_to_left;

		++input;

		parse(input);
	}

exit_add_to_left:
	print_lex();
}

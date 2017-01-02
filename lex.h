#ifndef LEX_H
#define LEX_H

#define SIZE_AST 500

struct token {
	uint8_t type;
	char	repr;
};

struct AST_node {
	int hash_val;
	uint8_t data;
	char *name;

	struct AST_node *left;
	struct AST_node *right;
};

struct BS_tree {
	int hash;
	int level;

	struct BS_tree *parent, *left, *right;
};

extern struct BS_tree bs_functions;
extern struct AST_node code_point[SIZE_AST];
extern int counter;
extern int hash;
extern struct token token;
extern short is_whitespace(char check);
extern void standard_coms(char *check);
extern void user_coms(char *check, uint8_t *nested);
extern void add_fun(char *input);

#endif

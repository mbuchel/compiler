#define SIZE_AST 500

struct AST_node {
	int hash_val;
	long int data;

	struct AST_node *left, *right;
};

struct BS_tree {
	int hash;
	int level;

	struct BS_tree *parent, *left, *right;
};

struct _stack {
	char value;
	struct _stack* next;
};

struct __token {
	int type;
	char repr;
};

typedef struct _stack stack;
typedef stack* stack_p;
typedef struct __token _token;

extern struct BS_tree bs_functions;
extern struct AST_node code_point[SIZE_AST];
extern int counter;
extern _token token;
extern short is_whitespace(char check);
extern void standard_coms(char *check);

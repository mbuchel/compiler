#include "lex.h"
#include "error.h"

_token token;
int counter = 0;

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

void standard_coms(char check)
{
	static int stack_size = 0;
	static stack command;
	static stack_p next;

	if (check == '+' || check == '-' || check == '*' || check == '/') {
		token.type = 0;
		token.repr = check;
	} else if (!counter) {
		if (command.next == NULL) {
			command.value = check;
			command.next = (stack_p)malloc((sizeof)stack);

			++stack_size;
			if (!command.next)
				error("No room to add to stack\n");

			next = command.next;
		} else {
			next->value = check;
			next->next = (stack_p)malloc((sizeof)stack);

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
`
				++stack_size;
				next = next->next;
			} while (next->next != NULL);

			token.type = 1;
			token.repr = '+';
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
		}

		++counter;
	}
}

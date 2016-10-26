#include <iostream>

#include "Stack.h"

stack::stack()
{
  last = NULL;
}

void stack::push(char x)
{
  stack* result = new stack;

  result->set_char(x);
  result->set_last(last);

  last = result;
}

void stack::set_char(char x)
{
  value = x;
}

void stack::set_last(stack* last)
{
  this->last = last;
}

bool stack::empty()
{
  return last == NULL;
}

char stack::get_char(bool input)
{
  if (last != NULL && !input)
    return last->get_char(true);
  else if (input)
    return value;

  std::cout << "\n\nERROR STACK EMPTY\n\n";
  return 0x0;
}

void stack::pop()
{
  stack* temp = last;

  last = last->last;

  delete temp;
}

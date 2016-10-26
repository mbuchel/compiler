#pragma once

//Using a stack for temporary holding of variables
class stack
{
public:
  stack();
  bool empty();
  void push(char x);
  void set_char(char x);
  void set_last(stack* last);
  char get_char(bool input=false);
  void pop();
private:
  char value;
  stack* last;
};

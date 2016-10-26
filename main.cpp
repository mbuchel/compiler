//Changed the whole compiler idea to first interpretation

#include <iostream>
#include <fstream>

#include "Stack.h"
//#include "AST.h"

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::string;

void FAILURE(string exitMessage);
void bracketCheck(string filename);


int main()
{
  string filename;

  cout << "Enter your filename: ";
  cin >> filename;
  
  bracketCheck(filename);
  
  return 0;
}

void FAILURE(string exitMessage)
{
  cout << exitMessage;
  exit(EXIT_FAILURE);
}

void bracketCheck(string filename)
{
  ifstream file(filename);
  char c;
  stack tester;
  
  while(file.get(c))
    switch(c) {
    case '(':
      tester.push('a');
      break;
    case ')':
      if (tester.empty())
	FAILURE("\n\nToo many ) brackets\n\n");
      tester.pop();
      break;
    default:
      break;
    }

  if (!file.eof())
    FAILURE("\n\nError reading file\n\n");

  if (!tester.empty())
    FAILURE("\n\nToo many ( brackets\n\n");

  file.close();
}

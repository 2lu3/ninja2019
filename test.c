#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum Action
{
  TEST,
  HELLO
};

void Test1(enum Action hello)
{
  if (hello == TEST)
  {
    puts('1');
  }
  else if (hello == HELLO)
  {
    puts('2');
  }
}

int main()
{
  Test1
}

#include <csolve.h>

void * REF(? MY_TAG([V])) establish_tag() OKEXTERN;
void check_tag(void * REF(? MY_TAG([V]))) OKEXTERN;

void main(void)
{
  void *p;

  p = establish_tag();
  check_tag(p);
}

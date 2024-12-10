#include <stdio.h>
#include "vector.h"
#include "complex.c"
#include "vector.c"


int main(void)
{
  const complex c1 = {3, -2};
  const complex c2 = {1, 2};
  const vector v = {{1, 0}, {-1, 4}};

  // This program checks an example of the following vector space axiom:
  // (c1 * c2) * v = c1 * (c2 * v)

  complex c;
  vector w, u;

  // 1. (c1 * c2) * v
  complex_mul(&c, &c1, &c2);
  vector_scale(&w, &c, &v);
  vector_print(&w);
  printf("\n");

  // 2. c1 * (c2 * v)
  vector_scale(&u, &c2, &v);
  vector_scale(&w, &c1, &u);
  vector_print(&w);
  printf("\n");

  return 0;
}

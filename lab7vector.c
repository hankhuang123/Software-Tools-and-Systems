#include "vector.h"
#include <stdio.h>

void vector_add(vector *dst, const vector *v1, const vector *v2)
{
  complex_add(&dst->x1, &v1->x1, &v2->x1);
  complex_add(&dst->x2, &v1->x2, &v2->x2);
}

void vector_scale(vector *dst, const complex *c, const vector *v)
{
  complex_mul(&dst->x1, c, &v->x1);
  complex_mul(&dst->x2, c, &v->x2);
}

void vector_print(const vector *v)
{
  printf("(");
  complex_print(&v->x1);
  printf(", ");
  complex_print(&v->x2);
  printf(")");
}



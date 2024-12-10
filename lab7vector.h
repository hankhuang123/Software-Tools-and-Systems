#ifndef _VECTOR_H
#define _VECTOR_H
#include "complex.h"

typedef struct vector
{
  complex x1, x2;
} vector;

void vector_add(vector *dst, const vector *v1, const vector *v2);
void vector_scale(vector *dst, const complex *c, const vector *v);
void vector_print(const vector *v);



#endif
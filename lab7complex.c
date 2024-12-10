#include "complex.h"
#include <stdio.h>
/* Part 1: Complex numbers. */

void complex_add(complex *dst, const complex *z1, const complex *z2)
{
  dst->re = z1->re + z2->re;
  dst->im = z1->im + z2->im;
}

void complex_mul(complex *dst, const complex *z1, const complex *z2)
{
  dst->re = z1->re * z2->re - z1->im * z2->im;
  dst->im = z1->re * z2->im + z1->im * z2->re;
}

void complex_print(const complex *z)
{
  printf("%f + %f i", z->re, z->im);
}
